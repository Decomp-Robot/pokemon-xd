#ifndef POKEMONDATABIOS_H
#define POKEMONDATABIOS_H
#include <types.h>
#include <PokemonData.h>

extern "C" unsigned long pokemonDataBiosGetName(PokemonData* data);
extern "C" unsigned long pokemonDataBiosGetStatusFaceMenuSpriteId(PokemonData* data, u16 form);
#endif
