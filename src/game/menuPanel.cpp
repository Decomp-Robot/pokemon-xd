#include <esaba.h>
#include <GSmsg.h>
#include <menuDataBios.h>
#include <menuSpriteBios.h>
#include <menuSub.h>
#include <winMsg.h>
#include <string.h>
#include <menu.h>
#include <menuPanel.h>
#include <menuIds.h>
#include <msgctrl.h>
#include <winSeq.h>
#include <winSprite.h>
#include <window.h>

// The numeric input panel asks the player for a number, such as how many of an
// item the hero hands over. Its rows print the digits of the number being
// edited, the cursor hook steps that number between the bounds the panel was
// opened with, and the sequence hook runs the window's open and close.

// Argument the numeric input panel is opened with. The window hands the record
// back to the hooks of its rows.
struct INPUT_NUMBER_ARG {
    long mCurrent; // number being edited.
    long mUpper;   // highest number the cursor can select.
    long mLower;   // lowest number the cursor can select.
    long mInitial; // number the panel was opened with.
};

// Number being edited by the open panel. The hooks of the panel's rows reach it
// through the window, so it outlives the opening call.
static INPUT_NUMBER_ARG _inputnumberarg;

// Menu data id of the numeric input panel.
enum {
    INPUT_NUMBER_MENU_ID = 185,
};

// Window sequences run when a menu window opens and closes. Several menus start
// the same pair.
enum {
    MENU_SEQ_OPEN = 36,
    MENU_SEQ_CLOSE = 379,
};

// Rows of the panel that report the number being edited: its tens digit, its
// ones digit and how many the hero holds of the item.
enum {
    INPUT_NUMBER_ITEM_TENS = 1284,
    INPUT_NUMBER_ITEM_ONES = 1283,
    INPUT_NUMBER_ITEM_REMAINING = 1297,
};

long _menuPanelInputNumber(short, short, long initial, long *, long lower,
                           long upper, bool closeOnExit);

extern "C" {

// Ask the player for a number between lower and upper.
bool menuPanelInputNumber(long initial, long *output, long lower, long upper,
                          bool closeOnExit)
{
    long result = _menuPanelInputNumber(0, 0, initial, output, lower, upper, closeOnExit);

    if (result >= 0) {
        *output = result;
        return true;
    }
    *output = initial;
    return false;
}

// Close the numeric input panel once its caller has the number.
void menuPaneInputNumberClose(void)
{
    menuClose(INPUT_NUMBER_MENU_ID);
    menuCloseSync(INPUT_NUMBER_MENU_ID, true);
}

}

// Open the panel and return the number the player chose.
long _menuPanelInputNumber(short, short, long initial, long *, long lower,
                           long upper, bool closeOnExit)
{
    long result;

    _inputnumberarg.mCurrent = initial;
    _inputnumberarg.mUpper = upper;
    _inputnumberarg.mLower = lower;
    _inputnumberarg.mInitial = initial;
    result = menuOpenCustom(INPUT_NUMBER_MENU_ID, windowGetActiveID(), 0, 0, true, 1,
                            &_inputnumberarg);
    if (result >= 0) {
        result = _inputnumberarg.mCurrent;
    }
    if (closeOnExit) {
        menuPaneInputNumberClose();
    }
    return result;
}

extern "C" {

// Print the number being edited on the row that asked for it.
void menuPanelInputNumberHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    INPUT_NUMBER_ARG *arg = (INPUT_NUMBER_ARG *)window->mArg;

    switch (sprite->mItemID) {
    case INPUT_NUMBER_ITEM_TENS:
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, arg->mCurrent / 10);
        break;
    case INPUT_NUMBER_ITEM_ONES:
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, arg->mCurrent % 10);
        break;
    case INPUT_NUMBER_ITEM_REMAINING:
        msgctrlSetValue(MSGCTRL_VALUE_NUMBER, arg->mInitial - arg->mCurrent + esabaGetEsaItemNum());
        break;
    default:
        break;
    }
}

// Step the number being edited while up or down repeats.
void menuPanelInputNumberCursor(tagWINDOW_WORK *window)
{
    INPUT_NUMBER_ARG *arg;
    u16 repeat;

    windowGetKeyInfo();
    arg = (INPUT_NUMBER_ARG *)window->mArg;
    repeat = windowGetKeyRepeat();
    if (repeat & WINDOW_KEY_UP) {
        if (arg->mCurrent < arg->mUpper) {
            arg->mCurrent++;
        }
    } else if (repeat & WINDOW_KEY_DOWN) {
        if (arg->mCurrent > arg->mLower) {
            arg->mCurrent--;
        }
    }
}

