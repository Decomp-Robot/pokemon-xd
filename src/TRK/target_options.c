#include "nub.h"

/* Whether the game wants the debugger to drive the board's serial port. */
u8 GetUseSerialIO(void)
{
    return gTRKUseSerialIO;
}

void TRKSetUseSerialIO(u8 value)
{
    gTRKUseSerialIO = value;
}
