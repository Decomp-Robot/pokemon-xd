#ifndef FIGHT_ACTION_H
#define FIGHT_ACTION_H

#include <types.h>

typedef struct FIGHT_ACTION FIGHT_ACTION;
typedef struct FIGHT_ACTION_DATA_dd_ FIGHT_ACTION_DATA_dd_;
typedef struct FIGHT_KIND_DATA FIGHT_KIND_DATA;

typedef u8 (*FIGHT_ACTION_FUNC)(FIGHT_ACTION *action);

struct FIGHT_ACTION {
    u16 mKind;
    u32 mBuff;
    FIGHT_ACTION_DATA_dd_ *mActionData;
    void *mBuffDataPtr;
    u32 mBuffDataId;
    void *mActorFightTargetPtr;
    FIGHT_ACTION_DATA_dd_ *mMotoFightActionDataPtr;
    s32 mFifoBanme;
    void *mDispBuff[4];
};

struct FIGHT_ACTION_DATA_dd_ {
    u16 mKind;
    u32 mBuff;
};

struct FIGHT_KIND_DATA {
    s8 mPri;
    FIGHT_ACTION_FUNC mFlowFunc;
    FIGHT_ACTION_FUNC mDispFunc;
};

enum FightActionKind {
    FIGHT_ACTION_KIND_FOCUS_PUNCH_PREPARE = 12,
    FIGHT_ACTION_KIND_FIGHT_OUT_POKEMON_WAZA = 19
};

enum { FIGHT_ACTION_FIFO_NUM = 32 };

#ifdef __cplusplus
extern "C" {
#endif
u16 fightActionGetKindDataId(FIGHT_ACTION *action);
s8 fightActionGetPri(FIGHT_ACTION *action);
void fightActionDispFifoAll(void);
u8 fightActionCreateAndFlowFifo(FIGHT_ACTION *action, FIGHT_ACTION_DATA_dd_ *motoActionData,
                                void *pokemon, u16 kind, s32 bufferSelector, void *actionData);
u8 fightActionDispFifo(void);
u8 fightActionFlowFifo(FIGHT_ACTION *action);
u8 fightActionDisp(FIGHT_ACTION *action);
u8 fightActionFifoOut(FIGHT_ACTION *out);
FIGHT_ACTION *fightActionFifoIn(FIGHT_ACTION *action);
void fightActionFifoInit(void);
u8 fightActionCheckValid(void *action);
u8 fightActionCreate(FIGHT_ACTION *action, FIGHT_ACTION_DATA_dd_ *motoActionData, void *pokemon,
                     u16 kind, s32 bufferSelector, void *actionData);
void fightActionInit(FIGHT_ACTION *action);
FIGHT_ACTION_DATA_dd_ *fightActionSearchDataPtr(FIGHT_ACTION *action);

void fightActionBiosSetFifoBanme(FIGHT_ACTION *action, s32 banme);
void fightActionBiosSetKind(FIGHT_ACTION *action, u16 kind);
void fightActionBiosSetBuff(FIGHT_ACTION *action, u32 buff);
void fightActionBiosSetFightActionDataPtr(FIGHT_ACTION *action, void *ptr);
void fightActionBiosSetActorFightTargetPtr(FIGHT_ACTION *action, void *ptr);
void fightActionBiosSetBuffDataPtr(FIGHT_ACTION *action, void *arg);
void fightActionBiosSetBuffDataId(FIGHT_ACTION *action, u32 buffDataId);
void fightActionBiosSetMotoFightActionDataPtr(FIGHT_ACTION *action, FIGHT_ACTION_DATA_dd_ *ptr);
void fightActionBiosSetDispBuff(FIGHT_ACTION *action, u16 id, void *ptr);
void fightActionBiosCopy(FIGHT_ACTION *dst, FIGHT_ACTION *src);
u16 fightActionBiosGetKind(FIGHT_ACTION *action);
u32 fightActionBiosGetBuff(FIGHT_ACTION *action);
void *fightActionBiosGetBuffDataPtr(FIGHT_ACTION *action);
FIGHT_ACTION_DATA_dd_ *fightActionBiosGetFightActionDataPtr(FIGHT_ACTION *action);

FIGHT_KIND_DATA *fightActionKindDataBiosGetPtr(u16 kind);
s8 fightActionKindDataBiosGetPri(FIGHT_KIND_DATA *kindData);
FIGHT_ACTION_FUNC fightActionKindDataBiosGetDispFuncPtr(FIGHT_KIND_DATA *kindData);
FIGHT_ACTION_FUNC fightActionKindDataBiosGetFlowFuncPtr(FIGHT_KIND_DATA *kindData);

u16 fightActionDataBiosGetKind(FIGHT_ACTION_DATA_dd_ *data);
u32 fightActionDataBiosGetBuff(FIGHT_ACTION_DATA_dd_ *data);
#ifdef __cplusplus
}
#endif

#endif