// Run the sequences that open and close the panel's window.
void menuPanelInputNumberCtrl(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQ_OPEN);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    default:
        break;
    }
}

}

static long _DrawSelectOffset = 12;
static long _addwidth = 8;
static long _cursorOffset = 5;
static long _lsizecursorYoffset = 2;
static long _backWidth = 5;

enum SelectParam {
    SELECT_PARAM_WINDOW_STYLE = 0,
    SELECT_PARAM_MESSAGES = 1,
    SELECT_PARAM_ENTRY_COUNT = 2,
    SELECT_PARAM_VISIBLE_COUNT = 3,
    SELECT_PARAM_WRAP = 4,
    SELECT_PARAM_ALLOW_CANCEL = 5
};

enum SelectSpriteID {
    SELECT_CURSOR_TALL = 125,
    SELECT_SCROLL_ARROW = 567
};

static const long SELECT_TEXT_X = 26;
static const long SELECT_EMPTY_ROW_HEIGHT = 20;
static const long SELECT_TALL_ROW_HEIGHT = 20;

enum DecimalInputParam {
    DECIMAL_PARAM_VALUE = 0,
    DECIMAL_PARAM_MODE = 1
};

enum LvUpParam {
    LVUP_PARAM_STATUS = 0
};

enum LvUpMenuID {
    LVUP_MENU_FIELD = 41
};

enum LvUpSequenceID {
    LVUP_SEQUENCE_OPEN = 40,
    LVUP_SEQUENCE_CLOSE = 44
};

enum LvUpItemID {
    LVUP_ITEM_WINDOW = 453,
    LVUP_ITEM_HP = 454,
    LVUP_ITEM_ATTACK = 455,
    LVUP_ITEM_DEFENSE = 456,
    LVUP_ITEM_SPEED = 457,
    LVUP_ITEM_SP_ATTACK = 458,
    LVUP_ITEM_SP_DEFENSE = 459
};

enum LvUpMessageID {
    LVUP_MESSAGE_NUMBER = 17006,
    LVUP_MESSAGE_PLUS = 17101
};

static const long LVUP_PLUS_OFFSET = 48;

struct DECIMAL_INPUT_WORK {
    long mDigit;
};

