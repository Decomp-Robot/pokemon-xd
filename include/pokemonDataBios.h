#ifndef POKEMONDATABIOS_H
#define POKEMONDATABIOS_H
#include <types.h>
#include <PokemonData.h>

extern "C" unsigned long pokemonDataBiosGetName(PokemonData* data);
extern "C" unsigned long pokemonDataBiosGetStatusFaceMenuSpriteId(PokemonData* data, u16 form);
extern "C" u32 pokemonDataBiosGetTukamaetaFlag(PokemonData* data);
extern "C" u32 pokemonDataBiosGetMitaFlag(PokemonData* data);
#endif
