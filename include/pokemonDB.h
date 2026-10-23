#ifndef POKEMONDB_H
#define POKEMONDB_H

#include <types.h>
#include <Pokemon.h>
#include <fightPokemon.h>
#include <fightWaza.h>

#ifdef __cplusplus
extern "C" {
#endif
u32 pokemonDB_GetName(u16);
bool pokemonDB_GetTeachWaza(u16, u16);

FightPokemon *fightOutPokemon_GetMotoFightPokemonPtr(FightOutPokemon *pokemon);
FIGHT_WAZA *fightOutPokemon_GetFightWazaPtr(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetUseWazaDataId(FightOutPokemon *pokemon);
s16 fightOutPokemon_GetMyselfDamageAtkValue(FightOutPokemon *pokemon);
s16 fightOutPokemon_GetMyselfDamageSpeValue(FightOutPokemon *pokemon);
s32 fightOutPokemon_GetKaigaraDamageValue(FightOutPokemon *pokemon);
void fightOutPokemon_SetKaigaraDamageValue(FightOutPokemon *pokemon, s32 damage);
u16 fightOutPokemon_GetFightoutTurnCount(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetOumuWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetOumuWazaAllyDataId(FightOutPokemon *pokemon);
void fightOutPokemon_SetNoPressureFlag(FightOutPokemon *pokemon, u32 flag);
u16 fightOutPokemon_GetLastUseWazaDataId(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetSuccessCnt(FightOutPokemon *pokemon);
void fightOutPokemon_SetSuccessCnt(FightOutPokemon *pokemon, u8 count);

u16 pokemon_GetPokemonWazaDataId(Pokemon *pokemon, u16 slot);
u8 pokemon_GetPokemonWazaPp(Pokemon *pokemon, u16 slot);
void pokemon_SetPokemonWazaPp(Pokemon *pokemon, u16 slot, u8 pp);
#ifdef __cplusplus
}
#endif

#endif