extern "C" {

long menuPanelDrawSelect2(tagWINDOW_WORK *window)
{
    u32 *messages;
    long visibleCount;
    long count;
    long displayMaximum;
    s32 rowHeight;
    s32 height;
    s32 maxRowHeight;
    s32 width;
    s32 y;
    s32 i;
    CURSOR cursor = window->mCursor;
    u8 style;
    s32 spacing;
    spacing = _DrawSelectOffset;
    height = 0;
    width = 0;
    maxRowHeight = 0;
    style = windowGetParam(window, SELECT_PARAM_WINDOW_STYLE);
    messages = (u32 *)windowGetAllocPtr(window);
    visibleCount = (s8)windowGetParam(window, SELECT_PARAM_ENTRY_COUNT);
    count = visibleCount;
    displayMaximum = windowGetParam(window, SELECT_PARAM_VISIBLE_COUNT);
    if (visibleCount > displayMaximum)
        visibleCount = displayMaximum;

    for (i = 0; i < count; ++i) {
        u32 size = GSmsgGetMessageSize(messages[i]);
        if (width < (s32)(size >> 16))
            width = size >> 16;
        if (maxRowHeight < (s32)(size & 0xFFFF))
            maxRowHeight = size & 0xFFFF;
        if (i < visibleCount) {
            height += (size & 0xFFFF) + spacing;
        }
    }
    width += SELECT_TEXT_X;
    width += _addwidth;
    height -= spacing;
    switch (style) {
    case SELECT_WINDOW_NORMAL:
        menuSubDrawNormalWindow2(window->mLayerID, 0, 0, width + 2, height,
                                 window->mRgba.mColorA);
        break;
    case SELECT_WINDOW_MESSAGE:
        menuSubDrawMsgWindow2(window->mLayerID, 0, 0, width + 2, height,
                              window->mRgba.mColorA, 1);
        break;
    case SELECT_WINDOW_SIMPLE:
        menuSubDrawSimpleWindow(window->mLayerID, 0, 0, width + 2, height - 2);
        break;
    }

    MenuSpriteValue *arrow = menuSpriteBiosGetValPtr(menuSpriteBiosGetPtr(SELECT_SCROLL_ARROW));
    if (count > displayMaximum && cursor.x != 0)
        windowDrawSprite(width / 2 - arrow->mWidth / 2, -18, window,
                         SELECT_SCROLL_ARROW, 2, window->mLayerID);
    if (count > displayMaximum && count - cursor.x != displayMaximum)
        windowDrawSprite(width / 2 - arrow->mWidth / 2, height + 10, window,
                         SELECT_SCROLL_ARROW, 0, window->mLayerID);

    // Empty-row tests use the unscrolled list; text uses the captured scroll offset.
    y = 1;
    for (i = 0; i < visibleCount; ++i) {
        if (messages[i] != 0)
            rowHeight = spacing + (GSmsgGetMessageSize(messages[i + cursor.x]) & 0xFFFF);
        else
            rowHeight = SELECT_EMPTY_ROW_HEIGHT;
        if (window->mCursor.y == i) {
            if (maxRowHeight >= SELECT_TALL_ROW_HEIGHT)
                menuSubDrawCursorBack(window->mLayerID, _cursorOffset - 5,
                    y - 7 + _lsizecursorYoffset, width + _backWidth,
                    rowHeight, window->mRgba.mColorA, MENU_SUB_CURSOR_BACK_STYLE_PLAIN);
            else
                menuSubDrawCursorBack(window->mLayerID, _cursorOffset - 6,
                    y - 6, width + _backWidth,
                    rowHeight, window->mRgba.mColorA, MENU_SUB_CURSOR_BACK_STYLE_VARIANT);
        }
        y += rowHeight;
    }

    y = 1;
    for (i = 0; i < visibleCount; ++i) {
        s32 messageIndex = i + cursor.x;
        if (messages[i] != 0) {
            rowHeight = spacing + (GSmsgGetMessageSize(messages[messageIndex]) & 0xFFFF);
            GSmsgPrint2(window->mLayerID, SELECT_TEXT_X, y,
                0xFFFFFF00 | window->mRgba.mColorA, messages[messageIndex]);
        } else
            rowHeight = SELECT_EMPTY_ROW_HEIGHT;
        f32 values[2];
        s32 cursorBlendColor;
        menuSubCursorAnimeValGet(values, &cursorBlendColor);
        if (window->mCursor.y == i) {
            if (maxRowHeight >= SELECT_TALL_ROW_HEIGHT)
                windowDrawSpriteCursor(_cursorOffset, y + _lsizecursorYoffset,
                    0xFFFFFF00 | window->mRgba.mColorA, window, SELECT_CURSOR_TALL,
                    0, window->mLayerID, values, cursorBlendColor);
            else
                windowDrawSpriteCursor(_cursorOffset, y,
                    0xFFFFFF00 | window->mRgba.mColorA, window, MENU_LIST_CURSOR_TEXTURE,
                    0, window->mLayerID, values, cursorBlendColor);
        }
        y += rowHeight;
    }
    return 0;
}

long menuPanelCtrlSelect2(tagWINDOW_WORK *window)
{
    menuSubCursorAnimeMain();
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            long messageBytes;
            const u32 *messages;
            messages = (const u32 *)windowGetParam(window, SELECT_PARAM_MESSAGES);
            messageBytes = windowGetParam(window, SELECT_PARAM_ENTRY_COUNT);
            messageBytes *= sizeof(u32);
            void *messageCopy = windowAllocMemory(window, messageBytes);
            if (messageCopy)
                memcpy(messageCopy, messages, messageBytes);
            bool allowCancel = windowGetParam(window, SELECT_PARAM_ALLOW_CANCEL);
            if (!allowCancel)
                menuSetStatus(window->mMenuID, MENU_STATUS_NO_CANCEL);
            winSeqSetMenu(window->mMenuID, MENU_SEQ_OPEN);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_DELETE:
        break;
    }
    return 0;
}

