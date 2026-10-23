#ifndef FIGHTSIDE_H
#define FIGHTSIDE_H

#include <fightTrainer.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
u8 fightSideCheckWriteJoutaiDataId(void *side, u32 status);
void fightSideWriteJoutaiDataId(void *side, u32 status, u32 value);

typedef struct FIGHT_SIDE FIGHT_SIDE;

FIGHT_TRAINER *fightSideGetValidFightTrainerPtr(FIGHT_SIDE *side, u16 index);

#ifdef __cplusplus
}
#endif

#endif
