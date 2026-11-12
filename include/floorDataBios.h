#ifndef FLOOR_DATA_BIOS_H
#define FLOOR_DATA_BIOS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct GSfloor_dd_;
u32 roomDataGetFsysGidForLanguage(struct GSfloor_dd_ *data);
struct GSfloor_dd_ *floorDataBiosGetPtr(s32 floorID);

#ifdef __cplusplus
}
#endif

#endif
