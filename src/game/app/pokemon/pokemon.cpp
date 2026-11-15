#include <types.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <pokemonDB.h>
#include <itemDataBios.h>
#include <GSsndPlayback.h>
#include <waza.h>

extern "C" {

// Move slots of a Pokemon record. Counting walks the four slots of a Pokemon
// and reports how many of them hold a move; forgetting drops the move of one
// slot and moves the moves behind it down, so the slots stay packed; copying
// replaces the move of one slot with the move of another.

int pokemonGetWazaCount(Pokemon *pokemon)
{
    int slot;
    int count;

    count = 0;
    if (!pokemonCheckValid(pokemon)) {
        return 0;
    }
    for (slot = 0; slot < 4; slot++) {
        if (pokemonWazaCheckValid(pokemon, slot)) {
            count++;
        }
    }

    return count;
}

void pokemonWazaForget(Pokemon *pokemon, int slot)
{
    u16 moveSlot;

    pokemonWazaInit(pokemon, slot);
    for (moveSlot = slot; moveSlot < 3; moveSlot++) {
        if (!pokemonWazaCheckValid(pokemon, moveSlot + 1)) {
            break;
        }
        pokemonWazaCopy(pokemon, moveSlot, moveSlot + 1);
    }
    pokemonWazaInit(pokemon, moveSlot);
}

void pokemonWazaCopy(Pokemon *pokemon, u16 destination, u16 source)
{
    PokemonWaza *destinationWaza;
    PokemonWaza *sourceWaza;

    if (pokemon == 0) {
        return;
    }
    destinationWaza = pokemonBiosGetPokemonWazaPtr(pokemon, destination, 0);
    sourceWaza = pokemonBiosGetPokemonWazaPtr(pokemon, source, 0);
    pokemonWazaBiosCopy(destinationWaza, sourceWaza);
}


void pokemonPlayVoice(Pokemon *pokemon)
{
    if (pokemon != 0) {
        PokemonData *data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon));
        if (data != 0) {
            GSsndPlay((u16)pokemonDataBiosGetVoice(data), 0, 0);
        }
    }
}

u8 pokemonGetDarkPokemonCallKakuritu(Pokemon *pokemon)
{
    return 100;
}

u8 pokemonGetDarkPokemonHyperJoutaiKakuritu(Pokemon *pokemon)
{
    u8 probability[5][5] = {
        {0, 0, 0, 0, 0},
        {10, 10, 10, 5, 2},
        {20, 15, 15, 10, 5},
        {35, 25, 20, 15, 10},
        {50, 40, 30, 20, 15}
    };
    u8 level;
    u8 pattern;
    pattern = darkPokemonGetDarkPokemon(pokemon_GetDarkpokemonDataId(pokemon))->getReversPattern();
    level = pokemonGetDarkPokemonLevel(pokemon);

    if (pokemon == 0) {
        return 0;
    }
    if (!pokemonIsDarkPokemon(pokemon)) {
        return 0;
    }
    if (level > 4) {
        level = 4;
    }
    if (pattern > 4) {
        pattern = 4;
    }
    if (level <= 4 && pattern < 5) {
        return probability[level][pattern];
    }
    return 0;
}

u8 pokemonGetDarkPokemonLevel(Pokemon *pokemon)
{
    if (pokemon == 0) {
        return POKEMON_DARK_LEVEL_NONE;
    }
    if (pokemonIsDarkPokemon(pokemon) == 1) {
        u16 initialDp = pokemon_GetInitDp(pokemon);
        if (initialDp == 0) {
            initialDp = 1;
        }
        f32 dp = pokemonGetDp(pokemon);
        f32 percent;
        if (dp < 0.0f) {
            percent = 0.0f;
        } else {
            percent = 100.0f * dp / initialDp;
        }
        if (percent >= 99.9f) {
            return POKEMON_DARK_LEVEL_CLOSED;
        }
        if (percent >= 80.0f) {
            return POKEMON_DARK_LEVEL_FIRST;
        }
        if (percent >= 60.0f) {
            return POKEMON_DARK_LEVEL_SECOND;
        }
        if (percent >= 40.0f) {
            return POKEMON_DARK_LEVEL_THIRD;
        }
        if (percent >= 20.0f) {
            return POKEMON_DARK_LEVEL_FOURTH;
        }
        if (percent > 0.0f) {
            return POKEMON_DARK_LEVEL_FIFTH;
        }
        if (percent == 0.0f) {
            return POKEMON_DARK_LEVEL_OPEN;
        }
        return POKEMON_DARK_LEVEL_NONE;
    }
    return POKEMON_DARK_LEVEL_NONE;
}

