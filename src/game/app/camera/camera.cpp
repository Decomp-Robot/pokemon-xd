#include <GSAPI/GScolsys/GScolsys.h>
#include <GSgfxVideo.h>
#include <app/camera/CameraWork.h>
#include <GSAPI/GSres/GSres.h>
#include <GSAPI/GSmodel/GSmodel.h>
#include <GSAPI/GSthread/GSthread.h>
#include <GSAPI/GSmemman/GSmemman.h>
#include <GSAPI/GSlog/GSlog.h>
#include <GSAPI/GSinput/GSinput.h>
#include <GSAPI/GSsysdolphin_baselib/baselib/quatlib.h>
#include <app/floor/floorManager.h>
#include <app/floor/floorFieldCamera.h>
#include <app/menu/menu.h>
#include <time.h>
#include <string.h>
extern "C" unsigned char dbgMenuIsOpen(void);

extern "C" {
#include <math.h>
}

static const unsigned long CAMERA_MODULE_REGISTRATION_CATEGORY = 0x80;
static const unsigned long CAMERA_MODULE_UID = 0x11;

static void cameraResetOffsets(bool resetFov)
{
    pCamWork->offsetPosition = GSvec(0.0f, 0.0f, 0.0f);
    pCamWork->offsetRotation = GSvec(0.0f, 0.0f, 0.0f);
    f32 fov;
    if (resetFov) fov = 30.0f;
    pCamWork->offsetScale = GSvec(1.0f, 1.0f, 1.0f);
    if (resetFov) pCamWork->fov = fov;
}

static inline void cameraCopyVector(GSvec* to, GSvec* from)
{
    to->x = from->x;
    to->y = from->y;
    to->z = from->z;
}

static inline void cameraSetVector(GSvec* to, f32 x, f32 y, f32 z)
{
    to->x = x;
    to->y = y;
    to->z = z;
}

static inline FloorCamera* cameraGetFloor(u32 floorID)
{
    for (u32 i = 0; i < floordata_number->floorCount; ++i) {
        if (floorID == _pFloorCamera[i].floorID)
            return &_pFloorCamera[i];
    }
    return 0;
}

static inline _GScamera* cameraGetAnimationCamera()
{
    CameraWork* work = pCamWork;
    _GScamera* camera = (_GScamera*)GSresGetResource(work->animationGroup, work->animationID);
    if (!camera)
        camera = (_GScamera*)GSresGetResourceAllGroup(work->animationID);
    return camera;
}

static inline void rotateVector(Mtx matrix, GSvec* vector, const Quaternion* rotation)
{
    PSMTXQuat(matrix, rotation);
    PSMTXMultVecSR(matrix, vector, vector);
}

static inline f32 cameraAtan2(f32 y, f32 x)
{
    return (f32)atan2(y, x);
}

#pragma defer_codegen on
u8 cameraSetType(u8 type)
{
    pCamWork->flags = 0;
    if (pCamWork->type == type) return type;
    u8 previousType = pCamWork->type;
    pCamWork->type = type;
    cameraLockYTarget(0);
    return previousType;
}

u8 cameraGetType() { return pCamWork->type; }

_GScamera* cameraGetActive()
{
    _GScamera* camera = GScameraGetActiveCamera();
    if (camera) return camera;
    camera = (_GScamera*)GSresGetResource(0, 0);
    GScameraSetActiveCamera(camera);
    return camera;
}

void cameraSetTarget(u32 group, u32 id)
{
    pCamWork->targetGroup = group;
    pCamWork->targetID = id;
    pCamWork->targetPart = CAMERA_NO_PART;
}

void cameraSetTargetExt(u32 group, u32 id, s32 part)
{
    pCamWork->targetGroup = group;
    pCamWork->targetID = id;
    pCamWork->targetPart = part;
    cameraSetVector(&pCamWork->partTargetOffset, 0.0f, 0.0f, 0.0f);
}

void cameraSetPartTargetOffset(GSvec* offset) { cameraCopyVector(&pCamWork->partTargetOffset, offset); }

void cameraSetTargetOfs(GSvec* offset) { cameraCopyVector(&pCamWork->targetOffset, offset); }

void cameraGetTargetOfs(GSvec* offset) { *offset = pCamWork->targetOffset; }

void cameraSetTargetPos(GSvec* position) { cameraCopyVector(&pCamWork->targetPosition, position); }

void cameraGetTargetPos(GSvec* position) { *position = pCamWork->targetPosition; }

void cameraSetPosition(GSvec* position) { cameraCopyVector(&pCamWork->position, position); }

void cameraGetPosition(GSvec* position) { *position = pCamWork->position; }

void cameraSetRotation(GSvec* rotation)
{
    _GScamera* camera = (_GScamera*)GSresGetResource(0, 0);
    cameraCopyVector(&pCamWork->rotation, rotation);
    GScameraSetRotation(camera, rotation);
}

void cameraGetRotation(GSvec* rotation) { *rotation = pCamWork->rotation; }

void cameraMoveTarget(u8 type, u32 group, u32 id, f32 duration)
{
    #pragma unused(type)
    GSvec position(0.0f, 0.0f, 0.0f);
    cameraSetTarget(group, id);
    _GSmodel* model = (_GSmodel*)GSresGetResource(group, id);
    if (model) GSmodelGetPosition(model, &position);
    cameraMoveTargetPos(CAMERA_NORMAL, &position, duration);
}

void cameraMoveTargetPos(u8 type, GSvec* position, f32 duration)
{
    #pragma unused(type)
    if (duration < 0.0000001f && duration > -0.0000001f)
        cameraSetTargetPos(position);
    else {
        pCamWork->movePending = 1;
        pCamWork->targetPositionAnimation.start(&pCamWork->targetPosition, position, duration);
    }
}

void cameraMoveTargetOfs(u8 type, GSvec* offset, f32 duration)
{
    #pragma unused(type)
    pCamWork->movePending = 1;
    pCamWork->targetOffsetAnimation.start(&pCamWork->targetOffset, offset, duration);
}

void cameraMoveTargetXYZ(u8 type, f32 x, f32 y, f32 z, f32 duration)
{
    #pragma unused(type)
    GSvec position(x, y, z);
    if (duration < 0.0000001f && duration > -0.0000001f)
        cameraSetTargetPos(&position);
    else {
        pCamWork->movePending = 1;
        pCamWork->targetPositionAnimation.start(&pCamWork->targetPosition, &position, duration);
    }
}

void cameraMovePosition(u8 type, GSvec* position, f32 duration)
{
    #pragma unused(type)
    if (duration < 0.0000001f && duration > -0.0000001f)
        cameraSetPosition(position);
    else {
        pCamWork->movePending = 1;
        pCamWork->positionAnimation.start(&pCamWork->position, position, duration);
    }
}

