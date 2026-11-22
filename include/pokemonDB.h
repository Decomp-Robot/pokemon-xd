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

enum TOKUSEI_DATA_ID {
    TOKUSEI_DATA_ID_SAND_VEIL = 8,
    TOKUSEI_DATA_ID_CLOUD_NINE = 13,
    TOKUSEI_DATA_ID_SWIFT_SWIM = 33,
    TOKUSEI_DATA_ID_CHLOROPHYLL = 34,
    TOKUSEI_DATA_ID_RAIN_DISH = 44,
    TOKUSEI_DATA_ID_AIR_LOCK = 77,
    TOKUSEI_DATA_ID_SHADOW_TAG = 23,
    TOKUSEI_DATA_ID_LEVITATE = 26,
    TOKUSEI_DATA_ID_MAGNET_PULL = 42,
    TOKUSEI_DATA_ID_SOUNDPROOF = 43,
    TOKUSEI_DATA_ID_RUN_AWAY = 50,
    TOKUSEI_DATA_ID_ARENA_TRAP = 71,
    TOKUSEI_DATA_ID_BATTLE_ARMOR = 4,
    TOKUSEI_DATA_ID_SHELL_ARMOR = 75,
    TOKUSEI_DATA_ID_DAMP = 6,
    TOKUSEI_DATA_ID_SHIELD_DUST = 19,
    TOKUSEI_DATA_ID_CLEAR_BODY = 29,
    TOKUSEI_DATA_ID_KEEN_EYE = 51,
    TOKUSEI_DATA_ID_HYPER_CUTTER = 52,
    TOKUSEI_DATA_ID_WHITE_SMOKE = 73,
    TOKUSEI_DATA_ID_TRUANT = 54,
    TOKUSEI_DATA_ID_INSOMNIA = 15,
    TOKUSEI_DATA_ID_OWN_TEMPO = 20,
    TOKUSEI_DATA_ID_WONDER_GUARD = 25,
    TOKUSEI_DATA_ID_VITAL_SPIRIT = 72
};

u16 pokemon_GetInitDp(Pokemon *pokemon);
u16 pokemon_GetDarkpokemonDataId(Pokemon *pokemon);
void pokemon_SetDarkpokemonDataId(Pokemon *pokemon, u16 darkID);
u8 pokemon_GetDarkFlag(Pokemon *pokemon);

enum PokemonDataID {
    POKEMON_DATA_ID_PIDGEOTTO = 17,
    POKEMON_DATA_ID_BUTTERFREE = 12,
    POKEMON_DATA_ID_BEEDRILL = 15,
    POKEMON_DATA_ID_SPEAROW = 21,
    POKEMON_DATA_ID_FEAROW = 22,
    POKEMON_DATA_ID_VENOMOTH = 49,
    POKEMON_DATA_ID_WEEPINBELL = 70,
    POKEMON_DATA_ID_VICTREEBEL = 71,
    POKEMON_DATA_ID_MAGNETON = 82,
    POKEMON_DATA_ID_LAPRAS = 131,
    POKEMON_DATA_ID_LEDYBA = 165,
    POKEMON_DATA_ID_LEDIAN = 166,
    POKEMON_DATA_ID_CARVANHA = 330,
    POKEMON_DATA_ID_SHARPEDO = 331,
    POKEMON_DATA_ID_SOLROCK = 349,
    POKEMON_DATA_ID_LUNATONE = 348,
    POKEMON_DATA_ID_DUSKULL = 361,
    POKEMON_DATA_ID_DUSCLOPS = 362,
    POKEMON_DATA_ID_FARFETCHD = 83,
    POKEMON_DATA_ID_CHANSEY = 113,
    POKEMON_DATA_ID_LUGIA = 249,
    POKEMON_DATA_ID_NIDORAN_F = 29,
    POKEMON_DATA_ID_NIDORAN_M = 32,
    POKEMON_DATA_ID_CASTFORM = 385
};
u32 pokemonDB_GetName(u16);
bool pokemonDB_GetTeachWaza(u16, u16);

