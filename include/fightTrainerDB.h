#ifndef FIGHT_TRAINER_DB_H
#define FIGHT_TRAINER_DB_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u16 fightTrainerDB_GetName(u32 trainerDataId);
u8 fightTrainerDB_GetKindDataId(u16 trainerDataId);

#ifdef __cplusplus
}
#endif

#endif
