#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <types.h>

struct NullSequence;
struct _GSmodel;

#ifdef __cplusplus
extern "C" {
#endif
struct _GSmodel *Sequence_GetGSmodel(u32 modelID);
s32 Sequence_GetModelSize(u32 modelID);
void Sequence_Free(struct NullSequence *sequence);
void Sequence_SetVisibility(struct NullSequence *sequence, bool visible);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

void Sequence_SetStatus(struct NullSequence *sequence, u8 status);
#ifdef __cplusplus
}
#endif

#endif
