#include <string.h>
#include <sodateya.h>
#include <savedata.h>
#include <pokemonBios.h>
#include <Pokemon.h>
#include <hero.h>
#include <darkPokemon.h>
#include <fightSeq.h>

// DP are charged in steps of this size; one step costs PRICE_UNIT on top of
// the base fee.
enum {
    SODATEYA_DP_UNIT = 100,
    SODATEYA_PRICE_UNIT = 100,
    SODATEYA_BASE_FEE = 100
};

// Day-care block of the save image.
typedef struct SodateyaSave {
    u8 occupied; // 0 while the slot is empty, 1 while a Pokemon is deposited
    u8 savedLevel; // level the Pokemon had when it was left here
    u8 pad[2];
    u32 savedDp; // DP the Pokemon had when it was left here
    Pokemon pokemon; // deposited Pokemon
} SodateyaSave;

// Day-care block of the save image, or null when the save data is unavailable.
inline SodateyaSave *sodateyaGetSavePtr(SodateyaSave *save)
{
    if (save == 0) {
        save = (SodateyaSave *)savedataGetStatus(0, SAVEDATA_BLOCK_SODATEYA);
    }
    return save;
}

// DP the deposited Pokemon had when it was left here, or an all-bits-set
// value while the slot is empty.
inline u32 sodateyaGetSavedDp(void)
{
    SodateyaSave *save = sodateyaGetSavePtr(0);

    return save->occupied ? save->savedDp : (u32)-1;
}

inline Pokemon *sodateyaGetSavedPokemon(void)
{
    return &sodateyaGetSavePtr(0)->pokemon;
}

// DP the deposited Pokemon has gained since it was left here.
inline int sodateyaGetDpGain(void)
{
    (void)sodateyaGetSavePtr(0);
    return ((int)sodateyaGetSavedDp() - pokemonBiosGetDp(sodateyaGetSavedPokemon())) / SODATEYA_DP_UNIT;
}

// Level the deposited Pokemon had when it was left here.
inline int sodateyaGetSavedLevel(void)
{
    SodateyaSave *save = sodateyaGetSavePtr(0);

    return save->occupied ? save->savedLevel : 0xFF;
}

// 0 while the day-care slot is empty, 1 while it is occupied, -1 on error.
int sodateyaeChekcRais(void)
{
    SodateyaSave *save = sodateyaGetSavePtr(0);

    if (save == 0) {
        return -1;
    }
    switch (save->occupied) {
    case 1:
        return 1;
    case 0:
        return 0;
    }
    return -1;
}

// Fee for leaving a Pokemon at the day-care, added to the price of an item.
int sodateyaCalcPrice(int price)
{
    int fee;

    if (sodateyaGetSavedDp() != 0) {
        int dpGain = sodateyaGetDpGain();

        fee = dpGain ? dpGain * SODATEYA_PRICE_UNIT + SODATEYA_BASE_FEE : 0;
    } else {
        fee = 0;
    }
    return price * SODATEYA_PRICE_UNIT + SODATEYA_BASE_FEE + fee;
}

// Takes the deposited Pokemon back into the party slot it came from.
void sodateyaReclaim(int index)
{
    Pokemon *deposited;
    SodateyaSave *save;
    Pokemon *pokemon;

    pokemon = heroBiosGetPokemonPtr((Hero *)savedataGetStatus(0, SAVE_DATA_HERO), (u16)index);
    save = (SodateyaSave *)savedataGetStatus(0, SAVEDATA_BLOCK_SODATEYA);

    if (pokemon != 0) {
        deposited = &sodateyaGetSavePtr(save)->pokemon;
        if (deposited != 0) {
            memcpy(pokemon, deposited, sizeof(Pokemon));
            if (pokemonBiosGetDarkFlag(deposited)) {
                pokemonBiosGetDp(deposited);
            }
            DarkPokemon *dark = pokemon->getDarkPokemon();
            if (dark != 0) {
                dark->setPlace(DARK_POKEMON_WHERE_PARTY, 0);
            }
            if (save == 0) {
                save = (SodateyaSave *)savedataGetStatus(0, SAVEDATA_BLOCK_SODATEYA);
            }
            if (save != 0) {
                save->occupied = 0;
            }
            pokemonInit(deposited);
        }
    }
    pokemonAllKaihuku(pokemon);
}


// Leaves a party Pokemon at the day-care and closes the gap it leaves in the
// party.
void sodateyaRaise(int index)
{
    u8 level;
    int count;
    SodateyaSave *save;
    Pokemon *pokemon;
    Hero *hero;
    u8 raised;

    hero = (Hero *)savedataGetStatus(0, SAVE_DATA_HERO);
    pokemon = heroBiosGetPokemonPtr(hero, (u16)index);
    count = heroGetPokeomCount(hero);
    save = (SodateyaSave *)savedataGetStatus(0, SAVEDATA_BLOCK_SODATEYA);

    if (pokemon == 0) {
        raised = 0;
    } else {
        SodateyaSave *status;

        level = pokemonBiosGetLevel(pokemon);
        status = sodateyaGetSavePtr(save);
        if (status != 0) {
            status->savedLevel = level;
        }
        Pokemon *deposited = &sodateyaGetSavePtr(save)->pokemon;
        if (deposited == 0) {
            raised = 0;
        } else {
            if (pokemonIsJoutaiDataId(pokemon, JOUTAI_DARK_HYPER_MODE)) {
                pokemonInitJoutaiDataId(pokemon, JOUTAI_DARK_HYPER_MODE);
            }
            memcpy(deposited, pokemon, sizeof(Pokemon));
            if (pokemonBiosGetDarkFlag(pokemon)) {
                save->savedDp = pokemonBiosGetDp(pokemon);
            } else {
                save->savedDp = 0;
            }
            DarkPokemon *dark = pokemon->getDarkPokemon();
            if (dark != 0) {
                dark->setPlace(DARK_POKEMON_WHERE_SODATEYA, 0);
            }
            if (save == 0) {
                save = (SodateyaSave *)savedataGetStatus(0, SAVEDATA_BLOCK_SODATEYA);
            }
            if (save != 0) {
                save->occupied = 1;
            }
            pokemonInit(pokemon);
            raised = 1;
        }
    }

    if (raised) {
        if (index < 6) {
            int i;
            Pokemon *prev;

            for (i = index; i < count - 1; i++) {
                prev = heroBiosGetPokemonPtr(hero, (u16)i);
                pokemon = heroBiosGetPokemonPtr(hero, (u16)(i + 1));
                if (!pokemonCheckValid(pokemon)) {
                    break;
                }
                memcpy(prev, pokemon, sizeof(Pokemon));
            }
            pokemonInit(pokemon);
        }
    }
}

// DP the deposited Pokemon has gained since it was left at the day-care.
int sodateyaGetPokemonDPValue(void)
{
    return sodateyaGetDpGain();
}

// Levels the deposited Pokemon has gained since it was left at the day-care.
int sodateyaGetPokemonLevelUpValue(void)
{
    int savedLevel = sodateyaGetSavedLevel();
    u8 level = pokemonBiosGetLevel(sodateyaGetSavedPokemon());

    return level - (u8)savedLevel;
}
