#ifndef ITEM_PARAM_H
#define ITEM_PARAM_H

#include <types.h>

enum { ITEM_PARAM_ORIG_HEADER_SIZE = 6 };

struct ITEM_PARAM_ORIG {
    u8 mPackedData[8];
};

struct ITEM_PARAM {
    u8 mMeromeroFlag : 1;
    u8 mReviveAllFlag : 1;
    u8 mCriticalFlag : 1;
    u8 mAttackUp : 4;
    u8  : 1;
    u8 mDefenceUp : 4;
    u8 mQuickUp : 4;
    u8 mHitUp : 4;
    u8 mSpAttackUp : 4;
    u8 mGuardFlag : 1;
    u8 mLevelUpFlag : 1;
    u8 mSleepFlag : 1;
    u8 mPoisonFlag : 1;
    u8 mBurnFlag : 1;
    u8 mFreezeFlag : 1;
    u8 mParalyzeFlag : 1;
    u8 mConfuseFlag : 1;
    u8 mPPMaxUpFlag : 1;
    u8 mReviveFlag : 1;
    u8 mPPSelectFlag : 1;
    u8 mEvolutionFlag : 1;
    u8 mPPMaxFullFlag : 1;
    u8  : 3;
    s8 mFriend1Up;
    s8 mFriend2Up;
    s8 mFriend3Up;
    u8 mHPEffortUp;
    u8 mAttackEffortUp;
    u8 mHPUp;
    u8 mPPUp;
    u8 mDefenceEffortUp;
    u8 mQuickEffortUp;
    u8 mSpDefenceEffortUp;
    u8 mSpAttackEffortUp;
};

enum ItemRecoverType {
    ITEM_RECOVER_BATTLE_BOOST = 0,
    ITEM_RECOVER_REVIVE_ALL = 1,
    ITEM_RECOVER_LEVEL_UP = 2,
    ITEM_RECOVER_SLEEP = 3,
    ITEM_RECOVER_POISON = 4,
    ITEM_RECOVER_BURN = 5,
    ITEM_RECOVER_FREEZE = 6,
    ITEM_RECOVER_PARALYZE = 7,
    ITEM_RECOVER_CONFUSE = 8,
    ITEM_RECOVER_INFATUATION = 9,
    ITEM_RECOVER_MULTIPLE_STATUS = 10,
    ITEM_RECOVER_HP = 11,
    ITEM_RECOVER_ATTACK_EFFORT = 12,
    ITEM_RECOVER_HP_EFFORT = 13,
    ITEM_RECOVER_SP_ATTACK_EFFORT = 14,
    ITEM_RECOVER_SP_DEFENCE_EFFORT = 15,
    ITEM_RECOVER_QUICK_EFFORT = 16,
    ITEM_RECOVER_DEFENCE_EFFORT = 17,
    ITEM_RECOVER_EVOLUTION = 18,
    ITEM_RECOVER_PP_MAX_UP = 19,
    ITEM_RECOVER_PP_MAX_FULL = 20,
    ITEM_RECOVER_PP = 21,
    ITEM_RECOVER_NONE = 22,
};

extern ITEM_PARAM_ORIG itemParamOrigField;
extern ITEM_PARAM_ORIG itemParamOrigBattle;

#ifdef __cplusplus
extern "C" {
#endif

void itemParamConvertOrigFormat(ITEM_PARAM *dst, const ITEM_PARAM_ORIG *src);
s32 itemParamGetRecoverType(ITEM_PARAM *param);
s8 itemParamGetFriend3Up(ITEM_PARAM *param);
s8 itemParamGetFriend2Up(ITEM_PARAM *param);
s8 itemParamGetFriend1Up(ITEM_PARAM *param);
u8 itemParamGetPPMaxFullFlag(ITEM_PARAM *param);
u8 itemParamGetSpAttackEffortUp(ITEM_PARAM *param);
u8 itemParamGetSpDefenceEffortUp(ITEM_PARAM *param);
u8 itemParamGetQuickEffortUp(ITEM_PARAM *param);
u8 itemParamGetDefenceEffortUp(ITEM_PARAM *param);
u8 itemParamGetEvolutionFlag(ITEM_PARAM *param);
u8 itemParamGetPPUp(ITEM_PARAM *param);
u8 itemParamGetPPSelectFlag(ITEM_PARAM *param);
u8 itemParamGetHPUp(ITEM_PARAM *param);
bool itemParamGetReviveFlag(ITEM_PARAM *param);
u8 itemParamGetAttackEffortUp(ITEM_PARAM *param);
u8 itemParamGetHPEffortUp(ITEM_PARAM *param);
u8 itemParamGetPPMaxUpFlag(ITEM_PARAM *param);
u8 itemParamGetConfuseFlag(ITEM_PARAM *param);
u8 itemParamGetParalyzeFlag(ITEM_PARAM *param);
u8 itemParamGetFreezeFlag(ITEM_PARAM *param);
u8 itemParamGetBurnFlag(ITEM_PARAM *param);
u8 itemParamGetPoisonFlag(ITEM_PARAM *param);
u8 itemParamGetSleepFlag(ITEM_PARAM *param);
u8 itemParamGetLevelUpFlag(ITEM_PARAM *param);
u8 itemParamGetGuardFlag(ITEM_PARAM *param);
u8 itemParamGetSpAttackUp(ITEM_PARAM *param);
u8 itemParamGetHitUp(ITEM_PARAM *param);
u8 itemParamGetQuickUp(ITEM_PARAM *param);
u8 itemParamGetDefenceUp(ITEM_PARAM *param);
u8 itemParamGetAttackUp(ITEM_PARAM *param);
u8 itemParamGetCriticalFlag(ITEM_PARAM *param);
u8 itemParamGetMeromeroFlag(ITEM_PARAM *param);
ITEM_PARAM *itemParamGetPtr(u8 functionalID);

#ifdef __cplusplus
}
#endif

#endif
