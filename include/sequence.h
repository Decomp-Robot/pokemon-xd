#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <types.h>

struct NullSequence;
struct _GSmodel;
struct _GScamera;

/* Signed size categories returned by Sequence_GetModelSize for resource
   records and consumed by the menu model cameras. */
enum MODEL_SIZE_CLASS {
    MODEL_SIZE_CLASS_SMALLEST = -2,
    MODEL_SIZE_CLASS_SMALL = -1,
    MODEL_SIZE_CLASS_MEDIUM = 0,
    MODEL_SIZE_CLASS_LARGE = 1,
    MODEL_SIZE_CLASS_LARGER = 2,
    MODEL_SIZE_CLASS_LARGEST = 3
};

#ifdef __cplusplus
extern "C" {
#endif
struct _GSmodel *Sequence_GetGSmodel(u32 modelID);
s32 Sequence_GetModelSize(u32 modelID);
void Sequence_SetCamera(struct NullSequence *sequence, struct _GScamera *camera);
void Sequence_DoAnimation(struct NullSequence *sequence, int animation);
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
