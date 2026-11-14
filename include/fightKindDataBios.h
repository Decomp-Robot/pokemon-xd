#ifndef FIGHTKINDDATABIOS_H
#define FIGHTKINDDATABIOS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *fightKindDataBiosGetPtr(u16 kindId);
u32 fightKindDataBiosGetName(void *data);
u8 fightKindDataBiosGetRegulationFlag(void *fightKindData);
u8 fightKindDataBiosGetHostEnemyMsgFlag(void *fightKindData);
u8 fightKindDataBiosGetPokemonStatusMenuSubbarFlag(void *fightKindData);
u8 fightKindDataBiosGetDarkpokemonHypermodeFlag(void *fightKindData);
u8 fightKindDataBiosGetMonohiroiFlag(void *fightKindData);
u8 fightKindDataBiosGetDorobouFlag(void *fightKindData);
u8 fightKindDataBiosGetBossFlag(void *fightKindData);
u8 fightKindDataBiosGetKeikentihueruFlag(void *fightKindData);
u8 fightKindDataBiosGetDoItemSoubiTokukoutokubouupFlag(void *fightKindData);
u8 fightKindDataBiosGetDoHizukiMiyaburiFlag(void *fightKindData);
u8 fightKindDataBiosGetDoCriticalAttackFlag(void *fightKindData);
u8 fightKindDataBiosGetGetInfectPokerusFlag(void *fightKindData);
u8 fightKindDataBiosGetGetFriendFlag(void *fightKindData);
u8 fightKindDataBiosGetGetNekoniKobanFlag(void *fightKindData);
u8 fightKindDataBiosGetGetOkaneFlag(void *fightKindData);
u8 fightKindDataBiosGetOkanePoolFlag(void *fightKindData);
u8 fightKindDataBiosGetGetExpFlag(void *fightKindData);
u8 fightKindDataBiosGetDrawFlag(void *fightKindData);
u8 fightKindDataBiosGetNigeruFlag(void *fightKindData);
u8 fightKindDataBiosGetUseItemFlag(void *fightKindData);
u8 fightKindDataBiosGetDoZukanTukamaetaFlag(void *fightKindData);
u8 fightKindDataBiosGetDoZukanMitaFlag(void *fightKindData);
u8 fightKindDataBiosGetDoBadgeCheckFlag(void *fightKindData);
u8 fightKindDataBiosGetBackSaveDataFlag(void *fightKindData);

extern u32 *fight_kind_data_number;

#ifdef __cplusplus
}
#endif

#endif
