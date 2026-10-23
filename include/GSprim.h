#ifndef GSPRIM_H
#define GSPRIM_H
#include <GSgfxTypes.h>
#include <GSmathTypes.h>
#include <GStexture.h>

#ifdef __cplusplus
extern "C" {
#endif
void GSprimSetQuadVertexOrder(s32 order);
void GSprimDraw2DQuad(GSgfxColAu8 color, f32 x, f32 y, f32 width, f32 height);
void GSprimDraw2DQuad4V(GSvec2 *vertices, GSgfxColAu8 color);
void GSprimSetTextColor(GSgfxColAu8 color);
void GSprimSetTextWidth(f32 width);
void GSprimSetTextSize(f32 width, f32 height);
void GSprimWriteStr(f32 x, f32 y, const char *format, ...);
void GSprimDraw2DTexQuad4V(GSvec2 *vertices, GSvec2 *texCoords, GStexture *texture, GSgfxColAu8 color);
#ifdef __cplusplus
}
#endif
#endif
