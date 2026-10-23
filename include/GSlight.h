#ifndef GS_LIGHT_H
#define GS_LIGHT_H

#include <types.h>
#include <GSgfxTypes.h>

struct _GSlight;
extern "C" {
_GSlight* GSlightLoad(void *light);
void GSlightSetType(_GSlight* light, long type);
void GSlightSetActive(_GSlight* light, long active);
void GSlightFree(_GSlight* light);
void GSlightSetRenderLayer(_GSlight* light, long count, GSgfxLayerID* layers);
u8 GSlightCanAnimate(_GSlight* light);
void GSlightSetAnimIndex(_GSlight* light, unsigned long index);
void GSlightSetAnimRate(_GSlight* light, f32 rate);
void GSlightSetAnimType(_GSlight* light, long type);
void GSlightStartAnimation(_GSlight* light);
}

#endif
