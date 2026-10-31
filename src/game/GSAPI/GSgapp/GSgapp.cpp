#include <string.h>
#include <GSAPI/GSgapp/GSgapp.h>
#include <GSgfxVideo.h>
#include <GSmem.h>
#include <dolphin/os.h>

// Application run states.
#define GSAPP_STATE_FREE 0
#define GSAPP_STATE_MAIN 1
#define GSAPP_STATE_BKGD 2
#define GSAPP_STATE_VSYNC 3

// Stack for background apps, run from the OS idle thread.
#define GSAPP_BKGND_STACK_SIZE 8192

// Registered application record.
typedef struct _GSgapp {
    struct _GSgapp* prev;    // 0x00 previous record in the run list
    struct _GSgapp* next;    // 0x04 next record in the run list
    s32 state;               // 0x08 GSAPP_STATE_...
    s32 pauseLevel;          // 0x0C pauses this app while the game is paused
    u8 priority;             // 0x10 run order; lower values run first
    u8 blocked;              // 0x11 set to keep the app from running
    u16 pad;                 // 0x12 padding
    char name[32];           // 0x14 debug name
    u32 userData;            // 0x34
    GSgappCallback callback; // 0x38
} _GSgapp;

_GSgapp* _gappStructList;
s32 _gappMaxJobs;
s32 _gappMaxBkgndJobs;
s32 _gappTotalJobs;
static u8 _gappPause;
u8* _gappBkgndStackPtr;
_GSgapp* _gappCurrentGapp;
_GSgapp* _gappListTop;
_GSgapp* _gappCreateBGJobRequestListTop;

static void gappBackgroundCallback(void* param);
static void gappVSyncCallback(u32 retraceCount);

void GSgappSetName(s32 hdl, char* name)
{
    if (name != 0) {
        _GSgapp* app = &_gappStructList[hdl - 1];
        memcpy(app->name, name, sizeof(app->name) - 1);
        app->name[sizeof(app->name) - 1] = 0;
    }
}

void GSgappUnblock(s32 hdl)
{
    _GSgapp* app = &_gappStructList[hdl - 1];
    app->blocked = 0;
}

void GSgappSetPauseLevel(s32 hdl, s32 pauseLevel)
{
    _GSgapp* app = &_gappStructList[hdl - 1];
    app->pauseLevel = pauseLevel;
}

void GSgappPauseOff(void)
{
    _gappPause = 0;
}

void GSgappPauseOn(void)
{
    _gappPause = 1;
}

void GSgappBlock(s32 hdl)
{
    _GSgapp* app = &_gappStructList[hdl - 1];
    app->blocked = 1;
}

void GSgappTerminate(s32 hdl)
{
    _GSgapp* app = &_gappStructList[hdl - 1];
    BOOL enabled = OSDisableInterrupts();
    if (app->prev != 0) {
        app->prev->next = app->next;
    }
    if (app->next != 0) {
        app->next->prev = app->prev;
    }
    if (_gappListTop == app) {
        _gappListTop = app->next;
    }
    app->prev = 0;
    app->next = 0;
    OSRestoreInterrupts(enabled);
    app->state = GSAPP_STATE_FREE;
}

void GSgappUpdate(void)
{
    _GSgapp* app = _gappListTop;

    while (app != 0) {
        _GSgapp* next = app->next;
        if (app->state == GSAPP_STATE_MAIN && app->blocked == 0 && (_gappPause == 0 || app->pauseLevel == 0)) {
            _gappCurrentGapp = app;
            app->callback(((u32)app - (u32)_gappStructList) / sizeof(_GSgapp) + 1, app->userData);
        }
        app = next;
    }

    _gappCurrentGapp = 0;
}

// Finds a free record in the range of records assigned to the given app type.
static inline _GSgapp* gappFindFreeJob(s32 app)
{
    _GSgapp* job;
    u32 i;

    if (app == GSAPP_STATE_BKGD) {
        job = &_gappStructList[_gappMaxJobs];
        i = _gappMaxBkgndJobs;
    } else {
        job = _gappStructList;
        i = _gappMaxJobs;
    }

    for (; i != 0; i--) {
        if (job->state == GSAPP_STATE_FREE) {
            return job;
        }
        job++;
    }
    return 0;
}

