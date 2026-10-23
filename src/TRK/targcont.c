#include "nub.h"

/* Resume the target: it is no longer held, the port goes back to the code
   being debugged while the context is swapped, and the nub takes the port
   over again when it gets control back. */
u32 TRKTargetContinue(void)
{
    TRKTargetSetStopped(0);
    UnreserveEXI2Port();
    TRKSwapAndGo();
    ReserveEXI2Port();
    return 0;
}
