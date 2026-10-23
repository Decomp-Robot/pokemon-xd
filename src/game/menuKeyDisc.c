#include <types.h>

/*
 * Key disc debug menu (menuKeyDisc.o per the linker map). The debug menu
 * tables that register menu entry points like these are not part of the
 * final binary (see dbgMenuToolBattle.c), so neither function below is
 * called by shipped code.
 */

/* Resource id the key disc menu fetches from the current map's resource
 * group. It occurs nowhere else in the binary, and the resource system
 * never registers it from recovered code, so what the resource holds is
 * not known beyond the menu's own name. */
enum {
    KEYDISC_RESID = 0x117F0000,
};

/* Group/resource lookups shared with the floor and map code, which fetch
 * map resources as GSresGetResource(<map group id>, <resource id>). */
extern u32 zz_current_map_get_group_id(void);
extern void *GSresGetResource(u32 groupID, u32 resID);

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

/* Fetches the key disc resource of the current map's resource group and
 * discards the pointer: GSresGetResource only looks up resources that are
 * already registered, so this just makes sure it is fetched. */
void menuKeyDiscMain(void)
{
    GSresGetResource(zz_current_map_get_group_id(), KEYDISC_RESID);
}

/* Companion entry point of the key disc menu; compiles to a bare blr. Its
 * purpose is not recoverable: nothing in the binary references it and the
 * registering debug table is gone, so it keeps its placeholder name. */
void zz_000e748_(void)
{
}

#pragma force_active reset
