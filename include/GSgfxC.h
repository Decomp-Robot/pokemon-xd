#ifndef GS_GFX_C_H
#define GS_GFX_C_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void GSgfxZCompare(s32 enable, s32 comparison);
void GSgfxPixelMode(s32 colorUpdate, s32 alphaUpdate, s32 depthUpdate);
void GSgfxCullMode(s32 mode);
void GSgfxBlendMode(s32 mode, s32 sourceFactor, s32 destinationFactor);

#ifdef __cplusplus
}
#endif

#endif
