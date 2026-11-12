#ifndef ROOM_DATA_H
#define ROOM_DATA_H

#include <types.h>

typedef struct RoomData {
    u8 unknown0[2];
    u16 floorID;
    u32 mapResID;
    u8 unknown8[56];
} RoomData;

#ifdef __cplusplus
extern "C" {
#endif

u32 roomDataGetFsysGidForLanguage(RoomData *room);
u32 mapGetGroupID(RoomData *room);

#ifdef __cplusplus
}
#endif

#endif
