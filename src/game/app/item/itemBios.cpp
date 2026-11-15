#include <itemBios.h>
#include <itemParam.h>
#include <itemDataBios.h>

enum {
    ITEM_POCKET_MACHINE = 4,
    ITEM_TM_COUNT = 50,
    ITEM_NUMBER_INVALID = 255
};

struct ItemWazaMachineNo {
    u8 mHidenFlag;
    u32 mWazaID;
};

extern "C" {
extern ItemBallData item_ball_data[];
extern ItemSoubiData item_soubi_data[];
extern TasteData taste_data[];
extern ItemWazaMachineNo item_waza_machine_no[];
extern ITEM_PARAM item_param[];
extern u32 item_ball_data_number;
extern u32 item_soubi_data_number;
extern u32 taste_data_number;
extern u32 item_waza_machine_no_number;
extern u32 item_param_number;
extern u32 *item_data_index_number;
extern u16 *item_data_index;
extern u32 *item_data_prime_number;
extern ItemData *item_data_prime;
}

enum ItemParamField {
    ITEM_PARAM_MEROMERO = 0,
    ITEM_PARAM_REVIVE_ALL = 1,
    ITEM_PARAM_CRITICAL = 2,
    ITEM_PARAM_ATTACK = 3,
    ITEM_PARAM_DEFENCE = 4,
    ITEM_PARAM_QUICK = 5,
    ITEM_PARAM_HIT = 6,
    ITEM_PARAM_SP_ATTACK = 7,
    ITEM_PARAM_GUARD = 8,
    ITEM_PARAM_LEVEL_UP = 9,
    ITEM_PARAM_SLEEP = 10,
    ITEM_PARAM_POISON = 11,
    ITEM_PARAM_BURN = 12,
    ITEM_PARAM_FREEZE = 13,
    ITEM_PARAM_PARALYZE = 14,
    ITEM_PARAM_CONFUSE = 15,
    ITEM_PARAM_PP_MAX_UP = 16,
    ITEM_PARAM_HP_EFFORT = 17,
    ITEM_PARAM_ATTACK_EFFORT = 18,
    ITEM_PARAM_REVIVE = 19,
    ITEM_PARAM_HP = 20,
    ITEM_PARAM_PP_SELECT = 21,
    ITEM_PARAM_PP = 22,
    ITEM_PARAM_EVOLUTION = 23,
    ITEM_PARAM_DEFENCE_EFFORT = 24,
    ITEM_PARAM_QUICK_EFFORT = 25,
    ITEM_PARAM_SP_DEFENCE_EFFORT = 26,
    ITEM_PARAM_SP_ATTACK_EFFORT = 27,
    ITEM_PARAM_PP_MAX_FULL = 28,
    ITEM_PARAM_FRIEND1 = 29,
    ITEM_PARAM_FRIEND2 = 30,
    ITEM_PARAM_FRIEND3 = 31,
    ITEM_PARAM_FIELD_COUNT = 32,
};

enum ItemParamEncoding {
    ITEM_PARAM_FLAG = 0,
    ITEM_PARAM_BITS = 1,
    ITEM_PARAM_UNSIGNED_BYTE = 2,
    ITEM_PARAM_SIGNED_BYTE = 3
};

struct ItemParamConversion {
    ItemParamField mField;
    s32 mByteOffset;
    u8 mMask;
    ItemParamEncoding mEncoding;
};

static inline u8 itemParamGetReviveAllFlag(ITEM_PARAM *param)
{
    if (param == 0) {
        return 0;
    }
    return param->mReviveAllFlag != 0;
}

ItemData *itemDataBiosGetPtr(u16 itemID)
{
    if (itemID >= *item_data_index_number) {
        return 0;
    }
    u16 index = item_data_index[itemID];
    if (index >= *item_data_prime_number) {
        return 0;
    }
    return &item_data_prime[index];
}

u32 itemDataBiosGetName(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mNameMsgID;
}

u8 itemDataBiosCheckImportable(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mNameMsgID != 0;
}

u8 itemDataBiosCheckExportable(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    if (itemDataBiosGetImportant(data)) {
        return 0;
    }
    return 1;
}

