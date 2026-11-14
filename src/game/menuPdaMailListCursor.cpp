#include <menuPdaMailListXD.h>

#include <GSmem.h>
#include <GSmsg.h>
#include <msgctrl.h>
#include <menu.h>
#include <menuItemBios.h>
#include <sprite.h>
#include <menuPdaSub.h>
#include <winSeq.h>
#include <winSprite.h>

extern "C" s32 mailGetNbMailInMailbox(void);
extern "C" CURSOR *menuPdaMailXD(CURSOR *cursor, bool check, long flag);
void _pdaMailSort(void);
u16 *_sortwork;
static void _menuClose(long menuID);
void _menuPdaMailListPosition(void);
void _menuPdaMailListCalcPosition(CURSOR *cursor);
void _menuPdaMailCalc(f32 *position, f32 target, f32 speed);
u32 _getSubject(s32 index);
u32 _getSenderName(s32 index);
s32 _getMailID(s32 index);
extern "C" s32 pdaMailGetMailID(s32 index);

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

extern "C" {
long mailGetSortMode(void);
s32 mailGetMailIDInMailbox(s32 index);
u32 mailGetSenderName(s32 mailID);
u32 mailGetSubject(s32 mailID);
}

static const s32 MAIL_LIST_ROW_HEIGHT = 47;
static const s32 MAIL_LIST_SENDER_X_OFFSET = 47;
enum {
    MAIL_LIST_TEXT_MESSAGE = 17014,
    MAIL_LIST_PAGE_MESSAGE = 17007,
    MAIL_LIST_CURSOR_SLOT = 10,
    MAIL_LIST_CLIP_ITEM = 112
};
static const f32 MAIL_LIST_SECOND_LINE_Y = 21.0f;
static const f32 MAIL_LIST_SCROLL_SPEED = 20.0f;
static const u32 MAIL_LIST_SUBJECT_COLOR = 0xD5AA3300u;
static const u32 MAIL_LIST_SENDER_COLOR = 0xF0F0F000u;
static const u32 MAIL_LIST_PAGE_COLOR = 0xFFFFFF00u;

void _menuPdaMailListMain(tagWINDOW_WORK *window)
{
    CURSOR cursor = _getCursor();
    _menuPdaMailListPosition();
    _menuPdaMailListCalcPosition(&cursor);
    menuPdaSubCalcAlpha(_menuPdaMailAlpha, MAIL_LIST_VISIBLE_COUNT, cursor.y);
}

void _menuPdaMailListCalcPosition(CURSOR *cursor)
{
    _maillistwork.mTargetScrollY = -cursor->x * MAIL_LIST_ROW_HEIGHT;
}

void _menuPdaMailListPosition(void)
{
    if (_maillistwork.mScrollX != _maillistwork.mTargetScrollX)
        _menuPdaMailCalc(&_maillistwork.mScrollX, _maillistwork.mTargetScrollX, MAIL_LIST_SCROLL_SPEED);
    if (_maillistwork.mScrollY != _maillistwork.mTargetScrollY)
        _menuPdaMailCalc(&_maillistwork.mScrollY, _maillistwork.mTargetScrollY, MAIL_LIST_SCROLL_SPEED);
}

void _menuPdaMailCalc(f32 *position, f32 target, f32 speed)
{
    menuPdaSubCalcPositon(position, target, speed);
}

