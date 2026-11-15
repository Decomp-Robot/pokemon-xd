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
