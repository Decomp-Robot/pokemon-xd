#include <GSdvd.h>
#include <GSAPI/GSgapp/GSgapp.h>
#include <GSlog.h>
#include <GSmem.h>
#include <GSnetwork.h>
#include <pokecolo.h>
#include <winMsg.h>
#include <GSgfxC.h>
#include <dolphin/dvd.h>
#include <dolphin/os.h>

struct GSdvdFile {
    u8 inUse;
    DVDFileInfo info;
    GSdvdReadCompletionFunc completion;
    void *readBuffer;
    u32 readSize;
};

enum {
    GSDVD_ERROR_STATE_NONE,
    GSDVD_ERROR_STATE_COVEROPEN,
    GSDVD_ERROR_STATE_COVEROPEN_WAIT,
    GSDVD_ERROR_STATE_NODISC,
    GSDVD_ERROR_STATE_NODISC_WAIT,
    GSDVD_ERROR_STATE_WRONGDISC,
    GSDVD_ERROR_STATE_WRONGDISC_WAIT,
    GSDVD_ERROR_STATE_RETRY,
    GSDVD_ERROR_STATE_RETRY_WAIT,
    GSDVD_ERROR_STATE_FATALERROR,
    GSDVD_ERROR_STATE_FATALERROR_WAIT
};
enum { GSDVD_READ_PRIORITY = 2, GSDVD_ERROR_TASK_PRIORITY = 19, GSDVD_ERROR_TASK_CLASS = 1 };
enum { GSDVD_MESSAGE_NONE = 1 };
enum { GSDVD_ROM_VERSION_EX = 4 };

static u8 _DvdServerAddr[4] = {0};
static u8 _ReportServerAddr[4] = {0};
static u8 _GCClientAddr[4] = {0};
static u8 _Netmask[4] = {0};
static u8 _Gateway[4] = {0};
static u32 _errorMsg_CoverOpen = GSDVD_MESSAGE_NONE;
static u32 _errorMsg_NoDisc = GSDVD_MESSAGE_NONE;
static u32 _errorMsg_WrongDisc = GSDVD_MESSAGE_NONE;
static u32 _errorMsg_Retry = GSDVD_MESSAGE_NONE;
static u32 _errorMsg_FatalError = GSDVD_MESSAGE_NONE;
static char *_diskid_gameName = "GXXJ";
static char *_diskid_company = "01";

static u8 _dvdInitialized = 0;
static u16 _DvdServerPort = 0;
static u8 _DvdServerEnabled = 0;
static u16 _ReportServerPort = 0;
static u8 _ReportServerEnabled = 0;
static u8 _GCClientEnabled = 0;
static u32 _errorState = 0;
static s32 _GappHandle = 0;
static GSdvdFile *_dvdWork = 0;
static u32 _maxDvdWork = 0;

static void _gsdvdErrorTask(u32 appID, u32 userData);
static void _gsdvdError_MsgOpen(u32 message);
static void _errorTask_State_None(s32 status);
static void _AsyncCallback(s32 result, DVDFileInfo *info);
static void _AsyncReadCallback(s32 result, DVDFileInfo *info);
static GSdvdFile *_info2work(DVDFileInfo *info);
static s32 _SOInetAtoN(const char *address, u8 *out);
static void _freeDvdWork(GSdvdFile *file);
static GSdvdFile *_getNewDvdWork(void);
static void _initDvdWork(void);
static void *_dvdDoMalloc(u32 size);

u8 GSdvdFstRefresh(void) { return 0; }

static void _gsdvdErrorTask(u32 appID, u32 userData)
{
    s32 status = GSdvdGetDriveStatus();
    switch (_errorState) {
    case GSDVD_ERROR_STATE_NONE:
        _errorTask_State_None(status);
        break;
    case GSDVD_ERROR_STATE_COVEROPEN:
        _gsdvdError_MsgOpen(_errorMsg_CoverOpen);
        _errorState = GSDVD_ERROR_STATE_COVEROPEN_WAIT;
        break;
    case GSDVD_ERROR_STATE_COVEROPEN_WAIT:
        GSlogWrite("[GSDVD_ERROR_STATE_COVEROPEN_WAIT]  status = %d\n", status);
        if (status == DVD_STATE_COVER_OPEN || status == DVD_STATE_WAITING) break;
        winMsgCloseError(1);
        _errorState = GSDVD_ERROR_STATE_NONE;
        break;
    case GSDVD_ERROR_STATE_NODISC:
        _gsdvdError_MsgOpen(_errorMsg_NoDisc);
        _errorState = GSDVD_ERROR_STATE_NODISC_WAIT;
        break;
    case GSDVD_ERROR_STATE_NODISC_WAIT:
        if (status == DVD_STATE_NO_DISK) break;
        winMsgCloseError(1);
        _errorState = GSDVD_ERROR_STATE_NONE;
        break;
    case GSDVD_ERROR_STATE_WRONGDISC:
        _gsdvdError_MsgOpen(_errorMsg_WrongDisc);
        _errorState = GSDVD_ERROR_STATE_WRONGDISC_WAIT;
        break;
    case GSDVD_ERROR_STATE_WRONGDISC_WAIT:
        if (status == DVD_STATE_WRONG_DISK) break;
        winMsgCloseError(1);
        _errorState = GSDVD_ERROR_STATE_NONE;
        break;
    case GSDVD_ERROR_STATE_RETRY:
        _gsdvdError_MsgOpen(_errorMsg_Retry);
        _errorState = GSDVD_ERROR_STATE_RETRY_WAIT;
        break;
    case GSDVD_ERROR_STATE_RETRY_WAIT:
        if (status == DVD_STATE_RETRY) break;
        winMsgCloseError(1);
        _errorState = GSDVD_ERROR_STATE_NONE;
        break;
    case GSDVD_ERROR_STATE_FATALERROR:
        GSgfxResetDisable();
        _gsdvdError_MsgOpen(_errorMsg_FatalError);
        _errorState = GSDVD_ERROR_STATE_FATALERROR_WAIT;
        break;
    case GSDVD_ERROR_STATE_FATALERROR_WAIT:
        break;
    }
}