void cameraMovePositionXYZ(u8 type, f32 x, f32 y, f32 z, f32 duration)
{
    #pragma unused(type)
    GSvec position(x, y, z);
    if (duration < 0.0000001f && duration > -0.0000001f)
        cameraSetPosition(&position);
    else {
        pCamWork->movePending = 1;
        pCamWork->positionAnimation.start(&pCamWork->position, &position, duration);
    }
}

void cameraMoveFov(u8 type, f32 fov, f32 duration)
{
    #pragma unused(type)
    pCamWork->movePending = 1;
    pCamWork->fovAnimation.start(&pCamWork->fov, &fov, duration);
}

void cameraMoveRotation(u8 type, GSvec* rotation, f32 duration)
{
    #pragma unused(type)
    if (duration < 0.0000001f && duration > -0.0000001f)
        cameraSetRotation(rotation);
    else {
        pCamWork->movePending = 1;
        pCamWork->rotationAnimation.start(&pCamWork->rotation, rotation, duration);
    }
}

void cameraMoveRotationXYZ(u8 type, f32 x, f32 y, f32 z, f32 duration)
{
    #pragma unused(type)
    GSvec rotation(x, y, z);
    if (duration < 0.0000001f && duration > -0.0000001f)
        cameraSetRotation(&rotation);
    else {
        pCamWork->movePending = 1;
        pCamWork->rotationAnimation.start(&pCamWork->rotation, &rotation, duration);
    }
}

void cameraMoveTargetPosAcc(f32 acceleration) { pCamWork->targetPositionAnimation.setAcceleration(acceleration); }

void cameraMoveTargetOfsAcc(f32 acceleration) { pCamWork->targetOffsetAnimation.setAcceleration(acceleration); }

void cameraMovePositionAcc(f32 acceleration) { pCamWork->positionAnimation.setAcceleration(acceleration); }

void cameraMoveRotationAcc(f32 acceleration) { pCamWork->rotationAnimation.setAcceleration(acceleration); }

void cameraMoveFovAcc(f32 acceleration) { pCamWork->fovAnimation.setAcceleration(acceleration); }

void cameraReturn(f32 duration, u8 wait)
{
    _GScamera* camera = GScameraGetActiveCamera();
    if (floorFieldCameraDataExist()) {
        if (pCamWork->type == CAMERA_FREE) {
            GSvec position;
            GSvec front;
            GSvec offset = pCamWork->targetOffset;
            GScameraGetFrontVector(camera, &front);
            PSVECScale(&front, &front, pCamWork->distance);
            GScameraGetPosition(camera, &position);
            PSVECAdd(&position, &front, &position);
            PSVECSubtract(&position, &offset, &position);
            pCamWork->targetPosition = position;
        }
        cameraSetType(CAMERA_NORMAL);
        floorFieldCameraEnable();
        floorFieldCameraBlend(duration);
    } else {
        cameraSetType(CAMERA_NORMAL);
        FloorCamera* floor = cameraGetFloor(GSfloorGetCurrent());
        if (floor) {
            cameraSetHeight(floor->defaultHeight);
            cameraSetDistance(floor->defaultDistance);
            cameraSetFov(floor->defaultFov);
        }
        cameraMoveTarget(CAMERA_NORMAL, CAMERA_DEFAULT_GROUP, CAMERA_DEFAULT_TARGET, duration);
        cameraMoveRotationXYZ(CAMERA_NORMAL, 0.0f, floor->defaultRotY, 0.0f, duration);
    }
    cameraMoveEndCheck(wait);
}

u8 cameraMoveEndCheck(u8 wait)
{
    for (;;) {
        if (!pCamWork->targetPositionAnimation.isMove() &&
            !pCamWork->targetOffsetAnimation.isMove() &&
            !pCamWork->positionAnimation.isMove() &&
            !pCamWork->rotationAnimation.isMove() &&
            !pCamWork->fovAnimation.isMove())
            return 0;
        if (wait) GSthreadSwitch();
        else return 1;
    }
}

u8 cameraMoveEndCheckSpecial(u8 wait)
{
    for (;;) {
        if (!pCamWork->targetPositionAnimation.isMove() &&
            !pCamWork->positionAnimation.isMove() &&
            !pCamWork->rotationAnimation.isMove())
            return 0;
        if (wait) GSthreadSwitch();
        else return 1;
    }
}

void cameraMoveStop()
{
    pCamWork->targetPositionAnimation.stop();
    pCamWork->targetOffsetAnimation.stop();
    pCamWork->positionAnimation.stop();
    pCamWork->rotationAnimation.stop();
    pCamWork->fovAnimation.stop();
    pCamWork->movePending = 0;
}

void cameraPlayAnime(u32 group, u32 id, f32 frame, u8 loop)
{
    if (pCamWork->animationGroup || pCamWork->animationID)
        cameraStopAnime();
    u8 previousType = cameraSetType(CAMERA_ANIMATION);
    pCamWork->animationGroup = group;
    pCamWork->animationID = id;
    pCamWork->previousType = previousType;
    _GScamera* camera = cameraGetAnimationCamera();
    if (camera) {
        GScameraSetAnimIndex(camera, 0);
        if (loop) GScameraSetAnimType(camera, 1);
        else GScameraSetAnimType(camera, 0);
        GScameraSetAnimRate(camera, 0.5f);
        GScameraSetAnimFrame(camera, frame);
        GScameraStartAnimation(camera);
        GScameraSetActiveCamera(camera);
    }
}

void cameraPlayOffsetAnime(u32 group, u32 id, s32 frame, u8 loop)
{
    if (pCamWork->animationGroup || pCamWork->animationID)
        cameraStopAnime();
    u8 previousType = cameraSetType(CAMERA_OFFSET_ANIMATION);
    pCamWork->animationGroup = group;
    pCamWork->animationID = id;
    pCamWork->previousType = previousType;
    _GScamera* camera = cameraGetAnimationCamera();
    if (camera) {
        GScameraSetAnimIndex(camera, 0);
        if (loop) GScameraSetAnimType(camera, 1);
        else GScameraSetAnimType(camera, 0);
        GScameraSetAnimRate(camera, 0.5f);
        GScameraSetAnimFrame(camera, (f32)frame);
        GScameraStartAnimation(camera);
        cameraResetOffsets(false);
        GScameraSetActiveCamera((_GScamera*)GSresGetResource(0, 0));
    }
}

u8 cameraWaitSyncAnime(u8 wait)
{
    _GScamera* camera = cameraGetAnimationCamera();
    if (!camera) return 0;
    if (wait) {
        for (;;) {
            if (GScameraHasAnimationEnded(camera)) break;
            if (!GSthreadGetCurrentThread()) {
                GSlogWrite("cameraWaitSyncAnime()  スレッドから呼ぶようにしてください\n");
                break;
            }
            GSthreadSwitch();
        }
    } else if (!GScameraHasAnimationEnded(camera)) {
        return 1;
    }
    return 0;
}

