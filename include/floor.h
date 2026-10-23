#ifndef FLOOR_H
#define FLOOR_H

#include <types.h>

enum { FLOOR_ID_WAZA_DEBUG = 999 };

#ifdef __cplusplus
extern "C" {
#endif

void floorBlockMainScript(void);
void floorUnblockMainScript(void);

u8 GSfloorIsPop(void);
u8 GSfloorIsPush(void);
void GSfloorPop(void);
void GSfloorPush(u32 floorID);
void floorWaitForChange(void);
void floorSetFadeScript(s32 a, s32 b);

#ifdef __cplusplus
}
#endif

#endif
