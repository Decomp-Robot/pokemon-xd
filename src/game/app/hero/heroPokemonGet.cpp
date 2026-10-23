#include <darkPokemon.h>
#include <GSgfx.h>
#include <GSmsg.h>
#include <gamedata.h>
#include <hero.h>
#include <memo.h>

namespace {

enum {
    SPECIES_CELEBI = 0xFB,
    SPECIES_PIKACHU = 0x19,
    SPECIES_HOUOU = 0xFA,
    SPECIES_PLUSLE = 0x161,
    SPECIES_VAPOREON = 0x86,
    SPECIES_JOLTEON = 0x87,
};

enum {
    LEVEL_CELEBI = 10,
    LEVEL_PIKACHU = 10,
    LEVEL_HOUOU = 70,
    LEVEL_PRASLE = 13,
    LEVEL_VAPOREON = 30,
    LEVEL_JOLTEON = 26,
};

enum {
    TRAINER_ID_CELEBI = 0x7991,
    TRAINER_ID_PIKACHU = 0x7991,
    TRAINER_ID_HOUOU = 0x2740,
    TRAINER_ID_PRASLE = 0x911D,
};

enum {
    CATCH_FLOOR_GIFT = 0xFF,
    CATCH_FLOOR_FIELD = 0x259,
};

enum {
    GAMEDATA_VERSION_GIFT = 9,
    GAMEDATA_VERSION_HOUOU = 8,
    GAMEDATA_GENERATION_GIFT = 3,
    GAMEDATA_OPTION_RECORD = 1,
    GAMEDATA_OPTION_REGION = 4,
    GAMEDATA_OPTION_LANGUAGE = 5,
};

enum {
    CATCH_BALL_ENCOUNTER = 4,
    SEX_DATA_ID_GIFT = 0,
    SEX_DATA_ID_CELEBI = 1,
};

enum {
    FRIEND_LEVEL_GIFT = 70,
    FRIEND_LEVEL_PARTY = 220,
    VAPOREON_EXP = 0x7148,
    JOLTEON_EXP = 0x4A6F,
    ITEM_PIKACHU_GIFT = 0xCA,
};

enum {
    WAZA_HELPING_HAND = 0x10E,
    WAZA_WATER_GUN = 0x37,
    WAZA_QUICK_ATTACK = 0x62,
    WAZA_BITE = 0x2C,
    WAZA_SAND_ATTACK = 0x1C,
    WAZA_THUNDER_SHOCK = 0x54,
};

} // namespace

// Gift encounters and the two starting party pokemon.
extern "C" {

void heroPokemonGetCelebi(Hero *hero, u16 catchFloorID)
{
    GamedataAttest attest;
    Pokemon pokemon;
    u8 language = (u8)(u32)gamedataGetStatus(0, GAMEDATA_OPTION_LANGUAGE);
    u8 region = (u8)(u32)gamedataGetStatus(0, GAMEDATA_OPTION_REGION);

    gamedataAttestCreate(&attest, GAMEDATA_VERSION_GIFT, GAMEDATA_GENERATION_GIFT, region, language);
    pokemonCreate(&pokemon, SPECIES_CELEBI, LEVEL_CELEBI, &attest);
    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_FRIEND_LEVEL, 0, FRIEND_LEVEL_GIFT);
    pokemonSetCatchStatus(&pokemon, CATCH_FLOOR_GIFT, LEVEL_CELEBI, CATCH_BALL_ENCOUNTER, SEX_DATA_ID_CELEBI, TRAINER_ID_CELEBI, GSmsgGetStringPtr(1));
    u32 pid = pokemonCreateRndFit(&pokemon, -1, -1, 0, TRAINER_ID_CELEBI);

    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_PID, 0, pid);
    pokemonResetBasisStatus(&pokemon);
    memoDataSet(0, &pokemon);
    heroGetPokemon(hero, &pokemon, true);
}

void heroPokemonGetPikachu(Hero *hero, u16 catchFloorID)
{
    GamedataAttest attest;
    Pokemon pokemon;
    u8 language = (u8)(u32)gamedataGetStatus(0, GAMEDATA_OPTION_LANGUAGE);
    u8 region = (u8)(u32)gamedataGetStatus(0, GAMEDATA_OPTION_REGION);

    gamedataAttestCreate(&attest, GAMEDATA_VERSION_GIFT, GAMEDATA_GENERATION_GIFT, region, language);
    pokemonCreate(&pokemon, SPECIES_PIKACHU, LEVEL_PIKACHU, &attest);
    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_FRIEND_LEVEL, 0, FRIEND_LEVEL_GIFT);
    pokemonSetCatchStatus(&pokemon, CATCH_FLOOR_GIFT, LEVEL_PIKACHU, CATCH_BALL_ENCOUNTER, SEX_DATA_ID_GIFT, TRAINER_ID_PIKACHU, GSmsgGetStringPtr(1));
    u32 pid = pokemonCreateRndFit(&pokemon, -1, -1, 0, TRAINER_ID_PIKACHU);

    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_PID, 0, pid);
    pokemonDoItemSoubi(&pokemon, ITEM_PIKACHU_GIFT, 1);
    pokemonResetBasisStatus(&pokemon);
    memoDataSet(0, &pokemon);
    heroGetPokemon(hero, &pokemon, true);
}

