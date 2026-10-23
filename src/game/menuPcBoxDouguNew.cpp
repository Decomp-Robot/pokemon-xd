#include <item.h>
#include <itemBios.h>
#include <winMsg.h>
#include <menuPocketTool.h>
#include <menuPcBoxDougu.h>
#include <sprite.h>
#include <menuSub.h>
#include <hero.h>
#include <fade.h>
#include <msgctrl.h>
#include <menu.h>
#include <GSmsg.h>
#include <menuItemBios.h>
#include <itemDataBios.h>
#include <menuPtSpriteWork.h>
#include <winSprite.h>
#include <GSmsg.h>
#include <msgctrl.h>
#include <winMsg.h>
#include <winSeq.h>
#include <window.h>

// The PC box's item (dougu) menu: the drawer that lists the items kept in the
// box with the numbers the box holds of them, and the small number window that
// asks how many of an item are taken out or thrown away.
//
// printItemListDougu draws the list, one row per box slot, clipped to the
// rectangle of the list sprite. The list shows eight rows at a time; whenever
// the drawn window moves, the roll animation slides the rows by a quarter of a
// line per frame while their alpha fades, and the cursor sprite follows the row
// the player stands on.


// Argument the number window is opened with: the most of the item the player
// may enter.
struct NumWindowArg {
    s32 mMaxNum; // 0x00: highest number the window accepts.
};

// Alpha a rolling row of the list starts from; the roll fades it out.
#define ITEM_ROLL_ALPHA_MAX 255.0f
// Fraction of the fade the list rolls per frame.
#define ITEM_ROLL_STEP (1.0f / 30.0f)

// Item ids of the three digit rows of the number window, from the ones digit
// up to the hundreds digit; the zero closes the table.
static const s32 lNumDigitItemIDs[] = {0x4B0, 0x4AF, 0x4AE, 0};

// Row the animated item cursor was last drawn on; the row it moves to
// restarts the cursor anime.
static s32 sItemCursorRow;
// Pixels the item list is rolled by while a roll animation runs.
static f32 sListRollY;
// Non-zero while the list rolls the other way.
static s32 sListRollFlip;
// Row picked for the swap, -1 while no row is picked.
static s32 sSwapRow;
// Message the explanation line of the drawer shows.
static u32 sExplanationMsgID;
// Mode the drawer was opened in.
static s32 sHikidashiMode;
// First row of the box the item list shows.
static s32 sListTop;
// Cursor row within the drawn list.
static s32 sListCursorRow;
// Progress of the roll animation, 0 while the list stands still.
static f32 sRollProgress;
// Number the number window is editing.
static s32 sNumValue;

// Cursor anime x/y pairs the unit draws the two cursors with, one pair for the
// item cursor of the drawer and one for the item list cursor. The cursor anime
// overrides them while the window that owns the cursor is the active one.
extern "C" u32 sItemCursorAnimeX;
extern "C" u32 sItemCursorAnimeY;
extern "C" u32 sListCursorAnimeX;
extern "C" u32 sListCursorAnimeY;

// ---- engine and library entry points -------------------------------------

extern "C" {
u8 menuIsOpen(s32 windowID);
void menuButtonNormal(tagWINDOW_WORK *window);
void menuSubCursorAnimeStart(void);
void menuSubCursorAnimeInit(void);
void menuSubCursorAnimeMain(void);
// Draw the sprite item `itemID` of `window` at (x, y) on `layer`. Every texture
// of the item's texture chain is drawn in turn: the first one in `color` and
// each following one in `animeColor`, shifted by the cursor anime offsets
// `values`. Bits 0 and 1 of `flags` flip the sprite. The cursor anime hands in
// -1 for both colours while it holds no override.
// Number entry of the item menus: edits the value digit by digit with the up
// and down keys, wraps it to 0 at `max` and plays one of the two sound effects
// when the value changed. `digits` is how many digits the window shows.
s32 menuPocketInputNumber(tagWINDOW_WORK *window, s32 value, s32 max, u16 keys, s32 digits,
                          u32 seUp, u32 seDown);
}

// ---- item, box and hero data --------------------------------------------

