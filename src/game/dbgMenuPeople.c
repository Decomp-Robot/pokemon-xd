#include <floorManager.h>
#include <menu.h>
#include <floor.h>
#include <types.h>
#include <floorFieldCamera.h>

#pragma force_active on

#pragma peephole off


s32 dbgMenuDispChangePartyInfo(void)
{
    if (menuIsCheck(MENU_ID_PARTY_INFO)) {
        menuClose(MENU_ID_PARTY_INFO);
    } else {
        menuOpenCustom(MENU_ID_PARTY_INFO, 0, 0, 0, 1, 0);
        menuSetPosition(MENU_ID_PARTY_INFO, 400, 40);
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
        menuClose(MENU_ID_PEOPLE_EDITOR);
        floorUnblockMainScript();
    } else {
        floorFieldCameraDisable();
        menuOpenCustom(MENU_ID_PEOPLE_EDITOR, 0, 0, 0, 0, 0);
        floorBlockMainScript();
    }
    return 0;
}

#pragma peephole reset


#pragma force_active reset
