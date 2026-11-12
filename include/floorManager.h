#ifndef FLOOR_MANAGER_H
#define FLOOR_MANAGER_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
u32 floorGetCurrent(void);
u32 floorGetCurrentFloorGroupID(void);
u8 floorIsFloorPushed(void);
u8 floorIsPop(void);
u32 GSfloorGetCurrent(void);
u32 GSfloorGetCurrentFloorGroupID(void);
u8 GSfloorIsPop(void);
u8 GSfloorIsFloorPushed(void);
u8 GSfloorIsPush(void);
u8 floorIsPush(void);
void floorUnmappingScript(void *data);
void GSfloorPush(u32 floorID);
void GSfloorChange(u32 floorID);
void GSfloorPop(void);
void GSfloorOpen(u32 floorID);
void GSfloorStart(void);
void floorChange(u32 floorID);
void floorPop(void);
void floorPush(u32 floorID);
void floorOpen(u32 floorID);
void floorWaitForChange(void);
#ifdef __cplusplus
}
#endif
#endif
