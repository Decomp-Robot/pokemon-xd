#include <types.h>
#include <item.h>
#include <itemBios.h>
#include <itemData.h>
#include <heroItem.h>
#include <GSmsg.h>
#include <window.h>
#include <winSprite.h>
#include <menuItemBios.h>
#include <msgctrl.h>
#include <menuSub.h>
#include <menu.h>
#include <menuIds.h>

enum PocketCologneMenuID {
    POCKET_COLOGNE_ITEM_TITLE = 295,
    POCKET_COLOGNE_CURSOR_CANCEL = 297,
    POCKET_COLOGNE_CURSOR_VIVID = 298,
    POCKET_COLOGNE_CURSOR_EXCITE = 299,
    POCKET_COLOGNE_CURSOR_JOY = 300,
    POCKET_COLOGNE_ITEM_CANCEL = 301,
    POCKET_COLOGNE_ITEM_VIVID = 302,
    POCKET_COLOGNE_ITEM_EXCITE = 303,
    POCKET_COLOGNE_ITEM_JOY = 304
};

enum PocketCologneMessageID {
    POCKET_COLOGNE_MSG_CANCEL = 17045,
    POCKET_COLOGNE_MSG_QUANTITY_MARKER = 17039,
    POCKET_COLOGNE_MSG_NUMBER = 17007
};

enum { POCKET_COLOGNE_ROW_COUNT = 4 };

enum PocketCologneWindowSequence {
    POCKET_COLOGNE_SEQUENCE_OPEN = 96,
    POCKET_COLOGNE_SEQUENCE_CLOSE = 100
};

f32 ActMenuCursorBlinkTime;

struct PocketCologneItem {
    u16 mItemID;
    u16 mUnknown;
};

struct POCKET_COLOGNE_ARG {
    u32 mUnknown00;
    PocketCologneItem mItems[POCKET_COLOGNE_ROW_COUNT];
    s32 mItemCount;
};

int getItemNum(u16 itemID);
int getStockMax(u16 itemID);
int getItemName(u32 itemID);
void makePocketCologneArg(POCKET_COLOGNE_ARG *arg, u32 initialSelection);
extern "C" s32 menuPocketColognDrawLineCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
s32 getLineHeight(tagWINDOW_WORK *window);
extern "C" s32 menuPocketColognDrawCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
extern "C" s32 menuPocketColognDrawItemText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
extern "C" s32 menuPocketColognCursor(tagWINDOW_WORK *window);

// Opens the cologne picker and returns the item the player chose, or zero for
// cancel, an invalid row, or an item that is not in stock.
extern "C" u16 menuPocketCologne(int initialSelection) {
    static int first = 1;
    if (first) {
        first = 0;
    }

    POCKET_COLOGNE_ARG arg;
    makePocketCologneArg(&arg, initialSelection);

    s32 selected = menuOpenCustom(MENU_ID_POCKET_COLOGNE, windowGetActiveID(),
                                  0, 0, true, 1, &arg);
    menuClose(MENU_ID_POCKET_COLOGNE);
    menuCloseSync(MENU_ID_POCKET_COLOGNE, true);

    if (selected >= 0 && selected < arg.mItemCount) {
        u16 itemID = arg.mItems[selected].mItemID;
        if (getItemNum(itemID) != 0) {
            return itemID;
        }
        return 0;
    }
    return 0;
}

void makePocketCologneArg(POCKET_COLOGNE_ARG *arg, u32 initialSelection) {
    arg->mUnknown00 = initialSelection;
    arg->mItems[0].mItemID = ITEM_JOY_SCENT;
    arg->mItems[1].mItemID = ITEM_EXCITE_SCENT;
    arg->mItems[2].mItemID = ITEM_VIVID_SCENT;
    arg->mItems[3].mItemID = 0;
    arg->mItemCount = POCKET_COLOGNE_ROW_COUNT;
}