void cameraStopAnime()
{
    _GScamera* camera = cameraGetAnimationCamera();
    pCamWork->animationGroup = 0;
    pCamWork->animationID = 0;
    if (camera) GScameraStopAnimation(camera);
}

void cameraPauseAnime()
{
    _GScamera* camera = cameraGetAnimationCamera();
    if (camera) GScameraStopAnimation(camera);
}

void cameraResumeAnime()
{
    _GScamera* camera = cameraGetAnimationCamera();
    if (camera) GScameraStartAnimation(camera);
}

void cameraSetAnimeRate(f32 rate)
{
    _GScamera* camera = cameraGetAnimationCamera();
    if (camera) GScameraSetAnimRate(camera, rate);
}

void cameraSetTargetOfsXYZ(f32 x, f32 y, f32 z)
{
    cameraSetVector(&pCamWork->targetOffset, x, y, z);
}

void cameraSetTargetPosXYZ(f32 x, f32 y, f32 z)
{
    cameraSetVector(&pCamWork->targetPosition, x, y, z);
}

void cameraSetRotationXYZ(f32 x, f32 y, f32 z)
{
    GSvec rotation(x, y, z);
    cameraSetRotation(&rotation);
}

void cameraSetHeight(f32 height)
{
    FloorCamera* floor = cameraGetFloor(GSfloorGetCurrent());
    if (floor) floor->height = height;
    pCamWork->height = height;
}

void cameraSetDistance(f32 distance)
{
    FloorCamera* floor = cameraGetFloor(GSfloorGetCurrent());
    if (floor) floor->distance = distance;
    pCamWork->distance = distance;
}

void cameraSetRotY(f32 rotation)
{
    FloorCamera* floor = cameraGetFloor(GSfloorGetCurrent());
    if (floor) floor->rotY = rotation;
    pCamWork->rotation.y = rotation;
}

void cameraSetFov(f32 fov)
{
    if (fov < 3.0f) fov = 3.0f;
    if (fov > 120.0f) fov = 120.0f;
    FloorCamera* floor = cameraGetFloor(GSfloorGetCurrent());
    if (floor) floor->fov = fov;
    pCamWork->fov = fov;
}

f32 cameraGetHeight() { return pCamWork->height; }

f32 cameraGetDistance() { return pCamWork->distance; }

f32 cameraGetRotY() { return pCamWork->rotation.y; }

f32 cameraGetFov() { return pCamWork->fov; }

void cameraSetOffsetPosition(GSvec* position) { cameraCopyVector(&pCamWork->offsetPosition, position); }

void cameraSetOffsetRotation(GSvec* rotation) { cameraCopyVector(&pCamWork->offsetRotation, rotation); }

void cameraSetOffsetScale(GSvec* scale) { cameraCopyVector(&pCamWork->offsetScale, scale); }

void cameraSetMirrorFlag(u8 flags)
{
    pCamWork->flags &= CAMERA_LOCK_TARGET_HEIGHT | CAMERA_CAPTURE_TARGET_HEIGHT;
    pCamWork->flags |= flags;
}

void cameraLockTargetHeight(u8 lock)
{
    if (lock)
        pCamWork->flags |= CAMERA_LOCK_TARGET_HEIGHT | CAMERA_CAPTURE_TARGET_HEIGHT;
    else
        pCamWork->flags &= ~CAMERA_LOCK_TARGET_HEIGHT;
}

void cameraLockYTarget(u8 lock)
{
    if (lock)
        pCamWork->flags |= CAMERA_LOCK_Y_TARGET;
    else
        pCamWork->flags &= ~CAMERA_LOCK_Y_TARGET;
}

#pragma defer_codegen off
static inline void _cameraPerspectiveUpdate(_GScamera* camera)
{
    f32 fov, aspect, nearPlane, farPlane;
    GScameraGetPerspective(camera, &fov, &aspect, &nearPlane, &farPlane);
    GScameraSetPerspective(camera, pCamWork->fov, aspect, nearPlane, farPlane);
}

static inline void _cameraOrbitUpdate(_GScamera* camera)
{
    Mtx matrix;
    GSvec target;
    GSvec displacement(0.0f, pCamWork->height, pCamWork->distance);
    GSvec direction;
    PSMTXRotRad(matrix, 'Y', pCamWork->rotation.y);
    PSMTXMultVec(matrix, &displacement, &displacement);
    PSVECAdd(&pCamWork->targetPosition, &pCamWork->targetOffset, &target);
    PSVECAdd(&target, &displacement, &pCamWork->position);
    PSVECSubtract(&pCamWork->position, &target, &direction);
    if (direction.x < 0.00001f && direction.x > -0.00001f &&
        direction.z < 0.00001f && direction.z > -0.00001f)
        pCamWork->position.z += 1.0f;
    if (pCamWork->flags & CAMERA_CAPTURE_TARGET_HEIGHT) {
        pCamWork->flags &= ~CAMERA_CAPTURE_TARGET_HEIGHT;
        pCamWork->lockedTargetHeight = target.y;
    } else if (pCamWork->flags & CAMERA_LOCK_TARGET_HEIGHT) target.y = pCamWork->lockedTargetHeight;
    GScameraSetPosition(camera, &pCamWork->position);
    GScameraLookAt(camera, &gUpVec, &target);
    pCamWork->rotation.x = -(f32)atan2(pCamWork->height, pCamWork->distance);
    _cameraPerspectiveUpdate(camera);
}

static inline void _cameraLookAtUpdate(_GScamera* camera)
{
    GSvec target, direction;
    PSVECAdd(&pCamWork->targetPosition, &pCamWork->targetOffset, &target);
    if (pCamWork->flags & CAMERA_CAPTURE_TARGET_HEIGHT) {
        pCamWork->flags &= ~CAMERA_CAPTURE_TARGET_HEIGHT;
        pCamWork->lockedTargetHeight = target.y;
    } else if (pCamWork->flags & CAMERA_LOCK_TARGET_HEIGHT) target.y = pCamWork->lockedTargetHeight;
    GScameraSetPosition(camera, &pCamWork->position);
    GScameraLookAt(camera, &gUpVec, &target);
    PSVECSubtract(&pCamWork->position, &target, &direction);
    pCamWork->height = direction.y;
    pCamWork->distance = sqrtf(direction.x * direction.x + direction.z * direction.z);
    pCamWork->rotation.y = (f32)atan2(direction.x, direction.z);
    pCamWork->rotation.x = -(f32)atan2(pCamWork->height, pCamWork->distance);
    _cameraPerspectiveUpdate(camera);
}

