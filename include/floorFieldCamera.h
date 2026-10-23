#ifndef FLOOR_FIELD_CAMERA_H
#define FLOOR_FIELD_CAMERA_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 floorFieldCameraDataExist(void);
u8 floorFieldCameraGetEnabled(void);
void floorFieldCameraBlend(f32 duration);
void floorFieldCameraEnable(void);
void floorFieldCameraDisable(void);

#ifdef __cplusplus
}
#endif

#endif
