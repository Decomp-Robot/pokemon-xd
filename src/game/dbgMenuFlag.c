#include <types.h>
#include <winSprite.h>
#include <menu.h>
#include <menuSub.h>
#include <GSmsg.h>
#include <GSflag.h>
#include <stdio.h>

/* The shared flag catalog holds one linked record per flag entry. */
typedef struct {
    u8 attributes;
    u8 unknown01;
    u16 bitPosition;
    s16 nextId;
} FlagEntry;

extern FlagEntry *flag_data;

typedef struct {
    s16 top;
    s16 row;
} FlagCursor;

enum {
    FLAG_INFO_KEY_UP = 0x01,
    FLAG_INFO_KEY_DOWN = 0x02,
    FLAG_INFO_KEY_PAGE_UP = 0x04,
    FLAG_INFO_KEY_PAGE_DOWN = 0x08,
    FLAG_INFO_KEY_CLEAR = 0x40,
    FLAG_INFO_KEY_SET = 0x80,
    FLAG_INFO_ROW_HEIGHT = 13
};

#define FLAG_INFO_COLOR_SELECTED 0xFF0000FF
#define FLAG_INFO_COLOR_NORMAL 0xFFFFFFFF

char *_dbgMenuFlagInfoGetMsg(long listIndex);
u16 _dbgMenuFlagGetID(s32 cursor);

static FlagCursor _dbgMenuFlagCursor = { 0, 0 };

void dbgMenuFlagInfoDraw(tagWINDOW_WORK *window)
{
    long windowHeight;
    long maxRows;
    long visibleRows;
    long y;
    long row;
    long textColor;

    y = 40;
    maxRows = menuDataBiosGetDispMax(window->mMenuID);
    visibleRows = flag_data_number->count;

    if (visibleRows > maxRows) {
        visibleRows = maxRows;
    }
    windowHeight = visibleRows * FLAG_INFO_ROW_HEIGHT;
    menuSubDrawSimpleWindow(window->mLayerID, 30, 40, 125, windowHeight);
    if (_dbgMenuFlagCursor.top > 0) {
        menuSubDrawTriangle(window->mLayerID, 70, 33, 90, 33, 80, 23);
    }
    if (flag_data_number->count - _dbgMenuFlagCursor.top > maxRows) {
        menuSubDrawTriangle(window->mLayerID, 70, 40 + windowHeight + 7,
                            90, 40 + windowHeight + 7, 80, 40 + windowHeight + 17);
    }
    for (row = 0; row < visibleRows; row++) {
        if (_dbgMenuFlagCursor.row == row) {
            textColor = FLAG_INFO_COLOR_SELECTED;
        } else {
            textColor = FLAG_INFO_COLOR_NORMAL;
        }
        GSprint(window->mLayerID, 30, y, textColor,
                _dbgMenuFlagInfoGetMsg(row + _dbgMenuFlagCursor.top));
        GSprint(window->mLayerID, 130, y, textColor, "%d",
                GSflagGet(row + _dbgMenuFlagCursor.top));
        y += FLAG_INFO_ROW_HEIGHT;
    }
}

void dbgMenuFlagInfoCursor(tagWINDOW_WORK *window)
{
    const u16 repeatKeys = windowGetKeyInfo()->mKeyRepeat;
    const u16 heldKeys = windowGetKeyInfo()->mKeyOn;
    s16 flagCount = flag_data_number->count;
    s16 visibleRows = menuDataBiosGetDispMax(window->mMenuID);
    FlagCursor cursor;

    if (flagCount < visibleRows) {
        visibleRows = flagCount;
    }
    cursor = _dbgMenuFlagCursor;
    if (repeatKeys & FLAG_INFO_KEY_UP) {
        cursor.row--;
    } else if (repeatKeys & FLAG_INFO_KEY_DOWN) {
        cursor.row++;
    }
    if (repeatKeys & FLAG_INFO_KEY_PAGE_UP) {
        cursor.row -= menuDataBiosGetDispMax(window->mMenuID) - 1;
    } else if (repeatKeys & FLAG_INFO_KEY_PAGE_DOWN) {
        cursor.row += menuDataBiosGetDispMax(window->mMenuID) - 1;
    }
    if (cursor.row < 0) {
        cursor.top += cursor.row;
        cursor.row = 0;
        if (cursor.top < 0) {
            cursor.row = visibleRows - 1;
            cursor.top = flagCount - visibleRows;
        }
    } else if (cursor.row >= visibleRows) {
        cursor.top += cursor.row - (visibleRows - 1);
        cursor.row = visibleRows - 1;
        if (cursor.top + cursor.row >= flagCount) {
            cursor.top = 0;
            cursor.row = 0;
        }
    }
    if (heldKeys & FLAG_INFO_KEY_SET) {
        GSflagOn(cursor.top + cursor.row);
    } else if (heldKeys & FLAG_INFO_KEY_CLEAR) {
        GSflagOff(cursor.top + cursor.row);
    }
    _dbgMenuFlagCursor = cursor;
    window->mCursorIndex = cursor.top + cursor.row;
}

char *_dbgMenuFlagInfoGetMsg(long listIndex)
{
    static char buff[128];

    _dbgMenuFlagGetID(listIndex);
    sprintf(buff, "[%4d]", listIndex);
    return buff;
}

/* Id of the cursor-th entry of the flag list. */
u16 _dbgMenuFlagGetID(s32 cursor)
{
    s16 node;
    s32 i;

    if (cursor < 0) {
        return 0;
    }
    if (cursor >= flag_data_number->count) {
        return 0;
    }
    node = flag_data_number->head;
    if (cursor != 0) {
        for (i = 0; i < cursor - 1; i++) {
            node = flag_data[node].nextId;
        }
        node = flag_data[node].nextId;
    }
    return node;
}
