#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <types.h>

struct _GSmodel;

#ifdef __cplusplus
extern "C" {
#endif
struct _GSmodel *Sequence_GetGSmodel(u32 modelID);
s32 Sequence_GetModelSize(u32 modelID);
void Sequence_Free(void *sequence);
void Sequence_SetVisibility(void *sequence, u8 visible);
#ifdef __cplusplus
}
#endif
#endif