u8 itemDataBiosGetKind(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mBagPocket;
}

u16 itemDataBiosGetPrice(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mPrice;
}

u16 itemDataBiosGetCoupon(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mCoupon;
}

u8 itemDataBiosGetImportant(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mImportant != 0;
}

u8 itemDataBiosGetUseful(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mUsable != 0;
}

u8 itemDataBiosGetDeposit(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mDepositAble != 0;
}

u32 itemDataBiosGetDoc(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mDescriptionMsgID;
}

u16 itemDataBiosGetItemSoubiDataId(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mItemSoubiDataId;
}

u16 itemDataBiosGetFightUseKoukaDataId(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFightUseKoukaDataId;
}

u8 itemDataBiosGetWazaMachineNo(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    if (data->mBagPocket != ITEM_POCKET_MACHINE) {
        return ITEM_NUMBER_INVALID;
    }
    return data->mBuff;
}

u16 itemDataBiosGetWazaIDByWazaMachineNo(u8 machineNo)
{
    if (machineNo >= item_waza_machine_no_number) {
        return 0;
    }
    return item_waza_machine_no[machineNo].mWazaID;
}

u8 itemDataBiosGetHidenMachineNo(ItemData *data)
{
    u8 machineNo = itemDataBiosGetWazaMachineNo(data);
    if (machineNo == ITEM_NUMBER_INVALID) {
        return ITEM_NUMBER_INVALID;
    }
    if (machineNo < ITEM_TM_COUNT || machineNo >= item_waza_machine_no_number) {
        return ITEM_NUMBER_INVALID;
    }
    return machineNo - ITEM_TM_COUNT;
}

u8 itemDataBiosGetKinomiNo(u16 itemID)
{
    if (itemID < ITEM_CHERI_BERRY || itemID > ITEM_ENIGMA_BERRY) {
        return ITEM_NUMBER_INVALID;
    }
    return itemID - ITEM_CHERI_BERRY;
}

s8 itemDataBiosGetUseFriend(ItemData *data, u16 range)
{
    if (data == 0) {
        return 0;
    }
    if (range >= 3) {
        return 0;
    }
    return (s8)data->mUseFriend[range];
}

u32 itemDataBiosGetBuff(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mBuff;
}

u8 itemDataBiosGetItemEffectParam(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFunctionalID;
}

PocketItemUseFunc itemDataBiosGetFieldUseFunc(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFieldUseFunc;
}

PocketItemUseFunc itemDataBiosGetBattleUseFunc(ItemData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mBattleUseFunc;
}

ItemBallData *itemBallDataBiosGetPtr(u16 ballID)
{
    ItemBallData *data = &item_ball_data[ballID];
    if (ballID >= item_ball_data_number) {
        return 0;
    }
    return data;
}

u16 itemBallDataBiosGetFightKoukaDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFightKoukaDataId;
}

u32 itemBallDataBiosGetInWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mInWzxDataId;
}

u32 itemBallDataBiosGetOpenWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mOpenWzxDataId;
}

u32 itemBallDataBiosGetOutWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mOutWzxDataId;
}

u32 itemBallDataBiosGetDowninWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mDowninWzxDataId;
}

u32 itemBallDataBiosGetThrowWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mThrowWzxDataId;
}

u32 itemBallDataBiosGetSnatchAttackWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSnatchAttackWzxDataId;
}

u32 itemBallDataBiosGetSnatchBalllandWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSnatchBalllandWzxDataId;
}

u32 itemBallDataBiosGetSnatchMissWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSnatchMissWzxDataId;
}

u32 itemBallDataBiosGetSnatchPokeoutWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSnatchPokeoutWzxDataId;
}

u32 itemBallDataBiosGetSnatchShakeWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSnatchShakeWzxDataId;
}

u32 itemBallDataBiosGetSnatchSnatchWzxDataId(ItemBallData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSnatchSnatchWzxDataId;
}

u16 itemBiosGetItemDataId(ItemSlot *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mItemID;
}

u16 itemBiosGetNum(ItemSlot *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mNum;
}

void itemDataBiosSetName(ItemData *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mNameMsgID = value;
}

