#ifndef GSBOUND_H
#define GSBOUND_H

#include <GSmathTypes.h>

struct GSboundAnimHeader;
struct GSboundAnimFrame;
struct GSgfxLayer;

enum {
    GS_BOUND_EMPTY = 1 << 0,
    GS_BOUND_ALWAYS_IN_VIEW = 1 << 2,
    GS_BOUND_ANIMATED = 1 << 3
};

enum {
    GS_BOUND_DATA_NONE = 0,
    GS_BOUND_DATA_MODEL = 1,
    GS_BOUND_DATA_MATRIX = 2,
    GS_BOUND_DATA_SRT = 3
};

enum {
    GS_BOUND_OUT_OF_VIEW = 0,
    GS_BOUND_PARTLY_IN_VIEW = 1,
    GS_BOUND_IN_VIEW = 2
};

struct GSbound {
    u16 mFlags;
    u16 mDataType;
    void *mData;
    union {
        struct { GSvec mMin; GSvec mMax; } box;
        struct {
            u32 mAnimIndex;
            f32 mAnimFrame;
            GSboundAnimHeader *mAnimHeader;
            GSboundAnimFrame **mAnimData;
        } anim;
    } u;
};

#ifdef __cplusplus
extern "C" {
#endif
void GSboundGetShadowSize(GSvec *size, GSbound *bound, GSvec *from, GSvec *to, GSvec *direction);
void GSboundGetShadowVolume(GSbound *shadow, f32 height, GSbound *bound, GSvec *light);
void GSboundGetLocalPosition(GSbound *bound, GSvec *position);
void GSboundGetWorldPosition(GSbound *bound, GSvec *position);
void GSboundGetSize(GSbound *bound, GSvec *size, bool scale, bool rotate);
void GSboundGetMinMax(GSbound *bound, GSvec *min, GSvec *max, bool scale, bool rotate, bool translate, bool ignoreRoot);
void GSboundSetAnimFrame(GSbound *bound, f32 frame);
void GSboundSetAnimIndex(GSbound *bound, u32 index);
bool GSboundHasAnimData(GSbound *bound);
void GSboundAlwaysInView(GSbound *bound, bool enabled);
s32 GSboundIsInViewFast(GSbound *bound);
void GSboundAddPoint(GSbound *bound, f32 x, f32 y, f32 z);
void GSboundReset(GSbound *bound);
void GSboundSetDataPtr(GSbound *bound, s32 type, void *data);
void GSboundFree(GSbound *bound);
void GSboundCreate(GSbound *bound, GSboundAnimHeader *header);
#ifdef __cplusplus
}
#endif
void GSboundDraw(GSbound *bound, GSgfxLayer *layer);

#endif
