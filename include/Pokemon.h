#ifndef POKEMON_H
#define POKEMON_H

#include <types.h>

// Pokemon -- party/battle pokemon record.
typedef struct Pokemon Pokemon;
struct Pokemon {
    u8 _00[0x02]; // 0x00-0x01: unknown.
    u16 mItemDataId; // 0x02: held item data ID.
    u32 _04[0x08]; // 0x04-0x23: unknown.
    unsigned long mCatchTrainerRnd; // 0x24: controller slot booked as the catch trainer.
    u8 _28[0xC4 - 0x28]; // 0x28-0xC3: unknown.

#ifdef __cplusplus
    u16 getItemDataId() const;
    u8 isDarkPokemon() const;
    unsigned long getCatchTrainerRnd() const;
    void setCatchTrainerRnd(unsigned long rnd);
#endif
};

// DarkPokemon -- the dark side of a shadow pokemon. Only the members the
// status menu asks about are declared here.
#ifdef __cplusplus
struct DarkPokemon {
#ifdef __cplusplus
    u8 isWazaNew(u8 index) const;
    void setWazaNew(u8 index, bool value);
#endif
};
#else
typedef struct DarkPokemon DarkPokemon;
#endif

// Fields read out of a pokemon with pokemonGetStatus.
enum POKEMON_STATUS_FIELD {
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
    POKEMON_STATUS_MOVE = 0x7F
};

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
