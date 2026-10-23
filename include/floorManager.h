#ifndef FLOOR_MANAGER_H
#define FLOOR_MANAGER_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
u32 floorGetCurrent(void);
u32 GSfloorGetCurrent(void);
u32 GSfloorGetCurrentFloorGroupID(void);
u8 GSfloorIsPop(void);
void GSfloorPush(u32 floorID);
void GSfloorChange(u32 floorID);
void floorWaitForChange(void);
#ifdef __cplusplus
}
#endif
#endif
