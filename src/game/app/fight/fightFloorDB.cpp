#include <fightFloor.h>
#include <fightEncountBios.h>
#include <fightKindDataBios.h>
#include <app/fight/fightType.h>

extern "C" {

u8 fightFloor_GetFightKindRegulationFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetRegulationFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindHostEnemyMsgFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetHostEnemyMsgFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindPokemonStatusMenuSubbarFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetPokemonStatusMenuSubbarFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindDarkpokemonHypermodeFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetDarkpokemonHypermodeFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindMonohiroiFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetMonohiroiFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindDorobouFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetDorobouFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindBossFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetBossFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindKeikentihueruFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetKeikentihueruFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindDoItemSoubiTokukoutokubouupFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetDoItemSoubiTokukoutokubouupFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindDoHizukiMiyaburiFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetDoHizukiMiyaburiFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindDoCriticalAttackFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetDoCriticalAttackFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindGetInfectPokerusFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetGetInfectPokerusFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindGetFriendFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetGetFriendFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindGetNekoniKobanFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetGetNekoniKobanFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindGetOkaneFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetGetOkaneFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindOkanePoolFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetOkanePoolFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindGetExpFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetGetExpFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindDrawFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetDrawFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindNigeruFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetNigeruFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindUseItemFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetUseItemFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindDoZukanTukamaetaFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetDoZukanTukamaetaFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindDoZukanMitaFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetDoZukanMitaFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindDoBadgeCheckFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetDoBadgeCheckFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u8 fightFloor_GetFightKindBackSaveDataFlag(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightKindDataBiosGetBackSaveDataFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
}

u16 fightFloor_GetFightSideFightOutPokemonSideMax(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightTypeGetFightSideFightOutPokemonMax(fightFloor_GetFightType(floor));
}

u8 fightFloor_GetFightTypeFightoutPokemonNum(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightTypeDataBiosGetFightoutPokemonNum(fightTypeDataBiosGetPtr(fightFloor_GetFightType(floor)));
}

u8 fightFloor_GetFightTypeEntryPokemonNum(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightTypeDataBiosGetEntryPokemonNum(fightTypeDataBiosGetPtr(fightFloor_GetFightType(floor)));
}

u8 fightFloor_GetFightTypeTrainerNum(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightTypeDataBiosGetTrainerNum(fightTypeDataBiosGetPtr(fightFloor_GetFightType(floor)));
}

u32 fightFloor_GetEnvSndId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightEncountGetEnvSndDataId(fightFloor_GetEncountDataId(floor));
}

u32 fightFloor_GetBgmSndId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightFloorGetBgmSndDataId(floor);
}

u32 fightFloor_GetSyoukaiWzxDataId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightFloorDB_GetSyoukaiWzxDataId(fightFloor_GetFightFloorDataId(floor));
}

u8 fightFloor_GetTikeiDataId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightFloorDB_GetTikeiDataId(fightFloor_GetFightFloorDataId(floor));
}

u16 fightFloor_GetFightFloorDataId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightEncountDataBiosGetFightFloorDataId(fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(floor)));
}

u32 fightFloor_GetTrainer(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightEncountDataBiosGetTrainer(fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(floor)));
}

u8 fightFloor_GetFightType(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightEncountDataBiosGetFightType(fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(floor)));
}

u32 fightFloor_GetFightKind(void *floorData)
{
    FIGHT_FLOOR *floor = (FIGHT_FLOOR *)floorData;
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return fightEncountDataBiosGetFightKind(fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(floor)));
}

u16 fightFloor_GetFirstAttackRnd(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return floor->mFirstAttackRnd;
}

FightOutPokemon **fightFloor_GetFightOutPokemonPtrAryPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return floor->mFightOutPokemonPtrAry;
}

FightOutPokemon *fightFloor_GetFightOutPokemonPtrAry(FIGHT_FLOOR *floor, u16 index)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    if (index >= FIGHT_OUT_POKEMON_COUNT) return 0;
    return floor->mFightOutPokemonPtrAry[index];
}

