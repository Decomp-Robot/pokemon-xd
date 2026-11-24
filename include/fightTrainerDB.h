#ifndef FIGHT_TRAINER_DB_H
#define FIGHT_TRAINER_DB_H

#include <types.h>
#include <bool.h>

#ifdef __cplusplus
extern "C" {
#endif

u16 fightTrainerDB_GetName(u16 trainerDataId);
u8 fightTrainerDB_GetKindDataId(u16 trainerDataId);
u16 fightTrainerDB_GetAiDataId(u16 trainerDataId);

enum FightAiKeyPlayerFlag {
    FIGHT_AI_KEY_PLAYER_NONE = 0,
    FIGHT_AI_KEY_PLAYER_STANDARD = 1,
    FIGHT_AI_KEY_PLAYER_ACE = 2,
    FIGHT_AI_KEY_PLAYER_BOSS = 3
};

#ifdef __cplusplus
}
#endif

#endif
