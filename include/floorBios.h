#ifndef FLOORBIOS_H
#define FLOORBIOS_H
#include <types.h>
#include <roomData.h>
#include <app/floor/floorCharacterBios.h>
struct FloorTreasure {
    u8 flags;
    u8 count;
    u8 reserved02;
    u8 reserved03;
    u16 floorID;
    u16 clearFlag;
    u8 reserved08;
    u8 reserved09;
    u16 reserved0A;
    u32 value;
    u8 reserved10[12];
};
#ifdef __cplusplus
extern "C" {
#endif
u8 floorDataBiosGetFloorKind(void*);
RoomData* floorDataBiosGetCurrentPtr();
u32 floorDataBiosGetMapResID(RoomData* floor);
void floorDataBiosSetMapResID(RoomData* floor, u32 res);
void floorGetLightInfo(u32 *resource, long *count);
FloorTreasure* floorEventGetTresureList(u32 id);
#ifdef __cplusplus
}
#endif
#endif
