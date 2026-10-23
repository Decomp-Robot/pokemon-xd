#ifndef FLOOR_DATA_BIOS_H
#define FLOOR_DATA_BIOS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RoomData;
u32 roomDataGetFsysGidForLanguage(struct RoomData *data);
struct RoomData *floorDataBiosGetPtr(s32 floorID);

#ifdef __cplusplus
}
#endif

#endif
