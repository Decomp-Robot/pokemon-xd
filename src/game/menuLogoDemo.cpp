#include <GSfsys.h>
#include <GSgfxVideo.h>
#include <GSthread.h>
#include <dolphin/os.h>
#include <types.h>

// Progressive mode requested by the logo screens.
enum ProgressiveModeChange {
    PROGRESSIVE_MODE_CHANGE_KEEP = -1,
    PROGRESSIVE_MODE_CHANGE_OFF = 0,
    PROGRESSIVE_MODE_CHANGE_ON = 1,
};

ProgressiveModeChange _progressive_mode_change =
    PROGRESSIVE_MODE_CHANGE_KEEP;

// Screen copy filter tap sets selected by _menuSetScreenFilter.
static u8 _screen_filter_off[7] = {0, 0, 21, 22, 21, 0, 0};
static u8 _screen_filter_default[7] = {8, 8, 10, 12, 10, 8, 8};

void _menuSetScreenFilter(bool filter) {
    if (filter) {
        GSgfxVideoSetFilter(_screen_filter_default);
    } else {
        GSgfxVideoSetFilter(_screen_filter_off);
    }
}

s32 _sound_wave_complete;
volatile s32 _gsfsysinit_complete;

void* _menuGSfsysInitThread(void*) {
    GSfsysInit(64, 0, 0, 0);
    _gsfsysinit_complete = 1;
    return 0;
}

void _menuGSfsysInit(void) {
    static u8 stack[4096];
    static OSThread thread;

    if (_gsfsysinit_complete != 1) {
        OSCreateThread(&thread, _menuGSfsysInitThread, 0,
                       &stack[4096 - 4], 4096, 16,
                       OS_THREAD_ATTR_DETACH);
        OSResumeThread(&thread);
    }
}

void _menuGSfsysInitWait(void) {
    while (_gsfsysinit_complete == 0) {
        GSthreadSwitch();
    }
}

void menuGSfsysInit(void) {
    _menuGSfsysInit();
}

void menuGSfsysInitWait(void) {
    _menuGSfsysInitWait();
}
