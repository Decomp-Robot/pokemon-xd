#ifndef GS_GFX_C_H
#define GS_GFX_C_H

#include <types.h>

typedef struct GSgfxDL GSgfxDL;
typedef struct GSgfxVF GSgfxVF;
typedef struct GStexture GStexture;

/* Storage the movie capture writes its frames to. */
typedef enum GSgfxCaptureTarget {
    GSGFX_CAPTURE_TARGET_BBA = 0,
    GSGFX_CAPTURE_TARGET_SD = 1
} GSgfxCaptureTarget;

#ifdef __cplusplus
extern "C" {
#endif

u8 GSgfxResetDisable(void);

void GSgfxZCompare(u8 enable, s32 comparison);
void GSgfxAlphaCompare(u8 enable, s32 compare0, u8 reference0, s32 operation,
                       s32 compare1, u8 reference1);
void GSgfxPixelMode(u8 colorUpdate, u8 alphaUpdate, u8 depthUpdate);
void GSgfxCullMode(s32 mode);
void GSgfxBlendMode(s32 mode, s32 sourceFactor, s32 destinationFactor);
u8 GSgfxLayerGetStatsRenderMode(void);
void GSgfxLayerSetStatsRenderMode(u8 mode);

/* Movie capture API */
void GSgfxCaptureMovieStart(s32 frames);
void GSgfxCaptureMovieStop(void);
void GSgfxCaptureSetTarget(GSgfxCaptureTarget target);

/* Graphics display-list and vertex-format API */
void GSgfxDLBegin(GSgfxDL* list, GSgfxVF* vf, u16 blockCount, u16 vertexCount);
void GSgfxDLEnd(void);
void GSgfxDLEndSurface(void);
void GSgfxDLBeginSurface(u16 vertexCount);
void GSgfxDLDraw(GSgfxDL* displayList);
void GSgfxSetupRenderAndSelect(GSgfxVF* vf);
void GSgfxSetTUnitUserAlphaArg(u8 stage, u32 a, u32 b, u32 c, u32 d);
void GSgfxSetTUnitUserAlphaOp(u8 stage, u32 operation, u32 bias, u32 scale, u8 clamp, u32 output);
void GSgfxSetTUnitUserColorArg(u8 stage, u32 a, u32 b, u32 c, u32 d);
void GSgfxSetTUnitUserColorOp(u8 stage, u32 operation, u32 bias, u32 scale, u8 clamp, u32 output);
void GSgfxSelectTexture(u8 textureMap, GStexture* texture);
void GSgfxTexCoord2f(f32 s, f32 t);
void GSgfxVertex2s16(s16 x, s16 y);
void GSgfxVertex2f(f32 x, f32 y);
void GSgfxVertex3f(f32 x, f32 y, f32 z);
void GSgfxColor4u8(u8 r, u8 g, u8 b, u8 a);
void GSgfxSetLineWidth(f32 width);
u16 GSgfxTokenMark(void);
void GSgfxSetPrimitiveType(s32 type);
void GSgfxBeginSurface(u16 vertexCount);
void GSgfxEndSurface(void);

#ifdef __cplusplus
}
#endif

#endif
