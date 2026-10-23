#ifndef ZOKUSEI_H
#define ZOKUSEI_H

#include <PokemonData.h>
#include <types.h>

enum PokemonType {
    POKEMON_TYPE_NORMAL = 0,
    POKEMON_TYPE_FIGHTING = 1,
    POKEMON_TYPE_FLYING = 2,
    POKEMON_TYPE_POISON = 3,
    POKEMON_TYPE_GROUND = 4,
    POKEMON_TYPE_ROCK = 5,
    POKEMON_TYPE_BUG = 6,
    POKEMON_TYPE_GHOST = 7,
    POKEMON_TYPE_STEEL = 8,
    POKEMON_TYPE_FIRE = 10,
    POKEMON_TYPE_WATER = 11,
    POKEMON_TYPE_GRASS = 12,
    POKEMON_TYPE_ELECTRIC = 13,
    POKEMON_TYPE_PSYCHIC = 14,
    POKEMON_TYPE_ICE = 15,
    POKEMON_TYPE_DRAGON = 16,
    POKEMON_TYPE_DARK = 17,
};

#ifdef __cplusplus
extern "C" {
#endif

u32 zokuseiGetPokemonSpriteID(PokemonSpeciesStats *data, s32 index);
u16 zokuseiBiosGetSmallSpriteID(u16 type);

#ifdef __cplusplus
}
#endif

#endif
