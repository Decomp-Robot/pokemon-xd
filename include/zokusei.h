#ifndef ZOKUSEI_H
#define ZOKUSEI_H

#include <types.h>

struct PokemonData;

#ifdef __cplusplus
extern "C" {
#endif
u16 zokuseiGetPokemonSpriteID(struct PokemonData *data, u16 index);
u8 zokuseiBiosGetWazaKind(u16 zokuseiDataId);
u16 zokuseiGetWazaJoutai_PokemonId(u16 zokusei, u16 pokemonID);
u16 zokuseiGetWazaJoutaiAsDefenseZokuseiAry(u16 zokusei, u16 *defense, u16 count);
u16 zokuseiBiosGetSpriteID(u16 moveID);
u32 zokuseiBiosGetSmallSpriteID(u16 dataID);
u32 zokuseiBiosGetName(u16 dataID);
extern u32 *zokusei_data_number;
#ifdef __cplusplus
}
#endif

#endif
