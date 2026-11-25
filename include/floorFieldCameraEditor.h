#ifndef FLOOR_FIELD_CAMERA_EDITOR_H
#define FLOOR_FIELD_CAMERA_EDITOR_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 floorFieldCameraEditorGetMode(void);
void floorReleaseFieldCameraEditor(void);
void floorInitFieldCameraEditor(u32 floorID);
void floorUpdateFieldCameraEditor(u32 appID, u32 userData);
void floorFieldCameraEditorStopTest(void);
u32 floorFieldCameraEditorGetFloorID(void);

#ifdef __cplusplus
}
#endif

#endif
