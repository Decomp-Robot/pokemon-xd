#ifndef APP_FLOOR_DATA_BIOS_H
#define APP_FLOOR_DATA_BIOS_H

#include <types.h>
#include <roomData.h>

#ifdef __cplusplus
extern "C" {
#endif

RoomData *floorDataBiosGetPtr(s32 roomID);

#ifdef __cplusplus
}
#endif

#endif
