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

extern "C" u16 fightFloorDataBiosGetFightSideDataId(FIGHT_FLOOR_DATA *data, u8 side)
{
    if (data == 0) {
        return 0;
    }
    if (side >= FIGHT_FLOOR_SIDE_COUNT) {
        return 0;
    }
    return data->mFightSideDataId[side];
}

extern "C" u16 fightFloorDataBiosGetFloorDataId(FIGHT_FLOOR_DATA *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFloorDataId;
}

extern "C" FIGHT_FLOOR_DATA *fightFloorDataBiosGetPtr(u16 index)
{
    if (index >= *fight_floor_data_number) {
        return 0;
    }
    return &fight_floor_data[index];
}

extern "C" void fightFloorBiosSetAppointTokuseiDataId(FIGHT_FLOOR *floor, u16 dataId)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointTokuseiDataId = dataId;
}

extern "C" void fightFloorBiosSetAppointItemDataId(FIGHT_FLOOR *floor, u16 dataId)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointItemDataId = dataId;
}

extern "C" void fightFloorBiosSetAppointWazaDataId(FIGHT_FLOOR *floor, u16 dataId)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointWazaDataId = dataId;
}

extern "C" void fightFloorBiosSetAppointPokemonDataId(FIGHT_FLOOR *floor, u16 dataId)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointPokemonDataId = dataId;
}

extern "C" void fightFloorBiosSetWazakoukaMsgId(FIGHT_FLOOR *floor, u32 msgId)
{
    if (floor == 0) {
        return;
    }
    floor->mWazakoukaMsgId = msgId;
}

extern "C" void fightFloorBiosSetCriticalMsgId(FIGHT_FLOOR *floor, u32 msgId)
{
    if (floor == 0) {
        return;
    }
    floor->mCriticalMsgId = msgId;
}

extern "C" void fightFloorBiosSetAttackMsgId(FIGHT_FLOOR *floor, u32 msgId)
{
    if (floor == 0) {
        return;
    }
    floor->mAttackMsgId = msgId;
}

extern "C" void fightFloorBiosSetAppointMsgId(FIGHT_FLOOR *floor, u32 msgId)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointMsgId = msgId;
}

extern "C" void fightFloorBiosSetAppointItemPtr(FIGHT_FLOOR *floor, void *item)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointItem = item;
}

extern "C" void fightFloorBiosSetAppointSidePtr(FIGHT_FLOOR *floor, FIGHT_SIDE *side)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointSide = side;
}

extern "C" void fightFloorBiosSetAppointWazaPtr(FIGHT_FLOOR *floor, FIGHT_WAZA *waza)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointWaza = waza;
}

extern "C" void fightFloorBiosSetAppointTrainerPtr(FIGHT_FLOOR *floor, FIGHT_TRAINER *trainer)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointTrainer = trainer;
}

extern "C" void fightFloorBiosSetAppointPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    floor->mAppointPokemon = pokemon;
}

extern "C" void fightFloorBiosSetIrekaePokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    floor->mIrekaePokemon = pokemon;
}

extern "C" void fightFloorBiosSetEscapePokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    floor->mEscapePokemon = pokemon;
}

extern "C" void fightFloorBiosSetDefensePokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    floor->mDefensePokemon = pokemon;
}

extern "C" void fightFloorBiosSetAttackPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon)
{
    if (floor == 0) {
        return;
    }
    floor->mAttackPokemon = pokemon;
}

extern "C" void fightFloorBiosSetEncountDataId(FIGHT_FLOOR *floor, u16 dataId)
{
    if (floor == 0) {
        return;
    }
    floor->mEncountDataId = dataId;
}

extern "C" void fightFloorBiosSetTurnCount(FIGHT_FLOOR *floor, u16 count)
{
    if (floor == 0) {
        return;
    }
    floor->mTurnCount = count;
}

extern "C" u16 fightFloorBiosGetAppointTokuseiDataId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointTokuseiDataId;
}

extern "C" u16 fightFloorBiosGetAppointItemDataId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointItemDataId;
}

extern "C" u16 fightFloorBiosGetAppointWazaDataId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointWazaDataId;
}

extern "C" u16 fightFloorBiosGetAppointPokemonDataId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointPokemonDataId;
}

extern "C" u32 fightFloorBiosGetWazakoukaMsgId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mWazakoukaMsgId;
}

extern "C" u32 fightFloorBiosGetCriticalMsgId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mCriticalMsgId;
}

extern "C" u32 fightFloorBiosGetAttackMsgId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAttackMsgId;
}

extern "C" u32 fightFloorBiosGetAppointMsgId(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointMsgId;
}

extern "C" void *fightFloorBiosGetAppointItemPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointItem;
}

extern "C" FIGHT_SIDE *fightFloorBiosGetAppointSidePtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointSide;
}

extern "C" FIGHT_WAZA *fightFloorBiosGetAppointWazaPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointWaza;
}

extern "C" FIGHT_TRAINER *fightFloorBiosGetAppointTrainerPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointTrainer;
}

extern "C" FightOutPokemon *fightFloorBiosGetAppointPokemonPtr(FIGHT_FLOOR *floor)
{
    if (floor == 0) {
        return 0;
    }
    return floor->mAppointPokemon;
}
