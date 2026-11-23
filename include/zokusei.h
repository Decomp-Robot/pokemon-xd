#ifndef ZOKUSEI_H
#define ZOKUSEI_H

#include <types.h>

struct PokemonData;

enum { ZOKUSEI_DATA_ID_COUNT = 18 };

typedef struct ZokuseiData {
    u8 wazaKind;
    u16 spriteID;
    u16 smallSpriteID;
    u32 nameMessageId;
    u16 joutai[ZOKUSEI_DATA_ID_COUNT];
} ZokuseiData;

#ifdef __cplusplus
extern "C" {
#endif
u16 zokuseiGetPokemonSpriteID(struct PokemonData *data, u16 index);
u8 zokuseiBiosGetWazaKind(u16 zokuseiDataId);
u16 zokuseiBiosGetWazaJoutai(u32 zokuseiDataId, u32 defenseZokuseiDataId);
u16 zokuseiGetWazaJoutai_PokemonId(u16 zokusei, u16 pokemonID);
s32 zokuseiGetAisyouValue(u16 zokuseiDataId, u8 direction, u8 filter);
u16 zokuseiGetWazaJoutaiAsDefenseZokuseiAry(u16 zokusei, u16 *defense, u16 count);
u16 zokuseiGetWazaJoutai(u16 zokuseiDataId, u16 defenseZokuseiDataId);
u16 zokuseiBiosGetSpriteID(u16 zokuseiDataId);
u16 zokuseiBiosGetSmallSpriteID(u16 zokuseiDataId);
u32 zokuseiBiosGetName(u16 zokuseiDataId);
extern u32 *zokusei_data_number;
extern ZokuseiData *zokusei_data;
#ifdef __cplusplus
}
#endif

#endif
