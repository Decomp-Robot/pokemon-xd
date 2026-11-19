#ifndef APP_FLOOR_DATA_BIOS_H
#define APP_FLOOR_DATA_BIOS_H

#include <types.h>
#include <GSvec.h>
#include <roomData.h>
#include <app/floor/floorCharacterBios.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 floorTresureGetResID(u16 tableIndex);
RoomData *floorDataBiosGetPtr(u32 floorID);
RoomData *floorDataBiosGetCurrentPtr(void);
u8 floorDataBiosGetArea(RoomData *floor);
u8 floorDataBiosGetFloorKind(RoomData *floor);
u16 floorDataBiosGetFloorID(RoomData *floor);
u32 floorDataBiosGetMapResID(RoomData *floor);
void floorDataBiosSetMapResID(RoomData *floor, u32 resID);
u32 floorDataBiosGetPlaceName(RoomData *floor);
FloorRoomCallback floorDataBiosGetPreFunc(RoomData *floor);
FloorUpdateCallback floorDataBiosGetMainFunc(RoomData *floor);
FloorRoomCallback floorDataBiosGetPostFunc(RoomData *floor);
struct FloorFieldCameraList *floorDataBiosGetFieldCameraListPtr(RoomData *floor);
FloorPosList *floorDataBiosGetPosListPtr(RoomData *floor);
u8 floorDataBiosGetPosListData(u32 index, GSvec *position, f32 *angle);
FloorCharacter *floorDataBiosGetCharInfo(RoomData *floor, u32 index);
u32 floorDataBiosGetCharNum(RoomData *floor);
u32 floorDataBiosModelIDGetFogState(u32 modelID);
u32 floorDataBiosGetSecondaryRenderNum(RoomData *room);
u32 floorDataBiosGetSecondaryRenderID(RoomData *room, u32 index);
u32 floorDataBiosGetShadowReciveNum(RoomData *room);
u32 floorDataBiosGetShadowReciveID(RoomData *room, u32 index);
u32 floorDataBiosGetShadowLightID(RoomData *room);
u32 floorDataBiosGetLensFlareResID(RoomData *room);
u32 floorDataBiosGetSunResID(RoomData *room);

#ifdef __cplusplus
}
#endif

#endif
