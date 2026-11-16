#include <floorFieldCamera.h>
#include <floorDataBios.h>
#include <app/floor/floorModule.h>
#include <floorBios.h>
#include <GSBinStream.h>
#include <GSmath.h>
#include <GSmodel.h>
#include <GSres.h>
#include <GSlog.h>
#include <GSgfxVideo.h>
#include <app/camera/camera.h>
#include <app/camera/CameraType.h>
#include <libc/math.h>

static FloorModuleInterface _fieldCameraModule;
static unsigned long _fieldCamera_hModule;
u32 gNumFieldCameras;
FloorFieldCameraRecord* gFieldCameraList;
static bool gFieldCameraEnabled;
static bool gFieldCameraPaused;
static bool gFieldCameraInitialized;
static u8 gFieldCameraMode;
static bool gFieldCameraBlending;
u16 g_currentIndex;
f32 gCameraInitHeight;
f32 gCameraHeight;
f32 gCameraInitDistance;
f32 gCameraDistance;
f32 gCameraInitRotY;
f32 gCameraRotY;
f32 gCameraInitFOV = 30.0f;
f32 gCameraFOV = 30.0f;
f32 gPowValue;
f32 g_BlendingTime;
f32 g_BlendingMaxTime = 1.0f;

enum { FIELD_CAMERA_STATE_MAGIC = 967, FIELD_CAMERA_STATE_VERSION = 0,
       FIELD_CAMERA_MODULE_TYPE = 10, FIELD_CAMERA_MODULE_MAGIC = 21,
       FIELD_CAMERA_TARGET_GROUP = 0, FIELD_CAMERA_TARGET_RESOURCE = 100 };

void _fieldCameraExitFloor(unsigned int, FloorEnterMode);
void _fieldCameraEnterFloor(unsigned int, FloorEnterMode);
const char* _fieldCameraGetModuleName();
unsigned long _fieldCameraGetMagicNumber();
unsigned long _fieldCameraGetStateSize(FloorStateMode);
void _fieldCameraRestoreStateData(FloorStateMode, void*, unsigned long);
void _fieldCameraMakeStateData(FloorStateMode, void*, unsigned long);
void floorReleaseFieldCamera();
bool _floorFieldCameraCalculateBlendParams(GSvec*, f32*, f32*, f32*, f32*);
bool _floorFieldCameraCalculatePathParams(GSvec*, f32*, f32*, f32*, f32*);
void _floorFieldCameraSetParams(f32, f32, f32, f32);
void _floorFieldCameraGetTargetPos(GSvec*);
void _floorFieldCameraDetectMode();

