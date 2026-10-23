#ifndef GS_CAMERA_H
#define GS_CAMERA_H

#include <GSgfx.h>
#include <hsd/cobjtypes.h>

struct _HSD_CObj;
struct _HSD_CameraAnim;

struct GScameraDesc {
    _HSD_CObjDesc *mCamera;
    _HSD_CameraAnim **mAnimations;
};

enum GScameraAnimType {
    GSCAMERA_ANIM_ONCE = 0,
    GSCAMERA_ANIM_LOOP = 1,
    GSCAMERA_ANIM_PINGPONG = 2
};

typedef struct _GScamera GScamera;

struct _GScamera {
    u8 mValid;                  // 0x00
    u8 mLookAt;                 // 0x01
    u8 mMatrixDirty;            // 0x02
    u8 mHasAnimation;           // 0x03
    GScameraDesc *mDesc;        // 0x04
    _HSD_CObj *mCObj;           // 0x08
    _HSD_WObjDesc mEyeDesc;       // 0x0C
    _HSD_WObjDesc mInterestDesc; // 0x20
    _HSD_CObjDesc mCameraDesc;   // 0x34
    GSvec mPosition;            // 0x6C
    GSvec mPreviousPosition;    // 0x78
    GSvec mRotation;            // 0x84
    f32 mViewMatrix[3][4];      // 0x90
    f32 mInverseViewMatrix[3][4]; // 0xC0
    GSvec mUp;                  // 0xF0
    GSvec mInterest;            // 0xFC
    int mAnimType;              // 0x108
    unsigned int mAnimCount;    // 0x10C
    unsigned int mAnimIndex;    // 0x110
    f32 mAnimRate;              // 0x114
    f32 mAnimFrame;             // 0x118
    f32 mAnimFrameCount;        // 0x11C
    u8 mAnimEnded;              // 0x120
    s8 mAnimDirection;          // 0x121
    u8 mAnimating;              // 0x122
    u8 mFlags;                  // 0x123
};

struct GScameraState {
    u8 mAnimating;             // 0x00
    u8 mAnimEnded;             // 0x01
    GSvec mPosition;           // 0x04
    GSvec mPreviousPosition;   // 0x10
    GSvec mRotation;           // 0x1C
    f32 mViewMatrix[3][4];     // 0x28
    f32 mInverseViewMatrix[3][4]; // 0x58
    GSvec mUp;                 // 0x88
    GSvec mInterest;           // 0x94
    int mAnimType;             // 0xA0
    unsigned int mAnimIndex;   // 0xA4
    f32 mAnimRate;             // 0xA8
    f32 mAnimFrame;            // 0xAC
};

enum GScameraFlag {
    GSCAMERA_CLIPPING_ENABLED = 0x01,
    GSCAMERA_CUSTOM_VIEWPORT = 0x02,
    GSCAMERA_CUSTOM_SCISSOR = 0x04
};

extern "C" {
void GScameraPushState(_GScamera *camera, GScameraState *state);
void GScameraPopState(_GScamera *camera, GScameraState *state);
f32 GScameraGetAnimFrame(_GScamera *camera);
u8 GScameraHasAnimationEnded(_GScamera *camera);
void GScameraStopAnimation(_GScamera *camera);
u8 GScameraIsAnimating(_GScamera *camera);
void GScameraSetAnimType(_GScamera *camera, int type);
void GScameraSetAnimIndex(_GScamera *camera, unsigned int index);
void GScameraSetAnimRate(_GScamera *camera, f32 rate);
void GScameraSetAnimFrame(_GScamera *camera, f32 frame);
void GScameraStartAnimation(_GScamera *camera);
f32 (*GScameraGetMatrixPtr(_GScamera *camera))[4];
f32 (*GScameraGetInvMatrixPtr(_GScamera *camera))[4];
f32 (*GScameraGetProjMatrixPtr(_GScamera *camera))[4];
void GScameraGetDistanceVector(_GScamera *camera, GSvec *distance);
void GScameraGetLookAt(_GScamera *camera, GSvec *up, GSvec *interest);
void GScameraLookAt(_GScamera *camera, GSvec *up, GSvec *interest);
void GScameraGetFrontVector(_GScamera *camera, GSvec *front);
void GScameraGetRotation(_GScamera *camera, GSvec *rotation);
void GScameraGetPosition(_GScamera *camera, GSvec *position);
void GScameraGetPerspective(_GScamera *camera, f32 *fov, f32 *aspect,
                            f32 *nearPlane, f32 *farPlane);
void GScameraSetRotation(_GScamera *camera, GSvec *rotation);
void GScameraSetPosition(_GScamera *camera, GSvec *position);
void GScameraSetPerspective(_GScamera *camera, f32 fov, f32 aspect,
                            f32 nearPlane, f32 farPlane);
void GScameraSetScissor(_GScamera *camera, u8 enabled, unsigned int left,
                        unsigned int top, unsigned int width,
                        unsigned int height);
void GScameraSetViewport(_GScamera *camera, u8 enabled, f32 left, f32 top,
                        f32 right, f32 bottom);
void GScameraLoadProjection(_GScamera *camera, GSgfxLayer *layer);
void GScameraLoadCameraMatrix(_GScamera *camera, GSgfxLayerID layer);
void GScameraLoadActiveCameraMatrix(GSgfxLayerID layer);
_GScamera *GScameraGetActiveCamera(void);
void GScameraSetActiveCamera(_GScamera *camera);
void GScameraFree(_GScamera *camera);
u8 GScameraIsValid(_GScamera *camera);
_GScamera *GScameraLoad(GScameraDesc *desc);
_GScamera *GScameraCreate(void);
void GScameraInit(u32 count);
extern _GScamera *_activeCamera;
}

#endif
