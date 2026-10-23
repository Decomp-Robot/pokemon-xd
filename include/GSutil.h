#ifndef GS_UTIL_H
#define GS_UTIL_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Cosine lookup table produced by GSmathInitCosTable. Entries 0 through 180 hold cos(i * 0.5 degrees).
void GSmathInitCosTable(void);
extern f32 gCosTable[181];

#ifdef __cplusplus
}
#endif

#endif
