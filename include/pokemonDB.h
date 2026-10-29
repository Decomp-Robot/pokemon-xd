#ifndef POKEMONDB_H
#define POKEMONDB_H

#include <types.h>
#include <Pokemon.h>
#include <fightPokemon.h>
#include <fightWaza.h>
#include <itemFightItem.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    TOKUSEI_DATA_ID_SHADOW_TAG = 23,
    TOKUSEI_DATA_ID_LEVITATE = 26,
    TOKUSEI_DATA_ID_MAGNET_PULL = 42,
    TOKUSEI_DATA_ID_SOUNDPROOF = 43,
    TOKUSEI_DATA_ID_RUN_AWAY = 50,
    TOKUSEI_DATA_ID_ARENA_TRAP = 71,
    TOKUSEI_DATA_ID_BATTLE_ARMOR = 4,
    TOKUSEI_DATA_ID_SHELL_ARMOR = 75
};

enum PokemonDataID {
    POKEMON_DATA_ID_FARFETCHD = 83,
    POKEMON_DATA_ID_CHANSEY = 113,
    POKEMON_DATA_ID_LUGIA = 249,
    POKEMON_DATA_ID_NIDORAN_F = 29,
    POKEMON_DATA_ID_NIDORAN_M = 32,
    POKEMON_DATA_ID_CASTFORM = 385
};
u32 pokemonDB_GetName(u16);
bool pokemonDB_GetTeachWaza(u16, u16);

FightPokemon *fightOutPokemon_GetMotoFightPokemonPtr(FightOutPokemon *pokemon);
FIGHT_WAZA *fightOutPokemon_GetFightWazaPtr(FightOutPokemon *pokemon);
FIGHT_ITEM *fightOutPokemon_GetFightItemPtr(FightOutPokemon *pokemon);
void *fightOutPokemon_GetFightActionBuffPtr(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetUseWazaDataId(FightOutPokemon *pokemon);
s16 fightOutPokemon_GetMyselfDamageAtkValue(FightOutPokemon *pokemon);
s16 fightOutPokemon_GetMyselfDamageSpeValue(FightOutPokemon *pokemon);
s32 fightOutPokemon_GetKaigaraDamageValue(FightOutPokemon *pokemon);
void fightOutPokemon_SetKaigaraDamageValue(FightOutPokemon *pokemon, s32 damage);
u16 fightOutPokemon_GetFightoutTurnCount(FightOutPokemon *pokemon);
void fightOutPokemon_SetFightoutTurnCount(FightOutPokemon *pokemon, u16 count);
u8 fightOutPokemon_GetNamakeFlag(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetMotoWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetOumuWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetOumuWazaAllyDataId(FightOutPokemon *pokemon);
void fightOutPokemon_SetNoPressureFlag(FightOutPokemon *pokemon, u32 flag);
u16 fightOutPokemon_GetLastUseWazaDataId(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetSuccessCnt(FightOutPokemon *pokemon);
void fightOutPokemon_SetSuccessCnt(FightOutPokemon *pokemon, u8 count);

u16 pokemon_GetPokemonWazaDataId(Pokemon *pokemon, u16 slot);
u8 pokemon_GetPokemonWazaPp(Pokemon *pokemon, u16 slot);
void pokemon_SetPokemonWazaPp(Pokemon *pokemon, u16 slot, u8 pp);
u32 pokemon_GetTokuseiFlag(void *pokemonData);
u8 pokemonDB_GetTokuseiDataId(u16 species, u16 index);
Pokemon *fightPokemon_GetPokemonBuffPtr(FightPokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
