#include "trk.h"

void TRKDestructEvent(TRKEvent *event)
{
    TRKReleaseBuffer(event->msgBufID);
}


void TRKConstructEvent(TRKEvent *event, NubEventType eventType)
{
    event->eventType = eventType;
    event->eventID = 0;
    event->msgBufID = -1;
}


DSError TRKPostEvent(TRKEvent *event)
{
    DSError ret = DS_NoError;
    int nextEventID;

    TRKLock(&gTRKEventQueue);

    if (gTRKEventQueue.count == NUBQUEUELENGTH) {
        ret = DS_EventQueueFull;
    } else {
        nextEventID = (gTRKEventQueue.next + gTRKEventQueue.count) % NUBQUEUELENGTH;
        TRK_memcpy(&gTRKEventQueue.events[nextEventID], event, sizeof(TRKEvent));
        gTRKEventQueue.events[nextEventID].eventID = gTRKEventQueue.eventID;

        if (++gTRKEventQueue.eventID < kTRKFirstEventID) {
            gTRKEventQueue.eventID = kTRKFirstEventID;
        }

        gTRKEventQueue.count++;
    }

    TRKUnlock(&gTRKEventQueue);
    return ret;
}


BOOL TRKGetNextEvent(TRKEvent *event)
{
    BOOL status = 0;

    TRKLock(&gTRKEventQueue);
    if (gTRKEventQueue.count > 0) {
        TRK_memcpy(event, &gTRKEventQueue.events[gTRKEventQueue.next], sizeof(TRKEvent));
        gTRKEventQueue.count--;
        if (++gTRKEventQueue.next == NUBQUEUELENGTH) {
            gTRKEventQueue.next = 0;
        }
        status = 1;
    }
    TRKUnlock(&gTRKEventQueue);

    return status;
}


DSError TRKInitializeEventQueue(void)
{
    TRKLockInit(&gTRKEventQueue);
    TRKLock(&gTRKEventQueue);
    gTRKEventQueue.count = 0;
    gTRKEventQueue.next = 0;
    gTRKEventQueue.eventID = kTRKFirstEventID;
    TRKUnlock(&gTRKEventQueue);

    return DS_NoError;
}
