#include <types.h>
#include <Pokemon.h>
#include <PokemonData.h>
#include <pokemonBios.h>
#include <evolution.h>
#include <pokemonEvolution.h>
#include <hero.h>
#include <heroItem.h>
#include <itemData.h>
#include <fight.h>
#include <fightFloor.h>
#include <fightTrainer.h>
#include <fightTrainerStatus.h>
#include <pokemonDataBios.h>
#include <GSmsg.h>
#include <gamedata.h>
#include <savedata.h>
#include <memo.h>
#include <fade.h>

// Evolution methods a species evolution entry can name; the condition half of
// the entry is the level, friendship, beauty or item the method compares
// against. Trade and item evolutions are answered by
// getEvoPokemonCommunication and getEvoPokemonItem.
enum {
    EVOLUTION_METHOD_FRIENDSHIP = 1,
    EVOLUTION_METHOD_FRIENDSHIP_DAY = 2,
    EVOLUTION_METHOD_FRIENDSHIP_NIGHT = 3,
    EVOLUTION_METHOD_LEVEL = 4,
    EVOLUTION_METHOD_TRADE = 5,
    EVOLUTION_METHOD_TRADE_ITEM = 6,
    EVOLUTION_METHOD_ITEM = 7,
    EVOLUTION_METHOD_LEVEL_ATK_OVER_DEF = 8,
    EVOLUTION_METHOD_LEVEL_ATK_EQUAL_DEF = 9,
    EVOLUTION_METHOD_LEVEL_ATK_UNDER_DEF = 10,
    EVOLUTION_METHOD_LEVEL_RANDOM_LOW = 11,
    EVOLUTION_METHOD_LEVEL_RANDOM_HIGH = 12,
    EVOLUTION_METHOD_LEVEL_EMPTY_SLOT = 14,
    EVOLUTION_METHOD_LEVEL_BEAUTIFUL = 15,
    EVOLUTION_METHOD_FRIENDSHIP_ITEM = 16
};

// Minimum friendship required by the friendship evolution methods.
#define EVOLUTION_FRIENDSHIP_THRESHOLD 220

// Soubi effect of the held item that keeps a Pokemon from evolving.
#define SOUBI_NO_EVOLUTION 38

// Invalid Pokemon or evolution check.
#define EVOLUTION_SPECIES_INVALID 0xFFFF

#define EVOLUTION_MAX_WAZA 20

PokemonData *getPokemonData(Pokemon *pokemon);

s8 getEmptySlotID(Hero *hero)
{
    s32 slot;
    for (slot = 0; slot < 6; slot++) {
        if (!pokemonCheckValid((Pokemon *)heroGetStatus(hero, HERO_STATUS_POKEMON, slot))) {
            break;
        }
    }
    if (slot >= 6) {
        return -1;
    }
    return slot;
}

void evolutionAddPokemon(Pokemon *pokemon, u16 additionalSpecies, Hero *hero, int registerMemo)
{
    Pokemon addedPokemon;
    if (additionalSpecies != 0) {
        if (getEmptySlotID(hero) >= 0) {
            pokemonEvolutionCreateAddPokemon(&addedPokemon, pokemon, additionalSpecies);
            heroAddPokemon(hero, &addedPokemon);
            if (registerMemo) {
                memoDataSet(0, &addedPokemon);
            }
        }
    }
}

