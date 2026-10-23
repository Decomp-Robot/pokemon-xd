#ifndef FIGHTKINDDATABIOS_H
#define FIGHTKINDDATABIOS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *fightKindDataBiosGetPtr(u16 kindId);
u32 fightKindDataBiosGetName(void *data);

extern u32 *fight_kind_data_number;

#ifdef __cplusplus
}
#endif

#endif