static inline void _cameraDirectUpdate(_GScamera* camera)
{
    GScameraSetPosition(camera, &pCamWork->position);
    GScameraSetRotation(camera, &pCamWork->rotation);
    GSvec target, up;
    GScameraGetLookAt(camera, &up, &target);
    PSVECSubtract(&target, &pCamWork->targetOffset, &pCamWork->targetPosition);
    _cameraPerspectiveUpdate(camera);
}

static inline void _cameraOffsetUpdate(_GScamera* camera)
{
    Mtx matrix;
    GSvec target;
    GSvec displacement(0.0f, pCamWork->height, pCamWork->distance);
    PSMTXRotRad(matrix, 'Y', pCamWork->rotation.y);
    PSMTXMultVec(matrix, &displacement, &displacement);
    PSVECAdd(&pCamWork->targetPosition, &displacement, &pCamWork->position);
    PSVECAdd(&pCamWork->targetPosition, &pCamWork->targetOffset, &target);
    PSMTXRotRad(matrix, 'Z', pCamWork->offsetRotation.z);
    PSMTXMultVec(matrix, &gUpVec, &displacement);
    PSVECAdd(&target, &pCamWork->offsetPosition, &target);
    if (pCamWork->flags & CAMERA_CAPTURE_TARGET_HEIGHT) {
        pCamWork->flags &= ~CAMERA_CAPTURE_TARGET_HEIGHT;
        pCamWork->lockedTargetHeight = target.y;
    } else if (pCamWork->flags & CAMERA_LOCK_TARGET_HEIGHT) target.y = pCamWork->lockedTargetHeight;
    GScameraSetPosition(camera, &pCamWork->position);
    GScameraLookAt(camera, &displacement, &target);
    pCamWork->rotation.x = -(f32)atan2(pCamWork->height, pCamWork->distance);
    _cameraPerspectiveUpdate(camera);
}


extern "C" void cameraUpdate()
{
    _GScamera* camera = (_GScamera*)GSresGetResource(0, 0);
    CameraWork* initialWork = pCamWork;
    if (initialWork->movePending == 1) {
        f32 delta = timeGetLastFrameSec();
        if (pCamWork->targetPositionAnimation.update(delta))
            pCamWork->targetPosition = *pCamWork->targetPositionAnimation.getCurrentPos();
        if (pCamWork->targetOffsetAnimation.update(delta))
            pCamWork->targetOffset = *pCamWork->targetOffsetAnimation.getCurrentPos();
        if (pCamWork->positionAnimation.update(delta))
            pCamWork->position = *pCamWork->positionAnimation.getCurrentPos();
        if (pCamWork->rotationAnimation.update(delta))
            pCamWork->rotation = *pCamWork->rotationAnimation.getCurrentPos();
        if (pCamWork->fovAnimation.update(delta))
            pCamWork->fov = *pCamWork->fovAnimation.getCurrentPos();
        GSvec direction;
        switch (pCamWork->type) {
        case CAMERA_NORMAL:
        case CAMERA_LOOK_AT:
        case CAMERA_PAD_ROTATE:
            if (floorFieldCameraDataExist() && floorFieldCameraGetEnabled()) {
                PSVECAdd(&pCamWork->targetPosition, &pCamWork->targetOffset, &direction);
                PSVECSubtract(&pCamWork->position, &direction, &direction);
                pCamWork->height = direction.y;
                pCamWork->distance = sqrtf(direction.x * direction.x + direction.z * direction.z);
            }
            break;
        case CAMERA_FIXED_LOOK_AT:
        case CAMERA_FREE:
        case CAMERA_ANIMATION:
        case CAMERA_PAD_MOVE:
        case CAMERA_OFFSET_ANIMATION:
            break;
        case CAMERA_OFFSET:
            PSVECSubtract(&pCamWork->position, &pCamWork->targetPosition, &direction);
            pCamWork->height = direction.y;
            pCamWork->distance = sqrtf(direction.x * direction.x + direction.z * direction.z);
            if (!pCamWork->rotationAnimation.isMove() && !(pCamWork->flags & CAMERA_LOCK_Y_TARGET))
                pCamWork->rotation.y = (f32)atan2(direction.x, direction.z);
            break;
        }
        if (!cameraMoveEndCheck(0)) pCamWork->movePending = 0;
    } else {
        switch (initialWork->type) {
        case CAMERA_NORMAL:
        case CAMERA_LOOK_AT:
        case CAMERA_PAD_ROTATE:
        case CAMERA_OFFSET:
            if (initialWork->targetGroup || initialWork->targetID) {
                _GSmodel* model = (_GSmodel*)GSresGetResource(initialWork->targetGroup, initialWork->targetID);
                if (model) {
                    GSmodelGetPosition(model, &pCamWork->targetPosition);
                    if (pCamWork->targetPart >= 0) {
                        GSpart* part = GSmodelGetPart(model, pCamWork->targetPart);
                        if (part) {
                            GSpartGetTransform(part, &pCamWork->targetOffset, 0, 0);
                            PSVECSubtract(&pCamWork->targetOffset, &pCamWork->targetPosition, &pCamWork->targetOffset);
                            PSVECAdd(&pCamWork->targetOffset, &pCamWork->partTargetOffset, &pCamWork->targetOffset);
                            GSpartFree(part);
                        }
                    }
                }
            }
            break;
        case CAMERA_ANIMATION: {
            _GScamera* animation = (_GScamera*)GSresGetResource(initialWork->animationGroup, initialWork->animationID);
            if (!animation) animation = (_GScamera*)GSresGetResourceAllGroup(initialWork->animationID);
            camera = animation;
            if (!camera) {
                pCamWork->animationGroup = 0;
                pCamWork->animationID = 0;
                cameraSetType(pCamWork->previousType);
                pCamWork->targetGroup = CAMERA_DEFAULT_GROUP;
                pCamWork->targetID = CAMERA_DEFAULT_TARGET;
                pCamWork->targetPart = CAMERA_NO_PART;
                camera = (_GScamera*)GSresGetResource(0, 0);
            }
            break;
        }
        case CAMERA_FIXED_LOOK_AT:
        case CAMERA_FREE:
        case CAMERA_PAD_MOVE:
        case CAMERA_OFFSET_ANIMATION:
            break;
        }
    }
    switch (pCamWork->type) {
    case CAMERA_NORMAL: {
        _cameraOrbitUpdate(camera);
        break;
    }
    case CAMERA_LOOK_AT:
    case CAMERA_FIXED_LOOK_AT: {
        _cameraLookAtUpdate(camera);
        break;
    }
    case CAMERA_PAD_MOVE:
        _cameraPadMoveUpdate(camera);
        break;
    case CAMERA_PAD_ROTATE:
        _cameraPadRotateUpdate(camera);
        break;
    case CAMERA_FREE: {
        _cameraDirectUpdate(camera);
        break;
    }
    case CAMERA_OFFSET_ANIMATION:
        _cameraOffsetAnimeUpdate(camera);
        GScameraLoadActiveCameraMatrix((GSgfxLayerID)-1);
        return;
    case CAMERA_OFFSET: {
        _cameraOffsetUpdate(camera);
        break;
    }
    }
    GScameraSetActiveCamera(camera);
    GScameraLoadActiveCameraMatrix((GSgfxLayerID)-1);
}

