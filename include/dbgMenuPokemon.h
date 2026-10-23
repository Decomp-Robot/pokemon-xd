#ifndef DBGMENUPOKEMON_H
#define DBGMENUPOKEMON_H

#include <Pokemon.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 dbgMenuPokemonEditSub(Pokemon *pokemon, u8 inFight, u8 *appearanceChanged,
    u8 *statusChanged, u8 *abilityChanged, u8 *itemChanged);

#ifdef __cplusplus
}
#endif

#endif
