#ifndef MENU_TOP_H
#define MENU_TOP_H

#include <types.h>
#include <winSprite.h>

#ifdef __cplusplus
extern "C" {
#endif

void menuTopJumpVSMode(void);
void menuTopSelectCtrl(tagWINDOW_WORK *control);
void menuTopLoadHook(void);
void menuTopSelectMessageHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTopBattleSelectMessageHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTopBattleSelectHook(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTopPrintItemMessage(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, s32 msgID);
u8 menuTopShouldKeepSelectResources(void);
s32 menuTopRunBattleSelect(void);
void menuTopBattleSelectCtrl(tagWINDOW_WORK *control);
s32 menuTopSelect(void);
void menuTopSetInitCursor(s32 cursor);

#ifdef __cplusplus
}
#endif

#endif