extern "C" {
struct ItemSlot *pcboxGetItem(s32 box, s16 slot);
s32 pcboxGetNbItemSlot(s32 box);
void pcboxDelItem(s32 box, u16 itemID, u16 count);
void pcboxSwapItemSlot(s32 box, s16 from, s16 dest);
// How many of the item the hero can still take.
}

// Window ids of the menus.
enum {
    WIN_ID_DOUGU_DRAWER = 0x8A,
    WIN_ID_DOUGU_HIKIDASHI = 0x8B,
    WIN_ID_DOUGU_NUM = 0x8C,
    WIN_ID_DOUGU_CONFIRM = 0x8D,
    WIN_ID_DOUGU_DEBUG = 0x108,
};

// Sound effects of the menus.
enum {
    SE_MENU_ERROR = 0x13,
    SE_DOUGU_SCROLL = 0x441,
    SE_DOUGU_PICK = 0x442,
    SE_DOUGU_REJECT = 0x443,
    SE_DOUGU_PAGE = 0x444,
};

// Values the drawer substitutes into the text of its messages: the item a
// message names, how many of it the message acts on, and the number the count
// format of the item list prints.
enum {
    MSG_VALUE_ITEM_ID = 0x2D,    // the item a message names.
    MSG_VALUE_ITEM_COUNT = 0x2F, // how many of the item the message acts on.
    MSG_VALUE_NUMBER = 0x34,     // the number the count format prints.
};

// Message ids the drawer prints with.
enum {
    MSG_DOUGU_EMPTY_ROW = 0x3BBA,
    MSG_DOUGU_EMPTY_ROW_NAME = 0x3BBB,
    MSG_DOUGU_SWAP_ROW = 0x3BBC,
    MSG_DOUGU_TAKE_OUT = 0x3BBD,
    MSG_DOUGU_THROW_AWAY = 0x3BBE,
    MSG_DOUGU_THROW_AWAY_NUM = 0x3BBF,
    MSG_DOUGU_TAKE_OUT_MODE = 0x3BC0,
    MSG_DOUGU_THROW_AWAY_MODE = 0x3BC1,
    MSG_DOUGU_BOX_FULL = 0x3BC2,
    MSG_DOUGU_TAKE_OUT_DONE = 0x3BC3,
    MSG_DOUGU_THROW_AWAY_DONE = 0x3BC4,
    MSG_DOUGU_IMPORTANT = 0x3BC5,

    MSG_DOUGU_COUNT_FORMAT = 0x4270,
    MSG_DOUGU_SEPARATOR = 0x428F,
};

// Most of one item a box slot holds. The number column of the list measures
// itself with the widest count, and the number window enters three digits.
enum {
    ITEM_COUNT_MAX = 999,
};

// Menu item ids of the drawer's sprites: the list the rows are clipped to, the
// digit rows of the number window, the row a cursor stands on and the animated
// cursor drawn on it.
enum {
    ITEM_ROW_LIST = 0x4A7,
    ITEM_ROW_DIGIT = 0x4AB,
    ITEM_ROW_CURSOR = 0x4BA,
    ITEM_ROW_CURSOR_SPRITE = 0xC2,
};

// Geometry of the drawn item list.
enum {
    ITEM_LIST_ROWS = 8,
    ITEM_LIST_COUNT_RIGHT = 0x11A, // x the count column is right-aligned to.
    ITEM_ROW_HEIGHT = 0x22,
};

// Window steps the menus run through.
enum {
    WIN_SEQ_NUM_DIGITS = 0x70,
    WIN_SEQ_NUM_ACCEPT = 0x74,
    WIN_SEQ_HIKIDASHI_PREPARE = 0x90,
    WIN_SEQ_HIKIDASHI_LEAVE = 0x94,
};

// Item bag pockets.
enum {
    BAG_POCKET_HIDDEN_MACHINE = 5,
};

// Item list helpers called before they are defined.
s32 _getItemListLineHeight(void);
s32 menuPcBoxDouguConfirm(void);

