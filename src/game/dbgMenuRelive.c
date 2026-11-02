#include <types.h>
#include <hero.h>
#include <pokemonRelive.h>

/* Debug menu: make the party's dark Pokemon eligible for relive. */

#pragma force_active on

#pragma peephole off

s32 dbgMenuRelive(void)
{
    Pokemon *pokemon = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 0);

    if (!pokemonIsDarkPokemon(pokemon)) {
        return 0;
    }
    pokemonSetDp(pokemon, 0.0f);
    preReliveSetParameter(PRE_RELIVE_MODE_START_PURIFICATION, 0, 1);
    return 0;
}

#pragma peephole reset

#pragma force_active reset
