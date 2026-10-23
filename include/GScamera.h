#ifndef GS_CAMERA_H
#define GS_CAMERA_H

#include <GSgfx.h>

extern "C" {
void *GScameraGetActiveCamera(void);
void GScameraLoadActiveCameraMatrix(GSgfxLayerID pass);
void GScameraGetPerspective(void *cam, f32 *fovY, f32 *aspect, f32 *nearZ, f32 *farZ);
void GScameraGetPosition(void *cam, GSvec *pos);
void GScameraSetPerspective(void *cam, f32 fovY, f32 aspect, f32 nearZ, f32 farZ);
void GScameraLookAt(void *cam, GSvec *up, GSvec *position);
void *GScameraGetMatrixPtr(void *cam);
void *GScameraGetProjMatrixPtr(void *cam);
f32 GScameraGetAnimFrame(void *cam);
u8 GScameraIsAnimationRunning(void *cam);
void GScameraSetAnimIndex(void *cam, unsigned long index);
void GScameraSetAnimType(void *cam, long type);
void GScameraSetAnimRate(void *cam, f32 rate);
void GScameraSetAnimFrame(void *cam, f32 frame);
void GScameraStartAnimation(void *cam);
}

#endif
