#ifndef FLOOR_DATA_BIOS_H
#define FLOOR_DATA_BIOS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct GSfloor_dd_;
struct FloorFieldCameraList;
u32 roomDataGetFsysGidForLanguage(struct GSfloor_dd_ *data);
struct GSfloor_dd_ *floorDataBiosGetPtr(s32 floorID);
struct FloorFieldCameraList *floorDataBiosGetFieldCameraListPtr(struct GSfloor_dd_ *floor);

#ifdef __cplusplus
}
#endif

#endif