void itemDataBiosSetKind(ItemData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mBagPocket = value;
}

void itemDataBiosSetPrice(ItemData *data, u16 value)
{
    if (data == 0) {
        return;
    }
    data->mPrice = value;
}

void itemDataBiosSetImportant(ItemData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mImportant = value;
}

void itemDataBiosSetUseful(ItemData *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mUsable = value;
}

void itemDataBiosSetDoc(ItemData *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mDescriptionMsgID = value;
}

void itemDataBiosSetItemSoubiDataId(ItemData *data, u16 value)
{
    if (data == 0) {
        return;
    }
    data->mItemSoubiDataId = value;
}

void itemDataBiosSetFightUseKoukaDataId(ItemData *data, u16 value)
{
    if (data == 0) {
        return;
    }
    data->mFightUseKoukaDataId = value;
}

void itemDataBiosSetUseFriend(ItemData *data, u16 range, s8 value)
{
    if (data == 0) {
        return;
    }
    if (range >= 3) {
        return;
    }
    data->mUseFriend[range] = value;
}

void itemDataBiosSetBuff(ItemData *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mBuff = value;
}

void itemBallDataBiosSetFightKoukaDataId(ItemBallData *data, u16 value)
{
    if (data == 0) {
        return;
    }
    data->mFightKoukaDataId = value;
}

void itemBiosSetItemDataId(ItemSlot *data, u16 value)
{
    if (data == 0) {
        return;
    }
    data->mItemID = value;
}

void itemBiosSetNum(ItemSlot *data, u16 value)
{
    if (data == 0) {
        return;
    }
    data->mNum = value;
}

ItemSoubiData *itemSoubiDataBiosGetPtr(u16 soubiID)
{
    ItemSoubiData *data = &item_soubi_data[soubiID];
    if (soubiID >= item_soubi_data_number) {
        return 0;
    }
    return data;
}

u16 itemSoubiDataBiosGetFightKoukaDataId(ItemSoubiData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFightKoukaDataId;
}

TasteData *tasteDataGetPtr(u16 taste)
{
    TasteData *data = &taste_data[taste];
    if (taste >= taste_data_number) {
        return 0;
    }
    return data;
}

s32 tasteDataGetNigateMsgDataId(TasteData *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mNigateMsgDataId;
}

s32 tasteDataGetAisyou(TasteData *data, u16 nature)
{
    if (data == 0) {
        return 0;
    }
    if (nature >= 25) {
        return 0;
    }
    return (s8)data->mAisyou[nature];
}

ITEM_PARAM *itemParamGetPtr(u8 functionalID)
{
    ITEM_PARAM *param = &item_param[functionalID];
    if (functionalID >= item_param_number) {
        return 0;
    }
    return param;
}

u8 itemParamGetMeromeroFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mMeromeroFlag != 0;
}

u8 itemParamGetCriticalFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mCriticalFlag != 0;
}

u8 itemParamGetAttackUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mAttackUp;
}

u8 itemParamGetDefenceUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mDefenceUp;
}

u8 itemParamGetQuickUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mQuickUp;
}

u8 itemParamGetHitUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mHitUp;
}

u8 itemParamGetSpAttackUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSpAttackUp;
}

u8 itemParamGetGuardFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mGuardFlag != 0;
}

u8 itemParamGetLevelUpFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mLevelUpFlag != 0;
}

u8 itemParamGetSleepFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSleepFlag != 0;
}

u8 itemParamGetPoisonFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mPoisonFlag != 0;
}

u8 itemParamGetBurnFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mBurnFlag != 0;
}

u8 itemParamGetFreezeFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFreezeFlag != 0;
}

u8 itemParamGetParalyzeFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mParalyzeFlag != 0;
}

u8 itemParamGetConfuseFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mConfuseFlag != 0;
}

u8 itemParamGetPPMaxUpFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mPPMaxUpFlag != 0;
}

u8 itemParamGetHPEffortUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mHPEffortUp;
}

u8 itemParamGetAttackEffortUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mAttackEffortUp;
}

u8 itemParamGetReviveFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mReviveFlag != 0;
}

u8 itemParamGetHPUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mHPUp;
}

