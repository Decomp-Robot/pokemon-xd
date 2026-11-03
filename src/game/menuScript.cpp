#include <savedata.h>
#include <heroBios.h>
#include <Pokemon.h>
#include <pokemonBios.h>
#include <darkPokemon.h>
#include <types.h>

// Adds wazaID to the waza list the caller is building unless that waza is
// already in it. The caller advances count when the waza is appended.
bool _checkWazaOverlap(u16 *wazaList, u16 wazaID, u16 count)
{
    u16 i;

    for (i = 0; i < count; i++) {
        if (wazaList[i] == wazaID) {
            return true;
        }
    }
    wazaList[count] = wazaID;
    return false;
}

// A shadow Pokemon cannot be looked at until its relive flag is set.
bool _checkRelive(Pokemon *pokemon)
{
    u16 darkID;

    if (!pokemonCheckValid(pokemon)) {
        return false;
    }

    darkID = pokemonBiosGetDarkpokemonDataId(pokemon);
    if (darkID != 0) {
        return darkPokemonBiosGetReliveFlag(darkID) != 0;
    }

    return true;
}

// Party Pokemon in the given slot, or null when the slot holds nothing usable.
Pokemon *_getPokemonPtr(long index)
{
    Pokemon *pokemon;

    pokemon = heroBiosGetPokemonPtr((Hero *)savedataGetStatus(0, SAVE_DATA_HERO), (u16)index);

    if (pokemonCheckValid(pokemon)) {
        return pokemon;
    }

    return 0;
}
