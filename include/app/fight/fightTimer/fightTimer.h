#ifndef APP_FIGHT_FIGHTTIMER_FIGHTTIMER_H
#define APP_FIGHT_FIGHTTIMER_FIGHTTIMER_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Remaining command time limit in seconds.
f32 fightTimerCommandGetNokoriTime(void);

// Whether the fight timer that counts down the command time limit of the
// running rule ran out.
u8 fightTimerCommandIsOver(void);

// Remaining battle timer time in seconds.
f32 fightTimerAllGetNokoriTime(void);

#ifdef __cplusplus
}
#endif

#endif
