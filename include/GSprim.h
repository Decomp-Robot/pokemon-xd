#ifndef GS_PRIM_H
#define GS_PRIM_H

#include <GSgfxLight.h>
#include <GStexture.h>

enum GSprimQuadVertexOrder {
    GS_PRIM_QUAD_VERTEX_ORDER_STRIP = 0,
    GS_PRIM_QUAD_VERTEX_ORDER_QUAD = 1
};

extern "C" {
void GSprimSetQuadVertexOrder(long order);
void GSprimDraw2DQuad4V(const float vertices[4][2], GSgfxColAu8 color);
void GSprimDraw2DTexQuad4V(const float vertices[4][2], const float texCoords[4][2],
                          GStexture* texture, GSgfxColAu8 color);
}

#endif
