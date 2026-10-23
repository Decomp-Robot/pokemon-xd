#include <types.h>

/* Primitive-state helpers placed between the debug menus. */

extern s32 lbl_804EA680;

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

extern "C" s32 zz_000c27c_(void) { return 1; }

extern "C" void GSgfxDLPrimitiveType(s32 type)
{
    lbl_804EA680 = type;
}

extern "C" void _HSD_AObjForgetMemory(void)
{
    lbl_804EA680 = 0;
}

extern "C" void zz_000c298_(void) {}

#pragma force_active reset
