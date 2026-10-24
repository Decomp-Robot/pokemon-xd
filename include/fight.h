#ifndef FIGHT_H
#define FIGHT_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 fightReadResource(unsigned long groupID, unsigned long resourceID);
void fightFreeResource(unsigned long groupID, unsigned long resourceID);
u8 fightIsFight(void);
void fightSetFightResultId(u16 result);
u16 fightGetFightResultId(void);

u8 fightMainAllCamera(u8 simpleCamera, u8 stopCamera);
void fightMainWaitFrame(s32 frameCount);

#ifdef __cplusplus
}
#endif

#endif
