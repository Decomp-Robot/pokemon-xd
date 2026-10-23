#ifndef POKEMON_H
#define POKEMON_H

#include <types.h>

// Pokemon -- party/battle pokemon record.
struct Pokemon {
    u8 _00[0xC4];
};

#ifdef __cplusplus
extern "C" {
#endif

void pokemonInit(Pokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
