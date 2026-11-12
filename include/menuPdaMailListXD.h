#ifndef MENU_PDA_MAIL_LIST_XD_H
#define MENU_PDA_MAIL_LIST_XD_H

#include <winSprite.h>

extern "C" {
void menuPdaMailListButtonXD(tagWINDOW_WORK *window);
long menuPdaMailListCursorXD(tagWINDOW_WORK *window);
long menuPdaMailListCallBack(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
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

#endif
