#ifndef ROOM_DATA_H
#define ROOM_DATA_H

#include <types.h>

typedef struct RoomData {
    u8 unknown0[2];
    u16 floorID;
    u32 mapResID;
    u8 unknown8[56];
} RoomData;

typedef struct GSfloor_dd_ GSfloor_dd_;

#ifdef __cplusplus
extern "C" {
#endif

u32 roomDataGetFsysGidForLanguage(GSfloor_dd_ *room);
u32 mapGetGroupID(GSfloor_dd_ *room);

#ifdef __cplusplus
}
#endif

#endif