EVOLUTION_RESULT pokemonEvolutionAll(Pokemon *pokemon, u16 species, u16 additionalSpecies,
                       POKEMON_EVOLUTION_INFO *info, Hero *hero,
                       s32 registerMemo, s32 enableCancel, s32 fade)
{
    Pokemon evolvedPokemon;
    u16 wazaIDs[EVOLUTION_MAX_WAZA];
    u16 sceneWazaIDs[EVOLUTION_MAX_WAZA];
    u8 learnedSlots[EVOLUTION_MAX_WAZA];
    s32 wazaCount;
    EVOLUTION_RESULT result;
    s32 i;

    if (!pokemonCheckValid(pokemon)) {
        return EVOLUTION_RESULT_ERROR;
    }
    if (species == 0) {
        return EVOLUTION_RESULT_ERROR;
    }
    wazaCount = pokemonEvolution(&evolvedPokemon, pokemon, species, info, wazaIDs);
    if (wazaCount < 0) {
        return EVOLUTION_RESULT_ERROR;
    }
    if (fade) {
        fadeSet(FADE_OUT_BLACK, 0.5f);
        fadeCheck(1);
    }
    for (i = 0; i < wazaCount; i++) {
        sceneWazaIDs[i] = wazaIDs[i];
    }
    result = evolutionOpen(pokemon, &evolvedPokemon, enableCancel, sceneWazaIDs, wazaCount, learnedSlots);
    if (fade) {
        fadeSet(FADE_IN_BLACK, 0.5f);
        fadeCheck(1);
    }
    if (result == EVOLUTION_RESULT_ERROR) {
        return EVOLUTION_RESULT_ERROR;
    }
    if (result == EVOLUTION_RESULT_CANCEL) {
        return EVOLUTION_RESULT_CANCEL;
    }
    if (registerMemo) {
        memoDataSet(0, &evolvedPokemon);
    }
    pokemonBiosCopy(pokemon, &evolvedPokemon);
    for (i = 0; i < wazaCount; i++) {
        if (learnedSlots[i] != EVOLUTION_WAZA_NOT_LEARNED) {
            pokemonWazaCreate(pokemon, learnedSlots[i], wazaIDs[i]);
        }
    }
    if (additionalSpecies != 0) {
        evolutionAddPokemon(pokemon, additionalSpecies, hero, registerMemo);
    }
    if (info->method == EVOLUTION_METHOD_FRIENDSHIP_ITEM) {
        Hero *itemHero = 0;
        if (fightIsFight()) {
            FIGHT_TRAINER *trainer = fightFloorGetGcHeroFightTrainerPtr(0);
            if (trainer) {
                itemHero = (Hero *)fightTrainerGetStatus(trainer, 0, FIGHT_TRAINER_QUERY_HERO, 0);
            }
        }
        heroItemDecItemDataId(itemHero, info->condition, 1, -1);
    }
    return EVOLUTION_RESULT_COMPLETE;
}

static const u16 sShedinjaJapaneseName[] = { 0x30CC, 0x30B1, 0x30CB, 0x30F3, 0 };

s32 pokemonEvolutionCreateAddPokemon(Pokemon *destination, Pokemon *source, u16 species)
{
    u16 medalFields[] = {
        POKEMON_STATUS_FIELD_STYLE_MEDAL, POKEMON_STATUS_FIELD_BEAUTIFUL_MEDAL,
        POKEMON_STATUS_FIELD_CUTE_MEDAL, POKEMON_STATUS_FIELD_CLEVER_MEDAL,
        POKEMON_STATUS_FIELD_STRONG_MEDAL
    };
    u16 ribbonFields[] = {
        POKEMON_STATUS_FIELD_CHAMP_RIBBON, POKEMON_STATUS_FIELD_WINNING_RIBBON,
        POKEMON_STATUS_FIELD_VICTORY_RIBBON, POKEMON_STATUS_FIELD_BROMIDE_RIBBON,
        POKEMON_STATUS_FIELD_GANBA_RIBBON, POKEMON_STATUS_FIELD_MARINE_RIBBON,
        POKEMON_STATUS_FIELD_LAND_RIBBON, POKEMON_STATUS_FIELD_SKY_RIBBON,
        POKEMON_STATUS_FIELD_COUNTRY_RIBBON, POKEMON_STATUS_FIELD_NATIONAL_RIBBON,
        POKEMON_STATUS_FIELD_EARTH_RIBBON, POKEMON_STATUS_FIELD_WORLD_RIBBON
    };
    PokemonData *data;
    s32 i;

    if (!pokemonDataCheckValid(0, species)) {
        return 0;
    }
    if (!getPokemonData(source)) {
        return 0;
    }
    pokemonBiosCopy(destination, source);
    pokemonBiosSetPokemonDataId(destination, species);
    data = getPokemonData(destination);
    if (!data) {
        return 0;
    }
    if (species != POKEMON_SPECIES_SHEDINJA) {
        u16 *name = GSmsgGetStringPtr(pokemonDataBiosGetName(data));
        pokemonBiosSetNicknamePtr(destination, name);
    } else {
        switch (gamedataAttestBiosGetLangareaId(pokemonBiosGetAttest(destination))) {
        case GAMEDATA_ATTEST_LANGAREA_JAPANESE:
            pokemonBiosSetNicknamePtr(destination, sShedinjaJapaneseName);
            break;
        default:
            pokemonBiosSetNicknamePtr(destination, GSmsgGetStringPtr(pokemonDataBiosGetName(data)));
            break;
        }
    }
    pokemonBiosSetItemDataId(destination, 0);
    pokemonSetStatus(destination, 0, POKEMON_STATUS_FIELD_MARKINGS, 0, 0);
    pokemonSetStatus(destination, 0, POKEMON_STATUS_FIELD_AMARI, 0, 0);
    for (i = 0; i < (s32)(sizeof(medalFields) / sizeof(medalFields[0])); i++) {
        pokemonSetStatus(destination, 0, medalFields[i], 0, 0);
    }
    for (i = 0; i < (s32)(sizeof(ribbonFields) / sizeof(ribbonFields[0])); i++) {
        pokemonSetStatus(destination, 0, ribbonFields[i], 0, 0);
    }
    pokemonSetStatus(destination, 0, POKEMON_STATUS_FIELD_AMARI_RIBBON, 0, 0);
    pokemonInitJoutai(destination);
    pokemonSetStatus(destination, 0, POKEMON_STATUS_FIELD_MAIL_ID, 0, 0);
    pokemonResetBasisStatus(destination);
    return 1;
}

