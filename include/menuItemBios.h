#ifndef MENU_ITEM_BIOS_H
#define MENU_ITEM_BIOS_H

#include <types.h>
#include <bool.h>
#include <GSgfxTypes.h>

struct tagWINDOW_WORK;
struct tagSPRITE_WORK;

// Display hook a menu item hands to its sprite.
typedef void (*MENU_ITEM_HOOK)(struct tagWINDOW_WORK *, struct tagSPRITE_WORK *);

// Entries of a menu-item table; chained through mNextID.
typedef struct MENU_ITEM_dd_ tagMENU_ITEM_DATA;
struct MENU_ITEM_dd_ {
    u8 mShownFlag : 1;    // The row is shown in the menu.
    u8 mChainEndFlag : 1; // Last row of the item chain.
    u8 mAlphaMode : 2;
    u8 mUseFloorMenuLayer : 4;
    u8 mAlignMode : 2;
    u8 _01 : 6;
    u8 mAlpha;
    u8 _03;
    s16 mSpriteID;
    s16 mX;
    s16 mY;
    s16 mWidth;
    s16 mHeight;
    s16 mCursorSpriteID;
    u32 mMessageID;
    MENU_ITEM_HOOK mHook;
    s16 mNextID; // Next row of the item chain.
    s16 _1A;
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

// Layer the menu data entry `dataID` draws its windows on.
GSgfxLayerID menuItemBiosGetLayerID(s32 dataID);

extern tagMENU_ITEM_DATA _menu_item_tool[1682];

#ifdef __cplusplus
}
#endif

#endif
