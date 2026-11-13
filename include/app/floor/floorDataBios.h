#ifndef APP_FLOOR_DATA_BIOS_H
#define APP_FLOOR_DATA_BIOS_H

#include <types.h>
#include <roomData.h>

#ifdef __cplusplus
extern "C" {
#endif

GSfloor_dd_ *floorDataBiosGetPtr(s32 floorID);
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
