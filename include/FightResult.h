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
    kFightResultCommunicationError = 1,
    kFightResultHostVictory = 2,
    kFightResultOtherSideVictory = 3,
    kFightResultLoseWin = 4,
    kFightResultWinLose = 5,
    kFightResultDraw = 6,
    kFightResultOtherDraw = 7
};

#endif
