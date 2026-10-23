#include <types.h>
#include <GSgfx.h>

/* GS display-list primitive-type helpers. GSgfxDLPrimitiveType stores its
 * argument into the state word GSgfxDLPrimType; _menuTitleOptionInit calls
 * it with 0 from title-option menu init. _HSD_AObjForgetMemory clears the
 * same word from the tail of the unnamed init routine fn_800A4008; the map
 * name ties it to the HSD animation-object memory reclamation path. Nothing
 * in the shipped binary reads GSgfxDLPrimType back, so the word's DL
 * primitive-type role rests on the map names alone. The two zz stubs keep
 * their placeholder names: unreferenced, and the map does not name them. */

extern s32 GSgfxDLPrimType;

/* Only the two zz stubs are unreferenced; the pragma keeps them linked. */
#pragma force_active on

extern "C" s32 zz_000c27c_(void) { return 1; }

extern "C" void GSgfxDLPrimitiveType(GSgfxPrimType type)
{
    GSgfxDLPrimType = type;
}

extern "C" void _HSD_AObjForgetMemory(void)
{
    GSgfxDLPrimType = 0;
}

extern "C" void zz_000c298_(void) {}

#pragma force_active reset
