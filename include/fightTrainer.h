#ifndef FIGHT_TRAINER_H
#define FIGHT_TRAINER_H

#include <hero.h>
#include <types.h>

enum { FIGHT_TRAINER_STATUS_TYPE = 0x39 };

typedef struct FIGHT_TRAINER FIGHT_TRAINER;


#ifdef __cplusplus
extern "C" {
#endif

// A zero encounter number uses the current encounter.
void fightTrainerCreateFightTrainerDataIdToHero(u16 trainerDataId, s32 controllerId,
                                               Hero *hero, u16 encountNo);
Hero *fightTrainer_GetDataPtr(FIGHT_TRAINER *trainer);
s32 fightTrainerGetStatus(FIGHT_TRAINER *trainer, u16 trainerDataId,
                          s32 fieldId, s32 subIndex);
void fightTrainerSetStatus(FIGHT_TRAINER *trainer, u16 trainerDataId,
                           s32 fieldId, s32 subIndex, s32 value);
u8 fightTrainerIsSelectedItemBall(FIGHT_TRAINER *trainer);

#ifdef __cplusplus
}
#endif

#endif
