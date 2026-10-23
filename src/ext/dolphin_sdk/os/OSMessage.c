#include <dolphin.h>
#include <dolphin/os.h>

// UNUSED: the game never calls this; dead-stripped at link.
void OSInitMessageQueue(OSMessageQueue* mq, void* msgArray, s32 msgCount) {
    OSInitThreadQueue(&mq->queueSend);
    OSInitThreadQueue(&mq->queueReceive);
    mq->msgArray = msgArray;
    mq->msgCount = msgCount;
    mq->firstIndex = 0;
    mq->usedCount = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.
int OSSendMessage(OSMessageQueue* mq, void* msg, s32 flags) {
    BOOL enabled;
    s32 lastIndex;

    enabled = OSDisableInterrupts();
    while(mq->msgCount <= mq->usedCount) {
        if (!(flags & 1)) {
            OSRestoreInterrupts(enabled);
            return 0;
        }
        OSSleepThread(&mq->queueSend);
    }
    lastIndex = (mq->firstIndex + mq->usedCount) % mq->msgCount;
    ((u32*)mq->msgArray)[lastIndex] = (u32)msg;
    mq->usedCount++;
    OSWakeupThread(&mq->queueReceive);
    OSRestoreInterrupts(enabled);
    return 1;
}

// UNUSED: the game never calls this; dead-stripped at link.
int OSReceiveMessage(OSMessageQueue* mq, void* msg, s32 flags) {
    BOOL enabled = OSDisableInterrupts();

    while(mq->usedCount == 0) {
        if (!(flags & 1)) {
            OSRestoreInterrupts(enabled);
            return 0;
        }
        OSSleepThread(&mq->queueReceive);
    }
    if(msg != NULL) {
       *(u32*)msg = ((u32*)mq->msgArray)[mq->firstIndex];
    }
    
    mq->firstIndex = (mq->firstIndex + 1) % mq->msgCount;
    mq->usedCount--;
    OSWakeupThread(&mq->queueSend);
    OSRestoreInterrupts(enabled);
    return 1;
}

// UNUSED: the game never calls this; dead-stripped at link.
int OSJamMessage(OSMessageQueue* mq, void* msg, s32 flags) {
    BOOL enabled = OSDisableInterrupts();

    while(mq->msgCount <= mq->usedCount) {
        if(!(flags & 1)) {
            OSRestoreInterrupts(enabled);
            return 0;
        }
        OSSleepThread(&mq->queueSend);
    }
    mq->firstIndex = (mq->firstIndex + mq->msgCount - 1) % mq->msgCount;
    ((u32*)mq->msgArray)[mq->firstIndex] = (u32)msg;
    mq->usedCount++;
    OSWakeupThread(&mq->queueReceive);
    OSRestoreInterrupts(enabled);
    return 1;
}
