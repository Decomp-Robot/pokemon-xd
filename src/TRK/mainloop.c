#include "trk.h"

void TRKNubMainLoop(void)
{
    TRKEvent event;
    BOOL quit = FALSE;
    BOOL inputPending = FALSE;

    while (quit == FALSE) {
        if (TRKGetNextEvent(&event) != FALSE) {
            inputPending = FALSE;

            switch (event.eventType) {
            case NUBEVENT_Null:
                break;
            case NUBEVENT_Request:
                TRKDispatchMessage(TRKGetBuffer(event.msgBufID));
                break;
            case NUBEVENT_Shutdown:
                quit = TRUE;
                break;
            case NUBEVENT_Breakpoint:
            case NUBEVENT_Exception:
                TRKTargetInterrupt(&event);
                break;
            case NUBEVENT_Support:
                TRKTargetSupportRequest();
                break;
            }

            TRKDestructEvent(&event);
        } else if (!inputPending || *gTRKInputPendingPtr != 0) {
            inputPending = TRUE;
            TRKGetInput();
        } else {
            if (!TRKTargetStopped()) {
                TRKTargetContinue();
            }
            inputPending = FALSE;
        }
    }
}

