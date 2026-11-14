#include <types.h>
#include <Pokemon.h>
#include <PokemonData.h>
#include <pokemonBios.h>
#include <evolution.h>
#include <hero.h>
#include <heroItem.h>
#include <itemData.h>
#include <fight.h>
#include <fightFloor.h>
#include <fightTrainer.h>
#include <fightTrainerStatus.h>

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

// Species reported when the Pokemon cannot evolve.
#define EVOLUTION_SPECIES_NONE 0xFFFF

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
s32 getEvoPokemonCommunication(Pokemon *pokemon, POKEMON_EVOLUTION_INFO *info)
{
    s32 i;
    s32 species;
    PokemonData *data;
    u8 method;
    u16 condition;

    species = 0;
    data = getPokemonData(pokemon);
    if (data == 0) {
        return EVOLUTION_SPECIES_NONE;
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
s32 getEvoPokemonItem(Pokemon *pokemon, u16 itemID, POKEMON_EVOLUTION_INFO *info)
{
    s32 i;
    s32 species;
    PokemonData *data;
    u8 method;
    u16 condition;

    species = 0;
    data = getPokemonData(pokemon);
    if (data == 0) {
        return EVOLUTION_SPECIES_NONE;
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
s32 getEvoPokemonLevelUp(Pokemon *pokemon, u16 *additionalSpecies, POKEMON_EVOLUTION_INFO *info)
{
    s32 i;
    PokemonData *data;
    u32 additionalSpeciesId;
    u16 condition;
    u8 method;
    s32 species;
    Hero *hero;
    FIGHT_TRAINER *trainer;

    species = 0;
    additionalSpeciesId = 0;
    data = getPokemonData(pokemon);
    if (data == 0) {
        return EVOLUTION_SPECIES_NONE;
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

