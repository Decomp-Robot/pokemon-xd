#include <dolphin.h>
#include <dolphin/os.h>

// UNUSED: the game never calls this; dead-stripped at link.
void OSInitSemaphore(OSSemaphore* sem, s32 count) {
    BOOL enabled = OSDisableInterrupts();

    OSInitThreadQueue(&sem->queue);
    sem->count = count;
    OSRestoreInterrupts(enabled);
}

// UNUSED: the game never calls this; dead-stripped at link.
s32 OSWaitSemaphore(OSSemaphore* sem) {
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();

    while((sem->count = (count = sem->count)) <= 0) {
        OSSleepThread(&sem->queue);
    }

    sem->count--;
    OSRestoreInterrupts(enabled);
    return count;
}

// UNUSED: the game never calls this; dead-stripped at link.
s32 OSTryWaitSemaphore(OSSemaphore* sem) {
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = sem->count;
    if (sem->count > 0) {
        sem->count = sem->count - 1;
    }

    OSRestoreInterrupts(enabled);
    return count;
}

// UNUSED: the game never calls this; dead-stripped at link.
s32 OSSignalSemaphore(OSSemaphore* sem) {
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = sem->count;
    sem->count++;

    OSWakeupThread(&sem->queue);
    OSRestoreInterrupts(enabled);
    return count;
}

// UNUSED: the game never calls this; dead-stripped at link.
s32 OSGetSemaphoreCount(OSSemaphore* sem) {
    return sem->count;
}
