#ifndef GS_BOUND_H
#define GS_BOUND_H

#include <types.h>
#include <GSvec.h>

/* Model-space bound accumulator. */
struct GSbound {
    u16 mFlags;
    u16 mDataType;
    void *mData;
    u8 mShapeAndAnimationState[0x18];
};

extern "C" {
void GSboundGetSize(void *bound, GSvec *size, GSmtx *transform, bool localAxes);
bool GSboundHasAnimData(void *bound);
void GSboundSetAnimIndex(void *bound, u32 index);
void GSboundSetAnimFrame(void *bound, f32 frame);
void GSboundReset(void *bound);
void GSboundAddPoint(void *bound, f32 x, f32 y, f32 z);
}

#endif
