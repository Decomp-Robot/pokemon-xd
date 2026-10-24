#ifndef FIGHT_WAZA_H
#define FIGHT_WAZA_H

#include <types.h>

typedef struct FIGHT_WAZA FIGHT_WAZA;

struct FIGHT_WAZA {
    u8 _00[0x98];
    u8 mCritical; // 0x98
    u8 mDamageValue; // 0x99
    u8 _9A[2];
    s32 mDamage; // 0x9C
    u8 _A0[4];
    u16 mPower; // 0xA4
    u16 mType; // 0xA6
    u8 _A8[4];
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
void fightWaza_SetCritical(FIGHT_WAZA *waza, u32 critical);
void fightWazaInitJoutaiDataId(FIGHT_WAZA *waza, u16 joutaiDataId);
u8 *fightWazaCriticalDataBiosGetPtr(u16 criticalRatio);
u8 fightWazaCriticalDataBiosGetBunbo(u8 *criticalData);
u8 fightWazaIsJoutaiDataId(FIGHT_WAZA *waza, u16 move);

#ifdef __cplusplus
}
#endif

#endif
