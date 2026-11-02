#include <fightFloor.h>

extern "C" {

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
