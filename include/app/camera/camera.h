#ifndef APP_CAMERA_H
#define APP_CAMERA_H
#include <GSAPI/GScamera/GScamera.h>

extern "C" {
void cameraUpdate();
void cameraRefreshTargetPos();
void cameraSetGScamera(GScamera*);
void cameraResetFloor();
void cameraSetFloorDefault(f32, f32, f32);
void cameraInit();

void cameraLockYTarget(u8 lock);
void cameraLockTargetHeight(u8 lock);
void cameraSetMirrorFlag(u8 flags);
void cameraSetOffsetScale(GSvec* scale);
void cameraSetOffsetRotation(GSvec* rotation);
void cameraSetOffsetPosition(GSvec* position);
f32 cameraGetFov();
f32 cameraGetRotY();
f32 cameraGetDistance();
f32 cameraGetHeight();
void cameraSetFov(f32 fov);
void cameraSetRotY(f32 rotation);
void cameraSetDistance(f32 distance);
void cameraSetHeight(f32 height);
void cameraSetRotationXYZ(f32 x, f32 y, f32 z);
void cameraSetTargetPosXYZ(f32 x, f32 y, f32 z);
void cameraSetTargetOfsXYZ(f32 x, f32 y, f32 z);
void cameraSetAnimeRate(f32 rate);
void cameraResumeAnime();
void cameraPauseAnime();
void cameraStopAnime();
u8 cameraWaitSyncAnime(u8 wait);
void cameraPlayOffsetAnime(u32 group, u32 id, s32 frame, u8 loop);
void cameraPlayAnime(u32 group, u32 id, f32 frame, u8 loop);
void cameraMoveStop();
u8 cameraMoveEndCheckSpecial(u8 wait);
u8 cameraMoveEndCheck(u8 wait);
void cameraReturn(f32 duration, u8 wait);
void cameraMoveFovAcc(f32 acceleration);
void cameraMoveRotationAcc(f32 acceleration);
void cameraMovePositionAcc(f32 acceleration);
void cameraMoveTargetOfsAcc(f32 acceleration);
void cameraMoveTargetPosAcc(f32 acceleration);
void cameraMoveRotationXYZ(u8 type, f32 x, f32 y, f32 z, f32 duration);
void cameraMoveRotation(u8 type, GSvec* rotation, f32 duration);
void cameraMoveFov(u8 type, f32 fov, f32 duration);
void cameraMovePositionXYZ(u8 type, f32 x, f32 y, f32 z, f32 duration);
void cameraMovePosition(u8 type, GSvec* position, f32 duration);
void cameraMoveTargetXYZ(u8 type, f32 x, f32 y, f32 z, f32 duration);
void cameraMoveTargetOfs(u8 type, GSvec* offset, f32 duration);
void cameraMoveTargetPos(u8 type, GSvec* position, f32 duration);
void cameraMoveTarget(u8 type, u32 group, u32 id, f32 duration);
void cameraGetRotation(GSvec* rotation);
void cameraSetRotation(GSvec* rotation);
void cameraGetPosition(GSvec* position);
void cameraSetPosition(GSvec* position);
void cameraGetTargetPos(GSvec* position);
void cameraSetTargetPos(GSvec* position);
void cameraGetTargetOfs(GSvec* offset);
void cameraSetTargetOfs(GSvec* offset);
void cameraSetPartTargetOffset(GSvec* offset);
void cameraSetTargetExt(u32 group, u32 id, s32 part);
void cameraSetTarget(u32 group, u32 id);
_GScamera* cameraGetActive();
u8 cameraGetType();
u8 cameraSetType(u8 type);
}


#endif
