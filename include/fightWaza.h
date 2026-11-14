#ifndef FIGHT_WAZA_H
#define FIGHT_WAZA_H

#include <types.h>
#include <joutai.h>

typedef struct FIGHT_WAZA FIGHT_WAZA;

enum { FIGHT_WAZA_JOUTAI_COUNT = 9 };

struct FIGHT_WAZA {
    u8 mWazaBanme; // 0x00
    u8 _01[1];
    u16 mMotoWazaDataId; // 0x02
    u16 mUseWazaDataId; // 0x04
    u16 mTargetDataId; // 0x06
    _JOUTAI mJoutai[FIGHT_WAZA_JOUTAI_COUNT]; // 0x08
    u8 mCritical; // 0x98
    u8 mDamageValue; // 0x99
    u8 _9A[2];
    s32 mDamage; // 0x9C
    s32 mHitDamage; // 0xA0
    u16 mIryoku; // 0xA4
    u16 mZokusei; // 0xA6
    u8 mKaisuu; // 0xA8
    u8 mAutoMakeFlag; // 0xA9
    u8 _AA[2];
};

#ifdef __cplusplus
extern "C" {
#endif

u8 fightWazaIsHit(FIGHT_WAZA *waza);
u16 fightWaza_GetZokusei(FIGHT_WAZA *waza);
void fightWazaSetUseWazaStatus(FIGHT_WAZA *waza, u16 move);

void fightWazaBiosCopy(FIGHT_WAZA *dst, const FIGHT_WAZA *src);
void fightWazaInit(FIGHT_WAZA *waza);
void fightWazaCreate(FIGHT_WAZA *waza, s8 stockWaza, u16 wazaDataId, u16 targetDataId, u8 autoMakeFlag);
u16 fightWaza_GetIryoku(FIGHT_WAZA *waza);
u8 fightWaza_GetCritical(FIGHT_WAZA *waza);
u8 fightWaza_GetDamageValue(FIGHT_WAZA *waza);
void fightWaza_SetIryoku(FIGHT_WAZA *waza, u16 iryoku);
void fightWaza_SetCritical(FIGHT_WAZA *waza, u32 critical);
void fightWazaInitJoutaiDataId(FIGHT_WAZA *waza, u16 joutaiDataId);
u8 *fightWazaCriticalDataBiosGetPtr(u16 criticalRatio);
u8 fightWazaCriticalDataBiosGetBunbo(u8 *criticalData);
u8 fightWazaIsJoutaiDataId(FIGHT_WAZA *waza, u16 move);

void fightWaza_SetWazaBanme(FIGHT_WAZA *waza, u8 wazaBanme);
void fightWaza_SetMotoWazaDataId(FIGHT_WAZA *waza, u16 motoWazaDataId);
void fightWaza_SetUseWazaDataId(FIGHT_WAZA *waza, u16 useWazaDataId);
void fightWaza_SetTargetDataId(FIGHT_WAZA *waza, u16 targetDataId);
void fightWaza_SetZokusei(FIGHT_WAZA *waza, u16 zokusei);
void fightWaza_SetHitDamage(FIGHT_WAZA *waza, s32 hitDamage);
void fightWaza_SetDamage(FIGHT_WAZA *waza, s32 damage);
void fightWaza_SetDamageValue(FIGHT_WAZA *waza, u8 damageValue);
void fightWaza_SetKaisuu(FIGHT_WAZA *waza, u8 kaisuu);
void fightWaza_SetAutoMakeFlag(FIGHT_WAZA *waza, u8 autoMakeFlag);

u8 fightWaza_GetWazaBanme(FIGHT_WAZA *waza);
u32 fightWaza_GetMotoWazaDataId(FIGHT_WAZA *waza);
u32 fightWaza_GetUseWazaDataId(FIGHT_WAZA *waza);
u16 fightWaza_GetTargetDataId(FIGHT_WAZA *waza);
s32 fightWaza_GetHitDamage(FIGHT_WAZA *waza);
s32 fightWaza_GetDamage(FIGHT_WAZA *waza);
u8 fightWaza_GetKaisuu(FIGHT_WAZA *waza);
u8 fightWaza_GetAutoMakeFlag(FIGHT_WAZA *waza);
_JOUTAI *fightWaza_GetJoutaiPtr(FIGHT_WAZA *waza, u16 index);

void fightWazaBiosSetWazaBanme(FIGHT_WAZA *waza, s8 wazaBanme);
u8 fightWazaBiosGetWazaBanme(FIGHT_WAZA *waza);
void fightWazaBiosSetMotoWazaDataId(FIGHT_WAZA *waza, u16 motoWazaDataId);
u16 fightWazaBiosGetMotoWazaDataId(FIGHT_WAZA *waza);
void fightWazaBiosSetUseWazaDataId(FIGHT_WAZA *waza, u16 useWazaDataId);
u16 fightWazaBiosGetUseWazaDataId(FIGHT_WAZA *waza);
void fightWazaBiosSetTargetDataId(FIGHT_WAZA *waza, u16 targetDataId);
u16 fightWazaBiosGetTargetDataId(FIGHT_WAZA *waza);
_JOUTAI *fightWazaBiosGetJoutaiPtr(FIGHT_WAZA *waza, s32 index);
void fightWazaBiosSetCritical(FIGHT_WAZA *waza, u8 critical);
u8 fightWazaBiosGetCritical(FIGHT_WAZA *waza);
void fightWazaBiosSetDamageValue(FIGHT_WAZA *waza, u8 damageValue);
u8 fightWazaBiosGetDamageValue(FIGHT_WAZA *waza);
void fightWazaBiosSetDamage(FIGHT_WAZA *waza, s32 damage);
s32 fightWazaBiosGetDamage(FIGHT_WAZA *waza);
void fightWazaBiosSetHitDamage(FIGHT_WAZA *waza, s32 hitDamage);
s32 fightWazaBiosGetHitDamage(FIGHT_WAZA *waza);
void fightWazaBiosSetIryoku(FIGHT_WAZA *waza, u16 iryoku);
u16 fightWazaBiosGetIryoku(FIGHT_WAZA *waza);
void fightWazaBiosSetZokusei(FIGHT_WAZA *waza, u16 zokusei);
u16 fightWazaBiosGetZokusei(FIGHT_WAZA *waza);
void fightWazaBiosSetKaisuu(FIGHT_WAZA *waza, u8 kaisuu);
u8 fightWazaBiosGetKaisuu(FIGHT_WAZA *waza);
void fightWazaBiosSetAutoMakeFlag(FIGHT_WAZA *waza, u8 autoMakeFlag);
u8 fightWazaBiosGetAutoMakeFlag(FIGHT_WAZA *waza);
#ifdef __cplusplus
}
#endif

#endif
