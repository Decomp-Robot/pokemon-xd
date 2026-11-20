#include <fight.h>
#include <fightEncountBios.h>
#include <fightKindDataBios.h>
#include <app/fight/fightKind.h>

// Reports whether the running encounter uses the requested fight kind.
u8 fightKind_CheckFightKind(u8 kind) {
    FightEncountRecord *encount = fightEncountDataBiosGetPtr(fightGetFightEncountDataId());

    return (u8)fightEncountDataBiosGetFightKind(encount) == kind;
}

extern "C" {
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

extern FIGHT_KIND *fight_kind_data;
}

u8 fightKindDataBiosGetRegulationFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mRegulationFlag != 0;
}

u32 fightKindDataBiosGetName(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mName;
}

u8 fightKindDataBiosGetHostEnemyMsgFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mHostEnemyMsgFlag != 0;
}

u8 fightKindDataBiosGetPokemonStatusMenuSubbarFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mPokemonStatusMenuSubbarFlag != 0;
}

u8 fightKindDataBiosGetDarkpokemonHypermodeFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDarkpokemonHypermodeFlag != 0;
}

u8 fightKindDataBiosGetMonohiroiFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mMonohiroiFlag != 0;
}

u8 fightKindDataBiosGetDorobouFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDorobouFlag != 0;
}

u8 fightKindDataBiosGetBossFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mBossFlag != 0;
}

u8 fightKindDataBiosGetKeikentihueruFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mKeikentihueruFlag != 0;
}

u8 fightKindDataBiosGetDoItemSoubiTokukoutokubouupFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoItemSoubiTokukoutokubouupFlag != 0;
}

u8 fightKindDataBiosGetDoHizukiMiyaburiFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoHizukiMiyaburiFlag != 0;
}

u8 fightKindDataBiosGetDoHizukiAiFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoHizukiAiFlag != 0;
}

u8 fightKindDataBiosGetDoCriticalAttackFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoCriticalAttackFlag != 0;
}

u8 fightKindDataBiosGetGetInfectPokerusFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetInfectPokerusFlag != 0;
}

u8 fightKindDataBiosGetGetFriendFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetFriendFlag != 0;
}

u8 fightKindDataBiosGetGetNekoniKobanFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetNekoniKobanFlag != 0;
}

u8 fightKindDataBiosGetGetOkaneFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetOkaneFlag != 0;
}

u8 fightKindDataBiosGetOkanePoolFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mOkanePoolFlag != 0;
}

u8 fightKindDataBiosGetGetExpFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mGetExpFlag != 0;
}

u8 fightKindDataBiosGetDrawFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDrawFlag != 0;
}

u8 fightKindDataBiosGetNigeruFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mNigeruFlag != 0;
}

u8 fightKindDataBiosGetCallFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mCallFlag != 0;
}

u8 fightKindDataBiosGetUseItemFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mUseItemFlag != 0;
}

u8 fightKindDataBiosGetDoZukanTukamaetaFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoZukanTukamaetaFlag != 0;
}

u8 fightKindDataBiosGetDoZukanMitaFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoZukanMitaFlag != 0;
}

u8 fightKindDataBiosGetDoBadgeCheckFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mDoBadgeCheckFlag != 0;
}

FIGHT_KIND *fightKindDataBiosGetPtr(u16 kindId) {
    if (kindId > *fight_kind_data_number) {
        return 0;
    }
    return &fight_kind_data[(u16)kindId];
}

u8 fightKindDataBiosGetBackSaveDataFlag(FIGHT_KIND *fightKindData) {
    FIGHT_KIND *fightKindDataRecord = fightKindData;

    if (fightKindDataRecord == 0) {
        return 0;
    }
    return fightKindDataRecord->mBackSaveDataFlag != 0;
}
