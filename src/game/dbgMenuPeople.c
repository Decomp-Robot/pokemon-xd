#include <types.h>

/* Debug menus that open the party-info window and the people editor. */
/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on
u8 zz_010ee28_(s32 menuID);
void menuClose(s32 menuID);
void menuOpenCustom(s32 menuID, s32 a, s32 b, s32 c, s32 d, s32 e, ...);
void menuSetPosition(s32 menuID, s32 x, s32 y);
s32 zz_get_current_room_ID(void);
void zz_roomID_get_map_data(s32 roomID);
u8 floorFieldCameraDataExist(void);
void floorFieldCameraEnable(void);
void zz_011ddf8_(void);
void zz_011de1c_(void);
void zz_0123e50_(void);

#pragma peephole off


s32 dbgMenuDispChangePartyInfo(void)
{
    if (zz_010ee28_(0xC)) {
        menuClose(0xC);
    } else {
        menuOpenCustom(0xC, 0, 0, 0, 1, 0);
        menuSetPosition(0xC, 0x190, 0x28);
    }
    return 0;
}

s32 dbgMenuPeopleOpenEdit(void)
{
    zz_roomID_get_map_data(zz_get_current_room_ID());

    if (zz_010ee28_(0xA)) {
        if (floorFieldCameraDataExist()) {
            floorFieldCameraEnable();
        }
        menuClose(0xA);
        zz_011ddf8_();
    } else {
        zz_0123e50_();
        menuOpenCustom(0xA, 0, 0, 0, 0, 0);
        zz_011de1c_();
    }
    return 0;
}

#pragma peephole reset


#pragma force_active reset
