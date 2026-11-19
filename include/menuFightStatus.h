#ifndef MENU_FIGHT_STATUS_H
#define MENU_FIGHT_STATUS_H

#include <bool.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void menuFightStatusSetActive(u32 menuId, bool active);
void menuFightStatusSetHP(u32 menuId, s16 hp);
s32 menuFightStatusWaitAnimeEXP(u32 menuId, u8 wait);
void menuFightStatusStartAnimEXP(u32 menuId, u32 exp);
s32 menuFightStatusWaitAnimeHP(u32 menuId, u8 wait);
void menuFightStatusStartAnimHP(u32 menuId, s16 hp);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// Accumulates the elapsed fight time shown on the status screen.
void menuUpdateStatusTime(void);
#endif

#endif
