#ifndef CAMERA_H
#define CAMERA_H

#include <types.h>

struct GSvec;

enum {
    CAMERA_TYPE_DEFAULT = 0,
    CAMERA_TYPE_DEBUG_PRIMARY = 5,
    CAMERA_TYPE_DEBUG_SECONDARY = 6
};

#ifdef __cplusplus
extern "C" {
#endif
u8 cameraWaitSyncAnime(u8 wait);

u8 cameraSetType(u8 type);
void cameraGetPosition(struct GSvec *position);
void cameraSetPosition(struct GSvec *position);
void cameraGetRotation(struct GSvec *rotation);
void cameraSetRotation(struct GSvec *rotation);
void cameraGetTargetPos(struct GSvec *target);
void cameraSetTargetPos(struct GSvec *target);
void cameraGetTargetOfs(struct GSvec *offset);
void cameraSetTargetOfs(struct GSvec *offset);
void cameraSetFov(f32 fov);
void cameraPlayAnime(u32 groupID, u32 resourceID, f32 frame, u8 loop);

#ifdef __cplusplus
}
#endif

#endif