long menuPanelCursorSelect2(tagWINDOW_WORK *window)
{
    u16 repeat = windowGetKeyInfo()->mKeyRepeat;
    repeat |= windowGetKeyRepeat();
    s8 entryCount = windowGetParam(window, SELECT_PARAM_ENTRY_COUNT);
    s8 visibleCount = windowGetParam(window, SELECT_PARAM_VISIBLE_COUNT);
    bool wrap = windowGetParam(window, SELECT_PARAM_WRAP);
    if (entryCount < visibleCount)
        visibleCount = entryCount;
    // x is the first visible entry, and y is the selected row within the window.
    CURSOR cursor;
    CURSOR previous;
    cursor = window->mCursor;
    previous = cursor;
    if (repeat & WINDOW_KEY_UP)
        --cursor.y;
    else if (repeat & WINDOW_KEY_DOWN)
        ++cursor.y;
    if (cursor.y < 0) {
        cursor.x += cursor.y;
        cursor.y = 0;
        if (wrap) {
            if (cursor.x < 0) {
                cursor.y = (s8)(visibleCount - 1);
                cursor.x = (s8)(entryCount - visibleCount);
            }
        } else {
            if (cursor.x < 0) {
                cursor.y = 0;
                cursor.x = 0;
            }
        }
    } else if (cursor.y >= visibleCount) {
        cursor.x += cursor.y - (visibleCount - 1);
        cursor.y = (s8)(visibleCount - 1);
        if (wrap) {
            if (cursor.x + cursor.y >= entryCount) {
                cursor.x = 0;
                cursor.y = 0;
            }
        } else {
            if (cursor.x + cursor.y >= entryCount) {
                cursor.y = (s8)(visibleCount - 1);
                cursor.x = (s8)(entryCount - visibleCount);
            }
        }
    }
    if (menuCursorCmp(previous, cursor))
        menuSubCursorAnimeStart();
    window->mCursor = cursor;
    return 0;
}

long menuPanelDrawSelect(tagWINDOW_WORK *window)
{
    long visibleCount;
    u32 *messages;
    s32 layoutExtent;
    s32 maxRowHeight;
    s32 i;
    s32 y;
    u8 style;
    u8 displayMaximum;
    s32 height;
    s32 rowHeight;
    s32 spacing;
    spacing = _DrawSelectOffset;
    height = 0;
    layoutExtent = 0;
    maxRowHeight = 0;
    style = windowGetParam(window, SELECT_PARAM_WINDOW_STYLE);
    messages = (u32 *)windowGetAllocPtr(window);
    visibleCount = (s8)windowGetParam(window, SELECT_PARAM_ENTRY_COUNT);
    displayMaximum = menuDataBiosGetDispMax(window->mMenuID);
    if (visibleCount > displayMaximum)
        visibleCount = displayMaximum;

    for (i = 0; i < visibleCount; ++i) {
        u32 size = GSmsgGetMessageSize(messages[i]);
        if (layoutExtent < (s32)(size >> 16))
            layoutExtent = size >> 16;
        if (maxRowHeight < (s32)(size & 0xFFFF))
            maxRowHeight = size & 0xFFFF;
        height += (size & 0xFFFF) + spacing;
    }
    layoutExtent += SELECT_TEXT_X;
    layoutExtent += _addwidth;
    height -= spacing;
    switch (style) {
    case SELECT_WINDOW_NORMAL:
        menuSubDrawNormalWindow2(window->mLayerID, 0, 0, layoutExtent, height,
                                 window->mRgba.mColorA);
        break;
    case SELECT_WINDOW_MESSAGE:
        menuSubDrawMsgWindow2(window->mLayerID, 0, 0, layoutExtent, height,
                              window->mRgba.mColorA, 1);
        break;
    case SELECT_WINDOW_SIMPLE:
        menuSubDrawSimpleWindow(window->mLayerID, 0, 0, layoutExtent, height - 2);
        break;
    }

    y = 1;
    for (i = 0; i < visibleCount; ++i) {
        if (messages[i] != 0)
            rowHeight = spacing + (GSmsgGetMessageSize(messages[i]) & 0xFFFF);
        else
            rowHeight = SELECT_EMPTY_ROW_HEIGHT;
        if (window->mCursor.y == i) {
            if (maxRowHeight >= SELECT_TALL_ROW_HEIGHT)
                menuSubDrawCursorBack(window->mLayerID, _cursorOffset - 5,
                    y - 7 + _lsizecursorYoffset, layoutExtent + _backWidth,
                    rowHeight, window->mRgba.mColorA, MENU_SUB_CURSOR_BACK_STYLE_PLAIN);
            else
                menuSubDrawCursorBack(window->mLayerID, _cursorOffset - 6,
                    y - 6, layoutExtent + _backWidth,
                    rowHeight, window->mRgba.mColorA, MENU_SUB_CURSOR_BACK_STYLE_VARIANT);
        }
        y += rowHeight;
    }

    y = 1;
    for (i = 0; i < visibleCount; ++i) {
        if (messages[i] != 0) {
            layoutExtent = spacing + (GSmsgGetMessageSize(messages[i]) & 0xFFFF);
            GSmsgPrint2(window->mLayerID, SELECT_TEXT_X, y,
                0xFFFFFF00 | window->mRgba.mColorA, messages[i]);
        } else
            layoutExtent = SELECT_EMPTY_ROW_HEIGHT;
        f32 values[2];
        s32 cursorBlendColor;
        menuSubCursorAnimeValGet(values, &cursorBlendColor);
        if (window->mCursor.y == i) {
            if (maxRowHeight >= SELECT_TALL_ROW_HEIGHT)
                windowDrawSpriteCursor(_cursorOffset, y + _lsizecursorYoffset,
                    0xFFFFFF00 | window->mRgba.mColorA, window, SELECT_CURSOR_TALL,
                    0, window->mLayerID, values, cursorBlendColor);
            else
                windowDrawSpriteCursor(_cursorOffset, y,
                    0xFFFFFF00 | window->mRgba.mColorA, window, MENU_LIST_CURSOR_TEXTURE,
                    0, window->mLayerID, values, cursorBlendColor);
        }
        y += layoutExtent;
    }
    return 0;
}
long menuPanelCtrlSelect(tagWINDOW_WORK *window)
{
    menuSubCursorAnimeMain();
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            long messageBytes;
            const u32 *messages;
            messages = (const u32 *)windowGetParam(window, SELECT_PARAM_MESSAGES);
            messageBytes = windowGetParam(window, SELECT_PARAM_ENTRY_COUNT);
            messageBytes *= sizeof(u32);
            void *messageCopy = windowAllocMemory(window, messageBytes);
            if (messageCopy)
                memcpy(messageCopy, messages, messageBytes);
            winSeqSetMenu(window->mMenuID, MENU_SEQ_OPEN);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MENU_SEQ_CLOSE);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_DELETE:
        break;
    }
    return 0;
}

