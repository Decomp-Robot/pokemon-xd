#ifndef MENU_ITEM_BIOS_H
#define MENU_ITEM_BIOS_H

#include <types.h>

// One entry of a menu-item table, 0x1C bytes. The tables are strided 0x1C and
// are not the item table the item-data accessors read. Only the three row
// offsets at 0x08/0x0A/0x0C are named: the item cursor hook adds the first to
// its y position, the row printers place their values at the second, and the
// menus measure the first and the third for their columns. The rest stays
// padding.
typedef struct tagMENU_ITEM_DATA tagMENU_ITEM_DATA;
struct tagMENU_ITEM_DATA {
    u8 _00[0x08];  // 0x00-0x07: unknown.
    s16 mOffsetA;  // 0x08: row offset, a y offset for the item cursor.
    s16 mOffsetB;  // 0x0A: row offset, the x position of the row values.
    s16 mOffsetC;  // 0x0C: row offset measured for the menu columns.
    u8 _0E[0x0E];  // 0x0E-0x1B: remainder; unknown here.
};

#ifdef __cplusplus
extern "C" {
#endif

// Item data of `itemID` in the item table selected by the menu data
// entry `dataID`. Returns null outside the table.
tagMENU_ITEM_DATA *menuItemBiosGetPtr(long dataID, long itemID);

#ifdef __cplusplus
}
#endif

#endif
