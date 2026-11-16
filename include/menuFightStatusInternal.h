#ifndef MENU_FIGHT_STATUS_INTERNAL_H
#define MENU_FIGHT_STATUS_INTERNAL_H

#include <types.h>

struct MENU_FIGHT_STATUS_ANIM {
    s16 hpStart;
    s16 hpFrames;
    s16 hpElapsed;
    u16 expDone;
    u32 expStart;
    s16 expFrames;
    s16 expElapsed;
};

#endif
