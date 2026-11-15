#ifndef FLOORBIOS_H
#define FLOORBIOS_H
#include <types.h>
#include <roomData.h>
#include <app/floor/floorCharacterBios.h>
struct FloorTreasure { u8 flags; };
#ifdef __cplusplus
extern "C" {
#endif
u8 floorDataBiosGetFloorKind(void*);
RoomData* floorDataBiosGetCurrentPtr();
u32 floorDataBiosGetMapResID(RoomData* floor);
void floorDataBiosSetMapResID(RoomData* floor, u32 res);
FloorTreasure* floorEventGetTresureList(u32 id);
#ifdef __cplusplus
}
#endif
#endif
