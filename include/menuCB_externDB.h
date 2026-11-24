#ifndef MENU_CB_EXTERN_DB_H
#define MENU_CB_EXTERN_DB_H

#include <winSprite.h>

#ifdef __cplusplus
extern "C" {
#endif

void menuCB_BattleBeforeAfter3_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTool_BattleBA03_Button(tagWINDOW_WORK *window);
void menuTool_BattleBA03_Ctrl(tagWINDOW_WORK *window);
void menuTool_BattleBA01_Button(tagWINDOW_WORK *window);
void menuTool_BattleBA01_Ctrl(tagWINDOW_WORK *window);
void menuCB_GBAConnect_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTool_GBAConnect_Button(tagWINDOW_WORK *window);
void menuTool_GBAConnect_Ctrl(tagWINDOW_WORK *window);
void menuCB_error01_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTool_error01_Button(tagWINDOW_WORK *window);
void menuTool_error01_Ctrl(tagWINDOW_WORK *window);
void menuCB_error00_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTool_error00_Button(tagWINDOW_WORK *window);
void menuTool_error00_Ctrl(tagWINDOW_WORK *window);
void menuCB_Tunaide03_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTool_tunaide03_Button(tagWINDOW_WORK *window);
void menuTool_tunaide03_Ctrl(tagWINDOW_WORK *window);
void menuCB_Tunaide02_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTool_tunaide02_Button(tagWINDOW_WORK *window);
void menuTool_tunaide02_Ctrl(tagWINDOW_WORK *window);
void menuCB_Tunaide01_4_Disp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
void menuTool_tunaide01_4_Button(tagWINDOW_WORK *window);
void menuTool_tunaide01_4_Ctrl(tagWINDOW_WORK *window);
void menuCBRuleSetSpriteColor(tagSPRITE_WORK *sprite, bool selected);

#ifdef __cplusplus
}
#endif

#endif