// Number window: draw one digit row. The item the row carries picks the digit
// the row stands for, the number being edited supplies it.
extern "C" s32 menuPcBoxDouguNumDrawNum(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 digit;
    s32 i;
    s32 digitItemIDs[3];
    s32 divisor;
    s32 num;

    for (i = 0; i < 3; i++) {
        digitItemIDs[i] = lNumDigitItemIDs[i];
    }
    for (digit = 0; digit < 3; digit++) {
        if (digitItemIDs[digit] == sprite->mItemID) {
            break;
        }
    }

    if (digit >= 3) {
        return 0;
    }

    divisor = 1;

    for (i = 0; i < digit; i++) {
        divisor *= 10;
    }
    num = sNumValue / divisor;
    num = num % 10;
    msgctrlSetValue(MSG_VALUE_NUMBER, num);

    GSmsgPrintRight(winSpriteGetLayerID(window, sprite), 0, 0, sprite->mWidth, sprite->mHeight,
                    window->mColor, MSG_DOUGU_COUNT_FORMAT);
    return 0;
}

// Number window cursor: read the number the player entered and show the digit
// row the cursor stands on.
extern "C" s32 menuPcBoxDouguNumCursor(tagWINDOW_WORK *window)
{
    NumWindowArg *arg = (NumWindowArg *)window->mArg;
    u16 keys = windowGetKeyRepeat();
    s32 i;

    sNumValue = menuPocketInputNumber(window, sNumValue, arg->mMaxNum, keys, 3, SE_DOUGU_SCROLL,
                                      SE_DOUGU_PAGE);

    for (i = 0; i < 3; i++) {
        tagSPRITE_WORK *sprite = menuPtSpriteWorkGetByItemID(window, ITEM_ROW_DIGIT, 2 - i);

        winSpriteSetDisp(sprite, window->mCursor.y - i == 0);
    }
    return 0;
}