static void _gsdvdError_MsgOpen(u32 message)
{
    if (message != GSDVD_MESSAGE_NONE) winMsgOpenError(message, 1, 1);
}

static void _errorTask_State_None(s32 status)
{
    switch (status) {
    case DVD_STATE_FATAL_ERROR: _errorState = GSDVD_ERROR_STATE_FATALERROR; break;
    case DVD_STATE_COVER_OPEN: _errorState = GSDVD_ERROR_STATE_COVEROPEN; break;
    case DVD_STATE_NO_DISK: _errorState = GSDVD_ERROR_STATE_NODISC; break;
    case DVD_STATE_WRONG_DISK: _errorState = GSDVD_ERROR_STATE_WRONGDISC; break;
    case DVD_STATE_RETRY: _errorState = GSDVD_ERROR_STATE_RETRY; break;
    }
}

void GSdvdSetErrorMsg(u32 coverOpen, u32 noDisc, u32 wrongDisc, u32 retry, u32 fatalError)
{
    _errorMsg_CoverOpen = coverOpen;
    _errorMsg_NoDisc = noDisc;
    _errorMsg_WrongDisc = wrongDisc;
    _errorMsg_Retry = retry;
    _errorMsg_FatalError = fatalError;
}
u8 GSdvdSeekAsync(GSdvdFile *file, s32 offset, GSdvdReadCompletionFunc completion)
{
    if (_dvdInitialized == 0) return 0;
    if (file == NULL) return 0;
    file->completion = completion;
    return DVDSeekAsyncPrio(&file->info, offset, _AsyncCallback, GSDVD_READ_PRIORITY);
}
s32 GSdvdGetFileInfoStatus(GSdvdFile *file)
{
    if (_dvdInitialized == 0) return -1;
    if (file == NULL) return -1;
    return DVDGetCommandBlockStatus(&file->info.cb);
}
s32 GSdvdGetDriveStatus(void)
{
    if (_dvdInitialized == 0) return -1;
    return DVDGetDriveStatus();
}
s32 GSdvdWrite(GSdvdFile *file, void *buffer, u32 size, s32 offset) { return -1; }
s32 GSdvdRemove(const char *fileName)
{
    if (_dvdInitialized == 0) return 0;
    return 0;
}
GSdvdFile *GSdvdCreate(const char *fileName)
{
    if (_dvdInitialized == 0) return NULL;
    return NULL;
}
u32 GSdvdGetLength(GSdvdFile *file)
{
    if (_dvdInitialized == 0) return 0;
    if (file == NULL) return 0;
    return file->info.length;
}
s32 GSdvdClose(GSdvdFile *file)
{
    if (_dvdInitialized == 0) return 0;
    if (file == NULL) return 0;
    _freeDvdWork(file);
    return DVDClose(&file->info);
}
u8 GSdvdReadAsync(GSdvdFile *file, void *buffer, u32 size, s32 offset, GSdvdReadCompletionFunc completion)
{
    if (_dvdInitialized == 0) return 0;
    if (file == NULL) return 0;
    file->completion = completion;
    if ((u32)buffer & 0x1f) return 0;
    if (size & 0x1f) return 0;
    if (offset & 3) return 0;
    file->readBuffer = buffer;
    file->readSize = size;
    return DVDReadAsyncPrio(&file->info, buffer, size, offset, _AsyncReadCallback, GSDVD_READ_PRIORITY);
}
s32 GSdvdRead(GSdvdFile *file, void *buffer, u32 size, s32 offset)
{
    if (_dvdInitialized == 0) return -1;
    if (file == NULL) return -1;
    if ((u32)buffer & 0x1f) return -1;
    if (size & 0x1f) return -1;
    if (offset & 3) return -1;
    return DVDReadPrio(&file->info, buffer, size, offset, GSDVD_READ_PRIORITY);
}
u8 GSdvdOpenTest(const char *fileName)
{
    if (_dvdInitialized == 0) return 0;
    return DVDConvertPathToEntrynum(fileName) != -1;
}
GSdvdFile *GSdvdOpen(const char *fileName)
{
    GSdvdFile *file;
    if (_dvdInitialized == 0) return NULL;
    file = _getNewDvdWork();
    if (file == NULL) return NULL;
    if (!DVDOpen(fileName, &file->info)) {
        _freeDvdWork(file);
        return NULL;
    }
    return file;
}
void GSdvdSetGCClient(const char *address, const char *netmask, const char *gateway, u8 apply)
{
    _SOInetAtoN(address, _GCClientAddr);
    _SOInetAtoN(netmask, _Netmask);
    _SOInetAtoN(gateway, _Gateway);
    _GCClientEnabled = 1;
    if (apply == 1) GSnetworkSetIP(_GCClientAddr, _Netmask, _Gateway);
}
void GSdvdSetReportServer(const char *address, u16 port, u8 apply)
{
    _SOInetAtoN(address, _ReportServerAddr);
    _ReportServerPort = port;
    _ReportServerEnabled = 1;
    if (apply == 1) GSlogSetIP(_ReportServerAddr, port);
}
void GSdvdSetDVDServer(const char *address, u16 port)
{
    _SOInetAtoN(address, _DvdServerAddr);
    _DvdServerPort = port;
    _DvdServerEnabled = 1;
}
u8 GSdvdInit(u32 maxFiles)
{
    DVDDiskID *id;
    if (_dvdInitialized == 1) return 0;
    _maxDvdWork = maxFiles;
    _dvdWork = _dvdDoMalloc(sizeof(GSdvdFile) * maxFiles);
    if (_dvdWork == NULL) return 0;
    _initDvdWork();
    DVDInit();
    if (pokecoloGetRomVersion() != GSDVD_ROM_VERSION_EX) {
        switch (pokecoloGetCountryCode()) {
        case POKECOLO_COUNTRY_CODE_JAPAN: _diskid_gameName = "GXXJ"; break;
        case POKECOLO_COUNTRY_CODE_USA: _diskid_gameName = "GXXE"; break;
        case POKECOLO_COUNTRY_CODE_EUROPE: _diskid_gameName = "GXXP"; break;
        }
        id = DVDGetCurrentDiskID();
        id->gameName[0] = _diskid_gameName[0];
        id->gameName[1] = _diskid_gameName[1];
        id->gameName[2] = _diskid_gameName[2];
        id->gameName[3] = _diskid_gameName[3];
        id->company[0] = _diskid_company[0];
        id->company[1] = _diskid_company[1];
        id->diskNumber = 0;
        id->gameVersion = 0;
    }
    DVDSetAutoFatalMessaging(1);
    _GappHandle = GSgappCreate(GSDVD_ERROR_TASK_CLASS, GSDVD_ERROR_TASK_PRIORITY, 0, _gsdvdErrorTask);
    GSgappSetName(_GappHandle, "GSdvdErrorTask");
    _dvdInitialized = 1;
    return 1;
}
static void _AsyncCallback(s32 result, DVDFileInfo *info)
{
    GSdvdFile *file = _info2work(info);
    if (file && file->completion) file->completion(result, file);
}
static void _AsyncReadCallback(s32 result, DVDFileInfo *info)
{
    GSdvdFile *file = _info2work(info);
    if (file && file->completion) {
        DCInvalidateRange(file->readBuffer, file->readSize);
        file->completion(result, file);
    }
}
static GSdvdFile *_info2work(DVDFileInfo *info)
{
    u32 i;
    for (i = 0; i < _maxDvdWork; i++) {
        if (_dvdWork[i].inUse && &_dvdWork[i].info == info) return &_dvdWork[i];
    }
    return NULL;
}
static s32 _SOInetAtoN(const char *address, u8 *out) { return 0; }
static void _freeDvdWork(GSdvdFile *file)
{
    BOOL interrupts = OSDisableInterrupts();
    file->inUse = 0;
    OSRestoreInterrupts(interrupts);
}
static GSdvdFile *_getNewDvdWork(void)
{
    BOOL interrupts = OSDisableInterrupts();
    GSdvdFile *file = NULL;
    u32 i;
    for (i = 0; i < _maxDvdWork; i++) {
        if (_dvdWork[i].inUse != 1) {
            _dvdWork[i].inUse = 1;
            file = &_dvdWork[i];
            break;
        }
    }
    OSRestoreInterrupts(interrupts);
    return file;
}
static void _initDvdWork(void)
{
    u32 i;
    for (i = 0; i < _maxDvdWork; i++) _dvdWork[i].inUse = 0;
}
static void *_dvdDoMalloc(u32 size) { return GSmemAllocAlign(size, 32); }