s32 pokemonEvolution(Pokemon *destination, Pokemon *source, u16 species,
                     POKEMON_EVOLUTION_INFO *info, u16 *outWazaIDs)
{
    u16 *nickname;
    s32 wazaCount = 0;
    u32 language;
    u16 wazaID;
    u8 level;
    PokemonData *sourceData;
    GamedataAttest *attest;
    u8 learnsetIndex;
    u8 sourceLanguage;

    if (!pokemonCheckValid(source)) {
        return -1;
    }
    if (!pokemonDataCheckValid(0, species)) {
        return -1;
    }
    sourceData = getPokemonData(source);
    if (!sourceData) {
        return -1;
    }
    pokemonBiosCopy(destination, source);
    attest = pokemonBiosGetAttest(source);
    language = gamedataAttestBiosGetLangareaId(gamedataBiosGetGamedataAtttestPtr((GamedataSave *)savedataGetStatus(0, SAVE_DATA_GAME)));
    sourceLanguage = gamedataAttestBiosGetLangareaId(attest);
    if (sourceLanguage == language) {
        nickname = pokemon_GetNickname(destination);
        u16 *name = GSmsgGetStringPtr(pokemonDataBiosGetName(sourceData));
        if (GScharCmp(nickname, name) == 0) {
            u16 *evolvedName = GSmsgGetStringPtr(pokemonDataBiosGetName(pokemonDataBiosGetPtr(species)));
            pokemonBiosSetNicknamePtr(destination, evolvedName);
        }
    }
    pokemonBiosSetPokemonDataId(destination, species);
    pokemonResetBasisStatus(destination);
    if (info->method == EVOLUTION_METHOD_TRADE_ITEM) {
        pokemonDoItemSoubi(destination, 0, 0);
    }
    level = pokemonBiosGetLevel(destination);
    learnsetIndex = 0;
    while ((wazaID = pokemonGetOboeWazaDataId(destination, level, &learnsetIndex)) != 0) {
        if (pokemonSearchWazaDataId(destination, wazaID) == -1) {
            outWazaIDs[wazaCount++] = wazaID;
            if (wazaCount >= EVOLUTION_MAX_WAZA) {
                break;
            }
        }
        learnsetIndex++;
    }
    return wazaCount;
}

// First party slot that holds no Pokemon, or -1 when the party is full. An
// evolution that adds a Pokemon to the party needs one.
s8 getTemotiEmptySlotID(void)
{
    s32 slot;

    for (slot = 0; slot < 6; slot++) {
        if (!pokemonCheckValid((Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, slot))) {
            break;
        }
    }
    if (slot >= 6) {
        return -1;
    }
    return slot;
}

