#include "GSgfx.h"
#include "winSprite.h"
#include "window.h"
#include "GSmsg.h"
#include "msgctrl.h"
#include "menuIds.h"
#include "menuSub.h"
#include "waza.h"

// Menu data id of the move list this window draws.
enum { MENU_ID_MEW_WAZA = 268 };

// Message value slot the move name is written to before the window is printed.
enum { MSG_VALUE_MOVE_NAME = 0x4D };

// Item ids of the move rows, in the order the list shows the four generated moves.
enum {
    MENU_ITEM_MOVE_NAME_0 = 0x2C,
    MENU_ITEM_MOVE_NAME_1 = 0x2D,
    MENU_ITEM_MOVE_NAME_2 = 0x2E,
    MENU_ITEM_MOVE_NAME_3 = 0x2F,
    MENU_ITEM_MOVE_LIST = 0x2B,
};

// Window texts of the move rows and of the move list body.
enum {
    MSG_MOVE_NAME_0 = 0xC72D,
    MSG_MOVE_NAME_1 = 0xC72E,
    MSG_MOVE_NAME_2 = 0xC72F,
    MSG_MOVE_NAME_3 = 0xC730,
    MSG_MOVE_LIST = 0xC722,
};

extern "C" u16 mewWazaGet(long index);

u32 _GetMewWazaName(long index);

extern "C" long menuMewWazaGetMenuDataID(void)
{
    return MENU_ID_MEW_WAZA;
}

extern "C" long menuMewWazaDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    WINDOW_FREE_WORK *work = windowGetFreeWork(window);
    GSgfxLayerID layer = winSpriteGetLayerID(window, sprite);

    GSgfxLayerFindByID(layer);

    s32 height = sprite->mHeight;
    s16 width = sprite->mWidth;
    s16 dy = (s16)((height - 4) - (s16)height);
    spriteSetOffset(layer, (s16)(window->mScrollX + sprite->mX),
                    (s16)(window->mScrollY + sprite->mY + dy));

    spriteSetEnv(layer);

    switch (sprite->mItemID) {
    case MENU_ITEM_MOVE_NAME_0: {
        u32 name = (u32)GSmsgGetStringPtr(_GetMewWazaName(0));
        msgctrlSetValue(MSG_VALUE_MOVE_NAME, name);
        GSmsgPrint2(layer, 0, 0, window->mRgba.mColorA | 0xFFFFFF00,
                    MSG_MOVE_NAME_0);
        break;
    }
    case MENU_ITEM_MOVE_NAME_1:
        msgctrlSetValue(MSG_VALUE_MOVE_NAME,
                        (u32)GSmsgGetStringPtr(_GetMewWazaName(1)));
        GSmsgPrint2(layer, 0, 0, window->mRgba.mColorA | 0xFFFFFF00,
                    MSG_MOVE_NAME_1);
        break;
    case MENU_ITEM_MOVE_NAME_2:
        msgctrlSetValue(MSG_VALUE_MOVE_NAME,
                        (u32)GSmsgGetStringPtr(_GetMewWazaName(2)));
        GSmsgPrint2(layer, 0, 0, window->mRgba.mColorA | 0xFFFFFF00,
                    MSG_MOVE_NAME_2);
        break;
    case MENU_ITEM_MOVE_NAME_3:
        msgctrlSetValue(MSG_VALUE_MOVE_NAME,
                        (u32)GSmsgGetStringPtr(_GetMewWazaName(3)));
        GSmsgPrint2(layer, 0, 0, window->mRgba.mColorA | 0xFFFFFF00,
                    MSG_MOVE_NAME_3);
        break;
    case MENU_ITEM_MOVE_LIST:
        menuSubDrawMsgWindow2(layer, 0, -2, width, height,
                              window->mRgba.mColorA, 1);
        GSmsgPrint2(layer, 0, 0, window->mRgba.mColorA | 0xFFFFFF00,
                    MSG_MOVE_LIST);
        // mState holds the menu sequence step of this window. Steps 0, 1, 3 and 5
        // only print the value list; the active step also keeps the text printer
        // running, so the list keeps scrolling while the window is open.
        switch (window->mState) {
        case WINDOW_STATE_INIT:
            break;
        case 1:
            break;
        case WINDOW_STATE_CLOSING:
            break;
        case 5:
            break;
        case WINDOW_STATE_ACTIVE:
            if (window->mMenuID != MENU_ID_LEVEL_UP_STATUS) {
                GSmsgMain(layer, work->mMessageID, work->mKeyTrigger,
                          !(window->mStateFlags & WINDOW_RUN_UPDATE));
                work->mKeyTrigger = 0;
            }
            break;
        }
        break;
    }
    return 0;
}

u32 _GetMewWazaName(long index)
{
    struct WazaData *data = wazaDataBiosGetPtr(mewWazaGet(index));
    return wazaDataBiosGetName(data);
}
