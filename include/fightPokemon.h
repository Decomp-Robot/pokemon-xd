#ifndef FIGHTPOKEMON_H
#define FIGHTPOKEMON_H

#include <Pokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FightPokemon FightPokemon;
typedef struct FightOutPokemon FightOutPokemon;

Pokemon *fightPokemonGetPokemonPtr(FightPokemon *pokemon);
u8 figthPokemonGetLevel(FightPokemon *pokemon);
u16 fightPokemonGetItemDataId(FightPokemon *pokemon);
u8 fightOutPokemonIsUseHensinBuff(FightOutPokemon *pokemon);
u16 fightOutPokemonGetTokuseiDataId(FightOutPokemon *pokemon);
void fightOutPokemonSetTokuseiDataId(FightOutPokemon *pokemon, u16 abilityId);
u16 fightOutPokemonGetZokuseiDataId(FightOutPokemon *pokemon, u8 index);
void fightOutPokemonSetZokuseiDataId(FightOutPokemon *pokemon, u8 index, u16 typeId);
void fightOutPokemonInitJoutaiDataId(FightOutPokemon *pokemon, u16 statusId);

#ifdef __cplusplus
}
#endif

#endif
