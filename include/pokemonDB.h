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
    TOKUSEI_DATA_ID_NONE = 0,
    TOKUSEI_DATA_ID_STENCH,
    TOKUSEI_DATA_ID_DRIZZLE,
    TOKUSEI_DATA_ID_SPEED_BOOST,
    TOKUSEI_DATA_ID_BATTLE_ARMOR,
    TOKUSEI_DATA_ID_STURDY,
    TOKUSEI_DATA_ID_DAMP,
    TOKUSEI_DATA_ID_LIMBER,
    TOKUSEI_DATA_ID_SAND_VEIL,
    TOKUSEI_DATA_ID_STATIC,
    TOKUSEI_DATA_ID_VOLT_ABSORB,
    TOKUSEI_DATA_ID_WATER_ABSORB,
    TOKUSEI_DATA_ID_OBLIVIOUS,
    TOKUSEI_DATA_ID_CLOUD_NINE,
    TOKUSEI_DATA_ID_COMPOUND_EYES,
    TOKUSEI_DATA_ID_INSOMNIA,
    TOKUSEI_DATA_ID_COLOR_CHANGE,
    TOKUSEI_DATA_ID_IMMUNITY,
    TOKUSEI_DATA_ID_FLASH_FIRE,
    TOKUSEI_DATA_ID_SHIELD_DUST,
    TOKUSEI_DATA_ID_OWN_TEMPO,
    TOKUSEI_DATA_ID_SUCTION_CUPS,
    TOKUSEI_DATA_ID_INTIMIDATE,
    TOKUSEI_DATA_ID_SHADOW_TAG,
    TOKUSEI_DATA_ID_ROUGH_SKIN,
    TOKUSEI_DATA_ID_WONDER_GUARD,
    TOKUSEI_DATA_ID_LEVITATE,
    TOKUSEI_DATA_ID_EFFECT_SPORE,
    TOKUSEI_DATA_ID_SYNCHRONIZE,
    TOKUSEI_DATA_ID_CLEAR_BODY,
    TOKUSEI_DATA_ID_NATURAL_CURE,
    TOKUSEI_DATA_ID_LIGHTNING_ROD,
    TOKUSEI_DATA_ID_SERENE_GRACE,
    TOKUSEI_DATA_ID_SWIFT_SWIM,
    TOKUSEI_DATA_ID_CHLOROPHYLL,
    TOKUSEI_DATA_ID_ILLUMINATE,
    TOKUSEI_DATA_ID_TRACE,
    TOKUSEI_DATA_ID_HUGE_POWER,
    TOKUSEI_DATA_ID_POISON_POINT,
    TOKUSEI_DATA_ID_INNER_FOCUS,
    TOKUSEI_DATA_ID_MAGMA_ARMOR,
    TOKUSEI_DATA_ID_WATER_VEIL,
    TOKUSEI_DATA_ID_MAGNET_PULL,
    TOKUSEI_DATA_ID_SOUNDPROOF,
    TOKUSEI_DATA_ID_RAIN_DISH,
    TOKUSEI_DATA_ID_SAND_STREAM,
    TOKUSEI_DATA_ID_PRESSURE,
    TOKUSEI_DATA_ID_THICK_FAT,
    TOKUSEI_DATA_ID_EARLY_BIRD,
    TOKUSEI_DATA_ID_FLAME_BODY,
    TOKUSEI_DATA_ID_RUN_AWAY,
    TOKUSEI_DATA_ID_KEEN_EYE,
    TOKUSEI_DATA_ID_HYPER_CUTTER,
    TOKUSEI_DATA_ID_PICKUP,
    TOKUSEI_DATA_ID_TRUANT,
    TOKUSEI_DATA_ID_HUSTLE,
    TOKUSEI_DATA_ID_CUTE_CHARM,
    TOKUSEI_DATA_ID_PLUS,
    TOKUSEI_DATA_ID_MINUS,
    TOKUSEI_DATA_ID_FORECAST,
    TOKUSEI_DATA_ID_STICKY_HOLD,
    TOKUSEI_DATA_ID_SHED_SKIN,
    TOKUSEI_DATA_ID_GUTS,
    TOKUSEI_DATA_ID_MARVEL_SCALE,
    TOKUSEI_DATA_ID_LIQUID_OOZE,
    TOKUSEI_DATA_ID_OVERGROW,
    TOKUSEI_DATA_ID_BLAZE,
    TOKUSEI_DATA_ID_TORRENT,
    TOKUSEI_DATA_ID_SWARM,
    TOKUSEI_DATA_ID_ROCK_HEAD,
    TOKUSEI_DATA_ID_DROUGHT,
    TOKUSEI_DATA_ID_ARENA_TRAP,
    TOKUSEI_DATA_ID_VITAL_SPIRIT,
    TOKUSEI_DATA_ID_WHITE_SMOKE,
    TOKUSEI_DATA_ID_PURE_POWER,
    TOKUSEI_DATA_ID_SHELL_ARMOR,
    TOKUSEI_DATA_ID_UNUSED,
    TOKUSEI_DATA_ID_AIR_LOCK
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
    POKEMON_DATA_ID_CASTFORM = 385,
    POKEMON_DATA_ID_WOBBUFFET = 202,
    POKEMON_DATA_ID_SHUCKLE = 213,
    POKEMON_DATA_ID_SHEDINJA = 303,
    POKEMON_DATA_ID_WYNAUT = 360
};
u32 pokemonDB_GetName(u16);
u32 pokemonDB_GetShout(u16 pokemonDataId);
bool pokemonDB_GetTeachWaza(u16, u16);
u32 pokemonDB_GetPokebodyId(u16 species, u8 shiny);

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
void fightOutPokemon_SetWazaCritical(FightOutPokemon *pokemon, u32 critical);
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
struct FightOutPokemonEnemy *fightOutPokemon_GetFightOutPokemonEnemyPtr(FightOutPokemon *pokemon,
                                                                       u16 index);
