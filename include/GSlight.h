#ifndef GS_LIGHT_H
#define GS_LIGHT_H

#include <GSgfx.h>
#include <hsd/lobjtypes.h>

struct _HSD_AObj;
struct _HSD_CObj;
struct _HSD_LObjAnimAll;

struct GSlightDesc {
    _HSD_LObjDesc *mLight;
    _HSD_LObjAnimAll **mAnimations;
};

enum GSlightType {
    GSLIGHT_TYPE_AMBIENT = 0,
    GSLIGHT_TYPE_INFINITE = 1,
    GSLIGHT_TYPE_POINT = 2,
    GSLIGHT_TYPE_SPOT = 3,
    GSLIGHT_TYPE_INVALID = 255
};

enum GSlightAnimType {
    GSLIGHT_ANIM_ONCE = 0,
    GSLIGHT_ANIM_LOOP = 1,
    GSLIGHT_ANIM_PINGPONG = 2
};

struct GSlight {
    u8 mValid;
    u8 mActive;
    u8 mHasAnimation;
    u8 mAnimating;
    int mType;
    GSlightDesc *mDesc;
    _HSD_LObj *mLObj;
    _HSD_WObjDesc mPositionDesc;
    _HSD_WObjDesc mInterestDesc;
    _HSD_LObjDesc mLightDesc;
    f32 mAttenuation;
    unsigned int mAnimCount;
    int mAnimType;
    unsigned int mAnimIndex;
    f32 mAnimRate;
    f32 mAnimFrame;
    f32 mAnimFrameCount;
    u8 mAnimEnded;
    s8 mAnimDirection;
};

struct GSlightState {
    u8 mActive;
    u8 mAnimating;
    u8 mAnimEnded;
    s8 mAnimDirection;
    GSvec mPosition;
    GSvec mInterest;
    unsigned int mAnimIndex;
    f32 mAnimFrame;
    f32 mAnimRate;
    int mAnimType;
};

extern "C" {
void GSlightDeleteCurrentAll(void);
void GSlightSetupLights(_HSD_CObj *camera);
void GSlightPushState(GSlight *light, GSlightState *state);
void GSlightPopState(GSlight *light, GSlightState *state);
u8 GSlightHasAnimationEnded(GSlight *light);
void GSlightStopAnimation(GSlight *light);
void GSlightStartAnimation(GSlight *light);
void GSlightSetAnimType(GSlight *light, int type);
void GSlightSetAnimFrame(GSlight *light, f32 frame);
void GSlightSetAnimRate(GSlight *light, f32 rate);
void GSlightSetAnimIndex(GSlight *light, unsigned int index);
u8 GSlightCanAnimate(GSlight *light);
u8 GSlightGetActive(GSlight *light);
void GSlightGetTarget(GSlight *light, GSvec *target);
u8 GSlightGetPosition(GSlight *light, GSvec *position);
int GSlightGetType(GSlight *light);
void GSlightGetColor(GSlight *light, GXColor *color);
void GSlightSetActive(GSlight *light, int active);
void GSlightSetTarget(GSlight *light, GSvec *target);
void GSlightSetPosition(GSlight *light, GSvec *position);
void GSlightSetColor(GSlight *light, GXColor color);
void GSlightSetType(GSlight *light, int type);
void GSlightFree(GSlight *light);
GSlight *GSlightLoad(void *light);
GSlight *GSlightCreate(void);
void GSlightInit(u32 count);
extern GSlight *_lightStructList;
extern u32 _lightStructCount;
}

#endif
