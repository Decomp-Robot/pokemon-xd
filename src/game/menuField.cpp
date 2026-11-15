#include <types.h>
#include <winSprite.h>
#include <GSmsg.h>
#include <time.h>
#include <menu.h>
#include <libc/math.h>

// Field menu item drawing, cursor effects and selection-bar animation.

// Whether the field menu is open; the open check reports this flag.
u8 menuFieldOpen;
// Time the field menu cursor animation has run; the animation update wraps it.
f32 _fieldMenuTimer;

enum { FIELD_MENU_ROW_COUNT = 5 };

enum FieldMenuSpriteID {
    FIELD_MENU_BAR_0 = 21,
    FIELD_MENU_BAR_1 = 22,
    FIELD_MENU_BAR_2 = 23,
    FIELD_MENU_BAR_3 = 24,
    FIELD_MENU_BAR_4 = 25,
    FIELD_MENU_FLASH_CURSOR_0 = 26,
    FIELD_MENU_FLASH_CURSOR_1 = 27,
    FIELD_MENU_FLASH_CURSOR_2 = 28,
    FIELD_MENU_FLASH_CURSOR_3 = 29,
    FIELD_MENU_FLASH_CURSOR_4 = 30,
    FIELD_MENU_TEXT_0 = 31,
    FIELD_MENU_TEXT_1 = 32,
    FIELD_MENU_TEXT_2 = 33,
    FIELD_MENU_TEXT_3 = 34,
    FIELD_MENU_TEXT_4 = 35,
    FIELD_MENU_ALT_BAR_0 = 707,
    FIELD_MENU_ALT_BAR_1 = 708,
    FIELD_MENU_ALT_BAR_2 = 709,
    FIELD_MENU_ALT_BAR_3 = 710,
    FIELD_MENU_ALT_FLASH_CURSOR_0 = 711,
    FIELD_MENU_ALT_FLASH_CURSOR_1 = 712,
    FIELD_MENU_ALT_FLASH_CURSOR_2 = 713,
    FIELD_MENU_ALT_FLASH_CURSOR_3 = 714,
    FIELD_MENU_ALT_TEXT_0 = 715,
    FIELD_MENU_ALT_TEXT_1 = 716,
    FIELD_MENU_ALT_TEXT_2 = 717,
    FIELD_MENU_ALT_TEXT_3 = 718,
    FIELD_MENU_CURSOR_0 = 1643,
    FIELD_MENU_CURSOR_1 = 1644,
    FIELD_MENU_CURSOR_2 = 1645,
    FIELD_MENU_CURSOR_3 = 1646,
    FIELD_MENU_CURSOR_4 = 1647,
    FIELD_MENU_ALT_CURSOR_0 = 1648,
    FIELD_MENU_ALT_CURSOR_1 = 1649,
    FIELD_MENU_ALT_CURSOR_2 = 1650,
    FIELD_MENU_ALT_CURSOR_3 = 1651,
};

enum {
    FIELD_MENU_TEXT_SELECTED = 0xF0F0F0FF,
    FIELD_MENU_TEXT_UNSELECTED = 0xF0F0F080
};

// Per-row pulse used by the field-menu item sprites.
f32 _menuFiledAlpha[FIELD_MENU_ROW_COUNT];
u32 barcolor = 0xC0C0D000;
static f32 _cursorSec = 0.8f;
static f32 _max_alpha = 255.0f;
static f32 _min_alpha = 200.0f;
u32 _soundEnvID;
u32 _soundEnvVol;
f32 _menuFieldCursorAngle;

f32 _animAlpha(long row);
void _menuFieldMenuCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row);
void _menuFieldFlashCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row);
void _menuFieldCursorCheck(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row);
void _menuFieldDrawItemSub(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row);

extern "C" void menuFieldDrawItem2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);

