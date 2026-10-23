#ifndef FLOOR_FIELD_CAMERA_H
#define FLOOR_FIELD_CAMERA_H

#include <types.h>
#include <GSgapp.h>

typedef struct GSfloor_dd_ GSfloor_dd_;

typedef struct FloorCameraRegion {
    u8 rectangular;
    u8 reservedShape[3];
    f32 x, y, z;
    f32 cameraDistance, cameraHeight, cameraRotation;
    f32 radius, aspect, rotation;
    u8 reservedCameraParameters[8];
    f32 cameraFov;
} FloorCameraRegion;

typedef struct FloorCameraPositionList {
    u32 *count;
    FloorCameraRegion *regions;
} FloorCameraPositionList;

#ifdef __cplusplus
extern "C" {
#endif
u8 floorFieldCameraDataExist(void);
void floorFieldCameraEnable(void);
void floorFieldCameraDisable(void);
u8 floorFieldCameraGetMode(void);
FloorCameraPositionList *floorDataBiosGetPosListPtr(GSfloor_dd_ *floor);
u8 floorFieldCameraEditorGetMode(void);
void floorReleaseFieldCameraEditor(void);
void floorInitFieldCameraEditor(u32 roomID);
void floorUpdateFieldCameraEditor(GSgappID appID, void *userData);
void floorFieldCameraEditorStopTest(void);
u32 floorFieldCameraEditorGetFloorID(void);
#ifdef __cplusplus
}
void floorInitFieldCamera(GSfloor_dd_ *floor);
#endif

#endif