long menuPanelCursorSelect(tagWINDOW_WORK *window)
{
    u16 repeat = windowGetKeyInfo()->mKeyRepeat;
    repeat |= windowGetKeyRepeat();
    s8 entryCount = windowGetParam(window, SELECT_PARAM_ENTRY_COUNT);
    s8 visibleCount = menuDataBiosGetDispMax(window->mMenuID);
    if (entryCount > visibleCount)
        entryCount = visibleCount;
    else
        visibleCount = entryCount;
    // x is the first visible entry, and y is the selected row within the window.
    CURSOR cursor;
    CURSOR previous;
    cursor = window->mCursor;
    previous = cursor;
    if (repeat & WINDOW_KEY_UP)
        --cursor.y;
    else if (repeat & WINDOW_KEY_DOWN)
        ++cursor.y;
    if (cursor.y < 0) {
        cursor.x += cursor.y;
        cursor.y = 0;
        if (cursor.x < 0) {
            cursor.y = (s8)(visibleCount - 1);
            cursor.x = (s8)(entryCount - visibleCount);
        }
    } else if (cursor.y >= visibleCount) {
        cursor.x += cursor.y - (visibleCount - 1);
        cursor.y = (s8)(visibleCount - 1);
        if (cursor.x + cursor.y >= entryCount) {
            cursor.x = 0;
            cursor.y = 0;
        }
    }
    if (menuCursorCmp(previous, cursor))
        menuSubCursorAnimeStart();
    window->mCursor = cursor;
    window->mCursorIndex = cursor.x + cursor.y;
    return 0;
}
}

void _menuPanelDrawDigit(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                         long value, bool showPlus);

