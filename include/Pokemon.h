#ifndef POKEMON_H
#define POKEMON_H

#include <types.h>

// Pokemon -- party/battle pokemon record.
typedef struct Pokemon Pokemon;
struct Pokemon {
    u8 _00[0x02]; // 0x00-0x01: unknown.
    u16 mItemDataId; // 0x02: held item data ID.
    u32 _04[0x08]; // 0x04-0x23: unknown.
    unsigned long mCatchTrainerRnd; // 0x24: original trainer random ID.
    u8 _28[0xC4 - 0x28]; // 0x28-0xC3: unknown.

#ifdef __cplusplus
    u16 getItemDataId() const;
    u8 isDarkPokemon() const;
    unsigned long getCatchTrainerRnd() const;
    void setCatchTrainerRnd(unsigned long rnd);
#endif
};

// Shadow-data status thresholds: fought, snatched, and purified.
enum darkPokemonStatus {
    DARK_POKEMON_STATUS_NONE = 0,
    DARK_POKEMON_STATUS_FIGHT = 1,
    DARK_POKEMON_STATUS_SNATCH_SUCCESS = 3,
    DARK_POKEMON_STATUS_RELIVED = 4,
};

// DarkPokemon -- shadow Pokemon status and newly learned moves.
#ifdef __cplusplus
struct DarkPokemon {
    void setStatus(darkPokemonStatus status);
    darkPokemonStatus getStatus(void) const;
    void setUpdate(bool update);
    u8 isWazaNew(u8 index) const;
    void setWazaNew(u8 index, bool value);
};
#else
typedef struct DarkPokemon DarkPokemon;
#endif

// Fields read out of a pokemon with pokemonGetStatus.
enum POKEMON_STATUS_FIELD {
    POKEMON_STATUS_FIELD_CHAMP_RIBBON = 0xA3,
    POKEMON_STATUS_FIELD_WINNING_RIBBON = 0xA4,
    POKEMON_STATUS_FIELD_VICTORY_RIBBON = 0xA5,
    POKEMON_STATUS_FIELD_BROMIDE_RIBBON = 0xA6,
    POKEMON_STATUS_FIELD_GANBA_RIBBON = 0xA7,
    POKEMON_STATUS_FIELD_MARINE_RIBBON = 0xA8,
    POKEMON_STATUS_FIELD_LAND_RIBBON = 0xA9,
    POKEMON_STATUS_FIELD_SKY_RIBBON = 0xAA,
    POKEMON_STATUS_FIELD_COUNTRY_RIBBON = 0xAB,
    POKEMON_STATUS_FIELD_NATIONAL_RIBBON = 0xAC,
    POKEMON_STATUS_FIELD_EARTH_RIBBON = 0xAD,
    POKEMON_STATUS_FIELD_WORLD_RIBBON = 0xAE,
    POKEMON_STATUS_FIELD_STYLE_MEDAL = 0xB0,
    POKEMON_STATUS_FIELD_BEAUTIFUL_MEDAL = 0xB1,
    POKEMON_STATUS_FIELD_CUTE_MEDAL = 0xB2,
    POKEMON_STATUS_FIELD_CLEVER_MEDAL = 0xB3,
    POKEMON_STATUS_FIELD_STRONG_MEDAL = 0xB4,
    POKEMON_STATUS_FIELD_WAZA_PP = 0x80, // current PP of the given waza slot.
    POKEMON_STATUS_FIELD_WAZA_ID = 0x7F, // waza id of the given waza slot.
    POKEMON_STATUS_FIELD_TRAINER_ID = 0x75, // id of the trainer that met the pokemon.
    POKEMON_STATUS_FIELD_TRAINER_NAME = 0x76, // name of that trainer.
    POKEMON_STATUS_FIELD_NICKNAME = 0x77, // name the pokemon is given.
    POKEMON_STATUS_FIELD_SPECIES = 0x6E, // species of the pokemon.
    POKEMON_STATUS_FIELD_ATTEST = 0x70, // attest record; gamedataGetStatus(record, 2) selects the origin text.
    POKEMON_STATUS_FIELD_MET_PLACE = 0x71, // place the pokemon was caught in.
    POKEMON_STATUS_FIELD_MET_LEVEL = 0x72, // level the pokemon was caught at.
    POKEMON_STATUS_FIELD_LEVEL = 0x7A, // level the pokemon is on.
    POKEMON_STATUS_FIELD_EXP = 0x79, // total experience of the pokemon.
    POKEMON_STATUS_FIELD_HP = 0x83, // current HP.
    POKEMON_STATUS_FIELD_HP_MAX = 0x87, // maximum HP.
    POKEMON_STATUS_FIELD_ATTACK = 0x88, // battle stats shown on the summary page.
    POKEMON_STATUS_FIELD_DEFENSE = 0x89, // physical defense.
    POKEMON_STATUS_FIELD_SP_ATTACK = 0x8A, // special attack.
    POKEMON_STATUS_FIELD_SP_DEFENSE = 0x8B, // special defense.
    POKEMON_STATUS_FIELD_SPEED = 0x8C, // nimbleness/speed.
    POKEMON_STATUS_FIELD_POKERUS = 0xB5, // pokerus of the pokemon.
    POKEMON_STATUS_FIELD_MARKINGS = 0xBB, // markings the pokemon wears.
    POKEMON_STATUS_FIELD_NATURE = 0xBF, // nature index, into the nature table.
    POKEMON_STATUS_FIELD_WAZA_SEALED = 0xC2, // non-zero while the waza pages stay closed.
};

