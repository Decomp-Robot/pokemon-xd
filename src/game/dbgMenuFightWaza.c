#include <waza.h>
#include <menu.h>
#include <types.h>
#include <menuSub.h>


long dbgMenuFightGetWazaTypeId(u8 typeID);

#pragma peephole off

s32 dbgMenuFightWazaEditSub(u16 wazaNo)
{
    struct WazaData *waza;
    struct WazaData work;
    long typeID;
    long result;

    if (wazaNo == 0) {
        return 1;
    }
    if (wazaNo >= 0x163) {
        return 1;
    }

    waza = wazaDataBiosGetPtr(wazaNo);
    work = *waza;

    while (1) {
        result = menuOpenCustom(MENU_ID_DEBUG_MOVE_EDITOR, 0, 0, 0, 1, 1, wazaNo);
        if (result == -1) {
            menuCloseCustom(MENU_ID_DEBUG_MOVE_EDITOR, 0, 1);
            *waza = work;
            return -1;
        }
        if (result == -2) {
            if (menuOpen(MENU_ID_DEBUG_EDIT_CONFIRMATION, 1)) {
                menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
                continue;
            }
            menuCloseCustom(MENU_ID_DEBUG_EDIT_CONFIRMATION, 0, 1);
            break;
        }
        switch (menuGetCursorItemID(MENU_ID_DEBUG_MOVE_EDITOR)) {
        case MENU_ITEM_DEBUG_MOVE_TYPE_FIRST:
            typeID = dbgMenuFightGetWazaTypeId((u8)wazaGetStatus(0, wazaNo, WAZA_STATUS_TYPE, 0));
            if (typeID >= 0) {
                wazaSetStatus(0, wazaNo, WAZA_STATUS_TYPE, 0, typeID);
            }
            break;
        case MENU_ITEM_DEBUG_MOVE_TYPE_SECOND:
            typeID = dbgMenuFightGetWazaTypeId((u8)wazaGetStatus(0, wazaNo, WAZA_STATUS_TYPE, 1));
            if (typeID >= 0) {
                wazaSetStatus(0, wazaNo, WAZA_STATUS_TYPE, 1, typeID);
            }
            break;
        case MENU_ITEM_DEBUG_MOVE_TYPE_THIRD:
            typeID = dbgMenuFightGetWazaTypeId((u8)wazaGetStatus(0, wazaNo, WAZA_STATUS_TYPE, 2));
            if (typeID >= 0) {
                wazaSetStatus(0, wazaNo, WAZA_STATUS_TYPE, 2, typeID);
            }
            break;
        case MENU_ITEM_DEBUG_MOVE_RISK_FLAG:
            result = menuSubOpenYesNo(MENU_SUB_YES_NO_DEBUG_DIALOG, -1, -1, wazaGetStatus(0, wazaNo, WAZA_STATUS_RISK_FLAG, 0) == 0);
            if (result == 0) {
                wazaSetStatus(0, wazaNo, WAZA_STATUS_RISK_FLAG, 0, 1);
                break;
            }
            if (result == 1) {
                wazaSetStatus(0, wazaNo, WAZA_STATUS_RISK_FLAG, 0, 0);
            }
            break;
        }
    }

    menuCloseCustom(MENU_ID_DEBUG_MOVE_EDITOR, 0, 1);
    return 1;
}

#pragma peephole reset
