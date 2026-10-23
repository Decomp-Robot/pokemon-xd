#ifndef TOOLENTRY_TAISEN_H
#define TOOLENTRY_TAISEN_H

#include <types.h>

enum TaisenBattleType {
    TAISEN_BATTLE_DOUBLE = 0,
    TAISEN_BATTLE_SINGLE = 1,
    TAISEN_BATTLE_MULTI = 2
};

#ifdef __cplusplus
extern "C" {
#endif

s32 toolentryTaisenGetBattleType(void);

#ifdef __cplusplus
}
#endif

#endif