// Number window control: step 0 puts the digits up, step 3 marks the number as
// accepted.
extern "C" s32 menuPcBoxDouguNumCtrl(tagWINDOW_WORK *window)
{
    s8 state = window->mState;

    switch (state) {
    case 0:
        if (window->mFlag == 0) {
            winSeqSetMenu(WIN_ID_DOUGU_NUM, WIN_SEQ_NUM_DIGITS);
            window->mFlag = 1;
        }
        break;

    case 3:
        if (window->mFlag == 0) {
            winSeqSetMenu(WIN_ID_DOUGU_NUM, WIN_SEQ_NUM_ACCEPT);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

// Number window: ask how many of the item are taken out or thrown away.
// `value` is the number the window starts on and `maxNum` the most it lets the
// player enter. Returns the number the player entered, -1 when the window was
// left.
int menuPcBoxDouguNum(int value, int maxNum)
{
    NumWindowArg arg;
    s32 startDigit;

    sNumValue = value;
    // The cursor starts on the last of the three digit rows.
    startDigit = 2;
    arg.mMaxNum = maxNum;

    s32 result =
        menuOpenCustom(WIN_ID_DOUGU_NUM, windowGetActiveID(), &startDigit, 0, 1, 1, &arg);

    menuClose(WIN_ID_DOUGU_NUM);
    menuCloseSync(WIN_ID_DOUGU_NUM, 1);

    if (result == -1 || sNumValue == 0) {
        return -1;
    }
    return sNumValue;
}

// Bag pocket the item belongs to, ItemData::mBagPocket.
u8 getItemKind(u16 itemID)
{
    return itemDataBiosGetKind(itemDataBiosGetPtr(itemID));
}

// Moves box slot `from` onto slot `dest`.
void swapItemSlot(int from, int dest)
{
    pcboxSwapItemSlot(0, from, dest);
}

// Message id of the item instructions.
u32 getItemInstMsg(unsigned long itemID)
{
    return itemDataBiosGetDoc(itemDataBiosGetPtr((u16)itemID));
}

// Number of the item in the list row `menuPos`: the count half of the box slot
// the row shows.
u16 getItemNumFromMenuPos(int menuPos)
{
    u16 nbSlot;
    struct ItemSlot *slot;
    s32 i;
    s32 n;

    nbSlot = pcboxGetNbItemSlot(0);
    n = -1;
    for (i = 0; i < nbSlot; i++) {
        slot = pcboxGetItem(0, i);

        if (itemCheckValid(slot)) {
            if (++n >= menuPos) {
                return itemBiosGetNum(slot);
            }
        }
    }
    return 0;
}

// Message id of the item name.
u32 getItemNameMsg(u16 itemID)
{
    return itemDataBiosGetName(itemDataBiosGetPtr(itemID));
}

// Item id of the box slot the list row `row` shows.
u16 getItemIDFromMenuPos(int row)
{
    u16 nbSlot;
    struct ItemSlot *slot;
    s32 i;
    s32 n;

    nbSlot = pcboxGetNbItemSlot(0);
    n = -1;
    for (i = 0; i < nbSlot; i++) {
        slot = pcboxGetItem(0, i);

        if (itemCheckValid(slot)) {
            if (++n >= row) {
                return itemBiosGetItemDataId(slot);
            }
        }
    }
    return 0;
}

// Number of box rows the item list holds.
s32 getNbItemSlot(void)
{
    u16 nbSlot;
    s32 i;
    s32 nb;

    nb = 0;
    nbSlot = pcboxGetNbItemSlot(0);
    for (i = 0; i < nbSlot; i++) {
        if (itemCheckValid(pcboxGetItem(0, i))) {
            nb++;
        }
    }
    return nb;
}

// Message row of the drawer: it prints one of the two mode messages.
extern "C" s32 menuPcBoxDouguHikidasuDrawTitleText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 msgID;
    tagMENU_ITEM_DATA *item;
    s16 width;
    s32 half;

    if (sHikidashiMode == MENU_PCBOX_DOUGU_HIKIDASHI_TAKE_OUT) {
        msgID = MSG_DOUGU_TAKE_OUT_MODE;
    } else {
        msgID = MSG_DOUGU_THROW_AWAY_MODE;
    }
    sprite->mMsgID = msgID;
    item = menuPtItemGetBySpriteWork(window, sprite);
    width = sprite->mWidth / 2;
    half = (s16)(GSmsgGetMessageSize(msgID) >> 16) / 2;
    sprite->mX = item->mMessageX + (s16)(width - half);
    return 0;
}

// Roll a row into the bottom of the drawer window.
extern "C" s32 menuPcBoxDouguHikidasuDrawRollDown(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    bool disp;
    s32 row;

    row = sListTop + ITEM_LIST_ROWS;
    if (row < getNbItemSlot() + 1) {
        disp = 1;
    } else {
        disp = 0;
    }
    sprite->mRgba.mAlpha = (u8)(ITEM_ROLL_ALPHA_MAX * (1.0f - sRollProgress));
    winSpriteSetDisp(sprite, disp);
    return 0;
}

// Roll a row off the top of the drawer window.
extern "C" s32 menuPcBoxDouguHikidasuDrawRollUp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    bool disp;

    if (sListTop > 0) {
        disp = 1;
    } else {
        disp = 0;
    }
    sprite->mRgba.mAlpha = (u8)(ITEM_ROLL_ALPHA_MAX * (1.0f - sRollProgress));
    winSpriteSetDisp(sprite, disp);
    return 0;
}

// Draw the item list of the drawer: the name of each row on the left and the
// number the box holds of it on the right.
int printItemListDougu(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, unsigned long color)
{
    s16 w;
    int rowEndY;
    u16 itemID;
    int i;
    int row;
    int count;
    int nb;
    u32 msgID;
    int rowPix;
    int y;
    u32 col = color;
    int countColumnWidth;
    int rowY;

    rowY = 0;
    count = ITEM_LIST_ROWS;
    y = 0;

    nb = getNbItemSlot();
    row = sListTop;

    msgctrlSetValue(MSG_VALUE_NUMBER, ITEM_COUNT_MAX);
    s16 wNum = GSmsgGetMessageSize(MSG_DOUGU_COUNT_FORMAT) >> 16;
    s16 wSep = GSmsgGetMessageSize(MSG_DOUGU_SEPARATOR) >> 16;

    countColumnWidth = wNum + wSep;

    if (sListRollY != 0.0f && sListRollFlip != 0) {
        if (sListRollY < 0.0f) {
            row--;
            rowY = -1;
        } else {
            count = ITEM_LIST_ROWS + 1;
        }
        y = (int)sListRollY;
    }

    for (i = rowY; i < count && row < nb; i++, row++) {
        if (row >= 0) {
            rowPix = i * _getItemListLineHeight() - y;
            itemID = getItemIDFromMenuPos(row);
            msgID = getItemNameMsg(itemID);

            if (msgID != 0) {
                GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, rowPix, col, msgID);
            }

            if (getItemKind(itemID) == BAG_POCKET_HIDDEN_MACHINE) {
                continue;
            }

            GSmsgPrint2(winSpriteGetLayerID(window, sprite), ITEM_LIST_COUNT_RIGHT - countColumnWidth,
                        rowPix, col, MSG_DOUGU_SEPARATOR);
            msgctrlSetValue(MSG_VALUE_NUMBER, getItemNumFromMenuPos(row));
            w = GSmsgGetMessageSize(MSG_DOUGU_COUNT_FORMAT) >> 16;

            GSmsgPrint2(winSpriteGetLayerID(window, sprite), ITEM_LIST_COUNT_RIGHT - w, rowPix, col,
                        MSG_DOUGU_COUNT_FORMAT);
        }
    }

    if (i < count) {
        rowEndY = i * _getItemListLineHeight() - y;
        GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, rowEndY, col, MSG_DOUGU_EMPTY_ROW);
    }
    return 0;
}

