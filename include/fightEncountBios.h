#ifndef FIGHT_ENCOUNT_BIOS_H
#define FIGHT_ENCOUNT_BIOS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void fightEncountDataBiosSetDiskId(void *data, s32 id);
void fightEncountDataBiosSetFightTrainerDataId(void *data, s32 player, u16 id);

#ifdef __cplusplus
}
#endif

#endif