extern "C" void cameraRefreshTargetPos()
{
    if (pCamWork->targetGroup || pCamWork->targetID) {
        _GSmodel* model = (_GSmodel*)GSresGetResource(pCamWork->targetGroup, pCamWork->targetID);
        if (model) {
            GSmodelGetPosition(model, &pCamWork->targetPosition);
            if (pCamWork->targetPart >= 0) {
                GSpart* part = GSmodelGetPart(model, pCamWork->targetPart);
                if (part) {
                    GSpartGetTransform(part, &pCamWork->targetOffset, 0, 0);
                    PSVECSubtract(&pCamWork->targetOffset, &pCamWork->targetPosition, &pCamWork->targetOffset);
                    PSVECAdd(&pCamWork->targetOffset, &pCamWork->partTargetOffset, &pCamWork->targetOffset);
                    GSpartFree(part);
                }
            }
        }
    }
}

void _cameraOffsetAnimeUpdate(_GScamera* camera)
{
    _GScamera* animation = cameraGetAnimationCamera();
    if (!animation || animation == camera || !GScameraIsAnimating(animation) || GScameraHasAnimationEnded(animation))
        return;
    Quaternion sdkRotation;
    f32 fov, aspect, nearPlane, farPlane;
    GSvec position, target, up;
    GScameraGetPerspective(animation, &fov, &aspect, &nearPlane, &farPlane);
    GScameraGetLookAt(animation, &up, &target);
    GScameraGetPosition(animation, &position);
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    position.x *= pCamWork->offsetScale.x;
    position.y *= pCamWork->offsetScale.y;
    position.z *= pCamWork->offsetScale.z;
    target.x *= pCamWork->offsetScale.x;
    target.y *= pCamWork->offsetScale.y;
    target.z *= pCamWork->offsetScale.z;
    Quaternion rotation;
    EulerToQuat(&pCamWork->offsetRotation, &rotation.x);
    sdkRotation.x = rotation.x;
    sdkRotation.y = rotation.y;
    sdkRotation.z = rotation.z;
    sdkRotation.w = rotation.w;
    Mtx positionMatrix, targetMatrix, upMatrix;
    rotateVector(positionMatrix, &position, &sdkRotation);
    rotateVector(targetMatrix, &target, &sdkRotation);
    Quaternion sdkInverseRotation, inverseRotation;
    PSQUATInverse(&sdkRotation, &inverseRotation);
    sdkInverseRotation.x = inverseRotation.x;
    sdkInverseRotation.y = inverseRotation.y;
    sdkInverseRotation.z = inverseRotation.z;
    sdkInverseRotation.w = inverseRotation.w;
    rotateVector(upMatrix, &up, &sdkInverseRotation);
    if (pCamWork->flags & CAMERA_MIRROR_X) {
        target.x *= -1.0f;
        up.x *= -1.0f;
        position.x *= -1.0f;
    }
    if (pCamWork->flags & CAMERA_MIRROR_Y) {
        target.y *= -1.0f;
        up.y *= -1.0f;
        position.y *= -1.0f;
    }
    if (pCamWork->flags & CAMERA_MIRROR_Z) {
        target.z *= -1.0f;
        up.z *= -1.0f;
        position.z *= -1.0f;
    }
    PSVECAdd(&position, &pCamWork->offsetPosition, &position);
    PSVECAdd(&target, &pCamWork->offsetPosition, &target);
    if (pCamWork->flags & CAMERA_CAPTURE_TARGET_HEIGHT) {
        pCamWork->flags &= ~CAMERA_CAPTURE_TARGET_HEIGHT;
        pCamWork->lockedTargetHeight = target.y;
    } else if (pCamWork->flags & CAMERA_LOCK_TARGET_HEIGHT) target.y = pCamWork->lockedTargetHeight;
    GScameraSetPerspective(camera, fov, aspect, nearPlane, farPlane);
    GScameraSetPosition(camera, &position);
    GScameraLookAt(camera, &up, &target);
    pCamWork->position = position;
    pCamWork->targetPosition = pCamWork->offsetPosition;
    GSvec targetOffset;
    PSVECSubtract(&target, &pCamWork->targetPosition, &targetOffset);
    f32 x = targetOffset.x, y = targetOffset.y, z = targetOffset.z;
    cameraSetTargetOfsXYZ(x, y, z);
    PSVECSubtract(&position, &target, &position);
    pCamWork->height = position.y;
    pCamWork->distance = sqrtf(position.x * position.x + position.z * position.z);
    pCamWork->rotation.y = (f32)atan2(position.x, position.z);
    pCamWork->rotation.x = -(f32)atan2(pCamWork->height, pCamWork->distance);
}

void _cameraPadRotateUpdate(_GScamera* camera)
{
    if (dbgMenuIsOpen()) return;
    s32 rightX = GSinputGetRightStickXData(1, 1) * (s32)GSgfxVideoGetLastRenderTime();
    s32 rightY = GSinputGetRightStickYData(1, 1) * (s32)GSgfxVideoGetLastRenderTime();
    f32 inclination = cameraAtan2(pCamWork->height, pCamWork->distance);
    f32 radius = sqrtf(pCamWork->height * pCamWork->height + pCamWork->distance * pCamWork->distance);
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & CAMERA_PAD_FOV) {
        f32 newFov = rightY / 64.0f + cameraGetFov();
        cameraSetFov(newFov);
    } else if (GSinputButtonsPressed(GS_INPUT_PORT_1) & CAMERA_PAD_DISTANCE) {
        radius += rightY / 64.0f;
    } else {
        inclination -= rightY / 4096.0f;
    }
    pCamWork->height = radius * (f32)sin(inclination);
    pCamWork->distance = radius * (f32)cos(inclination);
    if (pCamWork->distance < 10.0f) pCamWork->distance = 10.0f;
    else if (pCamWork->distance > 500.0f) pCamWork->distance = 500.0f;
    cameraSetHeight(pCamWork->height);
    cameraSetDistance(pCamWork->distance);
    GSvec target;
    GSvec displacement(0.0f, pCamWork->height, pCamWork->distance);
    pCamWork->rotation.y += rightX / 2048.0f;
    cameraSetRotY(pCamWork->rotation.y);
    Mtx matrix;
    PSMTXRotRad(matrix, 'Y', pCamWork->rotation.y);
    PSMTXMultVec(matrix, &displacement, &displacement);
    PSVECAdd(&pCamWork->targetPosition, &pCamWork->targetOffset, &target);
    PSVECAdd(&target, &displacement, &pCamWork->position);
    if (pCamWork->flags & CAMERA_CAPTURE_TARGET_HEIGHT) {
        pCamWork->flags &= ~CAMERA_CAPTURE_TARGET_HEIGHT;
        pCamWork->lockedTargetHeight = target.y;
    } else if (pCamWork->flags & CAMERA_LOCK_TARGET_HEIGHT) target.y = pCamWork->lockedTargetHeight;
    GScameraSetPosition(camera, &pCamWork->position);
    GScameraLookAt(camera, &gUpVec, &target);
    pCamWork->rotation.x = -(f32)atan2(pCamWork->height, pCamWork->distance);
    f32 farPlane, nearPlane, aspect, fov;
    GScameraGetPerspective(camera, &fov, &aspect, &nearPlane, &farPlane);
    GScameraSetPerspective(camera, pCamWork->fov, aspect, nearPlane, farPlane);
}