extern "C" long menuPanelCursorDecimalInput(tagWINDOW_WORK *window)
{
    DECIMAL_INPUT_WORK *work;
    long delta;
    long mode;
    long digits;
    long radix;
    long value;
    work = (DECIMAL_INPUT_WORK *)windowGetFreeWork(window);
    delta = 0;
    value = windowGetParam(window, DECIMAL_PARAM_VALUE);
    mode = windowGetParam(window, DECIMAL_PARAM_MODE);
    if (mode == MENU_SUB_NUMBER_HEXADECIMAL) {
        digits = 8;
        radix = 16;
    } else {
        digits = 10;
        radix = 10;
    }
    u16 repeat = windowGetKeyInfo()->mKeyRepeat;
    if (repeat & WINDOW_KEY_RIGHT) {
        work->mDigit--;
    } else if (repeat & WINDOW_KEY_LEFT) {
        work->mDigit++;
    }
    if (work->mDigit < 0) {
        work->mDigit = 0;
        value = 0;
    }
    if (work->mDigit >= digits) {
        work->mDigit = digits - 1;
    }
    if (repeat & WINDOW_KEY_UP) {
        delta = 1;
        for (long i = 0; i < work->mDigit; i++) {
            delta *= radix;
        }
    } else if (repeat & WINDOW_KEY_DOWN) {
        delta = -1;
        for (long i = 0; i < work->mDigit; i++) {
            delta *= radix;
        }
    }
    long long result;
    if (mode == MENU_SUB_NUMBER_SIGNED_DECIMAL) {
        result = (long long)value + delta;
        if (result < -2147483648LL) {
            result = -2147483648LL;
        }
        if (result > 2147483647LL) {
            result = 2147483647LL;
        }
    } else {
        result = (long long)(unsigned long)value + delta;
        if (result < 0) {
            result = 0;
        }
        if (result > 4294967295LL) {
            result = 4294967295LL;
        }
    }
    window->mCursorIndex = result & 0xFFFFFFFF;
    windowSetParam(window, DECIMAL_PARAM_VALUE, result & 0xFFFFFFFF);
    return 0;
}

extern "C" long menuPanelDrawLvUpItem(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    MENU_LVUP_STATUS *status = (MENU_LVUP_STATUS *)windowGetParam(window, LVUP_PARAM_STATUS);
    switch (sprite->mItemID) {
    case LVUP_ITEM_WINDOW:
        if (window->mMenuID == LVUP_MENU_FIELD) {
            menuSubDrawNormalWindow2(window->mLayerID, 0, 0, sprite->mWidth,
                                     sprite->mHeight, window->mRgba.mColorA);
        } else {
            menuSubDrawMsgWindow2(window->mLayerID, 0, 0, sprite->mWidth,
                                  sprite->mHeight, window->mRgba.mColorA, 1);
        }
        break;
    case LVUP_ITEM_HP:
        _menuPanelDrawDigit(window, sprite, status->mHp, status->mShowPlus);
        break;
    case LVUP_ITEM_ATTACK:
        _menuPanelDrawDigit(window, sprite, status->mAttack, status->mShowPlus);
        break;
    case LVUP_ITEM_DEFENSE:
        _menuPanelDrawDigit(window, sprite, status->mDefense, status->mShowPlus);
        break;
    case LVUP_ITEM_SPEED:
        _menuPanelDrawDigit(window, sprite, status->mSpeed, status->mShowPlus);
        break;
    case LVUP_ITEM_SP_ATTACK:
        _menuPanelDrawDigit(window, sprite, status->mSpAttack, status->mShowPlus);
        break;
    case LVUP_ITEM_SP_DEFENSE:
        _menuPanelDrawDigit(window, sprite, status->mSpDefense, status->mShowPlus);
        break;
    }
    return 0;
}

extern "C" long menuPanelCtrlLvUp(tagWINDOW_WORK *window)
{
    bool start = false;
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            start = true;
        }
        if (winSeqIsCheck(window->mMenuID, LVUP_SEQUENCE_CLOSE) == true) {
            start = true;
        }
        if (start) {
            winSeqSetMenu(window->mMenuID, LVUP_SEQUENCE_OPEN);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, LVUP_SEQUENCE_CLOSE);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_DELETE:
        break;
    }
    return 0;
}

void _menuPanelDrawDigit(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                         long value, bool showPlus)
{
    if (showPlus) {
        short width = GSmsgGetMessageSize(LVUP_MESSAGE_PLUS) >> 16;
        GSmsgPrint2(window->mLayerID, sprite->mWidth - LVUP_PLUS_OFFSET - width,
                    0, 0xFFFFFF00 | window->mRgba.mColorA, LVUP_MESSAGE_PLUS);
    }
    msgctrlSetValue(MSGCTRL_VALUE_NUMBER, value);
    short width = GSmsgGetMessageSize(LVUP_MESSAGE_NUMBER) >> 16;
    GSmsgPrint2(window->mLayerID, sprite->mWidth - width, 0,
                0xFFFFFF00 | window->mRgba.mColorA, LVUP_MESSAGE_NUMBER);
}
