#include <dolphin.h>
#include <dolphin/dvd.h>

#include "__dvd.h"

typedef struct FSTEntry {
    /* 0x00 */ unsigned int isDirAndStringOff;
    /* 0x04 */ unsigned int parentOrPosition;
    /* 0x08 */ unsigned int nextEntryOrLength;
} FSTEntry;

static OSBootInfo* BootInfo;
static FSTEntry* FstStart;
static char* FstStringStart;
static u32 MaxEntryNum;
static u32 currentDirectory;

OSThreadQueue __DVDThreadQueue;
u32 __DVDLongFileNameFlag;

// prototypes
static inline BOOL isSame(const char* path, const char* string);
static inline u32 myStrncpy(char* dest, char* src, u32 maxlen);
static u32 entryToPath(u32 entry, char* path, u32 maxlen);
static inline BOOL DVDConvertEntrynumToPath(s32 entrynum, char* path, u32 maxlen);
static void cbForReadAsync(s32 result, DVDCommandBlock* block);
static void cbForReadSync(s32 result, DVDCommandBlock* block);
static void cbForSeekAsync(s32 result, DVDCommandBlock* block);

void __DVDFSInit(void) {
    BootInfo = (void*)OSPhysicalToCached(0);
    FstStart = BootInfo->FSTLocation;
    if (FstStart) {
        MaxEntryNum = FstStart->nextEntryOrLength;
        FstStringStart = (char*)FstStart + (MaxEntryNum* sizeof(FSTEntry));
    }
}

/* For convenience */
#define entryIsDir(i) (((FstStart[i].isDirAndStringOff & 0xff000000) == 0) ? FALSE : TRUE)
#define stringOff(i) (FstStart[i].isDirAndStringOff & ~0xff000000)
#define parentDir(i) (FstStart[i].parentOrPosition)
#define nextDir(i) (FstStart[i].nextEntryOrLength)
#define filePosition(i) (FstStart[i].parentOrPosition)
#define fileLength(i) (FstStart[i].nextEntryOrLength)

static BOOL isSame(const char* path, const char* string) {
    while (*string != '\0') {
        if (tolower(*path++) != tolower(*string++)) {
            return FALSE;
        }
    }
    
    if (*path == '/' || *path == '\0') {
        return TRUE;
    }
    
    return FALSE;
}

s32 DVDConvertPathToEntrynum(const char* pathPtr) {
    const char* ptr;
    char* stringPtr;
    BOOL isDir;
    u32 length;
    u32 dirLookAt;
    u32 i;
    const char* origPathPtr = pathPtr;
    const char* extentionStart;
    BOOL illegal;
    BOOL extention;
    
    ASSERTMSGLINE(318, pathPtr, "DVDConvertPathToEntrynum(): null pointer is specified  ");
    
    dirLookAt = currentDirectory;
    
    while (1) {
        if (*pathPtr == '\0') {
            return (s32)dirLookAt;
        } else if (*pathPtr == '/') {
            dirLookAt = 0;
            pathPtr++;
            continue;
        } else if (*pathPtr == '.') {
            if (*(pathPtr + 1) == '.') {
                if (*(pathPtr + 2) == '/') {
                    dirLookAt = parentDir(dirLookAt);
                    pathPtr += 3;
                    continue;
                } else if (*(pathPtr + 2) == '\0') {
                    return (s32)parentDir(dirLookAt);
                }
            } else if (*(pathPtr + 1) == '/') {
                pathPtr += 2;
                continue;
            } else if (*(pathPtr + 1) == '\0') {
                return (s32)dirLookAt;
            }
        }
        
        if (__DVDLongFileNameFlag == 0) {
            extention = FALSE;
            illegal = FALSE;
        
            for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++) {
                if (*ptr == '.') {
                    if ((ptr - pathPtr > 8) || (extention == TRUE)) {
                        illegal = TRUE;
                        break;
                    }
                    extention = TRUE;
                    extentionStart = ptr + 1;
            
                } else if (*ptr == ' ')
                    illegal = TRUE;
            }
        
            if ((extention == TRUE) && (ptr - extentionStart > 3))
                illegal = TRUE;
        
            if (illegal)
                OSPanic(__FILE__, 387,
                    "DVDConvertEntrynumToPath(possibly DVDOpen or DVDChangeDir or DVDOpenDir): "
                    "specified directory or file (%s) doesn't match standard 8.3 format. This is a "
                    "temporary restriction and will be removed soon\n",
                    origPathPtr);
        } else {
            for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++)
                ;
        }
        
        isDir = (*ptr == '\0') ? FALSE : TRUE;
        length = (u32)(ptr - pathPtr);
        
        ptr = pathPtr;
        
        for (i = dirLookAt + 1; i < nextDir(dirLookAt); i = entryIsDir(i) ? nextDir(i) : (i + 1)) {
            if ((entryIsDir(i) == FALSE) && (isDir == TRUE)) {
                continue;
            }
        
            stringPtr = FstStringStart + stringOff(i);
        
            if (isSame(ptr, stringPtr) == TRUE) {
                goto next_hier;
            }
        }
        
        return -1;
    