// Split value of the methods that choose between two evolutions by chance:
// the high half of the Pokemon random word, narrowed to ten values.
u32 getUniqueRandom(Pokemon *pokemon)
{
    return (pokemonBiosGetRnd(pokemon) >> 16) % 10;
}

// Time-of-day states used by friendship evolutions. The game keeps no clock,
// so getNoon reports neither and the two evolutions that wait for one never happen.
enum {
    EVOLUTION_TIME_NIGHT = 0,
    EVOLUTION_TIME_DAY = 1,
    EVOLUTION_TIME_NEITHER = 2
};

s32 getNoon(void)
{
    return EVOLUTION_TIME_NEITHER;
}

// Species data behind a Pokemon, or null when it carries no species.
PokemonData *getPokemonData(Pokemon *pokemon)
{
    u16 pokemonDataId;

    pokemonDataId = pokemonBiosGetPokemonDataId(pokemon);
    if (pokemonDataId == 0) {
        return 0;
    }
    return pokemonDataBiosGetPtr(pokemonDataId);
}

// Link trade evolution, or a trade while the Pokemon holds the item the entry
// names.
u16 getEvoPokemonCommunication(Pokemon *pokemon, POKEMON_EVOLUTION_INFO *info)
{
    s32 i;
    u16 species;
    PokemonData *data;
    u8 method;
    u16 condition;

    species = 0;
    data = getPokemonData(pokemon);
    if (data == 0) {
        return EVOLUTION_SPECIES_INVALID;
    }
    for (i = 0; i < 5; i++) {
        method = pokemonDataBiosGetSinkaKind(data, i);
        condition = pokemonDataBiosGetSinkaBuff(data, i);
        switch (method) {
        case EVOLUTION_METHOD_TRADE:
            species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
            info->method = method;
            info->condition = condition;
            break;
        case EVOLUTION_METHOD_TRADE_ITEM:
            if (condition == pokemonBiosGetItemDataId(pokemon)) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        }
    }
    return species;
}

// Item evolution: the item the entry names used on the Pokemon.
u16 getEvoPokemonItem(Pokemon *pokemon, u16 itemID, POKEMON_EVOLUTION_INFO *info)
{
    s32 i;
    u16 species;
    PokemonData *data;
    u8 method;
    u16 condition;

    species = 0;
    data = getPokemonData(pokemon);
    if (data == 0) {
        return EVOLUTION_SPECIES_INVALID;
    }
    for (i = 0; i < 5; i++) {
        method = pokemonDataBiosGetSinkaKind(data, i);
        condition = pokemonDataBiosGetSinkaBuff(data, i);
        switch (method) {
        case EVOLUTION_METHOD_ITEM:
            if (condition == itemID) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        }
    }
    return species;
}

