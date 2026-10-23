#include <types.h>

extern void OSResetSystem(int reset, u32 resetCode, int forceMenu);

// Reset the host connection by rebooting without arguments once the
// disconnect handshake has been exchanged.
__declspec(section ".init") void __TRK_reset(void)
{
    OSResetSystem(0, 0, 0);
}
