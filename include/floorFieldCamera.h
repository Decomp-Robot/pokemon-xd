#ifndef FLOOR_FIELD_CAMERA_H
#define FLOOR_FIELD_CAMERA_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FloorFieldCameraRecord {
    u8 rectangular;
    f32 x, y, z;
    f32 distance;
    f32 height;
    f32 rotationY;
    f32 radius;
    f32 aspect;
    f32 angle;
    f32 blendCurve;
    f32 blendTime;
    f32 fov;
} FloorFieldCameraRecord;

typedef struct FloorFieldCameraList {
    u32 *count;
    FloorFieldCameraRecord *records;
} FloorFieldCameraList;

enum FloorFieldCameraMode {
    FIELD_CAMERA_MODE_AREA = 1
};

u8 floorFieldCameraGetMode(void);
u8 floorFieldCameraDataExist(void);
u8 floorFieldCameraGetEnabled(void);
void floorFieldCameraBlend(f32 duration);
void floorFieldCameraEnable(void);
void floorFieldCameraDisable(void);

#ifdef __cplusplus
}

struct GSfloor_dd_;
void floorInitFieldCamera(GSfloor_dd_ *floor);
#endif

#endif