u8 itemParamGetPPSelectFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mPPSelectFlag != 0;
}

u8 itemParamGetPPUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mPPUp;
}

u8 itemParamGetEvolutionFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mEvolutionFlag != 0;
}

u8 itemParamGetDefenceEffortUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mDefenceEffortUp;
}

u8 itemParamGetQuickEffortUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mQuickEffortUp;
}

u8 itemParamGetSpDefenceEffortUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSpDefenceEffortUp;
}

u8 itemParamGetSpAttackEffortUp(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSpAttackEffortUp;
}

u8 itemParamGetPPMaxFullFlag(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mPPMaxFullFlag != 0;
}

s8 itemParamGetFriend1Up(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return (s8)data->mFriend1Up;
}

s8 itemParamGetFriend2Up(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return (s8)data->mFriend2Up;
}

s8 itemParamGetFriend3Up(ITEM_PARAM *data)
{
    if (data == 0) {
        return 0;
    }
    return (s8)data->mFriend3Up;
}

s32 itemParamGetRecoverType(ITEM_PARAM *param)
{
    if (param == 0) {
        return ITEM_RECOVER_NONE;
    }
    if (itemParamGetCriticalFlag(param) || itemParamGetAttackUp(param) ||
        itemParamGetDefenceUp(param) || itemParamGetQuickUp(param) ||
        itemParamGetHitUp(param) || itemParamGetSpAttackUp(param) ||
        itemParamGetGuardFlag(param)) return ITEM_RECOVER_BATTLE_BOOST;
    if (itemParamGetReviveAllFlag(param)) {
        return ITEM_RECOVER_REVIVE_ALL;
    }
    if (itemParamGetLevelUpFlag(param)) {
        return ITEM_RECOVER_LEVEL_UP;
    }
    s32 count = 0;
    ItemRecoverType type;
    if (itemParamGetSleepFlag(param)) { type = ITEM_RECOVER_SLEEP; count++; }
    if (itemParamGetPoisonFlag(param)) { type = ITEM_RECOVER_POISON; count++; }
    if (itemParamGetBurnFlag(param)) { type = ITEM_RECOVER_BURN; count++; }
    if (itemParamGetFreezeFlag(param)) { type = ITEM_RECOVER_FREEZE; count++; }
    if (itemParamGetParalyzeFlag(param)) { type = ITEM_RECOVER_PARALYZE; count++; }
    if (itemParamGetConfuseFlag(param)) { type = ITEM_RECOVER_CONFUSE; count++; }
    if (itemParamGetMeromeroFlag(param)) { type = ITEM_RECOVER_INFATUATION; count++; }
    if (count > 0) {
        return count > 1 ? ITEM_RECOVER_MULTIPLE_STATUS : type;
    }
    if (itemParamGetHPUp(param)) {
        return ITEM_RECOVER_HP;
    }
    if (itemParamGetAttackEffortUp(param)) {
        return ITEM_RECOVER_ATTACK_EFFORT;
    }
    if (itemParamGetHPEffortUp(param)) {
        return ITEM_RECOVER_HP_EFFORT;
    }
    if (itemParamGetSpAttackEffortUp(param)) {
        return ITEM_RECOVER_SP_ATTACK_EFFORT;
    }
    if (itemParamGetSpDefenceEffortUp(param)) {
        return ITEM_RECOVER_SP_DEFENCE_EFFORT;
    }
    if (itemParamGetQuickEffortUp(param)) {
        return ITEM_RECOVER_QUICK_EFFORT;
    }
    if (itemParamGetDefenceEffortUp(param)) {
        return ITEM_RECOVER_DEFENCE_EFFORT;
    }
    if (itemParamGetEvolutionFlag(param)) {
        return ITEM_RECOVER_EVOLUTION;
    }
    if (itemParamGetPPMaxUpFlag(param)) {
        return ITEM_RECOVER_PP_MAX_UP;
    }
    if (itemParamGetPPMaxFullFlag(param)) {
        return ITEM_RECOVER_PP_MAX_FULL;
    }
    if (itemParamGetPPSelectFlag(param) || itemParamGetPPUp(param)) {
        return ITEM_RECOVER_PP;
    }
    return ITEM_RECOVER_NONE;
}