void heroPokemonGetHouou(Hero *hero, u16 catchFloorID)
{
    GamedataAttest attest;
    Pokemon pokemon;
    u8 language = (u8)(u32)gamedataGetStatus(0, GAMEDATA_OPTION_LANGUAGE);
    u8 region = (u8)(u32)gamedataGetStatus(0, GAMEDATA_OPTION_REGION);

    gamedataAttestCreate(&attest, GAMEDATA_VERSION_HOUOU, GAMEDATA_GENERATION_GIFT, region, language);
    pokemonCreate(&pokemon, SPECIES_HOUOU, LEVEL_HOUOU, &attest);
    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_FRIEND_LEVEL, 0, FRIEND_LEVEL_GIFT);
    pokemonSetCatchStatus(&pokemon, CATCH_FLOOR_GIFT, LEVEL_HOUOU, CATCH_BALL_ENCOUNTER, SEX_DATA_ID_GIFT, TRAINER_ID_HOUOU, GSmsgGetStringPtr(1));
    u32 pid = pokemonCreateRndFit(&pokemon, -1, -1, 0, TRAINER_ID_HOUOU);

    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_PID, 0, pid);
    pokemonResetBasisStatus(&pokemon);
    memoDataSet(0, &pokemon);
    heroGetPokemon(hero, &pokemon, true);
}

void heroPokemonGetPrasle(Hero *hero, u16 catchFloorID)
{
    Pokemon pokemon;

    pokemonCreate(&pokemon, SPECIES_PLUSLE, LEVEL_PRASLE, (GamedataAttest *)gamedataGetStatus(0, GAMEDATA_OPTION_RECORD));
    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_FRIEND_LEVEL, 0, FRIEND_LEVEL_GIFT);
    pokemonSetCatchStatus(&pokemon, CATCH_FLOOR_FIELD, LEVEL_PRASLE, CATCH_BALL_ENCOUNTER, SEX_DATA_ID_GIFT, TRAINER_ID_PRASLE, GSmsgGetStringPtr(1));
    u32 pid = pokemonCreateRndFit(&pokemon, -1, -1, 0, TRAINER_ID_PRASLE);

    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_PID, 0, pid);
    pokemonResetBasisStatus(&pokemon);
    memoDataSet(0, &pokemon);
    heroGetPokemon(hero, &pokemon, true);
}

void heroPokemonGetEifie(Hero *hero, u16 catchFloorID)
{
    u32 trainerId = heroGetStatus(hero, HERO_STATUS_TRAINER_ID, 0);
    Pokemon pokemon;

    pokemonCreate(&pokemon, SPECIES_VAPOREON, LEVEL_VAPOREON, (GamedataAttest *)gamedataGetStatus(0, GAMEDATA_OPTION_RECORD));
    pokemonWazaCreate(&pokemon, 0, WAZA_HELPING_HAND);
    pokemonWazaCreate(&pokemon, 1, WAZA_WATER_GUN);
    pokemonWazaCreate(&pokemon, 2, WAZA_QUICK_ATTACK);
    pokemonWazaCreate(&pokemon, 3, WAZA_BITE);
    u32 pid = pokemonCreateRndFit(&pokemon, 0, -1, 0, trainerId);

    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_PID, 0, pid);
    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0, VAPOREON_EXP);
    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_FRIEND_LEVEL, 0, FRIEND_LEVEL_PARTY);
    pokemonResetBasisStatus(&pokemon);
    heroCatchPokemon(hero, &pokemon, CATCH_FLOOR_FIELD, CATCH_BALL_ENCOUNTER, 0);
}

void heroPokemonGetBlacky(Hero *hero, u16 catchFloorID)
{
    u32 trainerId = heroGetStatus(hero, HERO_STATUS_TRAINER_ID, 0);
    Pokemon pokemon;

    pokemonCreate(&pokemon, SPECIES_JOLTEON, LEVEL_JOLTEON, (GamedataAttest *)gamedataGetStatus(0, GAMEDATA_OPTION_RECORD));
    pokemonWazaCreate(&pokemon, 0, WAZA_QUICK_ATTACK);
    pokemonWazaCreate(&pokemon, 1, WAZA_SAND_ATTACK);
    pokemonWazaCreate(&pokemon, 2, WAZA_HELPING_HAND);
    pokemonWazaCreate(&pokemon, 3, WAZA_THUNDER_SHOCK);
    u32 pid = pokemonCreateRndFit(&pokemon, 0, -1, 0, trainerId);

    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_PID, 0, pid);
    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_EXP, 0, JOLTEON_EXP);
    pokemonSetStatus(&pokemon, 0, POKEMON_STATUS_FIELD_FRIEND_LEVEL, 0, FRIEND_LEVEL_PARTY);
    pokemonResetBasisStatus(&pokemon);
    heroCatchPokemon(hero, &pokemon, CATCH_FLOOR_FIELD, CATCH_BALL_ENCOUNTER, 0);
}

} // extern "C"
