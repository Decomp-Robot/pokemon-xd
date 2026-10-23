#include <types.h>
#include <floor.h>
#include <menuIds.h>
#include <floorRoom.h>
#include <floorFieldCamera.h>

/* Debug menus that open the party-info window and the people editor. */
/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on
u8 menuIsOpen(s32 menuID);
void menuClose(s32 menuID);
void menuOpenCustom(s32 menuID, s32 a, s32 b, s32 c, s32 d, s32 e, ...);
void menuSetPosition(s32 menuID, s32 x, s32 y);

#pragma peephole off


s32 dbgMenuDispChangePartyInfo(void)
{
    if (menuIsOpen(MENU_ID_PARTY_INFO)) {
        menuClose(MENU_ID_PARTY_INFO);
    } else {
        menuOpenCustom(MENU_ID_PARTY_INFO, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_PARTY_INFO, 0x190, 0x28);
    }
    return 0;
}

s32 dbgMenuPeopleOpenEdit(void)
{
    roomIDGetMapData(roomGetCurrentRoomID());

    if (menuIsOpen(MENU_ID_PEOPLE_EDITOR)) {
        if (floorFieldCameraDataExist()) {
            floorFieldCameraEnable();
        }
        menuClose(MENU_ID_PEOPLE_EDITOR);
        zz_011ddf8_();
    } else {
        zz_0123e50_();
        menuOpenCustom(MENU_ID_PEOPLE_EDITOR, 0, 0, 0, 0, 0);
        zz_011de1c_();
    }
    return 0;
}

#pragma peephole reset


#pragma force_active reset
