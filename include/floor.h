#ifndef FLOOR_H
#define FLOOR_H
#include <GSgfxTypes.h>
#include <types.h>
#include <roomData.h>
#ifdef __cplusplus
extern "C" {
#endif
void floorMapSetDisp(long on);
u32 floorReadMakeModelResID(u32 resource);
u32 floorReadMakeLightResID(u32 resource);
void floorSetLightInfo(u32 resource, long count);

GSgfxLayerID floorGetMenuLayerID(void);
s32 floorGetNextFloorID();
s32 floorGetPrevFloorID();
s32 floorGetCurrentGroupID();
void floorLink(s32 floorID, s32 mode);
void floorSetFadeScript(s32 scriptID, s32 fadeID);
RoomData* floorDataBiosGetPtr(s32 floorID);
void GSfloorPop();
#ifdef __cplusplus
}
#endif
#endif
