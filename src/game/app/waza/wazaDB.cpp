#include <types.h>
#include <fightWaza.h>
#include <waza.h>
#include <wazaDB.h>
#include <zokusei.h>

extern "C" {

// Setters of the fields of a fight move. A missing move leaves the call
// without effect.

void fightWaza_SetAutoMakeFlag(FIGHT_WAZA *waza, u8 autoMakeFlag)
{
    if (waza == 0) {
        return;
    }
    waza->mAutoMakeFlag = autoMakeFlag;
}

void fightWaza_SetKaisuu(FIGHT_WAZA *waza, u8 kaisuu)
{
    if (waza == 0) {
        return;
    }
    waza->mKaisuu = kaisuu;
}

void fightWaza_SetZokusei(FIGHT_WAZA *waza, u16 zokusei)
{
    if (waza == 0) {
        return;
    }
    waza->mZokusei = zokusei;
}

void fightWaza_SetIryoku(FIGHT_WAZA *waza, u16 iryoku)
{
    if (waza == 0) {
        return;
    }
    waza->mIryoku = iryoku;
}

void fightWaza_SetHitDamage(FIGHT_WAZA *waza, s32 hitDamage)
{
    if (waza == 0) {
        return;
    }
    waza->mHitDamage = hitDamage;
}

void fightWaza_SetDamage(FIGHT_WAZA *waza, s32 damage)
{
    if (waza == 0) {
        return;
    }
    waza->mDamage = damage;
}

void fightWaza_SetDamageValue(FIGHT_WAZA *waza, u8 damageValue)
{
    if (waza == 0) {
        return;
    }
    waza->mDamageValue = damageValue;
}

void fightWaza_SetCritical(FIGHT_WAZA *waza, u32 critical)
{
    if (waza == 0) {
        return;
    }
    waza->mCritical = critical;
}

void fightWaza_SetTargetDataId(FIGHT_WAZA *waza, u16 targetDataId)
{
    if (waza == 0) {
        return;
    }
    waza->mTargetDataId = targetDataId;
}

void fightWaza_SetUseWazaDataId(FIGHT_WAZA *waza, u16 useWazaDataId)
{
    if (waza == 0) {
        return;
    }
    waza->mUseWazaDataId = useWazaDataId;
}

void fightWaza_SetMotoWazaDataId(FIGHT_WAZA *waza, u16 motoWazaDataId)
{
    if (waza == 0) {
        return;
    }
    waza->mMotoWazaDataId = motoWazaDataId;
}

void fightWaza_SetWazaBanme(FIGHT_WAZA *waza, u8 wazaBanme)
{
    if (waza == 0) {
        return;
    }
    waza->mWazaBanme = wazaBanme;
}

// Getters of the same fields. A missing move reads as zero.

u8 fightWaza_GetAutoMakeFlag(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mAutoMakeFlag;
}

u8 fightWaza_GetKaisuu(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mKaisuu;
}

u16 fightWaza_GetZokusei(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mZokusei;
}

u16 fightWaza_GetIryoku(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mIryoku;
}

s32 fightWaza_GetHitDamage(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mHitDamage;
}

s32 fightWaza_GetDamage(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mDamage;
}

u8 fightWaza_GetDamageValue(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mDamageValue;
}

u8 fightWaza_GetCritical(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mCritical;
}

// One of the nine status slots the move can leave on its target.
_JOUTAI *fightWaza_GetJoutaiPtr(FIGHT_WAZA *waza, u16 index)
{
    if (waza == 0) {
        return 0;
    }
    if (index >= FIGHT_WAZA_JOUTAI_COUNT) {
        return 0;
    }
    return &waza->mJoutai[index];
}

u16 fightWaza_GetTargetDataId(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mTargetDataId;
}

u32 fightWaza_GetUseWazaDataId(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mUseWazaDataId;
}

u32 fightWaza_GetMotoWazaDataId(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mMotoWazaDataId;
}

u8 fightWaza_GetWazaBanme(FIGHT_WAZA *waza)
{
    if (waza == 0) {
        return 0;
    }
    return waza->mWazaBanme;
}

// Handler lookups. The fight and the trainer AI run a move through the
// function record the move points at; a move or function id the database
// cannot resolve hands back a null handler.

void *wazaDB_GetFightWazaWzxVariationFuncPtr(u16 move)
{
    struct WazaData *data;
    struct WazaFuncData *funcData;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    funcData = wazaFuncDataBiosGetPtr(wazaDataBiosGetWazaFuncDataId(data));
    if (funcData == 0) {
        return 0;
    }
    return funcData->mFightWazaWzxVariationFunc;
}

void *wazaDB_GetFightWazaWzxTypeFuncPtr(u16 move)
{
    struct WazaData *data;
    struct WazaFuncData *funcData;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    funcData = wazaFuncDataBiosGetPtr(wazaDataBiosGetWazaFuncDataId(data));
    if (funcData == 0) {
        return 0;
    }
    return funcData->mFightWazaWzxTypeFunc;
}

void *wazaDB_GetFightTrainerAiWazaDamageFightPokemonFuncPtr(u16 move)
{
    struct WazaData *data;
    struct WazaFuncData *funcData;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    funcData = wazaFuncDataBiosGetPtr(wazaDataBiosGetWazaFuncDataId(data));
    if (funcData == 0) {
        return 0;
    }
    return funcData->mFightTrainerAiWazaDamageFightPokemonFunc;
}

void *wazaDB_GetFightTrainerAiWazaDamageFuncPtr(u16 move)
{
    struct WazaData *data;
    struct WazaFuncData *funcData;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    funcData = wazaFuncDataBiosGetPtr(wazaDataBiosGetWazaFuncDataId(data));
    if (funcData == 0) {
        return 0;
    }
    return funcData->mFightTrainerAiWazaDamageFunc;
}

void *wazaDB_GetFightTrainerAiWazaHitFuncPtr(u16 move)
{
    struct WazaData *data;
    struct WazaFuncData *funcData;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    funcData = wazaFuncDataBiosGetPtr(wazaDataBiosGetWazaFuncDataId(data));
    if (funcData == 0) {
        return 0;
    }
    return funcData->mFightTrainerAiWazaHitFunc;
}

void *wazaDB_GetFightTrainerAiWazaValueFuncPtr(u16 move)
{
    struct WazaData *data;
    struct WazaFuncData *funcData;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    funcData = wazaFuncDataBiosGetPtr(wazaDataBiosGetWazaFuncDataId(data));
    if (funcData == 0) {
        return 0;
    }
    return funcData->mFightTrainerAiWazaValueFunc;
}

// Field queries of a move. A move the database cannot resolve reads as zero.

u8 wazaDB_GetCompatible(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mCompatible;
}

u32 wazaDB_GetWazawzxdataId(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mWazawzxdataId;
}

u8 wazaDB_GetRiskFlag(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mRiskFlag;
}

// Type ids of a move, one per slot of its type list.
u8 wazaDB_GetTypeId(u16 move, u16 index)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    if (index >= WAZA_DATA_TYPE_ID_COUNT) {
        return 0;
    }
    return data->mTypeId[index];
}

