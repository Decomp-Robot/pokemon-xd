#ifndef MENU_POCKET_BATTLE_DISK_H
#define MENU_POCKET_BATTLE_DISK_H

#include <types.h>
#include <winSprite.h>
#include <hero.h>
#include <fightTrainer.h>

s32 getCursorPositionY(void);

#ifdef __cplusplus
extern "C" {
#endif
s32 menuPocketBattleDiskDrawRollDown(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
s32 menuPocketBattleDiskDrawRollUp(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
s32 menuPocketBattleDiskPageBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
s32 menuPocketBattleDiskCtrl(tagWINDOW_WORK *window);
s32 menuPocketBattleDiskPrintItemKind(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
s32 menuPocketBattleDiskPrintItemList(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite);
u16 menuPocketBattleDiskMain(s32 unused, FIGHT_TRAINER *trainer, Hero *hero);
#ifdef __cplusplus
}
#endif

#endif
