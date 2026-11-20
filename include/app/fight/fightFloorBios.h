#ifndef APP_FIGHT_FIGHTFLOORBIOS_H
#define APP_FIGHT_FIGHTFLOORBIOS_H

#include <types.h>

typedef struct FIGHT_FLOOR FIGHT_FLOOR;

#ifdef __cplusplus
extern "C" {
#endif

// Record of the fight floor that is running.
FIGHT_FLOOR *fightFloorBiosGetFightFloorPtr(void);

#ifdef __cplusplus
}
#endif

#endif