// Draw hook of the list sprite: clip the rows to the list rectangle and print
// them.
extern "C" s32 menuPcBoxDouguHikidasuDrawItemText(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    tagSPRITE_WORK *list;
    s32 layer;

    list = menuPtSpriteWorkGetByItemID(window, ITEM_ROW_LIST, 0);
    layer = winSpriteGetLayerID(window, sprite);
    spriteSetScissor(layer, list->mX - sprite->mX, list->mY - sprite->mY, list->mWidth,
                     list->mHeight);
    printItemListDougu(window, sprite, window->mColor);
    spriteClearScissor(winSpriteGetLayerID(window, sprite));
    spriteSetEnv(winSpriteGetLayerID(window, sprite));
    return 0;
}

// Draw hook of the list cursor sprite: hand the cursor anime its default values
// and draw the cursor on the row the list stands on.
extern "C" s32 menuPcBoxDouguHikidasuDrawCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 y = sListCursorRow * _getItemListLineHeight();
    u32 animeValues[2];
    s32 animeColor;

    if (sListRollFlip == 0) {
        y += (s32)sListRollY;
    }

    animeValues[0] = sListCursorAnimeX;
    animeValues[1] = sListCursorAnimeY;

    animeColor = -1;

    if (window->mMenuID == windowGetActiveID()) {
        menuSubCursorAnimeValGet((f32 *)animeValues, &animeColor);
    }

    windowDrawSpriteCursor(0, y, -1, window, ITEM_ROW_CURSOR_SPRITE, 0,
                           winSpriteGetLayerID(window, sprite), (f32 *)animeValues, animeColor);
    winSpriteSetDisp(sprite, 0);
    return 0;
}

// Row hook of the drawer list: place the row sprite at the row the cursor
// stands on and show it while the row is inside the drawn window.
extern "C" s32 menuPcBoxDouguHikidasuDrawLineCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    tagMENU_ITEM_DATA *item;
    s32 y;

    item = menuPtItemGetBySpriteWork(window, sprite);

    if (sSwapRow >= 0) {
        s32 row = sSwapRow - sListTop;

        y = item->mOffsetA + row * _getItemListLineHeight();
        if (sListRollFlip != 0) {
            y -= (s32)sListRollY;
        }
        winSpriteSetDisp(sprite, row >= 0 && row < ITEM_LIST_ROWS);
    } else {
        y = item->mOffsetA + sListCursorRow * _getItemListLineHeight();
        if (sListRollFlip == 0) {
            y += (s32)sListRollY;
        }
        winSpriteSetDisp(sprite, 1);
    }

    sprite->mY = y;
    return 0;
}

// Draw the explanation line of the drawer.
extern "C" s32 menuPcBoxDouguHikidasuDrawExplanationText(tagWINDOW_WORK *window,
                                                        tagSPRITE_WORK *sprite)
{
    GSmsgPrint2(winSpriteGetLayerID(window, sprite), 0, 0, window->mColor, sExplanationMsgID);
    return 0;
}

// The explanation line shows `msgID` from now on.
void setExplanationMsgID(unsigned long msgID)
{
    sExplanationMsgID = msgID;
}

