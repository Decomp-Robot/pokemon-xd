#ifndef FLOORBIOS_H
#define FLOORBIOS_H
#include <types.h>
#include <roomData.h>
struct FloorCharacter;
struct FloorTreasure { u8 flags; };
#ifdef __cplusplus
extern "C" {
#endif
u8 floorDataBiosGetFloorKind(void*);
RoomData* floorDataBiosGetCurrentPtr();
u32 floorDataBiosGetMapResID(RoomData* floor);
void floorDataBiosSetMapResID(RoomData* floor, u32 res);
FloorCharacter* floorCharacterBiosFindByResID(u32 group, u32 id);
u16 floorCharacterBiosGetNameID(FloorCharacter* character);
u8 floorCharacterBiosGetTalkStartType(FloorCharacter* character);
u8 floorCharacterBiosGetTalkWallThrough(FloorCharacter* character);
FloorTreasure* floorEventGetTresureList(u32 id);
#ifdef __cplusplus
}
#endif
#endif
