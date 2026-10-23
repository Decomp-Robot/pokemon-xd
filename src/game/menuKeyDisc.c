#include <types.h>

/* Key disc menu: loads the key disc resource of the current map group. */
/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on
u32 zz_current_map_get_group_id(void);
void *GSresGetResource(u32 groupID, u32 resID);

void menuKeyDiscMain(void)
{
    GSresGetResource(zz_current_map_get_group_id(), 0x117F0000);
}

void zz_000e748_(void) {}

#pragma force_active reset
