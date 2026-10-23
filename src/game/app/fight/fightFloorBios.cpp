#include <fightFloor.h>

extern "C" void fightFloorBiosSetEncountFloorId(u32 floorId)
{
    encount_floor_id = floorId;
}

extern "C" u32 fightFloorBiosGetEncountFloorId(void)
{
    return encount_floor_id;
}

extern "C" void fightFloorBiosSetItemPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    floor->mItemPokemon = pokemon;
}

extern "C" FightOutPokemon *fightFloorBiosGetItemPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mItemPokemon;
}

extern "C" void fightFloorBiosSetTokuseiPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    floor->mTokuseiPokemon = pokemon;
}

extern "C" FightOutPokemon *fightFloorBiosGetTokuseiPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mTokuseiPokemon;
}

extern "C" void fightFloorBiosSetTuikakoukaPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    floor->mTuikakoukaPokemon = pokemon;
}

extern "C" FightOutPokemon *fightFloorBiosGetTuikakoukaPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mTuikakoukaPokemon;
}

extern "C" void fightFloorBiosSetKizetuPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    floor->mKizetuPokemon = pokemon;
}

extern "C" FightOutPokemon *fightFloorBiosGetKizetuPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mKizetuPokemon;
}

extern "C" void fightFloorBiosSetFirstAttackRnd(FIGHT_FLOOR *floor, u16 random)
{
    if (floor == 0) {
        return;
    }
    floor->mFirstAttackRnd = random;
}

extern "C" u16 fightFloorBiosGetFirstAttackRnd(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mFirstAttackRnd;
}

extern "C" void fightFloorBiosSetFightOutPokemonPtrAryPtr(FIGHT_FLOOR *floor, FightOutPokemon **pokemonAry)
{
    if (floor == 0) {
        return;
    }
    for (u16 slot = 0; slot < FIGHT_OUT_POKEMON_COUNT; slot++) {
        floor->mFightOutPokemonPtrAry[slot] = pokemonAry[slot];
    }
}

extern "C" FightOutPokemon **fightFloorBiosGetFightOutPokemonPtrAryPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mFightOutPokemonPtrAry;
}

extern "C" void fightFloorBiosSetFightOutPokemonPtrAry(FIGHT_FLOOR *floor, u16 slot, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    if (slot >= FIGHT_OUT_POKEMON_COUNT) {
        return;
    }
    floor->mFightOutPokemonPtrAry[slot] = pokemon;
}

extern "C" FightOutPokemon *fightFloorBiosGetFightOutPokemonPtrAry(FIGHT_FLOOR *floor, u16 slot)
{
    if (floor == 0) {
        return 0;
    }
    if (slot >= FIGHT_OUT_POKEMON_COUNT) {
        return 0;
    }
    return floor->mFightOutPokemonPtrAry[slot];
}

extern "C" void fightFloorBiosSetFightPokemonEntryCnt(FIGHT_FLOOR *floor, s16 count)
{
    if (floor == 0) {
        return;
    }
    floor->mFightPokemonEntryCnt = count;
}

extern "C" s16 fightFloorBiosGetFightPokemonEntryCnt(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mFightPokemonEntryCnt;
}

extern "C" void fightFloorDataBiosSetName(FIGHT_FLOOR_DATA *data, u32 name)
{
    if (data == 0) {
        return;
    }
    data->mName = name;
}

extern "C" u32 fightFloorDataBiosGetName(FIGHT_FLOOR_DATA *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mName;
}

extern "C" void fightFloorDataBiosSetEnvSndId(FIGHT_FLOOR_DATA *data, u32 dataId)
{
    if (data == 0) {
        return;
    }
    data->mEnvSndId = dataId;
}

extern "C" void fightFloorDataBiosSetBgmSndId(FIGHT_FLOOR_DATA *data, u32 dataId)
{
    if (data == 0) {
        return;
    }
    data->mBgmSndId = dataId;
}

extern "C" void fightFloorDataBiosSetSyoukaiWzxDataId(FIGHT_FLOOR_DATA *data, u32 dataId)
{
    if (data == 0) {
        return;
    }
    data->mSyoukaiWzxDataId = dataId;
}

extern "C" void fightFloorDataBiosSetTikeiDataId(FIGHT_FLOOR_DATA *data, u8 dataId)
{
    if (data == 0) {
        return;
    }
    data->mTikeiDataId = dataId;
}

extern "C" void fightFloorDataBiosSetFightSideDataId(FIGHT_FLOOR_DATA *data, u8 side, u16 dataId)
{
    if (data == 0) {
        return;
    }
    if (side >= FIGHT_FLOOR_SIDE_COUNT) {
        return;
    }
    data->mFightSideDataId[side] = dataId;
}

extern "C" void fightFloorDataBiosSetFloorDataId(FIGHT_FLOOR_DATA *data, u16 dataId)
{
    if (data == 0) {
        return;
    }
    data->mFloorDataId = dataId;
}

extern "C" u32 fightFloorDataBiosGetEnvSndId(FIGHT_FLOOR_DATA *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mEnvSndId;
}

extern "C" u32 fightFloorDataBiosGetBgmSndId(FIGHT_FLOOR_DATA *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mBgmSndId;
}

extern "C" u32 fightFloorDataBiosGetSyoukaiWzxDataId(FIGHT_FLOOR_DATA *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSyoukaiWzxDataId;
}

extern "C" u8 fightFloorDataBiosGetTikeiDataId(FIGHT_FLOOR_DATA *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mTikeiDataId;
}
