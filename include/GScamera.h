#ifndef GS_CAMERA_H
#define GS_CAMERA_H

#include <GSmathTypes.h>
#include <types.h>
#include <GSgfxTypes.h>

struct _HSD_CObj;
struct _GScamera {
    u8 _00[8];
    _HSD_CObj* mCObj;
    u8 _0C[0x60];
    GSvec position;
    GSvec previousPosition;
    GSvec rotation;
    GSmtx matrix;
    GSmtx secondaryMatrix;
    GSvec up;
    GSvec target;
    u8 _108[0x1C];
};
typedef _GScamera GScamera;
class GSgfxLayer;

struct GScameraState {
    u8 isAnimating;
    u8 hasAnimationEnded;
    u8 padding[2];
    GSvec position;
    GSvec previousPosition;
    GSvec rotation;
    GSmtx matrix;
    GSmtx secondaryMatrix;
    GSvec up;
    GSvec target;
    s32 animationType;
    u32 animationIndex;
    f32 animationRate;
    f32 animationFrame;
};

extern "C" {
GScamera* GScameraCreate(void);
GScamera* GScameraGetActiveCamera(void);
void GScameraSetActiveCamera(GScamera* camera);
bool GScameraIsValid(GScamera* camera);
void GScameraLoadProjection(GScamera* camera, GSgfxLayer* layer);
void GScameraLoadActiveCameraMatrix(GSgfxLayerID layer);
f32 GScameraGetAnimFrame(GScamera* camera);
u8 GScameraIsAnimating(GScamera* camera);
u8 GScameraHasAnimationEnded(GScamera* camera);
void GScameraSetAnimIndex(GScamera* camera, u32 index);
void GScameraSetAnimType(GScamera* camera, s32 type);
void GScameraSetAnimRate(GScamera* camera, f32 rate);
void GScameraSetAnimFrame(GScamera* camera, f32 frame);
void GScameraStartAnimation(GScamera* camera);
void GScameraStopAnimation(GScamera* camera);
GSmtx* GScameraGetMatrixPtr(GScamera* camera);
f32 (*GScameraGetProjMatrixPtr(GScamera* camera))[4];
void GScameraGetPerspective(GScamera* camera, f32* fov, f32* aspect, f32* nearZ, f32* farZ);
void GScameraSetPerspective(GScamera* camera, f32 fov, f32 aspect, f32 nearZ, f32 farZ);
void GScameraGetPosition(GScamera* camera, GSvec* position);
void GScameraSetPosition(GScamera* camera, const GSvec* position);
void GScameraSetRotation(GScamera* camera, const GSvec* rotation);
void GScameraGetFrontVector(GScamera* camera, GSvec* front);
void GScameraGetDistanceVector(GScamera* camera, GSvec* distance);
void GScameraGetLookAt(GScamera* camera, GSvec* up, GSvec* target);
void GScameraLookAt(GScamera* camera, const GSvec* up, const GSvec* target);
void GScameraPushState(GScamera* camera, GScameraState* state);
void GScameraPopState(GScamera* camera, const GScameraState* state);
}

#endif