void _cameraPadMoveUpdate(_GScamera* camera)
{
    f32 frameTime = GSgfxVideoGetLastRenderTime();
    f32 rightX;
    f32 divisor = 64.0f;
    if (dbgMenuIsOpen()) return;
    u32 changed = GSinputButtonsChanged(GS_INPUT_PORT_1);
    u32 pressed = GSinputButtonsPressed(GS_INPUT_PORT_1);
    if (pressed & changed & CAMERA_PAD_MENU) {
        if (menuIsCheck(MENU_ID_DEBUG_CAMERA)) menuClose(MENU_ID_DEBUG_CAMERA);
        else {
            menuOpenCustom(MENU_ID_DEBUG_CAMERA, 0, 0, 0, 1, 0);
            menuSetPosition(MENU_ID_DEBUG_CAMERA, 20, 260);
        }
    }
    Mtx matrix, yawMatrix, rollMatrix;
    PSMTXRotRad(matrix, 'X', pCamWork->rotation.x);
    PSMTXRotRad(yawMatrix, 'Y', pCamWork->rotation.y);
    PSMTXConcat(yawMatrix, matrix, matrix);
    PSMTXRotRad(rollMatrix, 'Z', pCamWork->rotation.z);
    PSMTXConcat(rollMatrix, matrix, matrix);
    f32 leftX = frameTime * GSinputGetLeftStickXData(1, 0);
    f32 leftY = frameTime * GSinputGetLeftStickYData(1, 0);
    rightX = frameTime * GSinputGetRightStickXData(1, 0);
    f32 rightY = frameTime * GSinputGetRightStickYData(1, 0);
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & CAMERA_PAD_DISTANCE) divisor *= 4.0f;
    GSvec movement, displacement;
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & CAMERA_PAD_VERTICAL) {
        displacement.x = leftX / divisor;
        displacement.y = -leftY / divisor;
        displacement.z = 0.0f;
    } else {
        displacement.x = leftX / divisor;
        displacement.y = 0.0f;
        displacement.z = leftY / divisor;
    }
    PSMTXMultVec(matrix, &displacement, &movement);
    PSVECAdd(&pCamWork->position, &movement, &pCamWork->position);
    if (GSinputButtonsPressed(GS_INPUT_PORT_1) & CAMERA_PAD_FOV) {
        f32 newFov = rightY / divisor + cameraGetFov();
        cameraSetFov(newFov);
    }
    else pCamWork->rotation.x -= rightY / (32.0f * divisor);
    pCamWork->rotation.y -= rightX / (32.0f * divisor);
    GScameraSetPosition(camera, &pCamWork->position);
    GScameraSetRotation(camera, &pCamWork->rotation);
    f32 farPlane, nearPlane, aspect, fov;
    GScameraGetPerspective(camera, &fov, &aspect, &nearPlane, &farPlane);
    GScameraSetPerspective(camera, pCamWork->fov, aspect, nearPlane, farPlane);
}


static inline void setMode(u8 type)
{
    if (pCamWork->type != type) {
        pCamWork->type = type;
        pCamWork->flags &= ~CAMERA_LOCK_Y_TARGET;
    }
}

static inline GScamera* getStateCamera()
{
    GScamera* camera = (GScamera*)GSresGetResource(0, 0);
    CameraWork* work = pCamWork;
    if (work->type == CAMERA_ANIMATION || work->type == CAMERA_OFFSET_ANIMATION) {
        GScamera* resource = (GScamera*)GSresGetResource(work->animationGroup, work->animationID);
        if (!resource) resource = (GScamera*)GSresGetResourceAllGroup(work->animationID);
        camera = resource;
    }
    return camera;
}

void cameraSetGScamera(GScamera* camera)
{
    if (camera) {
        GScamera* mainCamera = (GScamera*)GSresGetResource(0, 0);
        f32 fov, aspect, nearClip, farClip;
        GSvec distance;
        GScameraGetPerspective(camera, &fov, &aspect, &nearClip, &farClip);
        GScameraSetPerspective(mainCamera, fov, aspect, nearClip, farClip);
        GScameraGetDistanceVector(camera, &distance);
        pCamWork->flags = 0;
        setMode(CAMERA_FREE);
        mainCamera->target.x = camera->target.x;
        mainCamera->target.y = camera->target.y;
        mainCamera->target.z = camera->target.z;
        mainCamera->position.x = camera->position.x;
        mainCamera->position.y = camera->position.y;
        mainCamera->position.z = camera->position.z;
        f32 camHeight, camDistance, camFov, yaw;
        camFov = fov;
        yaw = (f32)atan2(distance.x, distance.z);
        camDistance = distance.z;
        camHeight = distance.y;
        FloorCamera* floor = cameraGetFloor(GSfloorGetCurrent());
        if (floor) {
            if (floor->flags) {
                camHeight = floor->height;
                camDistance = floor->distance;
                yaw = floor->rotY;
            } else {
                if (camHeight < 0.0f) camHeight = 0.0f;
                if (camDistance < 10.0f) {
                    camDistance = 30.0f;
                    camHeight = 0.0f;
                } else if (camDistance > 500.0f) {
                    camDistance = 500.0f;
                }
                floor->flags = FloorCamera::FROM_CAMERA;
                floor->defaultHeight = camHeight;
                floor->defaultDistance = camDistance;
                floor->defaultRotY = yaw;
                floor->defaultFov = camFov;
            }
        }
        cameraSetHeight(camHeight);
        cameraSetDistance(camDistance);
        cameraSetRotY(yaw);
        if (camFov < 3.0f) camFov = 3.0f;
        if (camFov > 120.0f) camFov = 120.0f;
        FloorCamera* fovFloor = cameraGetFloor(GSfloorGetCurrent());
        if (fovFloor) fovFloor->fov = camFov;
        pCamWork->fov = camFov;
    }
}