// Item list cursor: walk the list with the pad, swap the row that was picked
// with the row the cursor stands on, and show the explanation of that row.
extern "C" s32 menuPcBoxDouguHikidashiCursor(tagWINDOW_WORK *window)
{
    u16 repeat = windowGetKeyRepeat();
    u16 trg = windowGetKeyTrg();
    s32 row;
    u32 msgID;

    if (sListRollY != 0.0f) {
        return 0;
    }

    row = sListTop + sListCursorRow;

    if (sSwapRow < 0) {
        if ((trg & 0xC0) != 0 && getItemIDFromMenuPos(row) != 0) {
            sSwapRow = row;
            menuPtPlaySe(SE_DOUGU_PICK);
        }
    } else {
        if ((trg & 0xD0) != 0) {
            if (getItemIDFromMenuPos(row) != 0) {
                swapItemSlot(sSwapRow, row);
                menuPtPlaySe(SE_DOUGU_PICK);
            } else {
                menuPtPlaySe(SE_DOUGU_REJECT);
            }
            sSwapRow = -1;
        } else if ((trg & 0x20) != 0) {
            sSwapRow = -1;
            menuPtPlaySe(SE_DOUGU_REJECT);
        }
    }

    s32 nb = getNbItemSlot();
    s32 nbRows = nb + 1;
    s32 pages = (nbRows + ITEM_LIST_ROWS - 1) / ITEM_LIST_ROWS;
    s32 top = sListTop;
    s32 cursorRow = sListCursorRow;

    if ((repeat & 2) != 0) {
        sListCursorRow++;
        if (sListCursorRow + top >= nbRows) {
            sListCursorRow--;
        } else {
            if (sListCursorRow >= ITEM_LIST_ROWS) {
                sListTop++;
                sListCursorRow--;
                sListRollFlip = 1;
            } else {
                sListRollFlip = 0;
            }
            sListRollY = (f32)-_getItemListLineHeight();
            menuPtPlaySe(SE_DOUGU_SCROLL);
        }
    } else if ((repeat & 1) != 0) {
        if (cursorRow > 0 || top > 0) {
            sListCursorRow--;
            if (sListCursorRow < 0) {
                sListCursorRow = 0;
                sListTop--;
                sListRollFlip = 1;
            } else {
                sListRollFlip = 0;
            }
            sListRollY = (f32)_getItemListLineHeight();
            menuPtPlaySe(SE_DOUGU_SCROLL);
        }
    } else if ((repeat & 8) != 0) {
        s16 lastRow = nbRows - ITEM_LIST_ROWS;

        if (pages == 1) {
            sListCursorRow = (s16)(nbRows - 1);
        } else if (sListTop == lastRow) {
            sListCursorRow = ITEM_LIST_ROWS - 1;
        } else {
            sListTop += ITEM_LIST_ROWS;
            if (sListTop > lastRow) {
                sListTop = lastRow;
            }
        }
        if (top != sListTop || cursorRow != sListCursorRow) {
            menuPtPlaySe(SE_DOUGU_PAGE);
        }
    } else if ((repeat & 4) != 0) {
        if (sListTop == 0) {
            sListCursorRow = 0;
        } else {
            sListTop -= ITEM_LIST_ROWS;
            if (sListTop < 0) {
                sListTop = 0;
            }
        }
        if (top != sListTop || cursorRow != sListCursorRow) {
            menuPtPlaySe(SE_DOUGU_PAGE);
        }
    }

    if (top != sListTop || cursorRow != sListCursorRow) {
        menuSubCursorAnimeStart();
    }

    if (sSwapRow >= 0) {
        msgID = MSG_DOUGU_SWAP_ROW;
    } else {
        row = sListTop + sListCursorRow;
        msgID = getItemIDFromMenuPos(row);
        if (msgID != 0) {
            msgID = getItemInstMsg(msgID);
        } else {
            msgID = MSG_DOUGU_EMPTY_ROW_NAME;
        }
    }
    setExplanationMsgID(msgID);
    return 0;
}

// Button sound of the drawer; silent while a row is picked for the swap.
extern "C" void menuPcBoxDouguHikidashiButton(tagWINDOW_WORK *window)
{
    if (sSwapRow < 0) {
        menuButtonNormal(window);
    }
}

