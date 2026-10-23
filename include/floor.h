#ifndef FLOOR_H
#define FLOOR_H

#include "types.h"
#include "GSthread.h"

#ifdef __cplusplus
#include "GSgfxLayer.h"

// The floor (room) module. One floor is open at a time: the field script picks
// it (floorChangeFloor / floorOpenFloor), pushes it on the floor stack and the
// module then runs that floor's script threads on its own thread group. This
// header holds the pieces the offscreen movie, the floor stack API and the
// thread helpers need.

// The room's offscreen list, reached through the room data: the room data
// points at a resource record whose first member is the list itself.
enum FloorOffscreenMode {
    FloorOffscreenModelAnimation = 0,
    FloorOffscreenMovie = 1
};

struct FloorOffscreenRes {
    struct FloorOffscreenList *mList; // 0x0: offscreen list of the room.
};

// The room data of the open floor.
struct GSfloor_dd_ {
    u8 reservedRoomData[0x14];
    struct FloorCameraPositionList *cameraPositions; // 0x14: field camera regions.
    u8 reservedRoomResources[0x10];
    FloorOffscreenRes *mOffscreenRes; // 0x28: offscreen record of the room.
};

struct FloorOffscreenList {
    u32 *mCount;                          // 0x0: number of entries.
    struct FloorOffscreenEntry *mEntries; // 0x4: first entry.
};

// One entry of a room's offscreen list: a model animation that plays behind
// the room, or a movie in movie mode. floorSetOffscreenIndex selects an
// entry by mIndex and plays it on the screen texture it creates.
struct FloorOffscreenEntry {
    u8 mMode;        // 0x00: offscreen playback mode.
    u8 mPlaybackParameter; // 0x01: model animation index, or movie loop flag.
    u16 mIndex;      // 0x02: offscreen index the entry is selected by.
    u16 mWidth;      // 0x04: screen texture width.
    u16 mHeight;     // 0x06: screen texture height.
    u32 mTextureReceiverModelRes; // 0x08: model resource receiving the offscreen texture.
    u32 mModelRes;   // 0x0C: model resource, or the movie resource in movie mode.
    u32 mExtraRes;   // 0x10: extra resource the entry keeps loaded.
};

// Format the offscreen screen texture is created with.
enum { FLOOR_OFFSCREEN_TEX_FORMAT = 0x44 };

// Index of the movie entry in the room's offscreen list.
enum { FLOOR_OFFSCREEN_MOVIE_INDEX = 1000 };

// Stack size a floor thread gets when the caller does not pass one.
enum { FLOOR_THREAD_STACK_SIZE = 0x4000 };

// Trainer data records the floor module reads; the selector is the record id
// getTrainerDataStartOffset is called with.
enum { TRAINER_DATA_ANYWAY_SAVE = 0x16 };

// Header of the saved floor state in the trainer-data anyway-save record.
struct AnywaySaveData {
    u32 mSignature;    // 0x00: "ANSW" anyway-save signature.
    u32 mSaveDataSize; // 0x04: serialized floor state size, including its header.
};

// Stack of the floors that were pushed, owned by the floor manager.
struct FloorStackList;
class FloorModuleList;

// Floor manager: opens and closes the floors and keeps their stack. Only the
// members the floor module calls are declared here.
class FloorManager {
public:
    u8 makeSaveData(u8 **data, unsigned long *size);
    void waitForChange();
    void unblockModule(unsigned long handle);
    void blockModule(unsigned long handle);

    FloorModuleList *mModuleList; // 0x00: modules owned by the manager.
    FloorStackList *mStack; // 0x04: stack of the floors that were pushed.
};

// The open floor manager; null before GSfloorInit ran.
extern FloorManager *gFloorManager;

// Name of the floor script function that continues the floor once the memory
// card save is done.
extern const char floorSaveCallbackScriptName[];

void _floorFreeOffscreenData();
void _floorStackFreeResource(FloorManager *manager, u32 group, u32 res);
void _floorStackAddResource(FloorManager *manager, u32 group, u32 res);

#else
typedef struct GStexture GStexture;
typedef struct GSfloor_dd_ GSfloor_dd_;
typedef struct FloorRoomRecord FloorRoomRecord;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void floorBlockScriptModule(void);
void floorUnblockScriptModule(void);

u8 floorMoviePlay(u32 movieRes, u32 preloadMovie, u32 loop, u32 modelRes,
                  u16 width, u16 height);
void floorOffscreenPlayWait(void);
GStexture *floorGetOffscreenTexture(void);
u8 floorSetOffscreenIndex(s32 index);
void floorClearOffscreen(void);
void floorUpdateOffscreen(void);
void floorSetOffscreenTable(GSfloor_dd_ *floor);

struct FloorRoomRecord;
void floorSetRoomTable(const FloorRoomRecord *rooms, u32 roomCount);
void floorSetInitialRoom(u32 roomID);
void floorInitialOpenComplete(void);
u32 floorGetCurrentRoomID(void);
u32 floorGetPrevFloorID(void);
u32 floorGetCurrentFloorID(void);
u32 floorGetGroupID(void);
u8 floorIsPushd(void);
u8 floorIsPopd(void);
u8 floorIsPushFlag(void);
void floorChangeFloor(u32 floorID);
void floorPop(void);
void floorPush(u32 floorID);
void floorOpenFloor(u32 floorID);

u8 floorCheckAnywaySaveData(void);
void floorMemoryCardSaveCallback(void);
u8 floorMakeSaveData(u8 **data, unsigned long *size);
void floorStackFreeResource(u32 group, u32 res);
void floorStackAddResource(u32 group, u32 res);

void floorWaitForChange(void);
u8 floorExecScriptFunctionByName(const char *name, u32 argument0, u32 argument1, u32 argument2, u32 argument3);

GSthread *floorCreateThreadLocal(u32 priority, u32 stackSize, u32 flag, GSthreadEntry entry);
GSthread *floorCreateThread(u32 priority, u32 stackSize, u32 flag, GSthreadEntry entry);
void floorUnblockModuleHandle(u32 handle);
void floorBlockModuleHandle(u32 handle);
#ifdef __cplusplus
}
#endif

#endif
