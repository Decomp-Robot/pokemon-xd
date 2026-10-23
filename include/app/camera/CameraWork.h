#ifndef INCLUDE_APP_CAMERA_CAMERAWORK_H
#define INCLUDE_APP_CAMERA_CAMERAWORK_H

#include <app/camera/CameraType.h>
#include <GSAPI/GSvec/GSvec.h>
#include <app/camera/CameraAnimation.h>
#include <GSAPI/GScamera/GScamera.h>
#include <app/floor/floorModule.h>
#include <relglobal.h>

struct FloorCamera {
    enum InitState { UNINITIALIZED = 0, FROM_CAMERA = 1, DEFAULT_SET = 2 };
    s32 flags;
    u32 floorID;
    f32 defaultHeight;
    f32 defaultDistance;
    f32 defaultRotY;
    f32 defaultFov;
    f32 height;
    f32 distance;
    f32 rotY;
    f32 fov;
};

struct CameraWork {
    u8 type;
    u8 movePending;
    u8 previousType;
    u8 flags;
    GSvec position;
    GSvec rotation;
    GSvec targetPosition;
    GSvec targetOffset;
    u32 targetGroup;
    u32 targetID;
    s32 targetPart;
    f32 height;
    f32 distance;
    f32 fov;
    f32 lockedTargetHeight;
    CameraAnimation<f32> fovAnimation;
    CameraAnimation<GSvec> targetPositionAnimation;
    CameraAnimation<GSvec> targetOffsetAnimation;
    CameraAnimation<GSvec> positionAnimation;
    CameraAnimation<GSvec> rotationAnimation;
    u32 animationGroup;
    u32 animationID;
    GSvec offsetPosition;
    GSvec offsetRotation;
    GSvec offsetScale;
    GSvec partTargetOffset;
};

enum CameraFlag {
    CAMERA_MIRROR_X = 1,
    CAMERA_MIRROR_Y = 2,
    CAMERA_MIRROR_Z = 4,
    CAMERA_LOCK_TARGET_HEIGHT = 8,
    CAMERA_CAPTURE_TARGET_HEIGHT = 16,
    CAMERA_LOCK_Y_TARGET = 32
};

enum CameraTarget {
    CAMERA_DEFAULT_GROUP = 0,
    CAMERA_DEFAULT_TARGET = 100,
    CAMERA_NO_PART = -1
};

struct CameraStateData { CameraWork work; GScameraState camera; };
extern CameraWork _cameraWork, _cameraWorkBak;
extern GSvec gUpVec;
extern FloorModuleInterface _floorModuleCamera;
extern CameraWork* pCamWork;
extern FloorCamera* _pFloorCamera;

enum CameraPadButton {
    CAMERA_PAD_FOV = 0x20,
    CAMERA_PAD_DISTANCE = 0x40,
    CAMERA_PAD_MENU = 0x400,
    CAMERA_PAD_VERTICAL = 0x800
};


#include <app/camera/camera.h>

void _cameraOffsetAnimeUpdate(_GScamera* camera);
void _cameraPadRotateUpdate(_GScamera* camera);
void _cameraPadMoveUpdate(_GScamera* camera);
const char* _cameraGetModuleName();
unsigned long _cameraGetUID();
unsigned long _cameraGetStateSize(FloorStateMode);
void _cameraRestoreStateData(FloorStateMode, void*, unsigned long);
void _cameraMakeStateData(FloorStateMode, void*, unsigned long);


#endif
