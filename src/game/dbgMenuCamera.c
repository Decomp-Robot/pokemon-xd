#include <camera.h>
#include <menu.h>
#include <types.h>

#pragma force_active on

extern u8 menuIsOpen(s32 menuId);
extern void menuSetPosition(s32 menuId, s32 x, s32 y);
/* Drop the debug camera (type 6) and restore the current room's floor
 * camera defaults. */
extern void cameraResetFloorDefault(void);

#pragma peephole off

void zz_800056a0_(void)
{
}

/* Drop the debug camera and restore the room floor camera defaults. */
s32 dbgMenuCameraResetFloor(void)
{
    cameraResetFloorDefault();
    return 0;
}

/* Open the debug camera window when it is down, close it when it is up. */
s32 dbgMenuCameraChangeDisp(void)
{
    if (menuIsOpen(MENU_ID_DEBUG_CAMERA) != 0) {
        menuClose(MENU_ID_DEBUG_CAMERA);
    } else {
        menuOpenCustom(MENU_ID_DEBUG_CAMERA, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_DEBUG_CAMERA, 0x14, 0x104);
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
        cameraSetType(CAMERA_TYPE_DEFAULT);
        break;
    case 1:
        cameraSetType(CAMERA_TYPE_DEBUG_PRIMARY);
        break;
    case 2:
        cameraSetType(CAMERA_TYPE_DEBUG_SECONDARY);
        break;
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
