#ifndef THP_DRAW_H
#define THP_DRAW_H

#include <types.h>
class GSgfxLayer;

extern "C" {
u16 THPGXYuv2RgbDraw(void *yImage, void *uImage, void *vImage, s16 x, s16 y,
                     s16 textureWidth, s16 textureHeight, s16 polygonWidth, s16 polygonHeight);
void THPGXYuv2RgbSetup(void);
void THPGXRestore(void);
void THPGXSetRenderLayer(GSgfxLayer *layer);
}

#endif
