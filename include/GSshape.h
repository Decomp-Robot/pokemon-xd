#ifndef GSSHAPE_H
#define GSSHAPE_H

#include <GSgfxTypes.h>

extern "C" {
u32 shapeGetInstancedMemorySizeAll();
void shapeFreePrimitiveInstance(u16 shape, s16 instance);
s16 shapeCreatePrimitiveInstance(u16 shape);
void shapeUpdate(u16 shape, s16 instance, f32 time);
void shapeRender(u16 shape, s16 instance, GSgfxLayerID layer);
}

#endif
