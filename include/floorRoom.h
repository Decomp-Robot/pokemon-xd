#ifndef FLOOR_ROOM_H
#define FLOOR_ROOM_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 roomGetCurrentRoomID(void);
u32 fieldGetCurrentRoomID(void);
u32 floorGetPreviousMap(void);
u32 floorGetCurrentMap(void);
void floorUnlinkWithSE(u32 roomID, long soundEffect);
u32 floorFieldCameraEditorGetFloorID(void);
void floorInitFieldCameraEditor(u32 roomID);

#ifdef __cplusplus
}
#endif

#endif
