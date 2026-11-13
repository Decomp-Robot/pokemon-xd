#include <menuPdaMailListXD.h>

#include <GSmem.h>
#include <menu.h>
#include <menuPdaSub.h>
#include <winSeq.h>
#include <winSprite.h>

extern "C" s32 mailGetNbMailInMailbox(void);
extern "C" CURSOR *menuPdaMailXD(CURSOR *cursor, bool check, long flag);
void _pdaMailSort(void);
extern u16 *_sortwork;
extern "C" void _menuClose(long menuID);

enum {
    MAIL_LIST_VISIBLE_COUNT = 5,
    MAIL_LIST_ITEM_NONE = 0,
    MAIL_LIST_ITEM_CURSOR_ROW_1 = 129,
    MAIL_LIST_ITEM_CURSOR_ROW_2 = 130,
    MAIL_LIST_ITEM_CURSOR_ROW_3 = 131,
    MAIL_LIST_ITEM_CURSOR_ROW_4 = 132,
    MAIL_LIST_ITEM_CURSOR_ROW_5 = 133,
    MAIL_LIST_ITEM_SCROLL_BAR = 141,
    MAIL_LIST_SUB_MENU_ID = 110,
    MAIL_LIST_MENU_ID = 119,
    MAIL_LIST_SEQUENCE_OPEN = 306,
    MAIL_LIST_SEQUENCE_CLOSE = 310,
    MAIL_LIST_SCROLL_BAR_COLOR = 0xF2642200u
};

// Scroll work of the mail list window.
MAIL_LIST_WORK _maillistwork;
// Alpha every visible row of the list is drawn with.
f32 _menuPdaMailAlpha[5];

// Scroll arrow hooks of the PDA mail list: each shows or hides the arrow
// while the cursor stands at the down or up end of the list.
void _pdaMailListDownCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = _getCursor();
    s32 count = mailGetNbMailInMailbox();
    menuPdaSubCheckUpDownCursor(window, sprite, &cursor, count, MAIL_LIST_VISIBLE_COUNT, 0);
}

void _pdaMailListUpCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = _getCursor();
    s32 count = mailGetNbMailInMailbox();
    menuPdaSubCheckUpDownCursor(window, sprite, &cursor, count, MAIL_LIST_VISIBLE_COUNT, 1);
}

// Row cursor of the mail list: the row the cursor stands on is shown, the
// other rows fade out with the alpha the list holds for them.
void _pdaMailListCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    const s32 rowItemID[6] = {
        MAIL_LIST_ITEM_CURSOR_ROW_1, MAIL_LIST_ITEM_CURSOR_ROW_2,
        MAIL_LIST_ITEM_CURSOR_ROW_3, MAIL_LIST_ITEM_CURSOR_ROW_4,
        MAIL_LIST_ITEM_CURSOR_ROW_5, MAIL_LIST_ITEM_NONE
    };
    s32 count = mailGetNbMailInMailbox();
    s32 index;
    s32 row = window->mCursor.y;

    if (count == 0) {
        row = 0;
    }
    for (index = 0; index < MAIL_LIST_VISIBLE_COUNT; index++) {
        if (sprite->mItemID == rowItemID[index]) {
            break;
        }
    }
    sprite->mRgba.mAlpha = _menuPdaMailAlpha[index];
    if (row == index) {
        winSpriteSetDisp(sprite, 1);
    } else if (_menuPdaMailAlpha[index] == 0.0f) {
        winSpriteSetDisp(sprite, 0);
    }
}

// Scroll bar of the mail list: the bar follows the row the cursor stands on.
void _pdaMailListScrollBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 count = mailGetNbMailInMailbox();
    CURSOR cursor = _getCursor();

    menuPdaSubScrollBar(window, sprite, MAIL_LIST_SUB_MENU_ID, MAIL_LIST_ITEM_SCROLL_BAR,
                        MAIL_LIST_SCROLL_BAR_COLOR, count, MAIL_LIST_VISIBLE_COUNT,
                        (GSivec2 *)&cursor);
}

