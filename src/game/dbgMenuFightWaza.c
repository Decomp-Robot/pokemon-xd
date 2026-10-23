#include <types.h>
#include <menuSub.h>

/* Waza (move) debug sub menu.  The waza data block is 0x38 bytes per waza id
 * (1..0x162) and is snapshotted while the menu is open. */
struct WazaData {
    u32 _00[0x38 / 4];
};

void menuCloseCustom(long menuID, long a, long b);
long menuOpenCustom(long menuID, long a, long b, long c, long d, long e, ...);
long menuOpen(long menuID, long a);
long menuGetCursorItemID(long menuID);
long wazaDataBiosGetPtr(unsigned long wazaNo);
long wazaGetStatus(long a, unsigned long wazaNo, unsigned short fieldID, long index);
void wazaSetStatus(long a, unsigned long wazaNo, unsigned short fieldID, long index, long value);
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
        result = menuOpenCustom(0x20, 0, 0, 0, 1, 1, (unsigned short)wazaNo);
        if (result == -1) {
            menuCloseCustom(0x20, 0, 1);
            *waza = work;
            return -1;
        }
        if (result == -2) {
            if (menuOpen(0x55, 1)) {
                menuCloseCustom(0x55, 0, 1);
                continue;
            }
            menuCloseCustom(0x55, 0, 1);
            break;
        }
        switch (menuGetCursorItemID(0x20)) {
        case 0x89:
            typeID = dbgMenuFightGetWazaTypeId((u8)wazaGetStatus(0, wazaNo, 0x1A, 0));
            if (typeID >= 0) {
                wazaSetStatus(0, wazaNo, 0x1A, 0, typeID);
            }
            break;
        case 0x8A:
            typeID = dbgMenuFightGetWazaTypeId((u8)wazaGetStatus(0, wazaNo, 0x1A, 1));
            if (typeID >= 0) {
                wazaSetStatus(0, wazaNo, 0x1A, 1, typeID);
            }
            break;
        case 0x8B:
            typeID = dbgMenuFightGetWazaTypeId((u8)wazaGetStatus(0, wazaNo, 0x1A, 2));
            if (typeID >= 0) {
                wazaSetStatus(0, wazaNo, 0x1A, 2, typeID);
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

    menuCloseCustom(0x20, 0, 1);
    return 1;
}

#pragma peephole reset
