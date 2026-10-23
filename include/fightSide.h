#ifndef FIGHTSIDE_H
#define FIGHTSIDE_H

#include <fightTrainer.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FIGHT_SIDE FIGHT_SIDE;

FIGHT_TRAINER *fightSideGetValidFightTrainerPtr(FIGHT_SIDE *side, u16 index);

#ifdef __cplusplus
}
#endif

#endif
