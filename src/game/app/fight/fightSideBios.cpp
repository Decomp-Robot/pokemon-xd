#include <fightSide.h>

void fightSideDataBiosSetFightoutPokemonStatusMenuDataId(
    FIGHT_SIDE_DATA *data, u8 index, u32 menuDataId)
{
    if (data == 0) {
        return;
    }
    if (index >= 2) {
        return;
    }
    data->mFightoutPokemonStatusMenuDataId[index] = menuDataId;
}

void fightSideDataBiosSetFightTrainerStatusMenuDataId(
    FIGHT_SIDE_DATA *data, u8 index, u32 menuDataId)
{
    if (data == 0) {
        return;
    }
    if (index >= 2) {
        return;
    }
    data->mFightTrainerStatusMenuDataId[index] = menuDataId;
}

void fightSideDataBiosSetYrot(FIGHT_SIDE_DATA *data, u16 yrot) {
    if (data == 0) {
        return;
    }
    data->mYrot = yrot;
}

u32 fightSideDataBiosGetFightoutPokemonStatusMenuDataId(
    FIGHT_SIDE_DATA *data, u8 index)
{
    if (data == 0) {
        return 0;
    }
    if (index >= 2) {
        return 0;
    }
    return data->mFightoutPokemonStatusMenuDataId[index];
}

u32 fightSideDataBiosGetFightTrainerStatusMenuDataId(
    FIGHT_SIDE_DATA *data, u8 index)
{
    if (data == 0) {
        return 0;
    }
    if (index >= 2) {
        return 0;
    }
    return data->mFightTrainerStatusMenuDataId[index];
}

u16 fightSideDataBiosGetYrot(FIGHT_SIDE_DATA *data) {
    if (data == 0) {
        return 0;
    }
    return data->mYrot;
}

FIGHT_SIDE_DATA *fightSideDataBiosGetPtr(u16 dataId) {
    if (dataId >= *fight_side_data_number) {
        return 0;
    }
    return &fight_side_data[dataId];
}

void fightSideBiosSetMakibisiCheckFlag(FIGHT_SIDE *side, u8 flag) {
    if (side == 0) {
        return;
    }
    side->mMakibisiCheckFlag = flag;
}

void fightSideBiosSetFightSideDataId(FIGHT_SIDE *side, u16 dataId) {
    if (side == 0) {
        return;
    }
    side->mFightSideDataId = dataId;
}

u8 fightSideBiosGetMakibisiCheckFlag(FIGHT_SIDE *side) {
    if (side == 0) {
        return 0;
    }
    return side->mMakibisiCheckFlag;
}

FIGHT_TRAINER *fightSideBiosGetFightTrainerPtr(FIGHT_SIDE *side, u16 index) {
    if (side == 0) {
        return 0;
    }
    if (index >= FIGHT_SIDE_TRAINER_COUNT) {
        return 0;
    }
    return &side->mTrainerAry[index];
}

_JOUTAI *fightSideBiosGetJoutaiPtr(FIGHT_SIDE *side, u16 index) {
    if (side == 0) {
        return 0;
    }
    if (index >= FIGHT_SIDE_JOUTAI_COUNT) {
        return 0;
    }
    return &side->mJoutaiAry[index];
}

u16 fightSideBiosGetFightSideDataId(FIGHT_SIDE *side) {
    if (side == 0) {
        return 0;
    }
    return side->mFightSideDataId;
}
