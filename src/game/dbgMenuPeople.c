#include <floorManager.h>
#include <menu.h>
#include <floor.h>
#include <types.h>
#include <floor.h>
#include <menu.h>
#include <floorRoom.h>
#include <floorFieldCamera.h>

/* Debug menus that open the party-info window and the people editor. */
/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on
u8 menuIsCheck(s32 menuID);
void menuSetPosition(s32 menuID, s32 x, s32 y);
u8 floorFieldCameraDataExist(void);
void floorFieldCameraEnable(void);
void floorUnblockMainScript(void);
void floorBlockMainScript(void);
void floorFieldCameraDisable(void);

#pragma peephole off


s32 dbgMenuDispChangePartyInfo(void)
{
    if (menuIsCheck(MENU_ID_PARTY_INFO)) {
        menuClose(MENU_ID_PARTY_INFO);
    } else {
        menuOpenCustom(MENU_ID_PARTY_INFO, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_PARTY_INFO, 0x190, 0x28);
    }
    return 0;
}

s32 dbgMenuPeopleOpenEdit(void)
{
    floorDataBiosGetPtr(GSfloorGetCurrent());

    if (menuIsCheck(MENU_ID_PEOPLE_EDITOR)) {
        if (floorFieldCameraDataExist()) {
            floorFieldCameraEnable();
        }
        menuClose(0xA);
        floorUnblockMainScript();
    } else {
        floorFieldCameraDisable();
        menuOpenCustom(0xA, 0, 0, 0, 0, 0);
        floorBlockMainScript();
    }
    return 0;
}

#pragma peephole reset


#pragma force_active reset