void menuFieldDrawItem2(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case FIELD_MENU_BAR_0:
        _menuFieldDrawItemSub(window, sprite, 0);
        break;
    case FIELD_MENU_BAR_1:
        _menuFieldDrawItemSub(window, sprite, 1);
        break;
    case FIELD_MENU_BAR_2:
        _menuFieldDrawItemSub(window, sprite, 2);
        break;
    case FIELD_MENU_BAR_3:
        _menuFieldDrawItemSub(window, sprite, 3);
        break;
    case FIELD_MENU_BAR_4:
        _menuFieldDrawItemSub(window, sprite, 4);
        break;
    case FIELD_MENU_FLASH_CURSOR_0:
        _menuFieldFlashCursor(window, sprite, 0);
        break;
    case FIELD_MENU_FLASH_CURSOR_1:
        _menuFieldFlashCursor(window, sprite, 1);
        break;
    case FIELD_MENU_FLASH_CURSOR_2:
        _menuFieldFlashCursor(window, sprite, 2);
        break;
    case FIELD_MENU_FLASH_CURSOR_3:
        _menuFieldFlashCursor(window, sprite, 3);
        break;
    case FIELD_MENU_FLASH_CURSOR_4:
        _menuFieldFlashCursor(window, sprite, 4);
        break;
    case FIELD_MENU_CURSOR_0:
        _menuFieldMenuCursor(window, sprite, 0);
        break;
    case FIELD_MENU_CURSOR_1:
        _menuFieldMenuCursor(window, sprite, 1);
        break;
    case FIELD_MENU_CURSOR_2:
        _menuFieldMenuCursor(window, sprite, 2);
        break;
    case FIELD_MENU_CURSOR_3:
        _menuFieldMenuCursor(window, sprite, 3);
        break;
    case FIELD_MENU_CURSOR_4:
        _menuFieldMenuCursor(window, sprite, 4);
        break;
    case FIELD_MENU_TEXT_0:
        _menuFieldCursorCheck(window, sprite, 0);
        break;
    case FIELD_MENU_TEXT_1:
        _menuFieldCursorCheck(window, sprite, 1);
        break;
    case FIELD_MENU_TEXT_2:
        _menuFieldCursorCheck(window, sprite, 2);
        break;
    case FIELD_MENU_TEXT_3:
        _menuFieldCursorCheck(window, sprite, 3);
        break;
    case FIELD_MENU_TEXT_4:
        _menuFieldCursorCheck(window, sprite, 4);
        break;
    case FIELD_MENU_ALT_BAR_0:
        _menuFieldDrawItemSub(window, sprite, 0);
        break;
    case FIELD_MENU_ALT_BAR_1:
        _menuFieldDrawItemSub(window, sprite, 1);
        break;
    case FIELD_MENU_ALT_BAR_2:
        _menuFieldDrawItemSub(window, sprite, 2);
        break;
    case FIELD_MENU_ALT_BAR_3:
        _menuFieldDrawItemSub(window, sprite, 3);
        break;
    case FIELD_MENU_ALT_FLASH_CURSOR_0:
        _menuFieldFlashCursor(window, sprite, 0);
        break;
    case FIELD_MENU_ALT_FLASH_CURSOR_1:
        _menuFieldFlashCursor(window, sprite, 1);
        break;
    case FIELD_MENU_ALT_FLASH_CURSOR_2:
        _menuFieldFlashCursor(window, sprite, 2);
        break;
    case FIELD_MENU_ALT_FLASH_CURSOR_3:
        _menuFieldFlashCursor(window, sprite, 3);
        break;
    case FIELD_MENU_ALT_CURSOR_0:
        _menuFieldMenuCursor(window, sprite, 0);
        break;
    case FIELD_MENU_ALT_CURSOR_1:
        _menuFieldMenuCursor(window, sprite, 1);
        break;
    case FIELD_MENU_ALT_CURSOR_2:
        _menuFieldMenuCursor(window, sprite, 2);
        break;
    case FIELD_MENU_ALT_CURSOR_3:
        _menuFieldMenuCursor(window, sprite, 3);
        break;
    case FIELD_MENU_ALT_TEXT_0:
        _menuFieldCursorCheck(window, sprite, 0);
        break;
    case FIELD_MENU_ALT_TEXT_1:
        _menuFieldCursorCheck(window, sprite, 1);
        break;
    case FIELD_MENU_ALT_TEXT_2:
        _menuFieldCursorCheck(window, sprite, 2);
        break;
    case FIELD_MENU_ALT_TEXT_3:
        _menuFieldCursorCheck(window, sprite, 3);
        break;
    }
}

// Row hook of the field menu cursor sprites: the sprite shows the row the
// window's cursor is on, and the cursor animation restarts whenever the cursor
// has moved off the row the window is scrolled to.
void _menuFieldMenuCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    s16 cursorY = window->mCursor.y;
    s16 cursorTo = window->mPreviousCursor.y;

    if (cursorY == row) {
        if (cursorTo != cursorY) {
            winAnimeCursor(window, sprite);
            _fieldMenuTimer = 0.0f;
        }
        winSpriteSetDisp(sprite, true);
    } else {
        winSpriteSetDisp(sprite, false);
    }

    if (window->mState == WINDOW_STATE_CLOSING) {
        winSetSequence(&sprite->mSeq, 0);
    }
}

void _menuFieldFlashCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    if (window->mCursor.y == row) {
        sprite->mYaw = _menuFieldCursorAngle;
        winSpriteSetDisp(sprite, true);
    } else {
        winSpriteSetDisp(sprite, false);
    }
}

void _menuFieldCursorCheck(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    s16 x, y;
    u32 color;
    long cursor;

    cursor = window->mCursor.y;
    winSpriteSetDisp(sprite, false);
    if (sprite->mMsgID != 0) {
        if (cursor == row) {
            color = FIELD_MENU_TEXT_SELECTED;
        } else {
            color = FIELD_MENU_TEXT_UNSELECTED;
        }
        x = (sprite->mWidth - (s16)(GSmsgGetMessageSize(sprite->mMsgID) >> 16)) / 2;
        y = (sprite->mHeight - (s16)(GSmsgGetMessageSize(sprite->mMsgID) & 0xFFFF)) / 2 - 2;
        GSmsgPrint2(window->mLayerID, x, y, color, sprite->mMsgID);
    }
}

void _menuFieldDrawItemSub(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    long cursor = window->mCursor.y;
    f32 step = 255.0f * timeGetLastFrameSec() / 0.4f;
    if (cursor == row) {
        winSpriteSetDisp(sprite, true);
        _animAlpha(row);
    } else if (_menuFiledAlpha[row] > 0.0f) {
        _menuFiledAlpha[row] -= step;
        if (_menuFiledAlpha[row] < 0.0f) {
            _menuFiledAlpha[row] = 0.0f;
        }
    }
    sprite->mColor = barcolor | (u8)(s32)_menuFiledAlpha[row];
}

f32 _animAlpha(long row)
{
    _fieldMenuTimer += 3.1415927f * (2.0f * menuGetLastTime()) / _cursorSec;
    if (_fieldMenuTimer >= 6.2831855f) {
        _fieldMenuTimer -= 6.2831855f;
    }
    f32 range = _max_alpha - _min_alpha;
    f32 alpha = (1.0f + (f32)sin(_fieldMenuTimer)) / 2.0f;
    alpha = range * alpha;
    alpha += _min_alpha;
    if (alpha > 255.0f) {
        alpha = 255.0f;
    }
    _menuFiledAlpha[row] = alpha;
    return alpha;
}
