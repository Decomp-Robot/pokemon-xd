#ifndef FIGHT_ACTION_H
#define FIGHT_ACTION_H

#include <types.h>

typedef struct FIGHT_ACTION FIGHT_ACTION;

struct FIGHT_ACTION {
#ifdef __cplusplus
    FIGHT_ACTION() {}
#endif
    u8 _00[0x30]; // 0x00-0x2F: unknown.
};

#endif
