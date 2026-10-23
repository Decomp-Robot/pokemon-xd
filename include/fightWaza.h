#ifndef FIGHT_WAZA_H
#define FIGHT_WAZA_H

#include <types.h>

typedef struct FIGHT_WAZA FIGHT_WAZA;

#ifdef __cplusplus
extern "C" {
#endif

u8 fightWazaIsHit(FIGHT_WAZA *waza);
void fightWazaSetUseWazaStatus(FIGHT_WAZA *waza, u16 move);

#ifdef __cplusplus
}
#endif

#endif
