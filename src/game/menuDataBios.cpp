#include <menu.h>
#include <menuDataBios.h>
#include <menuItemBios.h>
#include <menuSpriteBios.h>
#include <pokecolo.h>
#include <dbgMenu.h>
#include <GSgfxTypes.h>

// mItemTable selects which of the item tables linked into the menu system
// holds the rows of a menu.
enum {
    MENU_ITEM_TABLE_ITEM = 0,
    MENU_ITEM_TABLE_MX = 1,
    MENU_ITEM_TABLE_TOOL = 2,
    MENU_ITEM_TABLE_DEBUG = 3,
    MENU_ITEM_TABLE_PDA = 4,
    MENU_ITEM_TABLE_BT = 5,
    MENU_ITEM_TABLE_XD = 6,
};

// Records menuDataBiosGetPtr falls back to when a caller names a menu that is
// out of range: the empty menu, and the debug menu.
enum {
    MENU_ID_EMPTY_MENU = 0,
    MENU_ID_DEBUG_MENU = 1,
};

extern "C" {
extern u32 _menu_sprite_number;
extern u32 _menu_data_number;
extern MenuSpriteData _menu_sprite[];
extern MenuData _menu_data[];
extern tagMENU_ITEM_DATA _menu_item[];
extern tagMENU_ITEM_DATA _menu_itemmx[];
extern tagMENU_ITEM_DATA _menu_item_tool[];
extern tagMENU_ITEM_DATA _menu_item_debug[];
extern tagMENU_ITEM_DATA _menu_item_pda[];
extern tagMENU_ITEM_DATA _menu_itembt[];
extern tagMENU_ITEM_DATA _menu_item_xd[];
}

// The game's text language, as returned by pokecoloGetLanguage. Sprite value
// blocks are stored per language in this order; every other language keeps
// the English block.
enum {
    LANGUAGE_ENGLISH = 2,
    LANGUAGE_GERMAN = 3,
    LANGUAGE_FRENCH = 4,
    LANGUAGE_ITALIAN = 5,
    LANGUAGE_SPANISH = 6,
};

extern "C" MenuSpriteValue *menuSpriteBiosGetValPtr(MenuSpriteData *data) {
    s32 valueIndex;
    switch (pokecoloGetLanguage()) {
    case LANGUAGE_ENGLISH: valueIndex = 0; break;
    case LANGUAGE_GERMAN: valueIndex = 1; break;
    case LANGUAGE_FRENCH: valueIndex = 2; break;
    case LANGUAGE_ITALIAN: valueIndex = 3; break;
    case LANGUAGE_SPANISH: valueIndex = 4; break;
    default: valueIndex = 0; break;
    }
    return &data->mValue[valueIndex];
}

extern "C" MenuSpriteData *menuSpriteBiosGetPtr(u32 spriteID) {
    if (spriteID >= _menu_sprite_number) {
        return 0;
    }
    return &_menu_sprite[spriteID];
}

extern "C" void menuItemBiosSetXY(s32 dataID, s32 itemID, s16 x, s16 y) {
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(dataID, itemID);
    if (item != 0) {
        item->mX = x;
        item->mY = y;
    }
}

extern "C" u8 menuItemBiosGetSelectFlag(s32 dataID, s32 itemID) {
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(dataID, itemID);
    if (item != 0) return item->mShownFlag != 0;
    return 0;
}

extern "C" void menuItemBiosSetSelectFlag(s32 dataID, s32 itemID, bool flag) {
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(dataID, itemID);
    if (item != 0) {
        item->mShownFlag = flag;
    }
}

extern "C" GSgfxLayerID menuItemBiosGetLayerID(s32 dataID) {
    MenuData *data = menuDataBiosGetPtr(dataID);
    if (data->mItemTable == MENU_ITEM_TABLE_DEBUG) {
        return dbgMenuGetLayerID();
    }
    return GSGFX_LAYER_MENU;
}

// The rows of a menu come from the item table its menu data selects.
extern "C" tagMENU_ITEM_DATA *menuItemBiosGetPtr(s32 dataID, s32 itemID) {
    MenuData *data = menuDataBiosGetPtr(dataID);
    if (data->mItemTable == MENU_ITEM_TABLE_ITEM) return &_menu_item[itemID];
    else if (data->mItemTable == MENU_ITEM_TABLE_MX) return &_menu_itemmx[itemID];
    else if (data->mItemTable == MENU_ITEM_TABLE_TOOL) return &_menu_item_tool[itemID];
    else if (data->mItemTable == MENU_ITEM_TABLE_DEBUG) return &_menu_item_debug[itemID];
    else if (data->mItemTable == MENU_ITEM_TABLE_PDA) return &_menu_item_pda[itemID];
    else if (data->mItemTable == MENU_ITEM_TABLE_BT) return &_menu_itembt[itemID];
    else if (data->mItemTable == MENU_ITEM_TABLE_XD) return &_menu_item_xd[itemID];
    return 0;
}

extern "C" void menuDataBiosGetXY(s32 menuID, s16 *x, s16 *y) {
    MenuData *data = menuDataBiosGetPtr(menuID);
    if (x != 0) *x = data->mPositionX;
    if (y != 0) *y = data->mPositionY;
}

extern "C" void menuDataBiosSetXY(s32 menuID, s16 x, s16 y) {
    MenuData *data = menuDataBiosGetPtr(menuID);
    data->mPositionX = x;
    data->mPositionY = y;
}

extern "C" u8 menuDataBiosGetDispMax(s32 menuID) {
    return menuDataBiosGetPtr(menuID)->mDisplayMaximum;
}

extern "C" MenuData *menuDataBiosGetPtr(s32 menuID) {
    if (menuID < 0) menuID = MENU_ID_EMPTY_MENU;
    if ((u32)menuID >= _menu_data_number) menuID = MENU_ID_DEBUG_MENU;
    return &_menu_data[menuID];
}