void cameraResetFloor()
{
    if (pCamWork->type == CAMERA_PAD_MOVE) {
        pCamWork->flags = 0;
        setMode(CAMERA_NORMAL);
    }
    FloorCamera* floor = cameraGetFloor(GSfloorGetCurrent());
    if (floor) {
        cameraSetHeight(floor->defaultHeight);
        cameraSetDistance(floor->defaultDistance);
        cameraSetRotY(floor->defaultRotY);
        cameraSetFov(floor->defaultFov);
    }
}

void cameraSetFloorDefault(f32 height, f32 distance, f32 yaw)
{
    FloorCamera* floor = cameraGetFloor(GSfloorGetCurrent());
    if (floor) {
        floor->defaultHeight = height;
        floor->defaultDistance = distance;
        floor->defaultRotY = yaw;
        if (floor->flags == FloorCamera::FROM_CAMERA) {
            floor->flags = FloorCamera::DEFAULT_SET;
            cameraSetHeight(height);
            cameraSetDistance(distance);
            cameraSetRotY(yaw);
        }
    }
}

void cameraInit()
{
    GSvec target(0.0f, 0.0f, 14.0f);
    GSvec up(0.0f, 1.0f, 0.0f);
    GSvec position(0.0f, 0.0f, 100.0f);
    memset(&_cameraWork, 0, sizeof(CameraWork));
    pCamWork = &_cameraWork;
    GScamera* camera = GScameraCreate();
    GSresRegisterResource(camera, 0, 0, 0);
    pCamWork->flags = 0;
    setMode(CAMERA_NORMAL);
    pCamWork->targetGroup = CAMERA_DEFAULT_GROUP;
    pCamWork->targetID = CAMERA_DEFAULT_TARGET;
    pCamWork->targetPart = CAMERA_NO_PART;
    pCamWork->targetOffset = target;
    _pFloorCamera = (FloorCamera*)GSmemAllocClr(floordata_number->floorCount * sizeof(FloorCamera));
    for (unsigned long i = 0; i < floordata_number->floorCount; i++) {
        _pFloorCamera[i].flags = FloorCamera::UNINITIALIZED;
        _pFloorCamera[i].floorID = floordata[i].floorID;
    }
    cameraResetOffsets(true);
    FloorModuleInterface_clear(&_floorModuleCamera);
    _floorModuleCamera.makeStateData = _cameraMakeStateData;
    _floorModuleCamera.restoreStateData = _cameraRestoreStateData;
    _floorModuleCamera.getStateSize = _cameraGetStateSize;
    _floorModuleCamera.getUID = _cameraGetUID;
    _floorModuleCamera.getModuleName = _cameraGetModuleName;
    _hModuleField = floorRegisterModule(CAMERA_MODULE_REGISTRATION_CATEGORY, 0, &_floorModuleCamera);
    GScameraLookAt(camera, &up, &target);
    GScameraSetPosition(camera, &position);
    GScameraSetActiveCamera(camera);
    GScameraLoadActiveCameraMatrix((GSgfxLayerID)0);
}

const char* _cameraGetModuleName() { return "camera"; }

unsigned long _cameraGetUID() { return CAMERA_MODULE_UID; }

unsigned long _cameraGetStateSize(FloorStateMode type)
{
    if (type == FLOOR_STATE_MODE_FIELD || type == FLOOR_STATE_MODE_FIGHT)
        return sizeof(CameraStateData);
    return 0;
}

void _cameraRestoreStateData(FloorStateMode type, void* data, unsigned long size)
{
#pragma unused(size)
    if (type == FLOOR_STATE_MODE_FIELD || type == FLOOR_STATE_MODE_FIGHT) {
        memcpy(pCamWork, data, sizeof(CameraWork));
        GScamera* camera = getStateCamera();
        if (pCamWork->flags & CAMERA_LOCK_TARGET_HEIGHT)
            pCamWork->flags |= CAMERA_CAPTURE_TARGET_HEIGHT;
        GScameraPopState(camera, (GScameraState*)((u8*)data + sizeof(CameraWork)));
        GScameraSetActiveCamera(camera);
    }
}

void _cameraMakeStateData(FloorStateMode mode, void* data, unsigned long size)
{
#pragma unused(size)
    if (mode == FLOOR_STATE_MODE_FIELD || mode == FLOOR_STATE_MODE_FIGHT) {
        GScamera* camera;
        u8* state = (u8*)data;
        camera = (GScamera*)GSresGetResource(0, 0);
        const CameraWork* work = pCamWork;
        if (work->type == CAMERA_ANIMATION || work->type == CAMERA_OFFSET_ANIMATION) {
            GScamera* resource = (GScamera*)GSresGetResource(work->animationGroup, work->animationID);
            if (!resource) resource = (GScamera*)GSresGetResourceAllGroup(work->animationID);
            camera = resource;
        }
        memcpy(state, pCamWork, sizeof(CameraWork));
        GScameraPushState(camera, (GScameraState*)(state + sizeof(CameraWork)));
    }
}

template <> void CameraAnimation<GSvec>::start(const GSvec* from, const GSvec* to, f32 duration)
{
    if (duration < 0.0000001f) {
        if (duration > -0.0000001f)
            return;
    }

    m_isRunning = 1;
    m_duration = duration;
    m_elapsed = 0.0f;
    m_start.x = from->x;
    m_start.y = from->y;
    m_start.z = from->z;
    m_end.x = to->x;
    m_end.y = to->y;
    m_end.z = to->z;
    m_acceleration = 0.05f;
    m_speed = 0.0f;
    m_state = CAMERA_ANIMATION_ACCELERATING;
}

template <> void CameraAnimation<GSvec>::stop()
{
    m_isRunning = 0;
    m_useAcceleration = 0;
}

template <> u8 CameraAnimation<GSvec>::isMove() const
{
    return m_isRunning;
}

template <> u8 CameraAnimation<GSvec>::update(f32 delta)
{
    GSvec step;
    f32 duration;
    f32 elapsed;

    if (!m_isRunning)
        return 0;
    if (m_useAcceleration)
        return updateAcceleration(delta);

    m_rampPending = 0;
    m_elapsed += delta;
    elapsed = m_elapsed;
    duration = m_duration;
    if (elapsed >= duration) {
        m_current.x = m_end.x;
        m_current.y = m_end.y;
        m_current.z = m_end.z;
        m_elapsed = 0.0f;
        m_isRunning = 0;
        m_useAcceleration = 0;
        return 1;
    }

    PSVECSubtract(&m_end, &m_start, &step);
    PSVECScale(&step, &step, elapsed / duration);
    PSVECAdd(&m_start, &step, &m_current);
    return 1;
}

