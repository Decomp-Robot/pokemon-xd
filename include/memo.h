#ifndef MEMO_H
#define MEMO_H
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
struct MemoData;
void memoDataSet(MemoData* memo, struct Pokemon* pokemon);

void memoGetScaleAngle(u16 pokemonID, f32 *scale, f32 *angle);
#ifdef __cplusplus
}
#endif
#endif
