#ifndef FIGHT_FLOOR_H
#define FIGHT_FLOOR_H

#include <types.h>
#include <fightTrainer.h>

#ifdef __cplusplus
extern "C" {
#endif
FIGHT_TRAINER *fightFloorGetGcHeroFightTrainerPtr(s32 flag);
void *fightFloorGetNowPtr(void);
#ifdef __cplusplus
}
#endif
#include <fightEncountData.h>

struct FightFloor;

extern "C" {
s32 fightFloor_GetStockPokemonNum(FightFloor *floor);
u32 fightFloor_GetFightKind(void *floor);
}

#endif
