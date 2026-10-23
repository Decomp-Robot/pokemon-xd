#ifndef FIGHT_ENCOUNT_DATA_H
#define FIGHT_ENCOUNT_DATA_H

#include <types.h>
#include <fightEncountBios.h>

enum FightBattleKind {
    FIGHT_BATTLE_KIND_WILD_POKEMON = 12,
    FIGHT_BATTLE_KIND_BATTLE_BINGO = 13
};

#ifdef __cplusplus
extern "C" {
#endif
void fightEncountDataBiosSetDiskId(void *data, s32 id);
void fightEncountDataSetEncounterId(void *data, s32 id);
u32 fightEncountDataBiosGetBattleKind(void* data);
void fightEncountDataBiosSetBattleKind(void* data, u8 value);
u32 fightEncountDataBiosGetTrainersPerSide(void* data);
void fightEncountDataBiosSetTrainersPerSide(void* data, u8 value);
u32 fightEncountDataBiosGetPokemon(void* data);
void fightEncountDataBiosSetBattleStyle(void* data, u8 value);
u32 fightEncountDataBiosGetPokemonPerPlayer(void* data);
void fightEncountDataBiosSetPokemonPerPlayer(void* data, u8 value);
u32 fightEncountDataBiosGetBattleField(void* data);
void fightEncountDataBiosSetBattleField(void* data, u16 value);
void fightEncountDataBiosSetFightTrainerDataId(void* data, s32 player, u16 trainerDataId);
u32 fightEncountDataBiosGetPlayerController(void* data, s32 player);
void fightEncountDataBiosSetPlayerController(void* data, s32 player, s32 devId);
u8 *fightEncountDataBiosGetPtr(u16 encounterID);
#ifdef __cplusplus
}
#endif

#endif
