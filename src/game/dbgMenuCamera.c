#include <types.h>

/* Debug camera menu. The two hooks that precede the menu callbacks in the
 * binary - the graphics capture target and the people-talk flag - are
 * release-mode stubs; the link evidence does not separate them from the
 * camera callbacks, so they stay with them. */

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

extern u8 zz_010ee28_(s32 menuId);
extern void menuClose(s32 menuId);
extern s32 menuOpenCustom(s32 menuId, ...);
extern void menuSetPosition(s32 menuId, s32 x, s32 y);
extern void zz_01980c4_(s32 cameraNo);
extern void zz_0199f88_(void);

#pragma peephole off

/* Set the target used by the debug screen capture. */
void GSgfxCaptureSetTarget(void)
{
}

/* Enable the people-talk flag display. */
s32 msgctrlSetPeopleTalkFlag(void)
{
    zz_0199f88_();
    return 0;
}

/* Open the debug camera window when it is down, close it when it is up. */
s32 dbgMenuCameraChangeDisp(void)
{
    if (zz_010ee28_(6) != 0) {
        menuClose(6);
    } else {
        menuOpenCustom(6, 0, 0, 0, 1, 0);
        menuSetPosition(6, 0x14, 0x104);
    }
    return 0;
}

/* Camera the debug camera menu entries select: entry 1 and entry 2 pick the
 * two debug cameras, everything else keeps the default camera. */
s32 dbgMenuCameraSetType(void* window, s32 index)
{
    switch (index) {
    case 0:
    default:
        zz_01980c4_(0);
        break;
    case 1:
        zz_01980c4_(5);
        break;
    case 2:
        zz_01980c4_(6);
        break;
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