// Window sequence callback: starts and advances the cursor animation while the
// cologne menu is open, and plays the close sequence once on the way out.
extern "C" s32 menuPocketColognCtrl(tagWINDOW_WORK *window) {
    switch (window->mState) {
    case WINDOW_SEQUENCE_OPEN:
        if (window->mFlag == 0) {
            winSeqSetMenu(MENU_ID_POCKET_COLOGNE, POCKET_COLOGNE_SEQUENCE_OPEN);
            ActMenuCursorBlinkTime = 0.0f;
            window->mFlag = 1;
            menuSubCursorAnimeInit();
        }
        break;
    case WINDOW_STATE_ACTIVE:
        ActMenuCursorBlinkTime += 1.0f / 30.0f;
        if (ActMenuCursorBlinkTime > 1.0f) {
            ActMenuCursorBlinkTime = 0.0f;
        }
        menuSubCursorAnimeMain();
        break;
    case WINDOW_SEQUENCE_CLOSE:
        if (window->mFlag == 0) {
            winSeqSetMenu(MENU_ID_POCKET_COLOGNE, POCKET_COLOGNE_SEQUENCE_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

extern "C" s32 menuPocketColognCursor(tagWINDOW_WORK *window) {
    POCKET_COLOGNE_ARG *arg = (POCKET_COLOGNE_ARG *)window->mArg;
    s16 oldCursor;
    u16 keys = windowGetKeyRepeat();
    s32 count = arg->mItemCount;

    oldCursor = window->mCursor.y;
    if (keys & WINDOW_KEY_DOWN) {
        s32 next = oldCursor + 1;
        if (next >= count) {
            next = count - 1;
        }
        window->mCursor.y = (s8)next;
    }
    if (keys & WINDOW_KEY_UP) {
        s32 next = window->mCursor.y - 1;
        if (next < 0) {
            next = 0;
        }
        window->mCursor.y = (s8)next;
    }
    if (oldCursor != window->mCursor.y) {
        menuSubCursorAnimeStart();
    }
    return 0;
}

inline s16 getMessageWidth(s32 message) {
    return GSmsgGetMessageSize(message) >> 16;
}
extern "C" s32 menuPocketColognDrawItemText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    s32 rowItems[POCKET_COLOGNE_ROW_COUNT] = {
        POCKET_COLOGNE_ITEM_JOY, POCKET_COLOGNE_ITEM_EXCITE,
        POCKET_COLOGNE_ITEM_VIVID, POCKET_COLOGNE_ITEM_CANCEL
    };
    void *arg = window->mArg;
    s32 row;
    s32 message;

    if (sprite->mItemID == POCKET_COLOGNE_ITEM_TITLE) {
        message = getItemName(ITEM_COLOGNE_CASE);
        s32 width = menuItemBiosGetPtr(window->mMenuID, POCKET_COLOGNE_ITEM_TITLE)->mWidth;
        s16 x = GSmsgGetMessageSize(message) >> 16;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), (width - x) / 2, 0, (u32)-1, message);
        return 0;
    }

    for (row = 0; row < POCKET_COLOGNE_ROW_COUNT; row++) {
        if (sprite->mItemID == rowItems[row]) {
            break;
        }
    }
    if (row < 0 || row >= ((POCKET_COLOGNE_ARG *)arg)->mItemCount) {
        return 0;
    }

    if (((POCKET_COLOGNE_ARG *)arg)->mItems[row].mItemID == 0) {
        message = POCKET_COLOGNE_MSG_CANCEL;
    } else {
        message = getItemName(((POCKET_COLOGNE_ARG *)arg)->mItems[row].mItemID);
    }
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, (u32)-1, message);
    if (((POCKET_COLOGNE_ARG *)arg)->mItems[row].mItemID != 0) {
        s32 width = menuItemBiosGetPtr(window->mMenuID, POCKET_COLOGNE_ITEM_JOY)->mWidth;
        int stockMax = getStockMax(((POCKET_COLOGNE_ARG *)arg)->mItems[row].mItemID);
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, stockMax);
        s32 columnWidth = getMessageWidth(POCKET_COLOGNE_MSG_NUMBER);
        columnWidth += getMessageWidth(POCKET_COLOGNE_MSG_QUANTITY_MARKER);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), width - (s16)columnWidth,
                    0, (u32)-1, POCKET_COLOGNE_MSG_QUANTITY_MARKER);
        int itemCount = getItemNum(((POCKET_COLOGNE_ARG *)arg)->mItems[row].mItemID);
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, itemCount);
        s32 countWidth = getMessageWidth(POCKET_COLOGNE_MSG_NUMBER);
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), width - countWidth, 0,
                    (u32)-1, POCKET_COLOGNE_MSG_NUMBER);
    }
    return 0;
}

extern "C" s32 menuPocketColognDrawCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    s32 cursorItems[POCKET_COLOGNE_ROW_COUNT] = {
        POCKET_COLOGNE_CURSOR_JOY, POCKET_COLOGNE_CURSOR_EXCITE,
        POCKET_COLOGNE_CURSOR_VIVID, POCKET_COLOGNE_CURSOR_CANCEL
    };
    s32 cursor = window->mCursor.y;

    if (sprite->mItemID == cursorItems[cursor]) {
        f32 cursorOffset[2] = { 0.0f, 0.0f };
        s32 animation = -1;
        if (window->mMenuID == windowGetActiveID()) {
            menuSubCursorAnimeValGet(cursorOffset, &animation);
        }
        windowDrawSpriteCursor(0, 0, (u32)-1, window, MENU_LIST_CURSOR_TEXTURE, 0,
                               winSpriteGetLayerID(window, sprite), cursorOffset,
                               animation);
    }
    winSpriteSetDisp(sprite, false);
    return 0;
}

s32 getLineHeight(tagWINDOW_WORK *window) {
    tagMENU_ITEM_DATA *second = menuItemBiosGetPtr(window->mMenuID, POCKET_COLOGNE_CURSOR_EXCITE);
    tagMENU_ITEM_DATA *first = menuItemBiosGetPtr(window->mMenuID, POCKET_COLOGNE_CURSOR_JOY);
    s32 height = second->mY - first->mY;

    return height < 0 ? -height : height;
}

extern "C" s32 menuPocketColognDrawLineCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite) {
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(window->mMenuID, sprite->mItemID);
    s32 lineHeight = getLineHeight(window);

    s32 y = window->mCursor.y * lineHeight;
    y = item->mY + y;
    sprite->mY = y;
    return 0;
}

int getItemName(u32 itemID) {
    return itemDataBiosGetName(itemDataBiosGetPtr(itemID));
}

int getStockMax(u16 itemID) {
    u16 stockMax;

    heroItemGetItemKindToItemAryPtr(NULL, itemDataBiosGetKind(itemDataBiosGetPtr(itemID)),
                                    NULL, &stockMax, NULL, NULL);
    return stockMax;
}

int getItemNum(u16 itemID) {
    u16 num;
    ItemSlot *ary;
    int i;
    int count = 0;

    ary = heroItemGetItemKindToItemAryPtr(NULL, itemDataBiosGetKind(itemDataBiosGetPtr(itemID)),
                                          &num, NULL, NULL, NULL);
    if (ary == NULL) {
        return 0;
    }

    for (i = 0; i < num; i++, ary++) {
        if (itemCheckValid(ary) && itemID == (u32)itemGetStatus(ary, 0, ITEM_STATUS_ITEM_DATA_ID, 0)) {
            count += itemBiosGetNum(ary);
        }
    }

    return count;
}