void pokemonAddDpFormPokemonDpFilterId(Pokemon *pokemon, u16 itemID, u16 filterID)
{
    PokemonSeikakuData *nature;
    PokemonSeikakuRateData *rate;
    s8 rateID = 0;
    if (pokemonIsDarkPokemon(pokemon)) {
        nature = pokemonSeikakuDataBiosGetPtr(pokemon_GetSeikaku(pokemon));
        if (nature != 0) {
            f32 change = pokemonDpFilterDataBiosGetValue(pokemonDpFilterDataBiosGetPtr(filterID));
            if (filterID == POKEMON_DP_FILTER_NADENADE) {
                ItemData *item = itemDataBiosGetPtr(itemID);
                if (item == 0) {
                    return;
                }
                if (itemDataBiosGetKind(item) != ITEM_KIND_COLOGNE) {
                    return;
                }
                change *= itemDataBiosGetBuff(item);
            }
            if (filterID == POKEMON_DP_FILTER_OPEN) {
                change = -1.0f * pokemonGetDp(pokemon);
            }
            if (filterID == POKEMON_DP_FILTER_FIGHTOUT) {
                rateID = pokemonSeikakuDataBiosGetReliveFightout(nature);
            } else if (filterID == POKEMON_DP_FILTER_WALK) {
                rateID = pokemonSeikakuDataBiosGetReliveWalk(nature);
            } else if (filterID == POKEMON_DP_FILTER_SODATEYA) {
                rateID = pokemonSeikakuDataBiosGetReliveSodateya(nature);
            } else if (filterID == POKEMON_DP_FILTER_NADENADE) {
                rateID = pokemonSeikakuDataBiosGetReliveNadenade(nature);
            } else if (filterID == POKEMON_DP_FILTER_CALL) {
                change = -3.0f;
                rateID = pokemonSeikakuDataBiosGetReliveCall(nature);
            }
            if (rateID != 0) {
                rate = pokemonSeikakuRateDataBiosGetPtr(rateID);
                if (rate != 0) {
                    u8 numerator = pokemonSeikakuRateDataBiosGetKake(rate);
                    u8 denominator = pokemonSeikakuRateDataBiosGetWaru(rate);
                    if (denominator == 0) {
                        return;
                    }
                    change *= numerator;
                    change /= denominator;
                } else {
                    return;
                }
            }
            change *= 100.0f;
            pokemonAddDp(pokemon, change);
        }
    }
}

void pokemonAddDp(Pokemon *pokemon, f32 change)
{
    if (pokemon != 0) {
        f32 dp = pokemonGetDp(pokemon);
        dp += change;
        if (dp < 0.0f) {
            dp = 0.0f;
        }
        pokemonSetDp(pokemon, dp);
    }
}

void pokemonSetDp(Pokemon *pokemon, f32 dp)
{
    if (pokemon != 0) {
        pokemonBiosSetDp(pokemon, (int)dp);
    }
}

f32 pokemonGetDp(Pokemon *pokemon)
{
    if (pokemon == 0) {
        return 0.0f;
    }
    return pokemonBiosGetDp(pokemon);
}

u8 pokemonIsDarkPokemon(Pokemon *pokemon)
{
    return pokemon_GetDarkFlag(pokemon);
}

void pokemonSetDarkPokemonStatus(Pokemon *pokemon, u16 shadowID)
{
    if (pokemon == 0) {
        return;
    }
    pokemonInitDarkPokemon(pokemon);
    pokemon_SetDarkpokemonDataId(pokemon, shadowID);
    pokemonSetDp(pokemon, darkPokemonBiosGetDarkPoint(shadowID));
    for (u8 slot = 0; slot < 4; slot++) {
        pokemonWazaCreate(pokemon, slot, WAZA_SHADOW_SPECIAL);
        u16 move = darkPokemonBiosGetTemotiDarkWaza(shadowID, slot);
        if (move != 0 && move != WAZA_SHADOW_SPECIAL) {
            pokemonWazaCreate(pokemon, slot, move);
        }
    }
    pokemon->setReversMode(false);
    DarkPokemon *dark = pokemon->getDarkPokemon();
    if (dark != 0) {
        dark->setDarkPokemonID(shadowID);
    }
}

}