long _pdaMailListDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = _getCursor();
    s32 count = mailGetNbMailInMailbox();
    u32 subjectColor = MAIL_LIST_SUBJECT_COLOR | window->mRgba.mColorA;
    u32 senderColor = MAIL_LIST_SENDER_COLOR | window->mRgba.mColorA;
    tagMENU_ITEM_DATA *area = menuItemBiosGetPtr(MAIL_LIST_SUB_MENU_ID, MAIL_LIST_CLIP_ITEM);
    tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(MAIL_LIST_SUB_MENU_ID, sprite->mItemID);
    spriteSetScissor(window->mLayerID, area->mX - item->mX, area->mY - item->mY, area->mWidth, area->mHeight);
    f32 y = _maillistwork.mScrollY;
    s32 x = (s32)_maillistwork.mScrollX;
    s32 secondX = (s32)(MAIL_LIST_SENDER_X_OFFSET + _maillistwork.mScrollX);
    for (s32 i = 0; i < count; i++) {
        if (i >= cursor.x - 1 && i < cursor.x + 6) {
            u32 message = _getSubject(i);
            if (message) {
                u32 text = (u32)GSmsgGetStringPtr(message);
                msgctrlSetValue(MSGCTRL_TEXT, text);
                GSmsgPrint2(window->mLayerID, x, (s32)y, subjectColor | window->mRgba.mColorA, MAIL_LIST_TEXT_MESSAGE);
            } else GSmsgPrint2(window->mLayerID, x, (s32)y, subjectColor | window->mRgba.mColorA, 0);
            message = _getSenderName(i);
            if (message) {
                msgctrlSetValue(MSGCTRL_TEXT, (u32)GSmsgGetStringPtr(message));
                GSmsgPrint2(window->mLayerID, secondX, (s32)(MAIL_LIST_SECOND_LINE_Y + y), senderColor | window->mRgba.mColorA, MAIL_LIST_TEXT_MESSAGE);
            } else GSmsgPrint2(window->mLayerID, secondX, (s32)(MAIL_LIST_SECOND_LINE_Y + y), senderColor | window->mRgba.mColorA, 0);
        }
        y += MAIL_LIST_ROW_HEIGHT;
    }
    spriteClearScissor(window->mLayerID);
    return 0;
}

long _pdaMailListPageBunbo(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 count = mailGetNbMailInMailbox();
    if (count != 0) {
        msgctrlSetValue(MSGCTRL_NUM_PANEL, count);
        s32 width = GSmsgGetMessageSize(MAIL_LIST_PAGE_MESSAGE) >> 16;
        GSmsgPrint2(window->mLayerID, sprite->mWidth - width, 0,
                    MAIL_LIST_PAGE_COLOR | window->mRgba.mColorA, MAIL_LIST_PAGE_MESSAGE);
    }
    return 0;
}

long _pdaMailListPageBunshi(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    s32 count = mailGetNbMailInMailbox();
    if (count != 0) {
        msgctrlSetValue(MSGCTRL_NUM_PANEL, window->mCursor.x + window->mCursor.y + 1);
        s32 width = GSmsgGetMessageSize(MAIL_LIST_PAGE_MESSAGE) >> 16;
        GSmsgPrint2(window->mLayerID, sprite->mWidth - width, 0,
                    MAIL_LIST_PAGE_COLOR | window->mRgba.mColorA, MAIL_LIST_PAGE_MESSAGE);
    }
    return 0;
}

void _pdaMailListPage(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    if (mailGetNbMailInMailbox() != 0) winSpriteSetDisp(sprite, 1);
    else winSpriteSetDisp(sprite, 0);
}

static void _menuClose(long menuID)
{
    menuClose(menuID);
    menuCloseSync(menuID, 1);
}

#pragma dont_inline on
void _sortReceive(u16 *work)
{
    s32 index = mailGetNbMailInMailbox() - 1;
    while (index >= 0) {
        u16 mailID = mailGetMailIDInMailbox(index);
        index--;
        *work++ = mailID;
    }
}

void _pdaMailSort(void)
{
    u16 *work = _sortwork;
    mailGetSortMode();
    _sortReceive(work);
}
#pragma dont_inline reset

void _setCursor(CURSOR cursor) { cursorBiosSetPos(MAIL_LIST_CURSOR_SLOT, cursor); }
CURSOR _getCursor(void) { return cursorBiosGetPos(MAIL_LIST_CURSOR_SLOT); }

u32 _getSenderName(s32 index) { return mailGetSenderName(_getMailID(index)); }
u32 _getSubject(s32 index) { return mailGetSubject(_getMailID(index)); }
s32 _getMailID(s32 index) { return pdaMailGetMailID(index); }

extern "C" s32 pdaMailGetMailID(s32 index)
{
    if (index < 0 || index >= mailGetNbMailInMailbox()) return -1;
    return _sortwork[index];
}