void floorFieldCameraInit()
{
    FloorModuleInterface_clear(&_fieldCameraModule);
    _fieldCameraModule.makeStateData = _fieldCameraMakeStateData;
    _fieldCameraModule.restoreStateData = _fieldCameraRestoreStateData;
    _fieldCameraModule.getStateSize = _fieldCameraGetStateSize;
    _fieldCameraModule.getUID = _fieldCameraGetMagicNumber;
    _fieldCameraModule.getModuleName = _fieldCameraGetModuleName;
    _fieldCameraModule.initialize = _fieldCameraEnterFloor;
    _fieldCameraModule.finalize = _fieldCameraExitFloor;
    _fieldCamera_hModule = floorRegisterModule(FIELD_CAMERA_MODULE_TYPE, 0, &_fieldCameraModule);
}
void _fieldCameraExitFloor(unsigned int, FloorEnterMode mode)
{
    if (mode != FLOOR_ENTER_PUSH) floorReleaseFieldCamera();
}
void _fieldCameraEnterFloor(unsigned int floorID, FloorEnterMode mode)
{
    GSfloor_dd_* floor = floorDataBiosGetPtr(floorID);
    if (floor) {
        if (mode != FLOOR_ENTER_NORMAL) {
            switch (mode) {
            case FLOOR_ENTER_PUSH:
                break;
            default:
                return;
            }
        }
        floorInitFieldCamera(floor);
    }
}
const char* _fieldCameraGetModuleName() { return "field camera"; }
unsigned long _fieldCameraGetMagicNumber() { return FIELD_CAMERA_MODULE_MAGIC; }
unsigned long _fieldCameraGetStateSize(FloorStateMode mode)
{
    if (mode == FLOOR_STATE_MODE_SAVE || mode == FLOOR_STATE_MODE_FIELD) {
        GSBinWriteSizeStream stream;
        stream.writeU16(FIELD_CAMERA_STATE_MAGIC);
        stream.writeU8(FIELD_CAMERA_STATE_VERSION);
        stream.writeBool(gFieldCameraEnabled);
        stream.writeBool(gFieldCameraInitialized);
        stream.writeBool(gFieldCameraBlending);
        stream.writeU16(g_currentIndex);
        stream.writeFloat(gCameraInitHeight);
        stream.writeFloat(gCameraHeight);
        stream.writeFloat(gCameraInitDistance);
        stream.writeFloat(gCameraDistance);
        stream.writeFloat(gCameraInitRotY);
        stream.writeFloat(gCameraRotY);
        stream.writeFloat(gCameraInitFOV);
        stream.writeFloat(gCameraFOV);
        stream.writeFloat(gPowValue);
        stream.writeFloat(g_BlendingTime);
        stream.writeFloat(g_BlendingMaxTime);
        return stream.getWriteSize();
    }
    return 0;
}
void _fieldCameraRestoreStateData(FloorStateMode mode, void* data, unsigned long size)
{
    if (mode == FLOOR_STATE_MODE_SAVE || mode == FLOOR_STATE_MODE_FIELD) {
        GSBinReadStream stream(data, size);
        if (stream.readU16() != FIELD_CAMERA_STATE_MAGIC) return;
        if (stream.readU8() != FIELD_CAMERA_STATE_VERSION) return;
        GSfloor_dd_* floor = floorDataBiosGetCurrentPtr();
        if (!floor) return;
        FloorFieldCameraList* list = floorDataBiosGetFieldCameraListPtr(floor);
        if (!list) return;
        gNumFieldCameras = *list->count;
        gFieldCameraList = list->records;
        gFieldCameraEnabled = stream.readBool();
        gFieldCameraInitialized = stream.readBool();
        gFieldCameraBlending = stream.readBool();
        g_currentIndex = stream.readU16();
        gCameraInitHeight = stream.readFloat();
        gCameraHeight = stream.readFloat();
        gCameraInitDistance = stream.readFloat();
        gCameraDistance = stream.readFloat();
        gCameraInitRotY = stream.readFloat();
        gCameraRotY = stream.readFloat();
        gCameraInitFOV = stream.readFloat();
        gCameraFOV = stream.readFloat();
        gPowValue = stream.readFloat();
        g_BlendingTime = stream.readFloat();
        g_BlendingMaxTime = stream.readFloat();
        _floorFieldCameraDetectMode();
        floorUpdateFieldCamera();
    }
}
void _fieldCameraMakeStateData(FloorStateMode mode, void* data, unsigned long size)
{
    if (mode == FLOOR_STATE_MODE_SAVE || mode == FLOOR_STATE_MODE_FIELD) {
        GSBinWriteStream stream(data, size);
        stream.writeU16(FIELD_CAMERA_STATE_MAGIC);
        stream.writeU8(FIELD_CAMERA_STATE_VERSION);
        stream.writeBool(gFieldCameraEnabled);
        stream.writeBool(gFieldCameraInitialized);
        stream.writeBool(gFieldCameraBlending);
        stream.writeU16(g_currentIndex);
        stream.writeFloat(gCameraInitHeight);
        stream.writeFloat(gCameraHeight);
        stream.writeFloat(gCameraInitDistance);
        stream.writeFloat(gCameraDistance);
        stream.writeFloat(gCameraInitRotY);
        stream.writeFloat(gCameraRotY);
        stream.writeFloat(gCameraInitFOV);
        stream.writeFloat(gCameraFOV);
        stream.writeFloat(gPowValue);
        stream.writeFloat(g_BlendingTime);
        stream.writeFloat(g_BlendingMaxTime);
    }
}
u8 floorFieldCameraGetMode() { return gFieldCameraMode; }
void floorReleaseFieldCamera() { gNumFieldCameras = 0; gFieldCameraList = 0; }
void floorInitFieldCamera(GSfloor_dd_* floor)
{
    FloorFieldCameraList* list = floorDataBiosGetFieldCameraListPtr(floor);
    gNumFieldCameras = 0;
    gFieldCameraList = 0;
    gFieldCameraInitialized = false;
    gFieldCameraEnabled = true;
    gCameraHeight = 0.0f;
    gCameraDistance = 0.0f;
    gCameraRotY = 0.0f;
    gCameraFOV = 30.0f;
    gPowValue = 0.0f;
    if (list) {
        gNumFieldCameras = *list->count;
        gFieldCameraList = list->records;
        _floorFieldCameraDetectMode();
        g_currentIndex = 0;
        gCameraInitHeight = gCameraHeight = gFieldCameraList->height;
        gCameraInitDistance = gCameraDistance = gFieldCameraList->distance;
        gCameraInitRotY = gCameraRotY = gFieldCameraList->rotationY;
        gCameraInitFOV = gCameraFOV = gFieldCameraList->fov;
        gPowValue = gFieldCameraList->blendCurve;
        gFieldCameraBlending = false;
        g_BlendingTime = 0.0f;
        floorUpdateFieldCamera();
    }
}
void floorUpdateFieldCamera()
{
    GSvec target;
    if (!gFieldCameraEnabled) return;
    if (gFieldCameraPaused) return;
    if (cameraGetType() != CAMERA_NORMAL) return;
    if (gNumFieldCameras == 0) return;
    if (gNumFieldCameras == 1) {
        _floorFieldCameraSetParams(gFieldCameraList->height, gFieldCameraList->distance, gFieldCameraList->rotationY, gFieldCameraList->fov);
        return;
    }
    cameraGetTargetPos(&target);
    f32 distance, height, rotationY, fov;
    height = gCameraHeight;
    distance = gCameraDistance;
    rotationY = gCameraRotY;
    fov = gCameraFOV;
    switch (gFieldCameraMode) {
    case FIELD_CAMERA_MODE_AREA:
        if (!_floorFieldCameraCalculatePathParams(&target, &height, &distance, &rotationY, &fov)) return;
        if (gFieldCameraInitialized && gFieldCameraBlending && g_BlendingMaxTime > 0.0f) {
            g_BlendingTime += GSgfxVideoGetLastRenderTime() / GSgfxVideoGetVsyncRate();
            if (g_BlendingTime >= g_BlendingMaxTime) {
                gFieldCameraBlending = false;
                gCameraHeight = height;
                gCameraDistance = distance;
                gCameraRotY = rotationY;
                gCameraFOV = fov;
            } else {
                f32 blend = g_BlendingTime / g_BlendingMaxTime;
                if (gPowValue < -0.00001f) {
                    gCameraHeight = blend * (height - gCameraInitHeight) + gCameraInitHeight;
                    gCameraDistance = blend * (distance - gCameraInitDistance) + gCameraInitDistance;
                    gCameraRotY = blend * (rotationY - gCameraInitRotY) + gCameraInitRotY;
                    gCameraFOV = blend * (fov - gCameraInitFOV) + gCameraInitFOV;
                } else if (gPowValue < 0.19999f) {
                    blend *= blend;
                    gCameraHeight = blend * (height - gCameraHeight) + gCameraHeight;
                    gCameraDistance = blend * (distance - gCameraDistance) + gCameraDistance;
                    gCameraRotY = blend * (rotationY - gCameraRotY) + gCameraRotY;
                    gCameraFOV = blend * (fov - gCameraFOV) + gCameraFOV;
                } else {
                    bool secondHalf = false;
                    blend *= 2.0f;
                    if (blend > 1.0f) { secondHalf = true; blend = 2.0f - blend; }
                    blend = 1.0f - std::cosf(1.57079637f * blend);
                    blend = std::pow(blend, gPowValue);
                    if (secondHalf) blend = 2.0f - blend;
                    blend *= 0.5f;
                    gCameraHeight = blend * (height - gCameraInitHeight) + gCameraInitHeight;
                    gCameraDistance = blend * (distance - gCameraInitDistance) + gCameraInitDistance;
                    gCameraRotY = blend * (rotationY - gCameraInitRotY) + gCameraInitRotY;
                    gCameraFOV = blend * (fov - gCameraInitFOV) + gCameraInitFOV;
                }
            }
        } else {
            gFieldCameraBlending = false;
            gCameraInitHeight = gCameraHeight = height;
            gCameraInitDistance = gCameraDistance = distance;
            gCameraInitRotY = gCameraRotY = rotationY;
            gCameraInitFOV = gCameraFOV = fov;
            gFieldCameraInitialized = true;
        }
        break;
    case FIELD_CAMERA_MODE_BLEND:
    default:
        if (!_floorFieldCameraCalculateBlendParams(&target, &height, &distance, &rotationY, &fov)) return;
        if (gFieldCameraInitialized) {
            gCameraHeight = 0.5f * gCameraHeight + 0.5f * height;
            gCameraDistance = 0.5f * gCameraDistance + 0.5f * distance;
            gCameraRotY = 0.5f * gCameraRotY + 0.5f * rotationY;
            gCameraFOV = 0.5f * gCameraFOV + 0.5f * fov;
        } else {
            gCameraHeight = height;
            gCameraDistance = distance;
            gCameraRotY = rotationY;
            gCameraFOV = fov;
            gFieldCameraInitialized = true;
        }
        break;
    }
    _floorFieldCameraSetParams(gCameraHeight, gCameraDistance, gCameraRotY, gCameraFOV);
}
void floorFieldCameraBlend(f32 duration)
{
    GSvec target, targetOffset, position, rotation, cameraOffset;
    GSmtx rotationMatrix;
    if (!gFieldCameraEnabled) return;
    if (gFieldCameraPaused) return;
    if (cameraGetType() != CAMERA_NORMAL) return;
    if (gNumFieldCameras == 0) return;
    _floorFieldCameraGetTargetPos(&target);
    cameraGetTargetOfs(&targetOffset);
    f32 distance, height, rotationY, fov;
    if (gNumFieldCameras == 1) {
        height = gFieldCameraList->height;
        distance = gFieldCameraList->distance;
        rotationY = gFieldCameraList->rotationY;
        fov = gFieldCameraList->fov;
    } else {
        height = cameraGetHeight();
        distance = cameraGetDistance();
        rotationY = cameraGetRotY();
        fov = cameraGetFov();
        switch (gFieldCameraMode) {
        case FIELD_CAMERA_MODE_AREA:
            if (!_floorFieldCameraCalculatePathParams(&target, &height, &distance, &rotationY, &fov)) return;
            break;
        case FIELD_CAMERA_MODE_BLEND:
    default:
            if (!_floorFieldCameraCalculateBlendParams(&target, &height, &distance, &rotationY, &fov)) return;
            break;
        }
    }
    GSvecSet(&cameraOffset, 0.0f, height, distance);
    GSmtxMakeYRotation(&rotationMatrix, rotationY);
    GSvecTransform(&cameraOffset, &rotationMatrix, &cameraOffset);
    GSvecAdd(&position, &target, &targetOffset);
    GSvecAdd(&position, &position, &cameraOffset);
    rotation.y = rotationY;
    rotation.x = -std::atan2f(height, distance);
    rotation.z = 0.0f;
    cameraMoveTargetOfs(CAMERA_NORMAL, &targetOffset, duration);
    cameraMoveTargetOfsAcc(0.08f);
    cameraMoveTargetPos(CAMERA_NORMAL, &target, duration);
    cameraMoveTargetPosAcc(0.08f);
    cameraMovePosition(CAMERA_NORMAL, &position, duration);
    cameraMovePositionAcc(0.08f);
    cameraMoveRotation(CAMERA_NORMAL, &rotation, duration);
    cameraMoveRotationAcc(0.08f);
    cameraSetFov(fov);
}
u8 floorFieldCameraDataExist()
{
    u8 exists = 0;
    if (gNumFieldCameras && gFieldCameraList) exists = 1;
    return exists;
}
u8 floorFieldCameraGetEnabled() { return gFieldCameraEnabled; }
void floorFieldCameraUnpause() { gFieldCameraPaused = false; }
void floorFieldCameraPause() { gFieldCameraPaused = true; }
void floorFieldCameraDisable() { gFieldCameraEnabled = false; }
void floorFieldCameraEnable() { gFieldCameraEnabled = true; gFieldCameraInitialized = false; floorFieldCameraUnpause(); }
bool _floorFieldCameraCalculateBlendParams(GSvec* target, f32* height, f32* distance, f32* rotationY, f32* fov)
{
    GSvec cameraPosition;
    f32 totalWeight = 0.0f;
    f32 weightedDistance = 0.0f;
    f32 weightedHeight = 0.0f;
    f32 weightedRotationY = 0.0f;
    f32 weightedFov = 0.0f;
    for (u32 index = 0; index < gNumFieldCameras; ++index) {
        FloorFieldCameraRecord* camera = &gFieldCameraList[index];
        GSvecSet(&cameraPosition, camera->x, target->y, camera->z);
        f32 squareDistance = GSvecSquareDistance(&cameraPosition, target);
        if (squareDistance > 1.0f) {
            f32 weight = 0.001f / squareDistance;
            totalWeight += weight;
            weightedDistance += camera->distance * weight;
            weightedHeight += camera->height * weight;
            weightedRotationY += camera->rotationY * weight;
            weightedFov += camera->fov * weight;
        } else {
            totalWeight = 1.0f;
            weightedHeight = camera->height;
            weightedDistance = camera->distance;
            weightedRotationY = camera->rotationY;
            weightedFov = camera->fov;
            break;
        }
    }
    if (totalWeight == 0.0f) {
        GSlogWrite("floorUpdateFieldCamera: error updating field camera - divide by zero!");
        return false;
    }
    f32 inverseWeight = 1.0f / totalWeight;
    *height = weightedHeight * inverseWeight;
    *distance = weightedDistance * inverseWeight;
    *rotationY = weightedRotationY * inverseWeight;
    *fov = weightedFov * inverseWeight;
    return true;
}
bool _floorFieldCameraCalculatePathParams(GSvec* target, f32* height, f32* distance, f32* rotationY, f32* fov)
{
    u16 index;
    GSvec2 offset, targetPosition;
    GSvec2Set(&targetPosition, target->x, target->z);
    FloorFieldCameraRecord* camera = &gFieldCameraList[g_currentIndex];
    *height = camera->height;
    *distance = camera->distance;
    *rotationY = camera->rotationY;
    *fov = camera->fov;
    GSvec2Set(&offset, camera->x, camera->z);
    GSvec2Sub(&offset, &offset, &targetPosition);
    if (camera->rectangular) {
        GSvec2Rot(&offset, &offset, -camera->angle);
        f32 x = offset.x;
        x = (x < 0.0f) ? -x : x;
        if (x < camera->radius) {
            x = offset.y;
            x = (x < 0.0f) ? -x : x;
            if (x < camera->radius * camera->aspect) return true;
        }
    } else {
        if (GSvec2Length(&offset) <= camera->radius) return true;
    }
    camera = gFieldCameraList;
    for (index = 0; index < gNumFieldCameras; ++index, ++camera) {
        if (index == g_currentIndex) continue;
        bool inside = false;
        GSvec2Set(&offset, camera->x, camera->z);
        GSvec2Sub(&offset, &offset, &targetPosition);
        if (camera->rectangular) {
            GSvec2Rot(&offset, &offset, -camera->angle);
            f32 x = offset.x;
            x = (x < 0.0f) ? -x : x;
            if (x < camera->radius) {
                x = offset.y;
                x = (x < 0.0f) ? -x : x;
                if (x < camera->radius * camera->aspect) inside = true;
            }
        } else {
            if (GSvec2Length(&offset) <= camera->radius) inside = true;
        }
        if (inside) {
            *height = camera->height;
            *distance = camera->distance;
            *rotationY = camera->rotationY;
            *fov = camera->fov;
            g_currentIndex = index;
            gCameraInitHeight = gCameraHeight;
            gCameraInitDistance = gCameraDistance;
            gCameraInitRotY = gCameraRotY;
            gCameraInitFOV = gCameraFOV;
            gPowValue = camera->blendCurve;
            gFieldCameraBlending = true;
            g_BlendingTime = 0.0f;
            g_BlendingMaxTime = camera->blendTime;
            break;
        }
    }
    return true;
}
void _floorFieldCameraSetParams(f32 height, f32 distance, f32 rotationY, f32 fov)
{
    cameraSetHeight(height);
    cameraSetDistance(distance);
    cameraSetRotY(rotationY);
    cameraSetFov(fov);
}
void _floorFieldCameraGetTargetPos(GSvec* target)
{
    GSmodel* model = static_cast<GSmodel*>(GSresGetResource(FIELD_CAMERA_TARGET_GROUP, FIELD_CAMERA_TARGET_RESOURCE));
    if (model) GSmodelGetPosition(model, target);
    else cameraGetTargetPos(target);
}
void _floorFieldCameraDetectMode()
{
    gFieldCameraMode = FIELD_CAMERA_MODE_BLEND;
    FloorFieldCameraRecord* camera = gFieldCameraList;
    for (u32 index = 0; index < gNumFieldCameras; ++index, ++camera) {
        if (camera->radius <= 0.001f || camera->blendTime < 0.0f) return;
    }
    gFieldCameraMode = FIELD_CAMERA_MODE_AREA;
}

#include <GSBinStreamImpl.h>
