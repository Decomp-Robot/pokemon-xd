#ifndef GSBOUND_H
#define GSBOUND_H
#include <GSmathTypes.h>
struct GSboundAnimHeader;
struct GSgfxLayer;
enum { GS_BOUND_DATA_MODEL = 1 };

struct GSbound {
    u16 mFlags;
    u16 mDataType;
    void *mData;
    u32 mAnimIndex;
    f32 mAnimFrame;
    GSboundAnimHeader *mAnimHeader;
    void **mAnimData;
    u8 _18[8];
};

#ifdef __cplusplus
extern "C" {
#endif
void GSboundFree(GSbound *bound);
void GSboundCreate(GSbound *bound, GSboundAnimHeader *header);
void GSboundSetDataPtr(GSbound *bound, s32 type, void *data);
s32 GSboundIsInViewFast(GSbound *bound);
void GSboundGetWorldPosition(GSbound *bound, GSvec *position);
bool GSboundHasAnimData(GSbound *bound);
void GSboundSetAnimIndex(GSbound *bound, u32 index);
void GSboundSetAnimFrame(GSbound *bound, f32 frame);
void GSboundReset(GSbound *bound);
void GSboundAddPoint(GSbound *bound, f32 x, f32 y, f32 z);
void GSboundAlwaysInView(GSbound *bound, bool enabled);
void GSboundGetSize(GSbound *bound, GSvec *size, GSmtx *transform, bool applyRotation);
#ifdef __cplusplus
}
#endif
void GSboundDraw(GSbound *bound, GSgfxLayer *layer);
#endif
