#ifndef GS_GFX_C_H
#define GS_GFX_C_H

#include <types.h>

/* Storage the movie capture writes its frames to. */
enum {
    GSGFX_CAPTURE_TARGET_BBA = 0,
    GSGFX_CAPTURE_TARGET_SD = 1
};

#ifdef __cplusplus
extern "C" {
#endif

void GSgfxZCompare(s32 enable, s32 comparison);
void GSgfxPixelMode(s32 colorUpdate, s32 alphaUpdate, s32 depthUpdate);
void GSgfxCullMode(s32 mode);
void GSgfxBlendMode(s32 mode, s32 sourceFactor, s32 destinationFactor);
u8 GSgfxLayerGetStatsRenderMode(void);
void GSgfxLayerSetStatsRenderMode(u8 mode);
void GSgfxCaptureMovieStart(s32 frames);
void GSgfxCaptureMovieStop(void);
void GSgfxCaptureSetTarget(s32 target);

#ifdef __cplusplus
}
#endif

#endif
