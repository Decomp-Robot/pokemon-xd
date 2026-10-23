#include <types.h>

/* Debug menu that forces the hero's active pokemon to relive (purify) and
 * re-arms the relive parameters. */
extern const f32 lbl_804EBDF8;

/* Not called directly by other code; marked so the linker keeps them. */
#pragma force_active on

void *heroGetStatus(s32 a, s32 b, s32 c);
u8 zz_battle_pokemon_stats_check_if_shadow_pokemon_c(void);
void pokemonSetDp(void *pokemon, f32 dp);
void preReliveSetParameter(s32 a, s32 b, s32 c);

#pragma peephole off

s32 dbgMenuRelive(void)
{
    void *hero = heroGetStatus(0, 3, 0);

    if (!zz_battle_pokemon_stats_check_if_shadow_pokemon_c()) {
        return 0;
    }
    pokemonSetDp(hero, lbl_804EBDF8);
    preReliveSetParameter(0, 0, 1);
    return 0;
}

#pragma peephole reset

#pragma force_active reset
