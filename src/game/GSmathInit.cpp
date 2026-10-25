#include <types.h>
#include <dolphin/os.h>
#include <dolphin/os/OSFastCast.h>
#include <hsdRandom.h>
#include <GSutil.h>
#include <GSmathInit.h>

extern "C" void GSmathInit(void)
{
    OSInitFastCast();
    OSSetGQR6(OS_GQR_TYPE_S16, 6);
    *__hsd_rand_seed_hold = (u32)OSGetTime();
    GSmathInitCosTable();
}
