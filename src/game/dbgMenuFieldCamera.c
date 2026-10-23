#include <floorRoom.h>
#include <types.h>
#include <menu.h>

/* Field camera editor menu: opens the editor when the current room has its own
 * camera floor, and resets the editor cursor. */

/* Field camera editor work, reset when the room changes. */
extern s32 dbgFieldCameraRoomStateWord1;
extern s32 dbgFieldCameraRoomStateWord2;
extern s32 dbgFieldCameraRoomStateWord3;
/* Reset before opening the field camera editor. */
extern u8 dbgFieldCameraOpenState;

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

void menuSetPosition(long menuID, long x, long y);

#pragma peephole off

s32 dbgMenuFieldCameraChangeDisp(void)
{
    u32 roomID;

    if (!menuIsOpen(MENU_ID_FIELD_CAMERA_EDITOR)) {
        roomID = roomGetCurrentRoomID();
        if (roomID != floorFieldCameraEditorGetFloorID()) {
            floorInitFieldCameraEditor(roomID);
            dbgFieldCameraRoomStateWord1 = 0;
            dbgFieldCameraRoomStateWord2 = 0;
            dbgFieldCameraRoomStateWord3 = 0;
        }
        dbgFieldCameraOpenState = 0;
        menuOpenCustom(MENU_ID_FIELD_CAMERA_EDITOR, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_FIELD_CAMERA_EDITOR, 0xC, 0xA);
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
