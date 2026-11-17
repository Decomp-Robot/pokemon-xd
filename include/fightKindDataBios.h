#ifndef FIGHTKINDDATABIOS_H
#define FIGHTKINDDATABIOS_H

#include <types.h>

// One entry of the fight kind table: the policy flags that control what a
// fight of that kind allows and the name the fight kind is displayed with.
typedef struct FIGHT_KIND {
    u8 mBackSaveDataFlag;
    u8 mDoBadgeCheckFlag;
    u8 mDoZukanMitaFlag;
    u8 mDoZukanTukamaetaFlag;
    u8 mUseItemFlag;
    u8 mCallFlag;
    u8 mNigeruFlag;
    u8 mDrawFlag;
    u8 mGetExpFlag;
    u8 mGetOkaneFlag;
    u8 mOkanePoolFlag;
    u8 mGetNekoniKobanFlag;
    u8 mGetFriendFlag;
    u8 mGetInfectPokerusFlag;
    u8 mDoCriticalAttackFlag;
    u8 mDoHizukiAiFlag;
    u8 mDoHizukiMiyaburiFlag;
    u8 mDoItemSoubiTokukoutokubouupFlag;
    u8 mKeikentihueruFlag;
    u8 mBossFlag;
    u8 mDorobouFlag;
    u8 mMonohiroiFlag;
    u8 mDarkpokemonHypermodeFlag;
    u8 mPokemonStatusMenuSubbarFlag;
    u8 mHostEnemyMsgFlag;
    u8 mRegulationFlag;
    u32 mName;
} FIGHT_KIND;

#ifdef __cplusplus
extern "C" {
#endif

FIGHT_KIND *fightKindDataBiosGetPtr(u16 kindId);
u32 fightKindDataBiosGetName(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetRegulationFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetHostEnemyMsgFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetPokemonStatusMenuSubbarFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDarkpokemonHypermodeFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetMonohiroiFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDorobouFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetBossFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetKeikentihueruFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDoItemSoubiTokukoutokubouupFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDoHizukiMiyaburiFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDoHizukiAiFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDoCriticalAttackFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetGetInfectPokerusFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetGetFriendFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetGetNekoniKobanFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetGetOkaneFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetOkanePoolFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetGetExpFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDrawFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetNigeruFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetCallFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetUseItemFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDoZukanTukamaetaFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDoZukanMitaFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetDoBadgeCheckFlag(FIGHT_KIND *fightKindData);
u8 fightKindDataBiosGetBackSaveDataFlag(FIGHT_KIND *fightKindData);

extern u32 *fight_kind_data_number;

#ifdef __cplusplus
}
#endif

#endif
