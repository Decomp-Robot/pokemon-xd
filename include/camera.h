#ifndef CAMERA_H
#define CAMERA_H

#include <types.h>

typedef struct GSvec GSvec;

#ifdef __cplusplus
extern "C" {
#endif
u8 cameraGetType(void);
void cameraGetTargetOfs(GSvec *offset);
f32 cameraGetDistance(void);
f32 cameraGetHeight(void);
f32 cameraGetRotY(void);
f32 cameraGetFov(void);
void cameraLoadMapResource(s32 resourceGroup, unsigned long resourceId,
                         f32 startFrame, s32 loop);
#ifdef __cplusplus
}
#endif

#endif
