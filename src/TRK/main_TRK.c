#include "nub.h"

/* TRK_mainError -- why the last TRK_main pass ended. */
static s32 TRK_mainError;

/* The banner TRK_main announces the nub under: what a developer sees on the
   console when the debugger comes up. */
static const char gTRKMainBanner[] = "TRK_Main \n";

void TRK_main(void)
{
    MWTRACE(TRK_TRACE_LEVEL, gTRKMainBanner);
    TRK_mainError = TRKInitializeNub();
    if (TRK_mainError == 0) {
        TRKNubWelcome();
        TRKNubMainLoop();
    }
    TRK_mainError = TRKTerminateNub();
}