next_hier:
        if (!isDir) {
            return (s32)i;
        }
        
        dirLookAt = i;
        pathPtr += length + 1;
    }
}

BOOL DVDOpen(const char* fileName, DVDFileInfo* fileInfo) {
    s32 entry;
    char currentDir[128];
    
    ASSERTMSGLINE(491, fileName, "DVDOpen(): null pointer is specified to file name  ");
    ASSERTMSGLINE(492, fileInfo, "DVDOpen(): null pointer is specified to file info address  ");
    
    entry = DVDConvertPathToEntrynum(fileName);
    
    if (0 > entry) {
        DVDGetCurrentDir(currentDir, 128);
        OSReport("Warning: DVDOpen(): file '%s' was not found under %s.\n", fileName, currentDir);
        return FALSE;
    }
    
    if (entryIsDir(entry)) {
        ASSERTMSG1LINE(506, !entryIsDir(entry), "DVDOpen(): directory '%s' is specified as a filename  ", fileName);
        return FALSE;
    }
    
    fileInfo->startAddr = filePosition(entry);
    fileInfo->length = fileLength(entry);
    fileInfo->callback = (DVDCallback)NULL;
    fileInfo->cb.state = DVD_STATE_END;
    
    return TRUE;
}

BOOL DVDClose(DVDFileInfo* fileInfo) {
    ASSERTMSGLINE(530, fileInfo, "DVDClose(): null pointer is specified to file info address  ");
    DVDCancel(&(fileInfo->cb));
    return TRUE;
}

static u32 myStrncpy(char* dest, char* src, u32 maxlen) {
    u32 i = maxlen;
    
    while ((i > 0) && (*src != 0)) {
        *dest++ = *src++;
        i--;
    }
    
    return (maxlen - i);
}

static u32 entryToPath(u32 entry, char* path, u32 maxlen) {
    char* name;
    u32 loc;
    
    if (entry == 0) {
        return 0;
    }
    
    name = FstStringStart + stringOff(entry);
    
    loc = entryToPath(parentDir(entry), path, maxlen);
    
    if (loc == maxlen) {
        return loc;
    }
    
    *(path + loc++) = '/';
    
    loc += myStrncpy(path + loc, name, maxlen - loc);
    
    return loc;
}

static BOOL DVDConvertEntrynumToPath(s32 entrynum, char* path, u32 maxlen) {
    u32 loc;
    
    ASSERTMSG1LINE(622, (entrynum >= 0) && (entrynum < MaxEntryNum), "DVDConvertEntrynumToPath: specified entrynum(%d) is out of range  ", entrynum);
    ASSERTMSG1LINE(624, maxlen > 1, "DVDConvertEntrynumToPath: maxlen should be more than 1 (%d is specified)", maxlen);
    ASSERTMSGLINE(629, entryIsDir(entrynum), "DVDConvertEntrynumToPath: cannot convert an entry num for a file to path  ");
    
    loc = entryToPath((u32)entrynum, path, maxlen);
    
    if (loc == maxlen) {
        path[maxlen - 1] = '\0';
        return FALSE;
    }
    
    if (entryIsDir(entrynum)) {
        if (loc == maxlen - 1) {
            path[loc] = '\0';
            return FALSE;
        }
    
        path[loc++] = '/';
    }
    
    path[loc] = '\0';
    return TRUE;
}

BOOL DVDGetCurrentDir(char* path, u32 maxlen) {
    ASSERTMSG1LINE(671, (maxlen > 1), "DVDGetCurrentDir: maxlen should be more than 1 (%d is specified)", maxlen);
    return DVDConvertEntrynumToPath((s32)currentDirectory, path, maxlen);
}

