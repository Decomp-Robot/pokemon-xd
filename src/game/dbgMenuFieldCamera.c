#include <floorManager.h>
#include <menu.h>
#include <types.h>
#include <menu.h>

/* Field camera editor menu: opens the editor when the current room has its own
 * camera floor, and resets the editor cursor. */

/* Cursor of the editor window, reset when the room changes, and the flag the
 * editor initialises itself from. */
extern s32 gSelected_row;
extern s32 gSelected_column;
extern s32 gSelectedMenu_column;
extern u8 gSelectedInit;

#pragma force_active on

u8 menuIsCheck(s32 menuID);
void menuSetPosition(long menuID, long x, long y);
u32 floorFieldCameraEditorGetFloorID(void);
void floorInitFieldCameraEditor(u32 roomID);

#pragma peephole off

s32 dbgMenuFieldCameraChangeDisp(void)
{
    u32 roomID;

    if (!menuIsCheck(0x23)) {
        roomID = GSfloorGetCurrent();
        if (roomID != floorFieldCameraEditorGetFloorID()) {
            floorInitFieldCameraEditor(roomID);
            gSelected_row = 0;
            gSelected_column = 0;
            gSelectedMenu_column = 0;
        }
        gSelectedInit = 0;
        menuOpenCustom(0x23, 0, 0, 0, 1, 0);
        menuSetPosition(0x23, 0xC, 0xA);
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
