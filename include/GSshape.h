#ifndef GSSHAPE_H
#define GSSHAPE_H

#include <GSgfxTypes.h>

extern "C" {
void shapePrimitiveEditorRender(GSgfxLayerID layerID);
u32 shapeGetInstancedMemorySizeAll();
void shapeFreePrimitiveInstance(u16 shape, s16 instance);
s16 shapeCreatePrimitiveInstance(u16 shape);
void shapeUpdate(u16 shape, s16 instance, f32 time);
void shapeRender(u16 shape, s16 instance, GSgfxLayerID layer);
bool shapeInit(u16 count);
bool shapeLoad(u16 shape, void *data);
void shapeRelease(u16 shape);
s32 shapeGenerateIndex();
u32 shapeGetUsedMemorySize(u16 shape);
}

#endif
