#ifndef FIGHT_ACTION_H
#define FIGHT_ACTION_H

#include <types.h>

typedef struct FIGHT_ACTION FIGHT_ACTION;

struct FIGHT_ACTION {
#ifdef __cplusplus
    FIGHT_ACTION() {}
#endif
    u16 mKind;
    u8 _02[6];
    struct FIGHT_ACTION_DATA_dd_ *mActionData;
    void *mBufferData;
    u8 _10[4];
    void *mActorTarget;
    u8 _18[24];
};

enum FightActionKind {
    FIGHT_ACTION_KIND_FOCUS_PUNCH_PREPARE = 12,
    FIGHT_ACTION_KIND_FIGHT_OUT_POKEMON_WAZA = 19
};

#ifdef __cplusplus
extern "C" {
#endif
u8 fightActionCreate(FIGHT_ACTION *action, struct FIGHT_ACTION_DATA_dd_ *data, void *pokemon,
                     u16 kind, s32 bufferSelector, void *arg);
u8 fightActionFlowFifo(FIGHT_ACTION *action);
u8 fightActionCheckValid(void *action);
void fightActionBiosSetBuffDataPtr(FIGHT_ACTION *action, void *arg);
void *fightActionBiosGetBuffDataPtr(FIGHT_ACTION *action);
struct FIGHT_ACTION_DATA_dd_ *fightActionBiosGetFightActionDataPtr(FIGHT_ACTION *action);
#ifdef __cplusplus
}
#endif

struct FIGHT_ACTION_DATA_dd_ {
    u16 mKind;
    u16 _02;
    u32 mBuff;
};

#endif
