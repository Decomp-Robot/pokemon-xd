#include <types.h>
#include <heroStatus.h>
#include <Pokemon.h>

/* Debug menu that forces the hero's active pokemon to relive (purify) and
 * re-arms the relive parameters. */
extern const f32 purificationClearedDarkness;

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

void pokemonSetDp(void *pokemon, f32 dp);
void preReliveSetParameter(s32 a, s32 b, s32 c);

#pragma peephole off

s32 dbgMenuRelive(void)
{
    Pokemon *pokemon = (Pokemon *)heroGetStatus(0, HERO_STATUS_POKEMON, 0);

    if (!pokemonIsShadow(pokemon)) {
        return 0;
    }
    pokemonSetDp(pokemon, purificationClearedDarkness);
    preReliveSetParameter(0, 0, 1);
    return 0;
}

#pragma peephole reset

#pragma force_active reset