void fightOutPokemon_SetOumuWazaDataId(FightOutPokemon *pokemon, u16 wazaDataId);
u16 fightOutPokemon_GetOumuWazaAllyDataId(FightOutPokemon *pokemon);
void fightOutPokemon_SetNoPressureFlag(FightOutPokemon *pokemon, u32 flag);
u16 fightOutPokemon_GetLastUseWazaDataId(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetSuccessCnt(FightOutPokemon *pokemon);
void fightOutPokemon_SetSuccessCnt(FightOutPokemon *pokemon, u8 count);

void fightOutPokemon_SetZokuseiDataId(FightOutPokemon *pokemon, u8 index, u16 typeId);
void fightOutPokemon_SetMeetEnemyFightPokemonEntryId(FightOutPokemon *pokemon, u8 index, u16 entryId);
void fightOutPokemon_SetUsedItemDataId(FightOutPokemon *pokemon, u16 itemDataId);
void fightOutPokemon_SetNamakeFlag(FightOutPokemon *pokemon, u8 flag);
void fightOutPokemon_SetGamanDamageTargetId(FightOutPokemon *pokemon, u16 targetId);
void fightOutPokemon_SetGamanDamageValue(FightOutPokemon *pokemon, s16 damage);
void fightOutPokemon_SetFightPokemonPtr(FightOutPokemon *pokemon, FightPokemon *fightPokemon);
void fightOutPokemon_SetMotoFightPokemonPtr(FightOutPokemon *pokemon, FightPokemon *fightPokemon);
void fightPokemon_SetAiDamage(FightPokemon *pokemon, u16 index, u16 target, s32 damage);
void fightPokemon_SetHokakuFlag(FightPokemon *pokemon, u8 flag);
void fightPokemon_SetDarkOutFlag(FightPokemon *pokemon, u8 flag);
void fightPokemon_SetLevelUpFlag(FightPokemon *pokemon, u8 flag);
void fightPokemon_SetCatchEntryFlag(FightPokemon *pokemon, u8 flag);
void fightPokemon_SetEntryId(FightPokemon *pokemon, s16 entryId);
void fightPokemon_SetMotoPokemonPtr(FightPokemon *pokemon, Pokemon *original);
void pokemon_SetComboPartnerAll(Pokemon *pokemon, u8 value);
void pokemon_SetPoolFriend(Pokemon *pokemon, u16 level);
void pokemon_SetPoolExp(Pokemon *pokemon, u32 exp);
void pokemon_SetFightTrainerPokemonDataId(Pokemon *pokemon, u16 dataId);
void pokemon_SetAmari(Pokemon *pokemon, u16 value);
void pokemon_SetPara1Amari(Pokemon *pokemon, u16 value);
void pokemon_SetMailId(Pokemon *pokemon, u8 value);
void pokemon_SetPcboxMark(Pokemon *pokemon, u8 value);
void pokemon_SetFlagAmari(Pokemon *pokemon, u8 value);
void pokemon_SetFuseiFlag(Pokemon *pokemon, u8 flag);
void pokemon_SetTokuseiFlag(Pokemon *pokemon, u8 flag);
void pokemon_SetTamagoFlag(Pokemon *pokemon, u8 flag);
void pokemon_SetPokerus(Pokemon *pokemon, u8 value);

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
u16 pokemon_GetFightTrainerPokemonDataId(Pokemon *pokemon);
u8 pokemon_GetComboPartner(Pokemon *pokemon, u8 index, u8 mode);
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
