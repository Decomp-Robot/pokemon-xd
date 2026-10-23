#include <floor.h>
#include <types.h>

/* Key disc menu resource. */
enum {
    KEYDISC_RESID = 0x117F0000,
};

/* Group/resource lookups shared with the floor and map code, which fetch
 * map resources as GSresGetResource(<map group id>, <resource id>). */
extern void *GSresGetResource(u32 groupID, u32 resID);

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
