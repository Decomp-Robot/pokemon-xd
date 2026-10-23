#ifndef MENU_ITEM_BIOS_H
#define MENU_ITEM_BIOS_H

#include <types.h>

// Entries of a menu-item table; chained through mNextID.
typedef struct MENU_ITEM_dd_ tagMENU_ITEM_DATA;
struct MENU_ITEM_dd_ {
    u8 mShownFlag : 1;    // byte 0 bit 7: the row is shown in the menu.
    u8 mChainEndFlag : 1; // byte 0 bit 6: last row of the item chain.
    u8 mFlags : 6;        // byte 0 bits 0-5: unknown.
    u8 _01[3];
    s16 mSpriteID;
    s16 mMessageX; // 0x06: x origin the row's centered message is placed from.
    s16 mOffsetA;  // 0x08: row offset, a y offset for the item cursor.
    s16 mWidth;   // 0x0A: row width.
    s16 mHeight;  // 0x0C: row height.
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
u8 menuItemBiosGetSelectFlag(s32 dataID, s32 itemID);
#ifdef __cplusplus
void menuItemBiosSetSelectFlag(s32 dataID, s32 itemID, bool flag);
#else
void menuItemBiosSetSelectFlag(s32 dataID, s32 itemID, u8 flag);
#endif
void menuItemSetPosition(s32 menuId, s16 itemId, s16 x, s16 y);

#ifdef __cplusplus
}
#endif

#endif
