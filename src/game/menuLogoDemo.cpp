#include <GSfsys.h>
#include <GSthread.h>
#include <dolphin/os.h>
#include <types.h>

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
