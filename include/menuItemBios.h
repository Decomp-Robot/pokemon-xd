#ifndef MENU_ITEM_BIOS_H
#define MENU_ITEM_BIOS_H

#include <types.h>

typedef struct MENU_ITEM_dd_ MENU_ITEM_dd_;
typedef struct MENU_ITEM_dd_ tagMENU_ITEM_DATA;

// Menu item geometry, sprite, and the link to the next item.
struct MENU_ITEM_dd_ {
    u8 mShownFlag : 1;
    u8 mChainEndFlag : 1;
    u8 mFlags : 6;
    u8 mItemConfiguration[3];
    s16 mSpriteID;
    union { s16 mX; s16 mMessageX; };
    union { s16 mY; s16 mOffsetA; };
    union { s16 mWidth; s16 mOffsetB; };
    union { s16 mHeight; s16 mOffsetC; };
    u8 mItemCallbacks[0x0a];
    s16 mNextID;
    u8 mItemLinkState[2];
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
void menuItemSetPosition(s32 menuId, s16 itemId, s16 x, s16 y);

#ifdef __cplusplus
}
#endif

#endif