// Window control of the drawer: prepare the window and run the roll animation
// of the item list.
extern "C" s32 menuPcBoxDouguHikidashiCtrl(tagWINDOW_WORK *window)
{
    s8 state = window->mState;

    switch (state) {
    case 0:
        if (window->mFlag == 0) {
            u32 msgID;

            winSeqSetMenu(WIN_ID_DOUGU_HIKIDASHI, WIN_SEQ_HIKIDASHI_PREPARE);
            msgID = getItemIDFromMenuPos(sListTop + sListCursorRow);
            if (msgID != 0) {
                msgID = getItemInstMsg(msgID);
            } else {
                msgID = MSG_DOUGU_EMPTY_ROW_NAME;
            }
            setExplanationMsgID(msgID);
            sListRollFlip = 0;
            sListRollY = 0.0f;
            sSwapRow = -1;
            window->mFlag = 1;
            menuSubCursorAnimeInit();
        }
        break;

    case 2:
        if (sListRollY > 0.0f) {
            sListRollY -= (f32)(_getItemListLineHeight() / 4);
            if (sListRollY < 0.0f) {
                sListRollY = 0.0f;
            }
        }
        if (sListRollY < 0.0f) {
            sListRollY += (f32)(_getItemListLineHeight() / 4);
            if (sListRollY > 0.0f) {
                sListRollY = 0.0f;
            }
        }

        sRollProgress += ITEM_ROLL_STEP;
        if (sRollProgress > 1.0f) {
            sRollProgress = 0.0f;
        }
        menuSubCursorAnimeMain();
        break;

    case 3:
        if (window->mFlag == 0) {
            winSeqSetMenu(WIN_ID_DOUGU_HIKIDASHI, WIN_SEQ_HIKIDASHI_LEAVE);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

// Throw the picked number of the item away.
void suteru(int menuPos)
{
    s32 count;
    u16 itemID = getItemIDFromMenuPos(menuPos);

    if (getItemKind(itemID) == BAG_POCKET_HIDDEN_MACHINE) {
        menuPtPlaySe(SE_MENU_ERROR);
        winMsgOpen(2, MSG_DOUGU_IMPORTANT, 1, 0);
        winMsgClose(1);
        return;
    }

    setExplanationMsgID(MSG_DOUGU_THROW_AWAY);
    count = menuPcBoxDouguNum(1, getItemNumFromMenuPos(menuPos));

    if (count <= 0) {
        return;
    }

    msgctrlSetValue(MSG_VALUE_ITEM_ID, itemID);
    msgctrlSetValue(MSG_VALUE_ITEM_COUNT, count);
    setExplanationMsgID(MSG_DOUGU_THROW_AWAY_NUM);

    if (menuPcBoxDouguConfirm() == 0) {
        return;
    }

    pcboxDelItem(0, itemID, (u16)count);
    msgctrlSetValue(MSG_VALUE_ITEM_ID, itemID);
    msgctrlSetValue(MSG_VALUE_ITEM_COUNT, count);
    winMsgOpen(2, MSG_DOUGU_THROW_AWAY_DONE, 1, 0);
    winMsgClose(1);
}

// Take the picked number of the item out of the box into the bag.
void hikidasu(int menuPos)
{
    s32 count;
    u16 itemID = getItemIDFromMenuPos(menuPos);

    setExplanationMsgID(MSG_DOUGU_TAKE_OUT);

    if (getItemKind(itemID) != BAG_POCKET_HIDDEN_MACHINE) {
        count = menuPcBoxDouguNum(1, getItemNumFromMenuPos(menuPos));
    } else {
        count = 1;
    }

    if (count <= 0) {
        return;
    }

    if (heroItemCheckAddItemDataId(0, itemID) < count) {
        menuPtPlaySe(SE_MENU_ERROR);
        winMsgOpen(2, MSG_DOUGU_BOX_FULL, 1, 0);
        winMsgClose(1);
        return;
    }

    heroItemAddItemDataId(0, itemID, (u16)count, -1);
    pcboxDelItem(0, itemID, (u16)count);
    msgctrlSetValue(MSG_VALUE_ITEM_ID, itemID);
    msgctrlSetValue(MSG_VALUE_ITEM_COUNT, count);
    winMsgOpen(2, MSG_DOUGU_TAKE_OUT_DONE, 1, 0);
    winMsgClose(1);
}

// Run the drawer over the item list until the player leaves it.
void menuPcBoxDouguHikidashi(MENU_PCBOX_DOUGU_HIKIDASHI_MODE mode)
{
    sHikidashiMode = mode;
    menuOpen(WIN_ID_DOUGU_DRAWER, 0);
    fadeSet(2, 0.5f);
    fadeCheck(1);

    while (true) {
        if (menuOpen(WIN_ID_DOUGU_HIKIDASHI, 1) == -1) {
            break;
        }

        s32 row = sListTop + sListCursorRow;
        if (getItemIDFromMenuPos(row) == 0) {
            break;
        }

        switch (mode) {
        case MENU_PCBOX_DOUGU_HIKIDASHI_TAKE_OUT:
            hikidasu(row);
            break;
        case MENU_PCBOX_DOUGU_HIKIDASHI_THROW_AWAY:
            suteru(row);
            break;
        }
    }

    menuClose(WIN_ID_DOUGU_HIKIDASHI);
    menuCloseSync(WIN_ID_DOUGU_HIKIDASHI, 1);
    fadeSet(3, 0.5f);
    fadeCheck(1);
    menuClose(WIN_ID_DOUGU_DRAWER);
}

// One line of the item list is this many pixels high.
s32 _getItemListLineHeight(void)
{
    return ITEM_ROW_HEIGHT;
}

// Item cursor hook of the drawer: place the cursor sprite on the row the list
// stands on; the list cursor item draws the animated cursor itself.
extern "C" s32 menuPcBoxDouguConfirmDrawCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    tagMENU_ITEM_DATA *item;
    s32 y;
    u32 animeValues[2];
    s32 animeColor;

    y = window->mCursor.y * ITEM_ROW_HEIGHT;
    item = menuPtItemGetBySpriteWork(window, sprite);

    if (sprite->mItemID == ITEM_ROW_CURSOR) {
        if (sItemCursorRow != window->mCursor.y) {
            menuSubCursorAnimeStart();
        }

        animeValues[0] = sItemCursorAnimeX;
        animeValues[1] = sItemCursorAnimeY;
        animeColor = -1;

        if (window->mMenuID == windowGetActiveID()) {
            menuSubCursorAnimeValGet((f32 *)animeValues, &animeColor);
        }

        windowDrawSpriteCursor(0, y, -1, window, ITEM_ROW_CURSOR_SPRITE, 0,
                               winSpriteGetLayerID(window, sprite), (f32 *)animeValues, animeColor);
        winSpriteSetDisp(sprite, 0);
        sItemCursorRow = window->mCursor.y;
    } else {
        sprite->mY = item->mOffsetA + y;
    }
    return 0;
}

// Confirm window: ask whether the action is run. Returns whether the player
// answered yes.
s32 menuPcBoxDouguConfirm(void)
{
    s32 result;

    menuSubCursorAnimeInit();
    sItemCursorRow = 0;
    result = menuOpen(WIN_ID_DOUGU_CONFIRM, 1);
    menuClose(WIN_ID_DOUGU_CONFIRM);
    menuCloseSync(WIN_ID_DOUGU_CONFIRM, 1);

    s32 answer;

    if (result == -1) {
        answer = 0;
    } else if (result == 0) {
        answer = 1;
    } else {
        answer = 0;
    }
    return answer;
}

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

// Main hook of the drawer entry in the debug menu: nothing to do every frame.
extern "C" s32 menuPcBoxDouguDebugMain(void)
{
    return 0;
}

// Control hook of the drawer entry in the debug menu: show or hide its window.
extern "C" s32 menuPcBoxDouguDebugCtrl(void)
{
    if (menuIsOpen(WIN_ID_DOUGU_DEBUG) != 0) {
        menuClose(WIN_ID_DOUGU_DEBUG);
    } else {
        menuOpenCustom(WIN_ID_DOUGU_DEBUG, 0, 0, 0, 1, 0);
    }
    return 0;
}

#pragma force_active reset
