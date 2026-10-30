#include <floorManager.h>
#include <floorRoom.h>
#include <menu.h>
#include <types.h>

/* Field camera editor menu: opens the editor when the current room has its own
 * camera floor, and resets the editor cursor. */

/* Cursor of the field camera editor window; it survives reopenings and is
 * reset only when the editor switches to another floor's camera data. */
s32 gSelectedMenu_column;
s32 gSelected_column;
s32 gSelected_row;
extern u8 gSelectedInit;

#pragma force_active on

#pragma peephole off

s32 dbgMenuFieldCameraChangeDisp(void)
{
    u32 roomID;

    if (!menuIsCheck(MENU_ID_FIELD_CAMERA_EDITOR)) {
        roomID = GSfloorGetCurrent();
        if (roomID != floorFieldCameraEditorGetFloorID()) {
            floorInitFieldCameraEditor(roomID);
            gSelected_row = 0;
            gSelected_column = 0;
            gSelectedMenu_column = 0;
        }
        gSelectedInit = 0;
        menuOpenCustom(MENU_ID_FIELD_CAMERA_EDITOR, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_FIELD_CAMERA_EDITOR, 12, 10);
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
