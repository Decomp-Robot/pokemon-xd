#ifndef ROOM_DATA_H
#define ROOM_DATA_H

#include <types.h>

typedef struct RoomData RoomData;

#ifdef __cplusplus
extern "C" {
#endif

u32 roomDataGetFsysGidForLanguage(RoomData *room);

#ifdef __cplusplus
}
#endif

#endif
