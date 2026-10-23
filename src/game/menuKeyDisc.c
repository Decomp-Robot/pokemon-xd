#include <floor.h>
#include <GSres.h>
#include <types.h>

/* Key disc menu resource. */
enum {
    KEYDISC_RESID = 0x117F0000,
};

#pragma force_active on

void menuKeyDiscExit(void)
{
}

void menuKeyDiscMain(void)
{
    GSresGetResource(floorGetCurrentGroupID(), KEYDISC_RESID);
}

void menuKeyDiscInit(void)
{
}

#pragma force_active reset
