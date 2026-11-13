#ifndef MENU_PDA_MAIL_XD_H
#define MENU_PDA_MAIL_XD_H

#include <winSprite.h>

#ifdef __cplusplus
extern "C" {
#endif

long menuPdaMailCallBack(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuPdaMailXDScript(s32 mailIndex);
CURSOR *menuPdaMailXD(CURSOR *cursor, u8 cursorMove, u32 value);
void menuPdaMailContentsButtonXD(tagWINDOW_WORK *window);
long menuPdaMailContentsCursorXD(tagWINDOW_WORK *window);
long menuPdaMailCtrlXD(tagWINDOW_WORK *window);

#ifdef __cplusplus
}
#endif

#endif
