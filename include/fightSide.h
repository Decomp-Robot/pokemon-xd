#ifndef FIGHTSIDE_H
#define FIGHTSIDE_H

#include <fightTrainer.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
u8 fightSideCheckWriteJoutaiDataId(void *side, u16 status);
void fightSideWriteJoutaiDataId(void *side, u16 status, u16 targetId);

typedef struct FIGHT_SIDE FIGHT_SIDE;

FIGHT_TRAINER *fightSideGetValidFightTrainerPtr(FIGHT_SIDE *side, u16 index);

void fightSideInitJoutaiDataId(FIGHT_SIDE *side, u16 id);

#ifdef __cplusplus
}
#endif

#endif
