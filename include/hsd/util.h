#ifndef HSD_UTIL_H
#define HSD_UTIL_H

#include <dolphin/gx/GXStruct.h>

#ifdef __cplusplus
extern "C" {
#endif
void HSD_MulColor(GXColor *src0, GXColor *src1, GXColor *dst);
s32 HSD_GetNbBits(u32 value);
#ifdef __cplusplus
}
#endif

#endif
