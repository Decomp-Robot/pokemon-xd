#include <fightAction.h>

// Battle-action work area of the fight module.
struct FIGHT_ACTION_WORK : FIGHT_ACTION {
    FIGHT_ACTION_WORK() {}
};

#pragma force_active on
FIGHT_ACTION_WORK fight_action;
#pragma force_active reset