static inline void gappInsertJob(_GSgapp* job)
{
    _GSgapp* next;
    _GSgapp* cur = _gappListTop;

    while ((next = cur->next) != 0 && cur->priority < job->priority) {
        cur = next;
    }
    if (next == 0 && cur->priority < job->priority) {
        job->prev = cur;
        job->next = 0;
        cur->next = job;
    } else {
        if (cur->prev != 0) {
            cur->prev->next = job;
        }
        job->prev = cur->prev;
        job->next = cur;
        cur->prev = job;
        if (_gappListTop == cur) {
            _gappListTop = job;
        }
    }
}

s32 GSgappCreate(s32 app, s32 priority, s32 userData, GSgappCallback callback)
{
    _GSgapp* job;

    job = gappFindFreeJob(app);

    if (job == 0) {
        return 0;
    }

    job->prev = 0;
    job->next = 0;
    job->state = app;
    job->priority = priority;
    job->blocked = 0;
    job->userData = userData;
    job->callback = callback;
    job->pauseLevel = 0;
    job->name[0] = 0;

    if (_gappListTop == 0) {
        _gappListTop = job;
    } else {
        BOOL enabled = OSDisableInterrupts();
        if (job->state == GSAPP_STATE_BKGD) {
            job->next = _gappCreateBGJobRequestListTop;
            _gappCreateBGJobRequestListTop = job;
        } else {
            gappInsertJob(job);
        }
        OSRestoreInterrupts(enabled);
    }

    return ((u32)job - (u32)_gappStructList) / sizeof(_GSgapp) + 1;
}

void GSgappInit(s32 startApp, s32 count)
{
    s32 total = startApp + count;

    _gappMaxJobs = startApp;
    _gappMaxBkgndJobs = count;
    _gappTotalJobs = total;
    _gappCurrentGapp = 0;

    _gappStructList = (_GSgapp*)GSmemAlloc(total * sizeof(_GSgapp));
    if (_gappStructList != 0) {
        u32 i = 0;
        while (i < _gappTotalJobs) {
            _gappStructList[i].state = GSAPP_STATE_FREE;
            i++;
        }

        u8* stack = (u8*)GSmemAlloc(GSAPP_BKGND_STACK_SIZE);
        _gappBkgndStackPtr = stack;
        OSSetIdleFunction(gappBackgroundCallback, 0, stack + GSAPP_BKGND_STACK_SIZE - sizeof(u32), GSAPP_BKGND_STACK_SIZE - sizeof(u32));
        GSgfxVideoSetVSyncCallback(gappVSyncCallback);
    }
}

// Idle-function hook; runs background apps and services their creation
// requests while the main thread idles.
static void gappBackgroundCallback(void* param)
{
#pragma unused(param)
    while (1) {
        _GSgapp* app = _gappListTop;

        while (app != 0) {
            _GSgapp* next = app->next;
            if (app->state == GSAPP_STATE_BKGD && app->blocked == 0 && (_gappPause == 0 || app->pauseLevel == 0)) {
                _gappCurrentGapp = app;
                app->callback(((u32)app - (u32)_gappStructList) / sizeof(_GSgapp) + 1, app->userData);
            }
            app = next;
        }

        _gappCurrentGapp = 0;

        BOOL enabled = OSDisableInterrupts();
        _GSgapp* req = _gappCreateBGJobRequestListTop;
        while (req != 0) {
            _GSgapp* next = req->next;

            gappInsertJob(req);

            req = next;
        }
        _gappCreateBGJobRequestListTop = 0;
        OSRestoreInterrupts(enabled);
    }
}

// VSync hook, installed with the video library by GSgappInit; runs the
// apps registered with state GSAPP_STATE_VSYNC.
static void gappVSyncCallback(u32 retraceCount)
{
#pragma unused(retraceCount)
    _GSgapp* app = _gappListTop;

    while (app != 0) {
        _GSgapp* next = app->next;
        if (app->state == GSAPP_STATE_VSYNC && app->blocked == 0 && (_gappPause == 0 || app->pauseLevel == 0)) {
            _gappCurrentGapp = app;
            app->callback(((u32)app - (u32)_gappStructList) / sizeof(_GSgapp) + 1, app->userData);
        }
        app = next;
    }

    _gappCurrentGapp = 0;
}
