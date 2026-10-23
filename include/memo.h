#ifndef MEMO_H
#define MEMO_H

#include <types.h>
#include <Pokemon.h>

#ifdef __cplusplus
extern "C" {
#endif

void memoInit(void *data);
void memoInitDebug(void *data);
void memoDataSet(void *data, Pokemon *pokemon);
void memoDataSetPokemonID(void *data, u16 id);
s32 memoDataSetMemoFlag(void *data);
u16 memoDataGetCount(void *data);
u16 memoDataGetPokemonID(void *data, u16 index);
u32 memoDataGetPokemonRndFromID(void *data, u16 id);
u32 memoDataGetPokemonTrainerRndFromID(void *data, u16 id);
f32 memoGetScaleAngle(u16 id, f32 *scale, f32 *angle);

#ifdef __cplusplus
}
#endif

#endif
