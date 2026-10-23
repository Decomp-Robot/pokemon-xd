#ifndef GS_CAMERA_H
#define GS_CAMERA_H

#include <GSmathTypes.h>
#include <types.h>
#include <GSgfxTypes.h>

struct _GScamera;
class GSgfxLayer;

extern "C" {
_GScamera* GScameraGetActiveCamera();
bool GScameraIsValid(_GScamera* camera);
void GScameraLoadProjection(_GScamera* camera, GSgfxLayer* layer);
void GScameraLoadActiveCameraMatrix(GSgfxLayerID layer);
f32 GScameraGetAnimFrame(_GScamera* camera);
u8 GScameraIsAnimating(_GScamera* camera);
void GScameraSetAnimIndex(_GScamera* camera, u32 index);
void GScameraSetAnimType(_GScamera* camera, s32 type);
void GScameraSetAnimRate(_GScamera* camera, f32 rate);
void GScameraSetAnimFrame(_GScamera* camera, f32 frame);
void GScameraStartAnimation(_GScamera* camera);
GSmtx* GScameraGetMatrixPtr(_GScamera* camera);
f32 (*GScameraGetProjMatrixPtr(_GScamera* camera))[4];
void GScameraGetPerspective(_GScamera* camera, f32* fov, f32* aspect, f32* nearZ, f32* farZ);
void GScameraGetPosition(_GScamera* camera, GSvec* position);
void GScameraSetPerspective(_GScamera* camera, f32 fov, f32 aspect, f32 nearZ, f32 farZ);
void GScameraLookAt(_GScamera* camera, GSvec* up, GSvec* target);
}

#endif
