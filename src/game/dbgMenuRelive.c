#include <types.h>
#include <hero.h>

/* Debug menu that forces the hero's active pokemon to relive (purify) and
 * re-arms the relive parameters. */

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on


void preReliveSetParameter(s32 a, s32 b, s32 c);

#pragma peephole off

s32 dbgMenuRelive(void)
{
    Pokemon *pokemon = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 0);

    if (!pokemonIsDarkPokemon(pokemon)) {
        return 0;
    }
    pokemonSetDp(pokemon, 0.0f);
    preReliveSetParameter(0, 0, 1);
    return 0;
}

#pragma peephole reset

#pragma force_active reset
