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