void itemParamConvertOrigFormat(ITEM_PARAM *dst, const ITEM_PARAM_ORIG *src)
{
    ItemParamConversion conversion[ITEM_PARAM_FIELD_COUNT] = {
        { ITEM_PARAM_MEROMERO, 0, 0x80, ITEM_PARAM_FLAG },
        { ITEM_PARAM_REVIVE_ALL, 0, 0x40, ITEM_PARAM_FLAG },
        { ITEM_PARAM_CRITICAL, 0, 0x30, ITEM_PARAM_FLAG },
        { ITEM_PARAM_ATTACK, 0, 0x0F, ITEM_PARAM_BITS },
        { ITEM_PARAM_DEFENCE, 1, 0xF0, ITEM_PARAM_BITS },
        { ITEM_PARAM_QUICK, 1, 0x0F, ITEM_PARAM_BITS },
        { ITEM_PARAM_HIT, 2, 0xF0, ITEM_PARAM_BITS },
        { ITEM_PARAM_SP_ATTACK, 2, 0x0F, ITEM_PARAM_BITS },
        { ITEM_PARAM_GUARD, 3, 0x80, ITEM_PARAM_FLAG },
        { ITEM_PARAM_LEVEL_UP, 3, 0x40, ITEM_PARAM_FLAG },
        { ITEM_PARAM_SLEEP, 3, 0x20, ITEM_PARAM_FLAG },
        { ITEM_PARAM_POISON, 3, 0x10, ITEM_PARAM_FLAG },
        { ITEM_PARAM_BURN, 3, 0x08, ITEM_PARAM_FLAG },
        { ITEM_PARAM_FREEZE, 3, 0x04, ITEM_PARAM_FLAG },
        { ITEM_PARAM_PARALYZE, 3, 0x02, ITEM_PARAM_FLAG },
        { ITEM_PARAM_CONFUSE, 3, 0x01, ITEM_PARAM_FLAG },
        { ITEM_PARAM_PP_MAX_UP, 4, 0x20, ITEM_PARAM_FLAG },
        { ITEM_PARAM_HP_EFFORT, 4, 0x01, ITEM_PARAM_UNSIGNED_BYTE },
        { ITEM_PARAM_ATTACK_EFFORT, 4, 0x02, ITEM_PARAM_UNSIGNED_BYTE },
        { ITEM_PARAM_REVIVE, 4, 0x40, ITEM_PARAM_FLAG },
        { ITEM_PARAM_HP, 4, 0x04, ITEM_PARAM_UNSIGNED_BYTE },
        { ITEM_PARAM_PP_SELECT, 4, 0x10, ITEM_PARAM_FLAG },
        { ITEM_PARAM_PP, 4, 0x08, ITEM_PARAM_UNSIGNED_BYTE },
        { ITEM_PARAM_EVOLUTION, 4, 0x80, ITEM_PARAM_FLAG },
        { ITEM_PARAM_DEFENCE_EFFORT, 5, 0x01, ITEM_PARAM_UNSIGNED_BYTE },
        { ITEM_PARAM_QUICK_EFFORT, 5, 0x02, ITEM_PARAM_UNSIGNED_BYTE },
        { ITEM_PARAM_SP_DEFENCE_EFFORT, 5, 0x04, ITEM_PARAM_UNSIGNED_BYTE },
        { ITEM_PARAM_SP_ATTACK_EFFORT, 5, 0x08, ITEM_PARAM_UNSIGNED_BYTE },
        { ITEM_PARAM_PP_MAX_FULL, 5, 0x10, ITEM_PARAM_FLAG },
        { ITEM_PARAM_FRIEND1, 5, 0x20, ITEM_PARAM_SIGNED_BYTE },
        { ITEM_PARAM_FRIEND2, 5, 0x40, ITEM_PARAM_SIGNED_BYTE },
        { ITEM_PARAM_FRIEND3, 5, 0x80, ITEM_PARAM_SIGNED_BYTE },
    };
    s32 i;
    const u8 *packed = src->mPackedData;
    const u8 *payload = packed + ITEM_PARAM_ORIG_HEADER_SIZE;
    u8 value;
    s8 signedValue;
    u8 flag;
    for (i = 0; i < ITEM_PARAM_FIELD_COUNT; i++) {
        s32 bits = packed[conversion[i].mByteOffset];
        if (conversion[i].mEncoding == ITEM_PARAM_SIGNED_BYTE || conversion[i].mEncoding == ITEM_PARAM_UNSIGNED_BYTE) {
            if (bits & conversion[i].mMask) {
                value = *payload++;
                if (conversion[i].mEncoding == ITEM_PARAM_SIGNED_BYTE) signedValue = (s8)value;
            } else {
                if (conversion[i].mEncoding == ITEM_PARAM_SIGNED_BYTE) signedValue = 0;
            }
        } else {
            s32 mask = conversion[i].mMask;
            s32 encoded = bits & mask;
            s32 shift = 0;
            while (shift < 8 && !((mask >> shift) & 1)) {
                shift++;
            }
            value = (u8)encoded >> shift;
            if (conversion[i].mEncoding == ITEM_PARAM_FLAG) {
                if (!value) flag = 0;
                else flag = 1;
            }
        }
        switch (conversion[i].mField) {
        case ITEM_PARAM_MEROMERO: dst->mMeromeroFlag = flag; break;
        case ITEM_PARAM_CRITICAL: dst->mCriticalFlag = flag; break;
        case ITEM_PARAM_ATTACK: dst->mAttackUp = value; break;
        case ITEM_PARAM_DEFENCE: dst->mDefenceUp = value; break;
        case ITEM_PARAM_QUICK: dst->mQuickUp = value; break;
        case ITEM_PARAM_HIT: dst->mHitUp = value; break;
        case ITEM_PARAM_SP_ATTACK: dst->mSpAttackUp = value; break;
        case ITEM_PARAM_GUARD: dst->mGuardFlag = flag; break;
        case ITEM_PARAM_LEVEL_UP: dst->mLevelUpFlag = flag; break;
        case ITEM_PARAM_SLEEP: dst->mSleepFlag = flag; break;
        case ITEM_PARAM_POISON: dst->mPoisonFlag = flag; break;
        case ITEM_PARAM_BURN: dst->mBurnFlag = flag; break;
        case ITEM_PARAM_FREEZE: dst->mFreezeFlag = flag; break;
        case ITEM_PARAM_PARALYZE: dst->mParalyzeFlag = flag; break;
        case ITEM_PARAM_CONFUSE: dst->mConfuseFlag = flag; break;
        case ITEM_PARAM_PP_MAX_UP: dst->mPPMaxUpFlag = flag; break;
        case ITEM_PARAM_HP_EFFORT: dst->mHPEffortUp = value; break;
        case ITEM_PARAM_ATTACK_EFFORT: dst->mAttackEffortUp = value; break;
        case ITEM_PARAM_REVIVE: dst->mReviveFlag = flag; break;
        case ITEM_PARAM_HP: dst->mHPUp = value; break;
        case ITEM_PARAM_PP_SELECT: dst->mPPSelectFlag = flag; break;
        case ITEM_PARAM_PP: dst->mPPUp = value; break;
        case ITEM_PARAM_EVOLUTION: dst->mEvolutionFlag = flag; break;
        case ITEM_PARAM_DEFENCE_EFFORT: dst->mDefenceEffortUp = value; break;
        case ITEM_PARAM_QUICK_EFFORT: dst->mQuickEffortUp = value; break;
        case ITEM_PARAM_SP_DEFENCE_EFFORT: dst->mSpDefenceEffortUp = value; break;
        case ITEM_PARAM_SP_ATTACK_EFFORT: dst->mSpAttackEffortUp = value; break;
        case ITEM_PARAM_PP_MAX_FULL: dst->mPPMaxFullFlag = flag; break;
        case ITEM_PARAM_FRIEND1: dst->mFriend1Up = signedValue; break;
        case ITEM_PARAM_FRIEND2: dst->mFriend2Up = signedValue; break;
        case ITEM_PARAM_FRIEND3: dst->mFriend3Up = signedValue; break;
        }
    }
}
