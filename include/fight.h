#ifndef FIGHT_H
#define FIGHT_H

#include <types.h>
#include <GSthread.h>

#ifdef __cplusplus
class Hero;
struct FloorModuleInterface;
#endif

#ifdef __cplusplus
extern "C" {
#endif

u8 fightReadResource(unsigned long groupID, unsigned long resourceID);
void fightFreeResource(unsigned long groupID, unsigned long resourceID);
u8 fightIsFight(void);
void fightSetFightResultId(u16 result);
u16 fightGetFightResultId(void);

void fightInit(void);
void fightDataRewrite(void);
void fightFreeDeck(u16 encountNo);
u8 fightReadDeck(u16 encountNo);
void fightCloseAllMenu(void);
void fightSetFightEncountDataId(u16 encountNo);
u16 fightGetFightEncountDataId(void);
void fightSetGsthreadPtrFightMain(GSthread *thread);
void fightSetFightFlag(u8 flag);
void fightMain(void);
#ifdef __cplusplus
void fightSetMenuCtrlHero(Hero *hero);
Hero *fightGetMenuCtrlHero(void);
FloorModuleInterface *fightGetFloorModule(void);
#endif

u8 fightMainAllCamera(u8 simpleCamera, u8 stopCamera);
void fightMainWaitFrame(s32 frameCount);

#ifdef __cplusplus
}
#endif

#endif
