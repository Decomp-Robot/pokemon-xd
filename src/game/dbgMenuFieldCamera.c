#include <floorRoom.h>
#include <types.h>
#include <menuIds.h>

/* Field camera editor menu: opens the editor when the current room has its own
 * camera floor, and resets the editor cursor. */

/* Field camera editor work, reset when the room changes. */
extern s32 lbl_804EA698;
extern s32 lbl_804EA69C;
extern s32 lbl_804EA6A0;
/* Reset before opening the field camera editor. */
extern u8 lbl_804EA6A4;

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

u8 menuIsOpen(s32 menuID);
long menuOpenCustom(long menuID, long a, long b, long c, long d, long e, ...);
void menuSetPosition(long menuID, long x, long y);

#pragma peephole off

s32 dbgMenuFieldCameraChangeDisp(void)
{
    u32 roomID;

    if (!menuIsOpen(MENU_ID_FIELD_CAMERA_EDITOR)) {
        roomID = roomGetCurrentRoomID();
        if (roomID != floorFieldCameraEditorGetFloorID()) {
            floorInitFieldCameraEditor(roomID);
            lbl_804EA698 = 0;
            lbl_804EA69C = 0;
            lbl_804EA6A0 = 0;
        }
        lbl_804EA6A4 = 0;
        menuOpenCustom(MENU_ID_FIELD_CAMERA_EDITOR, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_FIELD_CAMERA_EDITOR, 0xC, 0xA);
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
