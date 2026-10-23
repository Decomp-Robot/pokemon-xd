#ifndef GSPRIM_H
#define GSPRIM_H
#include <GSgfxTypes.h>
#include <GSmathTypes.h>
#include <GStexture.h>

#ifdef __cplusplus
extern "C" {
#endif
void GSprimSetQuadVertexOrder(s32 order);
void GSprimDraw2DQuad4V(GSvec2 *vertices, GSgfxColAu8 color);
void GSprimDraw2DTexQuad4V(GSvec2 *vertices, GSvec2 *texCoords, GStexture *texture, GSgfxColAu8 color);
#ifdef __cplusplus
}
#endif
#endif
