#include <fightWaza.h>

// One entry of the hit probability ratio table: the fraction a move's hit
// probability is multiplied and divided by at that stage.
typedef struct FIGHT_WAZA_HIT_KAKURITU_DATA {
    u8 mKake; // multiplied part
    u8 mWaru; // divided part
} FIGHT_WAZA_HIT_KAKURITU_DATA;

extern "C" {
// The hit probability ratio table and the number of stages it holds.
extern u32 fight_waza_hit_kakuritu_data_number;
extern FIGHT_WAZA_HIT_KAKURITU_DATA fight_waza_hit_kakuritu_data[];

// The critical hit ratio table loaded for the running fight and the number
// of stages it holds.
extern u32 fight_waza_critical_data_number;
extern u8 fight_waza_critical_data[];

u8 fightWazaHitKakurituDataBiosGetWaru(FIGHT_WAZA_HIT_KAKURITU_DATA *hitKakurituData);
u8 fightWazaHitKakurituDataBiosGetKake(FIGHT_WAZA_HIT_KAKURITU_DATA *hitKakurituData);
FIGHT_WAZA_HIT_KAKURITU_DATA *fightWazaHitKakurituDataBiosGetPtr(u16 hitRatio);
}

// Divided part of the hit probability fraction at a stage, 1 for no stage.
u8 fightWazaHitKakurituDataBiosGetWaru(FIGHT_WAZA_HIT_KAKURITU_DATA *hitKakurituData) {
    if (hitKakurituData == 0) {
        return 1;
    }
    return hitKakurituData->mWaru;
}

// Multiplied part of the hit probability fraction at a stage, 1 for no stage.
u8 fightWazaHitKakurituDataBiosGetKake(FIGHT_WAZA_HIT_KAKURITU_DATA *hitKakurituData) {
    if (hitKakurituData == 0) {
        return 1;
    }
    return hitKakurituData->mKake;
}

// Hit probability fraction record of a stage, or null for no stage.
FIGHT_WAZA_HIT_KAKURITU_DATA *fightWazaHitKakurituDataBiosGetPtr(u16 hitRatio) {
    if (hitRatio >= fight_waza_hit_kakuritu_data_number) {
        return 0;
    }
    return &fight_waza_hit_kakuritu_data[hitRatio];
}

// Ratio the critical hit chance is divided by, 0 for no stage.
u8 fightWazaCriticalDataBiosGetBunbo(u8 *criticalData) {
    if (criticalData == 0) {
        return 0;
    }
    return criticalData[0];
}

// Critical ratio record of a stage, or null for no stage.
u8 *fightWazaCriticalDataBiosGetPtr(u16 criticalRatio) {
    if (criticalRatio >= fight_waza_critical_data_number) {
        return 0;
    }
    return &fight_waza_critical_data[criticalRatio];
}

void fightWazaBiosSetAutoMakeFlag(FIGHT_WAZA *waza, u8 autoMakeFlag) {
    if (waza == 0) {
        return;
    }
    waza->mAutoMakeFlag = autoMakeFlag;
}

void fightWazaBiosSetKaisuu(FIGHT_WAZA *waza, u8 kaisuu) {
    if (waza == 0) {
        return;
    }
    waza->mKaisuu = kaisuu;
}

void fightWazaBiosSetZokusei(FIGHT_WAZA *waza, u16 zokusei) {
    if (waza == 0) {
        return;
    }
    waza->mZokusei = zokusei;
}

void fightWazaBiosSetIryoku(FIGHT_WAZA *waza, u16 iryoku) {
    if (waza == 0) {
        return;
    }
    waza->mIryoku = iryoku;
}

void fightWazaBiosSetHitDamage(FIGHT_WAZA *waza, s32 hitDamage) {
    if (waza == 0) {
        return;
    }
    waza->mHitDamage = hitDamage;
}

void fightWazaBiosSetDamage(FIGHT_WAZA *waza, s32 damage) {
    if (waza == 0) {
        return;
    }
    waza->mDamage = damage;
}

void fightWazaBiosSetDamageValue(FIGHT_WAZA *waza, u8 damageValue) {
    if (waza == 0) {
        return;
    }
    waza->mDamageValue = damageValue;
}

void fightWazaBiosSetCritical(FIGHT_WAZA *waza, u8 critical) {
    if (waza == 0) {
        return;
    }
    waza->mCritical = critical;
}

void fightWazaBiosSetTargetDataId(FIGHT_WAZA *waza, u16 targetDataId) {
    if (waza == 0) {
        return;
    }
    waza->mTargetDataId = targetDataId;
}

void fightWazaBiosSetUseWazaDataId(FIGHT_WAZA *waza, u16 useWazaDataId) {
    if (waza == 0) {
        return;
    }
    waza->mUseWazaDataId = useWazaDataId;
}

void fightWazaBiosSetMotoWazaDataId(FIGHT_WAZA *waza, u16 motoWazaDataId) {
    if (waza == 0) {
        return;
    }
    waza->mMotoWazaDataId = motoWazaDataId;
}

void fightWazaBiosSetWazaBanme(FIGHT_WAZA *waza, s8 wazaBanme) {
    if (waza == 0) {
        return;
    }
    waza->mWazaBanme = wazaBanme;
}

u8 fightWazaBiosGetAutoMakeFlag(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mAutoMakeFlag;
}

u8 fightWazaBiosGetKaisuu(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mKaisuu;
}

u16 fightWazaBiosGetZokusei(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mZokusei;
}

u16 fightWazaBiosGetIryoku(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mIryoku;
}

s32 fightWazaBiosGetHitDamage(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mHitDamage;
}

s32 fightWazaBiosGetDamage(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mDamage;
}

u8 fightWazaBiosGetDamageValue(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mDamageValue;
}

u8 fightWazaBiosGetCritical(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mCritical;
}

_JOUTAI *fightWazaBiosGetJoutaiPtr(FIGHT_WAZA *waza, u16 index) {
    if (waza == 0) {
        return 0;
    }
    if (index >= FIGHT_WAZA_JOUTAI_COUNT) {
        return 0;
    }
    return &waza->mJoutai[index];
}

u16 fightWazaBiosGetTargetDataId(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mTargetDataId;
}

u16 fightWazaBiosGetUseWazaDataId(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mUseWazaDataId;
}

u16 fightWazaBiosGetMotoWazaDataId(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mMotoWazaDataId;
}

u8 fightWazaBiosGetWazaBanme(FIGHT_WAZA *waza) {
    if (waza == 0) {
        return 0;
    }
    return waza->mWazaBanme;
}

void fightWazaBiosCopy(FIGHT_WAZA *dst, const FIGHT_WAZA *src) {
    if (dst == 0) {
        return;
    }
    if (src == 0) {
        return;
    }
    *dst = *src;
}
