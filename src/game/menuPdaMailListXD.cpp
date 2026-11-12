#include <menuPdaMailListXD.h>
#include <window.h>
#include <winSeq.h>
#include <GSsndPlayback.h>

extern "C" s32 mailGetNbMailInMailbox(void);

enum {
    MAIL_LIST_VISIBLE_COUNT = 5,
    MAIL_LIST_SOUND_CURSOR = 1104,
    MAIL_LIST_SOUND_PAGE = 1107,
    MAIL_LIST_ITEM_CURSOR_ROW_1 = 129,
    MAIL_LIST_ITEM_CURSOR_ROW_2 = 130,
    MAIL_LIST_ITEM_CURSOR_ROW_3 = 131,
    MAIL_LIST_ITEM_CURSOR_ROW_4 = 132,
    MAIL_LIST_ITEM_CURSOR_ROW_5 = 133,
    MAIL_LIST_ITEM_UP = 139,
    MAIL_LIST_ITEM_DOWN = 140,
    MAIL_LIST_ITEM_SCROLL_BAR = 141,
    MAIL_LIST_ITEM_ENTRY = 151,
    MAIL_LIST_ITEM_LIGHT_COLOR = 152,
    MAIL_LIST_ITEM_GOLD_COLOR = 153,
    MAIL_LIST_ITEM_PAGE_COUNT = 154,
    MAIL_LIST_ITEM_PAGE_SEPARATOR = 155,
    MAIL_LIST_ITEM_PAGE_NUMBER = 156
};

void menuPdaMailListButtonXD(tagWINDOW_WORK *window)
{
    if (window != 0) {
        s32 count = mailGetNbMailInMailbox();
        if (!winSeqCheckMove(window->mMenuID)) {
            u16 key = windowGetKeyInfo()->mKeyTrg;
            if (count != 0 && (key & WINDOW_KEY_ACCEPT)) {
                window->mAccept = 1;
            }
            if (key & WINDOW_KEY_CANCEL) {
                window->mAccept = 1;
                window->mCancel = 1;
            }
        }
    }
}

long menuPdaMailListCursorXD(tagWINDOW_WORK *window)
{
    windowGetKeyInfo();
    CURSOR cursor = _getCursor();
    cursor.x = window->mCursor.x;
    cursor.y = window->mCursor.y;
    s32 count = mailGetNbMailInMailbox();
    s32 visibleCount = MAIL_LIST_VISIBLE_COUNT;
    if (count < MAIL_LIST_VISIBLE_COUNT) {
        visibleCount = count;
    }
    u16 key = windowGetKeyRepeat();
    if (visibleCount != 0) {
        if (key & WINDOW_KEY_UP) {
            if (cursor.y + cursor.x != 0) {
                GSsndPlayNow(MAIL_LIST_SOUND_CURSOR);
            }
            cursor.y--;
        } else if (key & WINDOW_KEY_DOWN) {
            if (count - 1 != cursor.y + cursor.x) {
                GSsndPlayNow(MAIL_LIST_SOUND_CURSOR);
            }
            cursor.y++;
        } else if (key & WINDOW_KEY_RIGHT) {
            if (count - 1 != cursor.y + cursor.x) {
                GSsndPlayNow(MAIL_LIST_SOUND_PAGE);
            }
            cursor.y += visibleCount;
        } else if (key & WINDOW_KEY_LEFT) {
            if (cursor.y + cursor.x != 0) {
                GSsndPlayNow(MAIL_LIST_SOUND_PAGE);
            }
            cursor.y -= visibleCount;
        }
        if (cursor.y < 0) {
            cursor.x += cursor.y;
            cursor.y = 0;
            if (cursor.x < 0) {
                cursor.x = 0;
                cursor.y = 0;
            }
        } else if (cursor.y >= visibleCount) {
            cursor.x = cursor.x + (cursor.y - (visibleCount - 1));
            cursor.y = (s8)(visibleCount - 1);
            if (cursor.x + cursor.y >= count) {
                cursor.x = (s8)(count - visibleCount);
                cursor.y = (s8)(visibleCount - 1);
            }
        }
    }
    _setCursor(cursor);
    window->mCursor.x = cursor.x;
    window->mCursor.y = cursor.y;
    return 0;
}

long menuPdaMailListCallBack(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    switch (sprite->mItemID) {
    case MAIL_LIST_ITEM_SCROLL_BAR:
        _pdaMailListScrollBar(window, sprite);
        break;
    case MAIL_LIST_ITEM_PAGE_NUMBER:
        _pdaMailListPageBunshi(window, sprite);
        break;
    case MAIL_LIST_ITEM_PAGE_SEPARATOR:
        _pdaMailListPage(window, sprite);
        break;
    case MAIL_LIST_ITEM_PAGE_COUNT:
        _pdaMailListPageBunbo(window, sprite);
        break;
    case MAIL_LIST_ITEM_ENTRY:
        _pdaMailListDraw(window, sprite);
        break;
    case MAIL_LIST_ITEM_LIGHT_COLOR:
        sprite->mRgba.mRed = 240;
        sprite->mRgba.mGreen = 240;
        sprite->mRgba.mBlue = 240;
        break;
    case MAIL_LIST_ITEM_GOLD_COLOR:
        sprite->mRgba.mRed = 213;
        sprite->mRgba.mGreen = 170;
        sprite->mRgba.mBlue = 51;
        break;
    case MAIL_LIST_ITEM_CURSOR_ROW_1:
    case MAIL_LIST_ITEM_CURSOR_ROW_2:
    case MAIL_LIST_ITEM_CURSOR_ROW_3:
    case MAIL_LIST_ITEM_CURSOR_ROW_4:
    case MAIL_LIST_ITEM_CURSOR_ROW_5:
        _pdaMailListCursor(window, sprite);
        break;
    case MAIL_LIST_ITEM_UP:
        _pdaMailListUpCursor(window, sprite);
        break;
    case MAIL_LIST_ITEM_DOWN:
        _pdaMailListDownCursor(window, sprite);
        break;
    }
    return 0;
}