// Window control of the mail list: plays the open and close sequences of the
// window and refreshes the list while it is up.
long menuPdaMailListCtrlXD(tagWINDOW_WORK *window)
{
    switch (window->mState) {
    case WINDOW_STATE_INIT:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MAIL_LIST_SEQUENCE_OPEN);
            window->mFlag = 1;
        }
        break;
    case WINDOW_STATE_ACTIVE:
        _menuPdaMailListMain(window);
        break;
    case WINDOW_STATE_CLOSING:
        if (window->mFlag == 0) {
            winSeqSetMenu(window->mMenuID, MAIL_LIST_SEQUENCE_CLOSE);
            window->mFlag = 1;
        }
        break;
    }
    return 0;
}

// Runs the mail list until it is cancelled or the cursor is within the visible
// rows, then returns the selected mailbox index or -1 on cancellation/empty list.
#pragma dont_inline on
long menuPdaMailListSubXD(long row, CURSOR *cursor)
{
    CURSOR cur;
    s32 ret;

    cur.x = cursor->x;
    cur.y = cursor->y;
    _setCursor(cur);
    while (true) {
        ret = menuOpen(MAIL_LIST_SUB_MENU_ID, 1);
        cur = _getCursor();
        if (ret == -1) {
            break;
        }
        if (cur.y < MAIL_LIST_VISIBLE_COUNT) {
            break;
        }
    }
    _menuClose(MAIL_LIST_SUB_MENU_ID);
    if (ret == -1 || mailGetNbMailInMailbox() == 0) {
        return -1;
    }
    cursor->y = cur.y;
    cursor->x = cur.x;
    return cur.x + cur.y;
}
#pragma dont_inline reset

// Mail list of the PDA: sorts the mailbox into the mail buffer, runs the list
// until the player leaves it, and releases the buffer again.
void menuPdaMailListXD(void)
{
    s32 row = 0;
    s32 count = mailGetNbMailInMailbox();
    CURSOR cursor;

    if (count > 0) {
        _sortwork = (u16 *)GSmemAlloc(count * 2);
    } else {
        _sortwork = 0;
    }
    _menuPdaMailListXDInit();
    _pdaMailSort();
    cursor.x = cursor.y = 0;
    menuOpen(MAIL_LIST_MENU_ID, 0);
    while (true) {
        row = menuPdaMailListSubXD(row, &cursor);
        if (row < 0) {
            break;
        }
        CURSOR *next = menuPdaMailXD(&cursor, 1, 0);
        cursor.y = next->y;
        cursor.x = next->x;
        menuPdaSubInitAlpha(_menuPdaMailAlpha, MAIL_LIST_VISIBLE_COUNT, cursor.y);
        _menuPdaMailListCalcPosition(&cursor);
        _maillistwork.mScrollY = _maillistwork.mTargetScrollY;
    }
    _menuClose(MAIL_LIST_MENU_ID);
    if (count > 0 && _sortwork != 0) {
        GSmemFree(_sortwork);
        _sortwork = 0;
    }
}

// Sets up the scroll work of the mail list for a freshly opened list.
void _menuPdaMailListXDInit(void)
{
    s32 count = mailGetNbMailInMailbox();

    _maillistwork.mCursor.x = _maillistwork.mCursor.y = 0;
    _maillistwork.mScrollX = 0.0f;
    _maillistwork.mScrollY = 0.0f;
    _maillistwork.mTargetScrollX = 0.0f;
    _maillistwork.mTargetScrollY = 0.0f;
    _maillistwork.mScrollState = 0;
    if (count >= MAIL_LIST_VISIBLE_COUNT) {
        _maillistwork.mVisibleCount = MAIL_LIST_VISIBLE_COUNT;
    } else {
        _maillistwork.mVisibleCount = count;
    }
    menuPdaSubInitAlpha(_menuPdaMailAlpha, MAIL_LIST_VISIBLE_COUNT, 0);
}