bool wazaDB_GetHidenFlag(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mHidenFlag;
}

u8 wazaDB_GetBouon(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mBouon;
}

u8 wazaDB_GetOujanosirusi(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mOujanosirusi;
}

u8 wazaDB_GetOumugaesi(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mOumugaesi;
}

u8 wazaDB_GetYokodori(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mYokodori;
}

u8 wazaDB_GetMajikku(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mMajikku;
}

u8 wazaDB_GetMamoru(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mMamoru;
}

u8 wazaDB_GetDageki(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mDageki;
}

u8 wazaDB_GetAddFightKoukaAvg(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mAddFightKoukaAvg;
}

u16 wazaDB_GetSeqId(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mSeqId;
}

s16 wazaDB_GetIryoku(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mIryoku;
}

u8 wazaDB_GetAvg(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mAvg;
}

u8 wazaDB_GetRangeId(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mRangeId;
}

// Priority of the move, negative for moves that go last.
s32 wazaDB_GetPri(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return (s8)data->mPri;
}

// Type kind the move counts as in a fight. Shadow (dark) moves use the kind
// stored in their own record, every other move the kind stored for its type.
u8 waza_GetZokuseiKindData(u16 move)
{
    struct WazaData *data;
    struct WazaData *darkData;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    if (wazaCheckDarkWaza(move) == 1) {
        darkData = wazaDataBiosGetPtr(move);
        if (darkData == 0) {
            return 0;
        }
        return darkData->mZokuseiKindData;
    }
    return zokuseiBiosGetWazaKind(data->mZokuseiDataId);
}

u8 wazaDB_GetZokuseiDataId(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mZokuseiDataId;
}

u8 wazaDB_GetPp(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mPp;
}

s32 wazaDB_GetName(u16 move)
{
    struct WazaData *data;

    data = wazaDataBiosGetPtr(move);
    if (data == 0) {
        return 0;
    }
    return data->mNameID;
}

}
