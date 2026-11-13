#ifndef ROOM_DATA_H
#define ROOM_DATA_H

#include <types.h>

typedef struct FloorMapResourceData {
    u32 sunResID;
    u32 shadowLightID;
    u32 shadowReciveID[8];
    u32 secondaryRenderID[3];
    u32 modelID[3];
} FloorMapResourceData;

typedef struct FloorMapResourceTable {
    FloorMapResourceData *resources;
} FloorMapResourceTable;

typedef struct RoomData {
    u8 unknown0[2];
    u16 floorID;
    u32 mapResID;
    FloorMapResourceTable *resourceTable;
    u8 unknown12[52];
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
