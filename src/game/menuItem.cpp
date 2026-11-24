#include <menuItemBios.h>
#include <menuItemMessages.h>

enum StandardMenuItemID {
    STANDARD_ITEM_ROOT,
    STANDARD_ITEM_ICON,
    STANDARD_ITEM_OPTION_0,
    STANDARD_ITEM_OPTION_1,
    STANDARD_ITEM_OPTION_2,
    STANDARD_ITEM_BACKDROP_A,
    STANDARD_ITEM_BACKDROP_B,
    STANDARD_ITEM_TITLE,
};

enum StandardSpriteID {
    STANDARD_SPRITE_BACKDROP_A = 57,
    STANDARD_SPRITE_BACKDROP_B = 58,
    STANDARD_SPRITE_ICON = 96,
    STANDARD_SPRITE_CURSOR = 190,
    STANDARD_SPRITE_TITLE_BACKDROP = 834,
};

extern "C" {

tagMENU_ITEM_DATA _menu_item[8] = {
    {
        1, 1, 0, 0, 0, 0, MENU_ITEM_ALPHA_OPAQUE, 0,
        0, 0,
        0, 0,
        0, 0,
        0,
        0,
        STANDARD_ITEM_ICON, 0,
    },
    {
        0, 0, 0, 0, 0, 0, MENU_ITEM_ALPHA_OPAQUE, 0,
        STANDARD_SPRITE_ICON, 0,
        0, 0,
        0, 0,
        0,
        0,
        STANDARD_ITEM_OPTION_0, 0,
    },
    {
        1, 0, 0, 0, 0, 0, MENU_ITEM_ALPHA_OPAQUE, 0,
        0, 0,
        0, 0,
        0, STANDARD_SPRITE_CURSOR,
        MSG_ITEM_MENU_OPTION_0,
        0,
        STANDARD_ITEM_OPTION_1, 0,
    },
    {
        1, 0, 0, 0, 0, 0, MENU_ITEM_ALPHA_OPAQUE, 0,
        0, 0,
        20, 0,
        0, STANDARD_SPRITE_CURSOR,
        MSG_ITEM_MENU_OPTION_1,
        0,
        STANDARD_ITEM_OPTION_2, 0,
    },
    {
        1, 1, 0, 0, 0, 0, MENU_ITEM_ALPHA_OPAQUE, 0,
        0, 0,
        40, 0,
        0, STANDARD_SPRITE_CURSOR,
        MSG_ITEM_MENU_OPTION_2,
        0,
        STANDARD_ITEM_BACKDROP_A, 0,
    },
    {
        1, 1, 0, 0, 0, 0, MENU_ITEM_ALPHA_OPAQUE, 0,
        STANDARD_SPRITE_BACKDROP_A, 0,
        0, 640,
        480, 0,
        0,
        0,
        STANDARD_ITEM_BACKDROP_B, 0,
    },
    {
        1, 1, 0, 0, 0, 0, MENU_ITEM_ALPHA_OPAQUE, 0,
        STANDARD_SPRITE_BACKDROP_B, 0,
        0, 640,
        480, 0,
        0,
        0,
        STANDARD_ITEM_TITLE, 0,
    },
    {
        0, 1, 0, 0, 0, 0, MENU_ITEM_ALPHA_OPAQUE, 0,
        STANDARD_SPRITE_TITLE_BACKDROP, 0,
        0, 640,
        480, 0,
        0,
        0,
        MENU_ITEM_CHAIN_NONE, 0,
    },
};

}
