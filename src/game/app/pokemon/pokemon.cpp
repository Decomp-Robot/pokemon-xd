#include <types.h>
#include <Pokemon.h>
#include <pokemonBios.h>

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

}
