#ifndef MENU_ITEM_BIOS_H
#define MENU_ITEM_BIOS_H

#include <types.h>
#include <bool.h>

// Entries of a menu-item table; chained through mNextID.
typedef struct MENU_ITEM_dd_ tagMENU_ITEM_DATA;
struct MENU_ITEM_dd_ {
    u8 mShownFlag : 1;    // The row is shown in the menu.
    u8 mChainEndFlag : 1; // Last row of the item chain.
    u8 mFlags : 6;
    u8 _01[3];
    s16 mSpriteID;
    s16 mX;
    s16 mY;
    s16 mWidth;
    s16 mHeight;
    u8 _0E[10];
    s16 mNextID; // Next row of the item chain.
    u8 _1A[2];
};

#ifdef __cplusplus
extern "C" {
#endif

// Item data of `itemID` in the item table selected by the menu data
// entry `dataID`. Menu data entries with no item table return null.
// Records chain through mNextID until mChainEndFlag.
tagMENU_ITEM_DATA *menuItemBiosGetPtr(s32 dataID, s32 itemID);
// Select state of `itemID` the PDA frame menus toggle.
u8 menuItemBiosGetSelectFlag(s32 dataID, s32 itemID);
void menuItemBiosSetSelectFlag(s32 dataID, s32 itemID, bool flag);
void menuItemBiosSetXY(s32 dataID, s32 itemID, s16 x, s16 y);

#ifdef __cplusplus
}
#endif

#endif
