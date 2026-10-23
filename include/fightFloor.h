#ifndef FIGHTFLOOR_H
#define FIGHTFLOOR_H

#include <fightTrainer.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *fightFloorGetPtr(void);
FIGHT_TRAINER *fightFloorGetGcHeroFightTrainerPtr(s32 flag);
void *fightFloorGetNowPtr(void);
Pokemon *fightFloorGetDefPokemonPtr(void *floor);
u32 fightFloorGetStatus(void *floor, u16 id, u16 field, u16 index);
void fightFloorSetTimeOutAllFightResult(void *floor);
FIGHT_TRAINER *fightFloorGetFightPokemonPtrToFightTrainerPtr(void *floor,
    FightPokemon *pokemon);

extern u32 *fight_floor_data_number;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
s32 fightFloor_GetStockPokemonNum(void *floor);
u32 fightFloor_GetFightKind(void *floor);
#ifdef __cplusplus
}
#endif
#endif
