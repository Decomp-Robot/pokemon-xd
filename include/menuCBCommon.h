#ifndef MENU_CB_COMMON_H
#define MENU_CB_COMMON_H

#include <types.h>

struct tagSPRITE_WORK;
struct MenuSpriteData;

#ifdef __cplusplus
extern "C" {
#endif

s32 menuCB_MessageDispCheck(void);
void menuCB_MessageDispOff(void);
void menuCB_MessageDispOn(s32 msgID, s32 windowID);
s32 menuCB_GetDisconnectedGBA(void);
void menuCBSpriteSetData(struct tagSPRITE_WORK *sprite, struct MenuSpriteData *data);
s32 menuCB_MenuRollback(s32 menuID);
s32 menuCB_MenuPop(void);
void menuCB_MenuPush(s32 menuID);
s32 menuCB_CurrentMenuID(void);
void menuCB_InitMenu(s32 menuID);

#ifdef __cplusplus
}
#endif

#endif
