#ifndef ZOKUSEI_H
#define ZOKUSEI_H

#include <types.h>

struct PokemonData;

#ifdef __cplusplus
extern "C" {
#endif
u32 zokuseiGetPokemonSpriteID(struct PokemonData *data, u16 index);
u16 zokuseiBiosGetSpriteID(u16 moveID);
u32 zokuseiBiosGetSmallSpriteID(u16 dataID);
u32 zokuseiBiosGetName(u16 dataID);
extern u32 *zokusei_data_number;
#ifdef __cplusplus
}
#endif

#endif