template <> u8 CameraAnimation<GSvec>::updateAcceleration(f32 delta)
{
    GSvec step;
    f32 duration;
    f32 elapsed;

    if (!m_isRunning)
        return 0;

    if (m_rampPending) {
        if (!m_useDirectRamp && m_frameRateScaled) {
            f32 sum = 0.0f;
            f32 step;
            f32 ramp;
            m_speed = 1.0f;
            step = 1.0f / (f32)GSgfxVideoGetVsyncRate();
            if (GSgfxVideoGetLockFrameRate() == 1)
                step *= 2.0f;
            ramp = 0.0f;
            while (ramp < 1.0f) {
                sum += step * ramp;
                ramp += m_acceleration;
            }
            m_decelerationTime = m_duration - sum;
            m_state = CAMERA_ANIMATION_CONSTANT_SPEED;
        }
        m_rampPending = 0;
    }

    m_elapsed += delta * m_speed;
    elapsed = m_elapsed;
    duration = m_duration;
    if (elapsed >= duration) {
        m_current.x = m_end.x;
        m_current.y = m_end.y;
        m_current.z = m_end.z;
        m_elapsed = 0.0f;
        m_isRunning = 0;
        m_useAcceleration = 0;
        return 1;
    }

    PSVECSubtract(&m_end, &m_start, &step);
    PSVECScale(&step, &step, elapsed / duration);
    PSVECAdd(&m_start, &step, &m_current);

    switch (m_state) {
    case CAMERA_ANIMATION_ACCELERATING:
        m_speed += m_acceleration;
        if (m_speed >= 1.0f) {
            m_state = CAMERA_ANIMATION_CONSTANT_SPEED;
            m_decelerationTime = m_duration - m_elapsed;
        }
        break;

    case CAMERA_ANIMATION_CONSTANT_SPEED:
        if (m_frameRateScaled && m_elapsed >= m_decelerationTime)
            m_state = CAMERA_ANIMATION_DECELERATING;
        break;

    case CAMERA_ANIMATION_DECELERATING:
        m_speed -= m_acceleration;
        if (m_speed < m_acceleration)
            m_speed = m_acceleration;
        break;
    }
    return 1;
}

template <> void CameraAnimation<GSvec>::setAcceleration(f32 acceleration)
{
    m_acceleration = acceleration;
    m_useAcceleration = 1;
    m_frameRateScaled = 1;
    m_useDirectRamp = 1;
}

template <> const GSvec* CameraAnimation<GSvec>::getCurrentPos() const
{
    return &m_current;
}

template <> void CameraAnimation<f32>::start(const f32* from, const f32* to, f32 duration)
{
    if (duration < 0.0000001f) {
        if (duration > -0.0000001f)
            return;
    }

    m_isRunning = 1;
    m_duration = duration;
    m_elapsed = 0.0f;
    m_start = *from;
    m_end = *to;
    m_acceleration = 0.05f;
    m_speed = 0.0f;
    m_state = CAMERA_ANIMATION_ACCELERATING;
}

template <> void CameraAnimation<f32>::stop()
{
    m_isRunning = 0;
    m_useAcceleration = 0;
}

template <> u8 CameraAnimation<f32>::isMove() const
{
    return m_isRunning;
}

template <> u8 CameraAnimation<f32>::update(f32 delta)
{
    if (!m_isRunning)
        return 0;
    if (m_useAcceleration)
        return updateAcceleration(delta);

    m_rampPending = 0;
    m_elapsed += delta;
    if (m_elapsed >= m_duration) {
        m_current = m_end;
        m_elapsed = 0.0f;
        m_isRunning = 0;
        m_useAcceleration = 0;
        return 1;
    }

    f32 difference = m_end - m_start;
    difference *= m_elapsed / m_duration;
    m_current = difference + m_start;
    return 1;
}

template <> u8 CameraAnimation<f32>::updateAcceleration(f32 delta)
{
    if (!m_isRunning)
        return 0;

    if (m_rampPending) {
        if (!m_useDirectRamp && m_frameRateScaled) {
            f32 sum = 0.0f;
            f32 step;
            f32 ramp;
            m_speed = 1.0f;
            step = 1.0f / (f32)GSgfxVideoGetVsyncRate();
            if (GSgfxVideoGetLockFrameRate() == 1)
                step *= 2.0f;
            ramp = 0.0f;
            while (ramp < 1.0f) {
                sum += step * ramp;
                ramp += m_acceleration;
            }
            m_decelerationTime = m_duration - sum;
            m_state = CAMERA_ANIMATION_CONSTANT_SPEED;
        }
        m_rampPending = 0;
    }

    m_elapsed += delta * m_speed;
    if (m_elapsed >= m_duration) {
        m_current = m_end;
        m_elapsed = 0.0f;
        m_isRunning = 0;
        m_useAcceleration = 0;
        return 1;
    }

    f32 difference = m_end - m_start;
    difference *= m_elapsed / m_duration;
    m_current = difference + m_start;

    switch (m_state) {
    case CAMERA_ANIMATION_ACCELERATING:
        m_speed += m_acceleration;
        if (m_speed >= 1.0f) {
            m_state = CAMERA_ANIMATION_CONSTANT_SPEED;
            m_decelerationTime = m_duration - m_elapsed;
        }
        break;

    case CAMERA_ANIMATION_CONSTANT_SPEED:
        if (m_frameRateScaled && m_elapsed >= m_decelerationTime)
            m_state = CAMERA_ANIMATION_DECELERATING;
        break;

    case CAMERA_ANIMATION_DECELERATING:
        m_speed -= m_acceleration;
        if (m_speed < m_acceleration)
            m_speed = m_acceleration;
        break;
    }
    return 1;
}

template <> void CameraAnimation<f32>::setAcceleration(f32 acceleration)
{
    m_acceleration = acceleration;
    m_useAcceleration = 1;
    m_frameRateScaled = 1;
    m_useDirectRamp = 1;
}

template <> const f32* CameraAnimation<f32>::getCurrentPos() const
{
    return &m_current;
}





CameraWork _cameraWork;
CameraWork _cameraWorkBak;
GSvec gUpVec(0.0f, 1.0f, 0.0f);
FloorModuleInterface _floorModuleCamera;
CameraWork* pCamWork = &_cameraWork;
FloorCamera* _pFloorCamera;


template <class T> CameraAnimation<T>::CameraAnimation()
{
    m_isRunning = 0;
    m_useAcceleration = 0;
    m_frameRateScaled = 0;
    m_useDirectRamp = 0;
    m_acceleration = 0.05f;
}
