#ifndef MENU_CB_BATTLE_H
#define MENU_CB_BATTLE_H

#include <types.h>
struct _MENUCBWORK;

#ifdef __cplusplus
extern "C" {
#endif
s32 menuCB_Battle(struct _MENUCBWORK *work);
void menuCB_InitBattle(struct _MENUCBWORK *work);
s32 menuCB_BattleResult(struct _MENUCBWORK *work);
#ifdef __cplusplus
}
#endif

#endif