s16 fightFloor_GetFightPokemonEntryCnt(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return floor->mFightPokemonEntryCnt;
}

u32 fightFloor_GetAppointMsgId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) return 0;
    }
    return floor->mAppointMsgId;
}

void *fightFloor_GetAppointItemPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mAppointItem;
}

FIGHT_SIDE *fightFloor_GetAppointSidePtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mAppointSide;
}

FIGHT_WAZA *fightFloor_GetAppointWazaPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mAppointWaza;
}

FIGHT_TRAINER *fightFloor_GetAppointTrainerPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mAppointTrainer;
}

FightOutPokemon *fightFloor_GetAppointPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mAppointPokemon;
}

FightOutPokemon *fightFloor_GetItemPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mItemPokemon;
}

FightOutPokemon *fightFloor_GetTokuseiPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mTokuseiPokemon;
}

FightOutPokemon *fightFloor_GetTuikakoukaPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mTuikakoukaPokemon;
}

FightOutPokemon *fightFloor_GetKizetuPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mKizetuPokemon;
}

FightOutPokemon *fightFloor_GetIrekaePokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mIrekaePokemon;
}

FightOutPokemon *fightFloor_GetEscapePokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mEscapePokemon;
}

FightOutPokemon *fightFloor_GetDefensePokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mDefensePokemon;
}

FightOutPokemon *fightFloor_GetAttackPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mAttackPokemon;
}

u16 fightFloor_GetEncountDataId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return floor->mEncountDataId;
}

u16 fightFloor_GetTurnCount(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    if (floor == 0) {
        return 0;
    }
    return floor->mTurnCount;
}

u8 fightFloor_GetIsJoutaiDataId(FIGHT_FLOOR *floor, u16 joutaiDataId)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    return fightFloorIsJoutaiDataId(floor, joutaiDataId);
}

_JOUTAI *fightFloor_GetJoutaiPtr(FIGHT_FLOOR *floor, u16 index)
{
    if (floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    if (index >= FIGHT_FLOOR_JOUTAI_COUNT) {
        return 0;
    }
    return &floor->mJoutaiAry[index];
}

u32 fightFloorDB_GetEnvSndId(u16 floorDataId)
{
    FIGHT_FLOOR_DATA *floorData = fightFloorDataBiosGetPtr(floorDataId);
    if (floorData == 0) {
        return 0;
    }
    return floorData->mEnvSndId;
}

u32 fightFloorDB_GetBgmSndId(u16 floorDataId)
{
    FIGHT_FLOOR_DATA *floorData = fightFloorDataBiosGetPtr(floorDataId);
    if (floorData == 0) {
        return 0;
    }
    return floorData->mBgmSndId;
}

u32 fightFloorDB_GetSyoukaiWzxDataId(u16 floorDataId)
{
    FIGHT_FLOOR_DATA *floorData = fightFloorDataBiosGetPtr(floorDataId);
    if (floorData == 0) {
        return 0;
    }
    return floorData->mSyoukaiWzxDataId;
}

u8 fightFloorDB_GetTikeiDataId(u16 floorDataId)
{
    FIGHT_FLOOR_DATA *floorData = fightFloorDataBiosGetPtr(floorDataId);
    if (floorData == 0) {
        return 0;
    }
    return floorData->mTikeiDataId;
}

u16 fightFloorDB_GetFightSideDataId(u16 floorDataId, u8 side)
{
    FIGHT_FLOOR_DATA *floorData = fightFloorDataBiosGetPtr(floorDataId);
    if (floorData == 0) {
        return 0;
    }
    if (side >= FIGHT_FLOOR_SIDE_COUNT) {
        return 0;
    }
    return floorData->mFightSideDataId[side];
}

u16 fightFloorDB_GetFloorDataId(u16 floorDataId)
{
    FIGHT_FLOOR_DATA *floorData = fightFloorDataBiosGetPtr(floorDataId);
    if (floorData == 0) {
        return 0;
    }
    return floorData->mFloorDataId;
}

}
