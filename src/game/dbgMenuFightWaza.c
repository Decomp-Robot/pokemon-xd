#include <menu.h>
#include <types.h>
#include <menuSub.h>
#include <waza.h>

/* Waza (move) debug sub menu.  The waza data block is 0x38 bytes per waza id
 * (1..0x162) and is snapshotted while the menu is open. */
struct WazaData {
    u32 _00[0x38 / 4];
};

long dbgMenuFightGetWazaTypeId(u8 typeID);

#pragma peephole off

s32 dbgMenuFightWazaEditSub(unsigned long wazaNo)
{
    struct WazaData *waza;
    struct WazaData work;
    long typeID;
    long result;

    if ((unsigned short)wazaNo == 0) {
        return 1;
    }
    if ((unsigned short)wazaNo >= 0x163) {
        return 1;
    }

    waza = (struct WazaData *)wazaDataBiosGetPtr(wazaNo);
    work = *waza;

    while (1) {
        result = menuOpenCustom(MENU_ID_DEBUG_MOVE_EDITOR, 0, 0, 0, 1, 1, (unsigned short)wazaNo);
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
        case 0x8C:
            typeID = menuSubOpenYesNo(0x7F, -1, -1, wazaGetStatus(0, wazaNo, 0x1B, 0) == 0);
            if (typeID == 0) {
                wazaSetStatus(0, wazaNo, 0x1B, 0, 1);
                break;
            }
            if (typeID == 1) {
                wazaSetStatus(0, wazaNo, 0x1B, 0, 0);
            }
            break;
        }
    }

    menuCloseCustom(MENU_ID_DEBUG_MOVE_EDITOR, 0, 1);
    return 1;
}

#pragma peephole reset
