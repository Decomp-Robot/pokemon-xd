#ifndef FIGHT_RESULT_H
#define FIGHT_RESULT_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
unsigned short fightGetFightResultId(void);
#ifdef __cplusplus
}
#endif

enum FightResult {
    kFightResultHostVictory = 2,
    kFightResultOtherSideVictory = 3
};

#endif
