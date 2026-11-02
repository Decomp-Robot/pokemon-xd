#ifndef GS_GFX_C_H
#define GS_GFX_C_H

#include <types.h>

typedef struct GSgfxDL GSgfxDL;
typedef struct GSgfxVF GSgfxVF;

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

/* Movie capture API */
void GSgfxCaptureMovieStart(s32 frames);
void GSgfxCaptureMovieStop(void);
void GSgfxCaptureSetTarget(s32 target);

/* Graphics display-list and vertex-format API */
void GSgfxDLBegin(GSgfxDL* list, GSgfxVF* vf, u16 blockCount, u16 vertexCount);
void GSgfxDLEnd(void);
void GSgfxDLEndSurface(void);
void GSgfxDLBeginSurface(u16 vertexCount);
void GSgfxDLDraw(GSgfxDL* displayList);
void GSgfxSetupRenderAndSelect(GSgfxVF* vf);
void GSgfxVertex3f(f32 x, f32 y, f32 z);
void GSgfxColor4u8(u8 r, u8 g, u8 b, u8 a);
void GSgfxSetLineWidth(f32 width);
void GSgfxSetPrimitiveType(s32 type);
void GSgfxBeginSurface(u16 vertexCount);
void GSgfxEndSurface(void);

#ifdef __cplusplus
}
#endif

#endif
