#include <types.h>

/* Field camera editor menu: opens the editor when the current room has its own
 * camera floor, and resets the editor cursor. */

/* Field camera editor work, reset when the room changes. */
extern s32 lbl_804EA698;
extern s32 lbl_804EA69C;
extern s32 lbl_804EA6A0;
/* Set while the field camera editor menu is being opened. */
extern u8 lbl_804EA6A4;

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

u8 zz_010ee28_(long menuID);
long menuOpenCustom(long menuID, long a, long b, long c, long d, long e, ...);
void menuSetPosition(long menuID, long x, long y);
u32 zz_get_current_room_ID(void);
u32 floorFieldCameraEditorGetFloorID(void);
void zz_0124aec_(u32 roomID);

#pragma peephole off

s32 dbgMenuFieldCameraChangeDisp(void)
{
    u32 roomID;

    if (!zz_010ee28_(0x23)) {
        roomID = zz_get_current_room_ID();
        if (roomID != floorFieldCameraEditorGetFloorID()) {
            zz_0124aec_(roomID);
            lbl_804EA698 = 0;
            lbl_804EA69C = 0;
            lbl_804EA6A0 = 0;
        }
        lbl_804EA6A4 = 0;
        menuOpenCustom(0x23, 0, 0, 0, 1, 0);
        menuSetPosition(0x23, 0xC, 0xA);
    }
    return 0;
}

#pragma peephole reset

#pragma force_active reset
