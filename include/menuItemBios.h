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
    u8 mShownFlag : 1;    // byte 0 bit 7: the row is shown in the menu.
    u8 mChainEndFlag : 1; // byte 0 bit 6: last row of the item chain.
    u8 mFlags : 6;        // byte 0 bits 0-5: unknown.
    u8 _01[0x07];         // 0x01-0x07: unknown.
    s16 mOffsetA;  // 0x08: row offset, a y offset for the item cursor.
    s16 mOffsetB;  // 0x0A: row offset, the x position of the row values.
    s16 mOffsetC;  // 0x0C: row offset measured for the menu columns.
    u8 _0E[0x0A];  // 0x0E-0x17: unknown here.
    s16 mNextID;   // 0x18: next row of the chain, read while bit6 is clear.
    u8 _1A[0x02];  // 0x1A-0x1B: unknown here.
};

#ifdef __cplusplus
extern "C" {
#endif

// Item data of `itemID` in the item table selected by the menu data
// entry `dataID`. Returns null outside the table. Records are 0x1C bytes and
// chain through mNextID until mChainEndFlag.
tagMENU_ITEM_DATA *menuItemBiosGetPtr(long dataID, long itemID);
// Select state of `itemID` the PDA frame menus toggle.
void menuItemBiosSetSelectFlag(s32 dataID, s32 itemID, s32 flag);

#ifdef __cplusplus
}
#endif

#endif