// Level up evolution: the methods that evolve the Pokemon itself report the
// species it becomes, while the empty slot method only names the additional
// Pokemon it adds to the party, which comes back through additionalSpecies.
u16 getEvoPokemonLevelUp(Pokemon *pokemon, u16 *additionalSpecies, POKEMON_EVOLUTION_INFO *info)
{
    s32 i;
    PokemonData *data;
    u16 additionalSpeciesId;
    u16 condition;
    u8 method;
    u16 species;
    Hero *hero;
    FIGHT_TRAINER *trainer;

    species = 0;
    additionalSpeciesId = 0;
    data = getPokemonData(pokemon);
    if (data == 0) {
        return EVOLUTION_SPECIES_INVALID;
    }
    for (i = 0; i < 5; i++) {
        method = pokemonDataBiosGetSinkaKind(data, i);
        condition = pokemonDataBiosGetSinkaBuff(data, i);
        switch (method) {
        case EVOLUTION_METHOD_FRIENDSHIP:
            if (pokemonBiosGetFriend(pokemon) >= EVOLUTION_FRIENDSHIP_THRESHOLD) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_FRIENDSHIP_DAY:
            if (pokemonBiosGetFriend(pokemon) >= EVOLUTION_FRIENDSHIP_THRESHOLD && getNoon() == EVOLUTION_TIME_DAY) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_FRIENDSHIP_NIGHT:
            if (pokemonBiosGetFriend(pokemon) >= EVOLUTION_FRIENDSHIP_THRESHOLD && getNoon() == EVOLUTION_TIME_NIGHT) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_LEVEL:
            if (pokemonBiosGetLevel(pokemon) >= condition) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_LEVEL_ATK_OVER_DEF:
            if (pokemonBiosGetLevel(pokemon) >= condition
                && pokemonBiosGetPhyDef(pokemon) < pokemonBiosGetPhyAtk(pokemon)) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_LEVEL_ATK_EQUAL_DEF:
            if (pokemonBiosGetLevel(pokemon) >= condition
                && pokemonBiosGetPhyDef(pokemon) == pokemonBiosGetPhyAtk(pokemon)) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_LEVEL_ATK_UNDER_DEF:
            if (pokemonBiosGetLevel(pokemon) >= condition
                && pokemonBiosGetPhyDef(pokemon) > pokemonBiosGetPhyAtk(pokemon)) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_LEVEL_RANDOM_LOW:
            if (pokemonBiosGetLevel(pokemon) >= condition && getUniqueRandom(pokemon) < 5) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_LEVEL_RANDOM_HIGH:
            if (pokemonBiosGetLevel(pokemon) >= condition && getUniqueRandom(pokemon) >= 5) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_LEVEL_EMPTY_SLOT:
            if (pokemonBiosGetLevel(pokemon) >= condition && getTemotiEmptySlotID() >= 0) {
                additionalSpeciesId = pokemonDataBiosGetSinkaPokemonDataId(data, i);
            }
            break;
        case EVOLUTION_METHOD_LEVEL_BEAUTIFUL:
            if (pokemonBiosGetBeautiful(pokemon) >= condition) {
                species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                info->method = method;
                info->condition = condition;
            }
            break;
        case EVOLUTION_METHOD_FRIENDSHIP_ITEM:
            if (pokemonBiosGetFriend(pokemon) >= EVOLUTION_FRIENDSHIP_THRESHOLD) {
                hero = 0;
                if (fightIsFight()) {
                    trainer = fightFloorGetGcHeroFightTrainerPtr(0);
                    if (trainer) {
                        hero = (Hero *)fightTrainerGetStatus(trainer, 0, FIGHT_TRAINER_QUERY_HERO, 0);
                    }
                }
                if (heroItemCheckHaveItemDataId(hero, condition) != 0) {
                    species = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                    info->method = method;
                    info->condition = condition;
                }
            }
            break;
        }
    }
    *additionalSpecies = additionalSpeciesId;
    return species;
}

// Soubi effect of the item the Pokemon holds, or 0 when it holds nothing.
u16 getItemSoubiDataID(Pokemon *pokemon)
{
    u16 itemID;

    itemID = pokemonGetSoubiItemDataId(pokemon);
    if (itemID == 0) {
        return 0;
    }
    return itemDataBiosGetItemSoubiDataId(itemDataBiosGetPtr(itemID));
}


u16 pokemonEvolutionCheck(Pokemon *pokemon, s32 kind, u16 itemID,
                          u16 *outAdditionalSpecies, POKEMON_EVOLUTION_INFO *info)
{
    u16 additionalSpecies = 0;
    u16 species;
    if (!pokemonCheckValid(pokemon)) {
        return EVOLUTION_SPECIES_INVALID;
    }
    if (getItemSoubiDataID(pokemon) == SOUBI_NO_EVOLUTION) {
        *outAdditionalSpecies = additionalSpecies;
        return 0;
    }
    switch (kind) {
    case EVOLUTION_CHECK_LEVEL_UP:
        species = getEvoPokemonLevelUp(pokemon, &additionalSpecies, info);
        break;
    case EVOLUTION_CHECK_ITEM:
        species = getEvoPokemonItem(pokemon, itemID, info);
        break;
    case EVOLUTION_CHECK_COMMUNICATION:
        species = getEvoPokemonCommunication(pokemon, info);
        break;
    default:
        species = EVOLUTION_SPECIES_INVALID;
        break;
    }
    *outAdditionalSpecies = additionalSpecies;
    return species;
}
