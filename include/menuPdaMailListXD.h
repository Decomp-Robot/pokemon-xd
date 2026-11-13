#ifndef MENU_PDA_MAIL_LIST_XD_H
#define MENU_PDA_MAIL_LIST_XD_H

#include <winSprite.h>

// Scroll work of the mail list window: the cursor row, the position the list
// scrolls to and the position it is drawn at, plus the row count of the last
// refresh of the list.
typedef struct MAIL_LIST_WORK {
    CURSOR mCursor;
    f32 mScrollX;
    f32 mScrollY;
    f32 mTargetScrollX;
    f32 mTargetScrollY;
    s32 mScrollState;
    s32 mVisibleCount;
} MAIL_LIST_WORK;

extern MAIL_LIST_WORK _maillistwork;
// Alpha the list draws each of its visible rows with.
extern f32 _menuPdaMailAlpha[5];

extern "C" {
void menuPdaMailListButtonXD(tagWINDOW_WORK *window);
long menuPdaMailListCursorXD(tagWINDOW_WORK *window);
long menuPdaMailListCallBack(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
long menuPdaMailListCtrlXD(tagWINDOW_WORK *window);
long menuPdaMailListSubXD(long row, CURSOR *cursor);
void menuPdaMailListXD(void);
}

CURSOR _getCursor(void);
void _setCursor(CURSOR cursor);
void _pdaMailListDownCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _pdaMailListUpCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _pdaMailListCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _pdaMailListScrollBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _pdaMailListPageBunshi(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _pdaMailListPage(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _pdaMailListPageBunbo(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _pdaMailListDraw(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void _menuPdaMailListXDInit(void);
void _menuPdaMailListMain(tagWINDOW_WORK *window);
void _menuPdaMailListCalcPosition(CURSOR *cursor);

#endif
