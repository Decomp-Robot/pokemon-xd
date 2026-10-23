#ifndef APP_FIGHT_FIGHTTIMER_FIGHTTIMER_H
#define APP_FIGHT_FIGHTTIMER_FIGHTTIMER_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Whether the fight timer that counts down the command time limit of the
// running rule ran out.
u8 fightTimerCommandIsOver(void);

#ifdef __cplusplus
}
#endif

#endif
