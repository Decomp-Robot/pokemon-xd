#include <fightKindDataBios.h>

// One entry of the fight kind table: the policy flags that control what a
// fight of that kind allows and the name the fight kind is displayed with.
typedef struct FIGHT_KIND_DATA {
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
} FIGHT_KIND_DATA;

extern "C" {
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
u8 fightKindDataBiosGetDoHizukiAiFlag(void *fightKindData);
u8 fightKindDataBiosGetDoCriticalAttackFlag(void *fightKindData);
u8 fightKindDataBiosGetGetInfectPokerusFlag(void *fightKindData);
u8 fightKindDataBiosGetGetFriendFlag(void *fightKindData);
u8 fightKindDataBiosGetGetNekoniKobanFlag(void *fightKindData);
u8 fightKindDataBiosGetGetOkaneFlag(void *fightKindData);
u8 fightKindDataBiosGetOkanePoolFlag(void *fightKindData);
u8 fightKindDataBiosGetGetExpFlag(void *fightKindData);
u8 fightKindDataBiosGetDrawFlag(void *fightKindData);
u8 fightKindDataBiosGetNigeruFlag(void *fightKindData);
u8 fightKindDataBiosGetCallFlag(void *fightKindData);
u8 fightKindDataBiosGetUseItemFlag(void *fightKindData);
u8 fightKindDataBiosGetDoZukanTukamaetaFlag(void *fightKindData);
u8 fightKindDataBiosGetDoZukanMitaFlag(void *fightKindData);
u8 fightKindDataBiosGetDoBadgeCheckFlag(void *fightKindData);
u8 fightKindDataBiosGetBackSaveDataFlag(void *fightKindData);

extern FIGHT_KIND_DATA *fight_kind_data;
}

u8 fightKindDataBiosGetRegulationFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mRegulationFlag != 0;
}

u32 fightKindDataBiosGetName(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mName;
}

u8 fightKindDataBiosGetHostEnemyMsgFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mHostEnemyMsgFlag != 0;
}

u8 fightKindDataBiosGetPokemonStatusMenuSubbarFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mPokemonStatusMenuSubbarFlag != 0;
}

u8 fightKindDataBiosGetDarkpokemonHypermodeFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDarkpokemonHypermodeFlag != 0;
}

u8 fightKindDataBiosGetMonohiroiFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mMonohiroiFlag != 0;
}

u8 fightKindDataBiosGetDorobouFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDorobouFlag != 0;
}

u8 fightKindDataBiosGetBossFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mBossFlag != 0;
}

u8 fightKindDataBiosGetKeikentihueruFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mKeikentihueruFlag != 0;
}

u8 fightKindDataBiosGetDoItemSoubiTokukoutokubouupFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoItemSoubiTokukoutokubouupFlag != 0;
}

u8 fightKindDataBiosGetDoHizukiMiyaburiFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoHizukiMiyaburiFlag != 0;
}

u8 fightKindDataBiosGetDoHizukiAiFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoHizukiAiFlag != 0;
}

u8 fightKindDataBiosGetDoCriticalAttackFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoCriticalAttackFlag != 0;
}

u8 fightKindDataBiosGetGetInfectPokerusFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetInfectPokerusFlag != 0;
}

u8 fightKindDataBiosGetGetFriendFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetFriendFlag != 0;
}

u8 fightKindDataBiosGetGetNekoniKobanFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetNekoniKobanFlag != 0;
}

u8 fightKindDataBiosGetGetOkaneFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetOkaneFlag != 0;
}

u8 fightKindDataBiosGetOkanePoolFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mOkanePoolFlag != 0;
}

u8 fightKindDataBiosGetGetExpFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetExpFlag != 0;
}

u8 fightKindDataBiosGetDrawFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDrawFlag != 0;
}

u8 fightKindDataBiosGetNigeruFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mNigeruFlag != 0;
}

u8 fightKindDataBiosGetCallFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mCallFlag != 0;
}

u8 fightKindDataBiosGetUseItemFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mUseItemFlag != 0;
}

u8 fightKindDataBiosGetDoZukanTukamaetaFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoZukanTukamaetaFlag != 0;
}

u8 fightKindDataBiosGetDoZukanMitaFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoZukanMitaFlag != 0;
}

u8 fightKindDataBiosGetDoBadgeCheckFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoBadgeCheckFlag != 0;
}

void *fightKindDataBiosGetPtr(u16 kindId) {
    if (kindId > *fight_kind_data_number) {
        return 0;
    }
    return &fight_kind_data[(u16)kindId];
}

u8 fightKindDataBiosGetBackSaveDataFlag(void *fightKindData) {
    FIGHT_KIND_DATA *fightKindDataRecord = static_cast<FIGHT_KIND_DATA *>(fightKindData);

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mBackSaveDataFlag != 0;
}
