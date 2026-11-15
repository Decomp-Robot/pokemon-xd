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

struct FloorFieldCameraList;
struct FloorOffscreenResources;

typedef struct GSfloor_dd_ {
    u8 unknown;
    u8 kind;
    u16 floorID;
    u32 mapResID;
    FloorMapResourceTable *resourceTable;
    u8 unknownData[8];
    struct FloorFieldCameraList **fieldCameraList;
    u8 unknownMid[16];
    struct FloorOffscreenResources **offscreen;
    u8 unknownTail[20];
} GSfloor_dd_;

typedef GSfloor_dd_ RoomData;

#ifdef __cplusplus
extern "C" {
#endif

u32 roomDataGetFsysGidForLanguage(GSfloor_dd_ *room);
u32 mapGetGroupID(GSfloor_dd_ *room);

#ifdef __cplusplus
}
#endif

#endif
