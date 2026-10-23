#ifndef FIGHT_RESULT_H
#define FIGHT_RESULT_H

#ifdef __cplusplus
extern "C" {
#endif
long fightGetFightResultId(void);
#ifdef __cplusplus
}
#endif

enum FightResult {
    kFightResultHostVictory = 2,
    kFightResultOtherSideVictory = 3
};

#endif