BOOL DVDReadAsyncPrio(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset, DVDCallback callback, s32 prio) {
    ASSERTMSGLINE(736, fileInfo, "DVDReadAsync(): null pointer is specified to file info address  ");
    ASSERTMSGLINE(737, addr, "DVDReadAsync(): null pointer is specified to addr  ");
    ASSERTMSGLINE(741, !OFFSET(addr, 32), "DVDReadAsync(): address must be aligned with 32 byte boundaries  ");
    ASSERTMSGLINE(743, !(length & 0x1F), "DVDReadAsync(): length must be  multiple of 32 byte  ");
    ASSERTMSGLINE(745, !(offset & 3), "DVDReadAsync(): offset must be multiple of 4 byte  ");

    DVD_ASSERTMSGLINE(750, (0 <= offset) && (offset <= fileInfo->length), "DVDReadAsync(): specified area is out of the file  ");
    
    DVD_ASSERTMSGLINE(756, (0 <= offset + length) && (offset + length < fileInfo->length + DVD_MIN_TRANSFER_SIZE), "DVDReadAsync(): specified area is out of the file  ");
    
    fileInfo->callback = callback;
    DVDReadAbsAsyncPrio(&(fileInfo->cb), addr, length, (s32)(fileInfo->startAddr + offset),
                        cbForReadAsync, prio);
    return TRUE;
}

#ifndef offsetof
#define offsetof(type, memb) ((u32) & ((type*)0)->memb)
#endif

static void cbForReadAsync(s32 result, DVDCommandBlock* block) {
    DVDFileInfo* fileInfo;

    fileInfo = (DVDFileInfo*)((char*)block - offsetof(DVDFileInfo, cb)); 
    ASSERTLINE(774, (void*) &fileInfo->cb == (void*) block);
    if (fileInfo->callback) {
        (fileInfo->callback)(result, fileInfo);
    }
}

s32 DVDReadPrio(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset, s32 prio) {
    int result;
    DVDCommandBlock* block;
    s32 state;
    BOOL enabled;
    s32 retVal;

    ASSERTMSGLINE(806, fileInfo, "DVDRead(): null pointer is specified to file info address  ");
    ASSERTMSGLINE(807, addr, "DVDRead(): null pointer is specified to addr  ");
    ASSERTMSGLINE(811, !OFFSET(addr, 32), "DVDRead(): address must be aligned with 32 byte boundaries  ");
    ASSERTMSGLINE(813, !(length & 0x1F), "DVDRead(): length must be  multiple of 32 byte  ");
    ASSERTMSGLINE(815, !(offset & 3), "DVDRead(): offset must be multiple of 4 byte  ");

    DVD_ASSERTMSGLINE(820, (0 <= offset) && (offset <= fileInfo->length), "DVDRead(): specified area is out of the file  ");
    DVD_ASSERTMSGLINE(826, (0 <= offset + length) && (offset + length < fileInfo->length + DVD_MIN_TRANSFER_SIZE), "DVDRead(): specified area is out of the file  ");

    block = &fileInfo->cb;
    result = DVDReadAbsAsyncPrio(block, addr, length, fileInfo->startAddr + offset, cbForReadSync, prio);
    if (result == 0) {
        return -1;
    }
    enabled = OSDisableInterrupts();

    while (1) {
        state = ((volatile DVDCommandBlock*)block)->state;
        if (state == 0) {
            retVal = (s32)block->transferredSize;
            break;
        } else if (state == -1) {
            retVal = -1;
            break;
        } else if (state == 10) {
            retVal = -3;
            break;
        }
        OSSleepThread(&__DVDThreadQueue);
    }

    OSRestoreInterrupts(enabled);
    return retVal;
}

static void cbForReadSync(s32 result, DVDCommandBlock* block) {
    OSWakeupThread(&__DVDThreadQueue);
}

int DVDSeekAsyncPrio(DVDFileInfo* fileInfo, s32 offset, DVDCallback callback, s32 prio) {
    ASSERTMSGLINE(898, fileInfo, "DVDSeek(): null pointer is specified to file info address  ");
    ASSERTMSGLINE(902, !(offset & 3), "DVDSeek(): offset must be multiple of 4 byte  ");

    DVD_ASSERTMSGLINE(907, (0 <= offset) && (offset <= fileInfo->length), "DVDSeek(): offset is out of the file  ");

    fileInfo->callback = callback;
    DVDSeekAbsAsyncPrio(&fileInfo->cb, (u32)(char*)fileInfo->startAddr + offset, cbForSeekAsync, prio);
    return 1;
}

static void cbForSeekAsync(s32 result, DVDCommandBlock* block) {
    DVDFileInfo* fileInfo;

    fileInfo = (DVDFileInfo*)((char*)block - offsetof(DVDFileInfo, cb)); 
    ASSERTLINE(925, (void*) &fileInfo->cb == (void*) block);
    if (fileInfo->callback) {
        (fileInfo->callback)(result, fileInfo);
    }
}
