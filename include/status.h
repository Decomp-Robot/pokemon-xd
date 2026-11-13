#ifndef STATUS_H
#define STATUS_H

#include <types.h>

enum STATUS_KIND {
    STATUS_KIND_NONE = 0,
    STATUS_KIND_GAMEDATA = 1,
    STATUS_KIND_PCBOX = 2,
    STATUS_KIND_HERO = 3,
    STATUS_KIND_ITEM = 4,
    STATUS_KIND_POKEMON = 5,
    STATUS_KIND_WAZA = 6,
    STATUS_KIND_FIGHT_FLOOR = 7,
    STATUS_KIND_FIGHT_SIDE = 8,
    STATUS_KIND_FIGHT_TRAINER = 9,
};

#ifdef __cplusplus
extern "C" {
#endif
s32 statusGetStatus(u8 statusKind, void* status, u16 dataId, u16 fieldId, u16 index);
#ifdef __cplusplus
}
#endif

#endif