void fightOutPokemon_SetSequencePtr(FightOutPokemon *pokemon, struct NullSequence *sequence);
FightPokemon *fightOutPokemon_GetMotoFightPokemonPtr(FightOutPokemon *pokemon);
FIGHT_WAZA *fightOutPokemon_GetFightWazaPtr(FightOutPokemon *pokemon);
FIGHT_ITEM *fightOutPokemon_GetFightItemPtr(FightOutPokemon *pokemon);
FIGHT_ACTION *fightOutPokemon_GetFightActionBuffPtr(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetUseWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonGetUseWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemonGetMotoWazaDataId(FightOutPokemon *pokemon);
void fightOutPokemon_SetUseWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemon_SetMotoWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
void fightOutPokemon_SetWazaDamageValue(FightOutPokemon *pokemon, u8 damage);
void fightOutPokemon_SetWazaCritical(FightOutPokemon *pokemon, u8 critical);
void fightOutPokemon_SetWazaJoutaiDataId(FightOutPokemon *pokemon, u16 joutaiDataId);
void fightOutPokemon_SetWazaInitJoutaiDataId(FightOutPokemon *pokemon, u16 joutaiDataId);
void fightOutPokemon_SetWazaDamage(FightOutPokemon *pokemon, s32 damage);
void fightOutPokemon_SetWazaIryoku(FightOutPokemon *pokemon, u16 iryoku);
void fightOutPokemon_SetWazaTargetId(FightOutPokemon *pokemon, u16 targetId);
void fightOutPokemon_SetWazaHitDamage(FightOutPokemon *pokemon, s32 hitDamage);
u8 fightOutPokemon_GetDamageValue(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetCritical(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetJoutaiDataId(FightOutPokemon *pokemon, u16 joutaiDataId);
u8 fightOutPokemon_GetIsHit(FightOutPokemon *pokemon);
s32 fightOutPokemon_GetDamage(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetIryoku(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetTargetId(FightOutPokemon *pokemon);
s32 fightOutPokemon_GetHitDamage(FightOutPokemon *pokemon);
s16 fightOutPokemon_GetMyselfDamageAtkValue(FightOutPokemon *pokemon);
s16 fightOutPokemon_GetMyselfDamageSpeValue(FightOutPokemon *pokemon);
s32 fightOutPokemon_GetKaigaraDamageValue(FightOutPokemon *pokemon);
void fightOutPokemon_SetKaigaraDamageValue(FightOutPokemon *pokemon, s32 damage);
u16 fightOutPokemon_GetFightoutTurnCount(FightOutPokemon *pokemon);
void fightOutPokemon_SetFightoutTurnCount(FightOutPokemon *pokemon, u16 count);
FIGHT_WAZA *fightOutPokemon_GetKeepFightWazaPtr(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetNamakeFlag(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetMotoWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetOumuWazaDataId(FightOutPokemon *pokemon);
u16 fightOutPokemon_GetOumuWazaAllyDataId(FightOutPokemon *pokemon);
void fightOutPokemon_SetNoPressureFlag(FightOutPokemon *pokemon, u32 flag);
u16 fightOutPokemon_GetLastUseWazaDataId(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetSuccessCnt(FightOutPokemon *pokemon);
void fightOutPokemon_SetSuccessCnt(FightOutPokemon *pokemon, u8 count);

// Contest medals and ribbons of a Pokemon; each setter writes one field of the
// medal record or of the ribbon word and leaves the others alone.
void pokemon_SetStyleMedal(Pokemon *pokemon, u8 value);
void pokemon_SetBeautifulMedal(Pokemon *pokemon, u8 value);
void pokemon_SetCuteMedal(Pokemon *pokemon, u8 value);
void pokemon_SetCleverMedal(Pokemon *pokemon, u8 value);
void pokemon_SetStrongMedal(Pokemon *pokemon, u8 value);
void pokemon_SetAmariRibbon(Pokemon *pokemon, u8 value);
void pokemon_SetWorldRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetEarthRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetNationalRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetCountryRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetSkyRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetLandRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetMarineRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetGanbaRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetBromideRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetVictoryRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetWinningRibbon(Pokemon *pokemon, bool flag);
void pokemon_SetChampRibbon(Pokemon *pokemon, bool flag);

// Contest condition of a Pokemon: the fur sheen and the five contest
// categories of the condition record.
void pokemon_SetFur(Pokemon *pokemon, u8 value);
void pokemon_SetStrong(Pokemon *pokemon, u8 value);
void pokemon_SetClever(Pokemon *pokemon, u8 value);
void pokemon_SetCute(Pokemon *pokemon, u8 value);
void pokemon_SetBeautiful(Pokemon *pokemon, u8 value);
void pokemon_SetStyle(Pokemon *pokemon, u8 value);
void pokemon_SetFriend(Pokemon *pokemon, u16 value);

// Individual values, effort values and calculated battle stats of a Pokemon,
// one setter per stat.
void pokemon_SetNimblenessRnd(Pokemon *pokemon, u16 value);
void pokemon_SetSpeDefRnd(Pokemon *pokemon, u16 value);
void pokemon_SetSpeAtkRnd(Pokemon *pokemon, u16 value);
void pokemon_SetPhyDefRnd(Pokemon *pokemon, u16 value);
void pokemon_SetPhyAtkRnd(Pokemon *pokemon, u16 value);
void pokemon_SetMaxHpRnd(Pokemon *pokemon, u16 value);
void pokemon_SetNimblenessEffort(Pokemon *pokemon, u16 value);
void pokemon_SetSpeDefEffort(Pokemon *pokemon, u16 value);
void pokemon_SetSpeAtkEffort(Pokemon *pokemon, u16 value);
void pokemon_SetPhyDefEffort(Pokemon *pokemon, u16 value);
void pokemon_SetPhyAtkEffort(Pokemon *pokemon, u16 value);
void pokemon_SetMaxHpEffort(Pokemon *pokemon, u16 value);
void pokemon_SetNimbleness(Pokemon *pokemon, u16 value);
void pokemon_SetSpeDef(Pokemon *pokemon, u16 value);
void pokemon_SetSpeAtk(Pokemon *pokemon, u16 value);
void pokemon_SetPhyDef(Pokemon *pokemon, u16 value);
void pokemon_SetPhyAtk(Pokemon *pokemon, u16 value);
void pokemon_SetMaxHp(Pokemon *pokemon, u16 value);

u16 pokemon_GetPokemonWazaDataId(Pokemon *pokemon, u16 slot);
u8 pokemon_GetPokemonWazaPp(Pokemon *pokemon, u16 slot);
void pokemon_SetPokemonWazaPp(Pokemon *pokemon, u16 slot, u8 pp);

u32 pokemon_GetTokuseiFlag(void *pokemonData);
u16 pokemonDB_GetBasisPhyAtk(u16 pokemonDataId);
u16 pokemonDB_GetBasisPhyDef(u16 pokemonDataId);
u8 pokemonDB_GetTokuseiDataId(u16 species, u16 index);
s16 fightPokemon_GetEntryId(FightPokemon *pokemon);
Pokemon *fightPokemon_GetMotoPokemonPtr(FightPokemon *pokemon);
Pokemon *fightPokemon_GetPokemonBuffPtr(FightPokemon *pokemon);
u8 fightPokemon_GetCatchEntryFlag(FightPokemon *pokemon);
void pokemon_SetItemDataId(Pokemon *pokemon, u16 itemDataId);

#ifdef __cplusplus
}
#endif

#endif
