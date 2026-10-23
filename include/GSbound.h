#ifndef GSBOUND_H
#define GSBOUND_H
#include <GSmathTypes.h>
struct GSbound {
    u16 mFlags;
    u16 mDataType;
    void *mData;
    u8 mShapeAndAnimationState[0x18];
};

#ifdef __cplusplus
extern "C" {
#endif
bool GSboundHasAnimData(GSbound *bound);
void GSboundSetAnimIndex(GSbound *bound, u32 index);
void GSboundSetAnimFrame(GSbound *bound, f32 frame);
void GSboundReset(GSbound *bound);
void GSboundAddPoint(GSbound *bound, f32 x, f32 y, f32 z);
void GSboundAlwaysInView(GSbound *bound, bool enabled);
void GSboundGetSize(GSbound *bound, GSvec *size, GSmtx *transform, bool localAxes);
#ifdef __cplusplus
}
#endif
#endif
