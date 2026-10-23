#include <types.h>

/* Debug camera menu. The two hooks that precede the menu callbacks in the
 * binary are small: GSgfxCaptureSetTarget is empty, and
 * msgctrlSetPeopleTalkFlag drops the debug camera and restores the room
 * floor camera defaults. */

/* The debug menus reach these entry points only through their dispatch
 * tables, so they have no direct callers (GSgfxCaptureSetTarget is called
 * once, from the field-init module); without force_active the linker's
 * dead stripping drops them. */
#pragma force_active on

extern u8 menuIsOpen(s32 menuId);
extern void menuClose(s32 menuId);
extern s32 menuOpenCustom(s32 menuId, ...);
extern void menuSetPosition(s32 menuId, s32 x, s32 y);
/* Set the camera type; returns the previous type. 5 and 6 are the debug
 * cameras. */
extern u8 cameraSetType(u8 cameraNo);
/* Drop the debug camera (type 6) and restore the current room's floor
 * camera defaults. */
extern void cameraResetFloorDefault(void);

#pragma peephole off

/* Set the target used by the debug screen capture. */
void GSgfxCaptureSetTarget(void)
{
}

/* Drop the debug camera and restore the room floor camera defaults. */
s32 msgctrlSetPeopleTalkFlag(void)
{
    cameraResetFloorDefault();
    return 0;
}

/* Open the debug camera window when it is down, close it when it is up. */
s32 dbgMenuCameraChangeDisp(void)
{
    if (menuIsOpen(6) != 0) {
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
        cameraSetType(0);
        break;
    case 1:
        cameraSetType(5);
        break;
    case 2:
        cameraSetType(6);
        break;
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
