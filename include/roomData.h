#ifndef ROOM_DATA_H
#define ROOM_DATA_H

#include <types.h>

/* Kinds of floor indexed by the room and layer configuration. */
enum FloorKind {
    FLOOR_KIND_FIELD = 1,
    FLOOR_KIND_FIGHT = 2,
    FLOOR_KIND_FIGHT_ALT = 4,
    FLOOR_KIND_MENU_EXTRA_A = 5,
    FLOOR_KIND_MENU_EXTRA_B = 6,
    FLOOR_KIND_MENU = 7
};

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

struct floorCharacter_dd_;
struct FloorFieldCameraList;
struct FloorOffscreenResources;
struct GSfloor_dd_;

/* One entry of a room's placement list; the heading is in degrees. */
typedef struct FloorPosData {
    s16 angle;
    f32 x;
    f32 y;
    f32 z;
} FloorPosData;

/* Placement list of a room, as loaded from the floor archive. */
typedef struct FloorPosList {
    u32 *count;
    FloorPosData *data;
} FloorPosList;

/* Character list of a room, as loaded from the floor archive. */
typedef struct FloorCharacterList {
    u32 *count;
    struct floorCharacter_dd_ *characters;
} FloorCharacterList;

/* Script callbacks stored in a room. The pre and post callbacks receive the
   room and its localized resource group; the main callback takes no arguments. */
typedef void (*FloorRoomCallback)(struct GSfloor_dd_ *room, u32 groupID);
typedef void (*FloorUpdateCallback)(void);

typedef struct GSfloor_dd_ {
    u8 area;
    u8 kind;
    u16 floorID;
    u32 mapResID;
    FloorMapResourceTable *resourceTable;
    FloorCharacterList **characterList;
    FloorPosList **posList;
    struct FloorFieldCameraList **fieldCameraList;
    u32 placeName;
    FloorRoomCallback preFunc;
    FloorUpdateCallback mainFunc;
    FloorRoomCallback postFunc;
    struct FloorOffscreenResources **offscreen;
    /* File system group IDs holding the room's resources, one per language. */
    u32 fsysGid[5];
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