// Marking bits of Pokemon field POKEMON_STATUS_FIELD_MARKINGS (0xBB). Slot N
// is visible when its bit is set; the bit drives the slot sprite of the same slot.
enum POKEMON_MARKING {
    POKEMON_MARKING_SLOT0 = 8, // marking slot 0.
    POKEMON_MARKING_SLOT1 = 4, // marking slot 1.
    POKEMON_MARKING_SLOT2 = 2, // marking slot 2.
    POKEMON_MARKING_SLOT3 = 1, // marking slot 3.
};

// Pokerus active-infection mask of Pokemon field POKEMON_STATUS_FIELD_POKERUS (0xB5).
// A non-zero low nibble draws the infected icon; a zero nibble with a non-zero
// byte draws the cured icon.
enum POKEMON_POKERUS {
    POKEMON_POKERUS_ACTIVE_MASK = 0xF, // active pokerus infection in the low nibble.
};

#ifdef __cplusplus
extern "C" {
#endif

/* pokemonGetStatus query IDs. POKEMON_STATUS_SPECIES reads the record's
   species halfword (zero for an empty slot); POKEMON_STATUS_MOVE reads the
   move ID of the slot the fourth argument selects. */
enum PokemonStatusQuery {
    POKEMON_STATUS_SPECIES = 0x6E,
    POKEMON_STATUS_MOVE = 0x7F,
    POKEMON_QUERY_LEVEL = 0x7A,
    POKEMON_QUERY_SPECIES = 0x6E,
    POKEMON_QUERY_MOVE_PP = 0x80,
    POKEMON_QUERY_ATTACK = 0x88,
    POKEMON_QUERY_DEFENSE = 0x89,
    POKEMON_QUERY_SPECIAL_ATTACK = 0x8A,
    POKEMON_QUERY_SPECIAL_DEFENSE = 0x8B,
    POKEMON_QUERY_SPEED = 0x8C,
    POKEMON_QUERY_IS_FAINTED = 0x7B,
    POKEMON_QUERY_MOVE_ID = 0x7F,
    POKEMON_QUERY_PARTY_RECORD = 0xCC
};

enum PokemonSex {
    POKEMON_SEX_MALE = 0,
    POKEMON_SEX_FEMALE = 1,
    POKEMON_SEX_NONE = 2,
};

enum PokemonAilment {
    POKEMON_CONDITION_NONE = 0,
    POKEMON_CONDITION_POISON = 3,
    POKEMON_CONDITION_BAD_POISON = 4,
    POKEMON_CONDITION_BURN = 5,
    POKEMON_CONDITION_FREEZE = 6,
    POKEMON_CONDITION_PARALYSIS = 7,
    POKEMON_CONDITION_SLEEP = 8,
};

#ifdef __cplusplus
bool pokemonCheckValid(Pokemon *pokemon);
#else
u8 pokemonCheckValid(Pokemon *pokemon);
#endif
u8 pokemon_GetTamagoFlag(Pokemon *pokemon);
u16 pokemonGetTokuseiDataId(Pokemon *pokemon);
u16 battlePokemonGetHeldItem(Pokemon *pokemon);
u16 battlePokemonDataGetSpecies(Pokemon *pokemon);
u16 battlePokemonStatusGetCurrentHp(Pokemon *pokemon);
u16 battlePokemonStatusGetMaxHp(Pokemon *pokemon);
s32 pokemonGetStatus(void *record, s32 speciesId, u16 field, u16 elementIndex);
void pokemon_SetOriginalTrainerRandom(Pokemon *pokemon, u32 rnd);
void pokemon_SetPersonalityRandom(Pokemon *pokemon, u32 rnd);
u8 pokemonIsShadow(Pokemon *pokemon);
u16 pokemonGetJoutaiMenuSpriteId(Pokemon *pokemon);
f32 pokemonGetDp(Pokemon *pokemon);
void pokemonReplace(Pokemon *dst, Pokemon *src);
u16 battlePokemonGetSpecies(Pokemon *pokemon);
u8 pokemonIsShadowPokemon(Pokemon *pokemon);
void pokemonInit(Pokemon *pokemon);
// Gender and nickname of a Pokemon. Nicknames are UTF-16 strings of at most
// ten characters; the name entry mode, the message layer and the link
// exchange each copy them into their own name buffers.
u8 pokemonGetSex(Pokemon *pokemon);
u16 *pokemon_GetNickname(Pokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
