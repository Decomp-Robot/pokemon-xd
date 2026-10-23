#ifndef FIGHT_ENCOUNT_DATA_H
#define FIGHT_ENCOUNT_DATA_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 *fightEncountDataBiosGetPtr(u16 encountNo);
u16 fightEncountDataBiosGetFightTrainerDataId(void *data, s32 player);

#ifdef __cplusplus
}
#endif

#endif
