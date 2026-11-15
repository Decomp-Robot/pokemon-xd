#include <GSflag.h>
#include <pokemonDataBios.h>
#include <types.h>

extern "C" {

// Whether the species has been caught, as its zukan caught flag reads.
u32 pokemonDataBiosGetTukamaetaFlag(PokemonData *data)
{
    if (data == 0) {
        return 0;
    }
    return GSflagGet(data->mTukamaetaFlagId);
}

// Whether the species has been seen, as its zukan seen flag reads.
u32 pokemonDataBiosGetMitaFlag(PokemonData *data)
{
    if (data == 0) {
        return 0;
    }
    return GSflagGet(data->mMitaFlagId);
}

s32 pokemonDataBiosGetVoice(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mVoice;
}

u32 pokemonDataBiosGetDoc(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mDescriptionMessageId;
}

u16 pokemonDataBiosGetWeight(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mWeight;
}

u16 pokemonDataBiosGetHeight(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mHeight;
}

u32 pokemonDataBiosGetTypeName(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mTypeNameMessageId;
}

u8 pokemonDataBiosGetColor(PokemonData *data, u16 index)
{
    PokemonBody *body = pokemonDataBiosGetCgDataPtr(data, index);
    if (body == 0) return 0;
    return body->mColor;
}

u32 pokemonDataBiosGetStatusFaceMenuSpriteId(PokemonData *data, u16 index)
{
    PokemonBody *body = pokemonDataBiosGetCgDataPtr(data, index);
    if (body == 0) return 0;
    return body->mStatusFaceMenuSpriteId;
}

u32 pokemonDataBiosGetPokebodyId(PokemonData *data, u16 index)
{
    PokemonBody *body = pokemonDataBiosGetCgDataPtr(data, index);
    if (body == 0) return 0;
    return body->mPokebodyId;
}

PokemonBody *pokemonDataBiosGetCgDataPtr(PokemonData *data, u16 index)
{
    if (data == 0) return 0;
    if (index >= POKEMON_DATA_BODY_COUNT) return 0;
    return &data->mBody[index];
}

u8 pokemonDataBiosGetWazaMcn(PokemonData *data, u16 index)
{
    if (data == 0) return 0;
    if (index >= POKEMON_DATA_MACHINE_COUNT) return 0;
    return data->mWazaMachineCompatibility[index] != 0;
}

u16 pokemonDataBiosGetGetWazaDataId(PokemonData *data, u16 index)
{
    PokemonLevelUpMove *move = pokemonDataBiosGetGetWazaPtr(data, index);
    if (move == 0) return 0;
    return move->mWazaDataId;
}

u8 pokemonDataBiosGetGetWazaLevel(PokemonData *data, u16 index)
{
    PokemonLevelUpMove *move = pokemonDataBiosGetGetWazaPtr(data, index);
    if (move == 0) return 0;
    return move->mLevel;
}

PokemonLevelUpMove *pokemonDataBiosGetGetWazaPtr(PokemonData *data, u16 index)
{
    if (data == 0) return 0;
    if (index >= POKEMON_DATA_LEVEL_UP_MOVE_COUNT) return 0;
    return &data->mGetWaza[index];
}

u16 pokemonDataBiosGetSinkaPokemonDataId(PokemonData *data, u16 index)
{
    PokemonEvolution *evolution = pokemonDataBiosGetSinkaPtr(data, index);
    if (evolution == 0) return 0;
    return evolution->mPokemonDataId;
}

u16 pokemonDataBiosGetSinkaBuff(PokemonData *data, u16 index)
{
    PokemonEvolution *evolution = pokemonDataBiosGetSinkaPtr(data, index);
    if (evolution == 0) return 0;
    return evolution->mCondition;
}

u8 pokemonDataBiosGetSinkaKind(PokemonData *data, u16 index)
{
    PokemonEvolution *evolution = pokemonDataBiosGetSinkaPtr(data, index);
    if (evolution == 0) return 0;
    return evolution->mKind;
}

PokemonEvolution *pokemonDataBiosGetSinkaPtr(PokemonData *data, u16 index)
{
    if (data == 0) return 0;
    if (index >= POKEMON_DATA_EVOLUTION_COUNT) return 0;
    return &data->mSinka[index];
}

u8 pokemonDataBiosGetTokuseiDataId(PokemonData *data, u16 index)
{
    if (data == 0) return 0;
    if (index >= POKEMON_DATA_ABILITY_COUNT) return 0;
    return data->mTokuseiDataId[index];
}

u32 pokemonDataBiosGetZokuseiDataId(PokemonData *data, u16 index)
{
    if (data == 0) return 0;
    if (index >= POKEMON_DATA_TYPE_COUNT) return 0;
    return data->mZokuseiDataId[index];
}

u16 pokemonDataBiosGetItemDataId(PokemonData *data, u16 index)
{
    if (data == 0) return 0;
    if (index >= POKEMON_DATA_ITEM_COUNT) return 0;
    return data->mItemDataId[index];
}

u16 pokemonDataBiosGetInitFriend(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mInitFriend;
}

u8 pokemonDataBiosGetSexRatio(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mSexRatio;
}

u8 pokemonDataBiosGetGet(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mCatchRate;
}

u8 pokemonDataBiosGetGrowDataId(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mGrowDataId;
}

u16 pokemonDataBiosGetGiveExp(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mGiveExp;
}

u16 pokemonDataBiosGetGiveNimblenessEffort(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_EFFORT_YIELD);
    if (stats == 0) return 0;
    return stats->mNimbleness;
}

u16 pokemonDataBiosGetGiveSpeDefEffort(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_EFFORT_YIELD);
    if (stats == 0) return 0;
    return stats->mSpecialDefense;
}

u16 pokemonDataBiosGetGiveSpeAtkEffort(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_EFFORT_YIELD);
    if (stats == 0) return 0;
    return stats->mSpecialAttack;
}

u16 pokemonDataBiosGetGivePhyDefEffort(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_EFFORT_YIELD);
    if (stats == 0) return 0;
    return stats->mPhysicalDefense;
}

u16 pokemonDataBiosGetGivePhyAtkEffort(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_EFFORT_YIELD);
    if (stats == 0) return 0;
    return stats->mPhysicalAttack;
}

u16 pokemonDataBiosGetGiveMaxHpEffort(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_EFFORT_YIELD);
    if (stats == 0) return 0;
    return stats->mMaxHp;
}

u16 pokemonDataBiosGetBasisNimbleness(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_BASE_STATS);
    if (stats == 0) return 0;
    return stats->mNimbleness;
}

u16 pokemonDataBiosGetBasisSpeDef(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_BASE_STATS);
    if (stats == 0) return 0;
    return stats->mSpecialDefense;
}

u16 pokemonDataBiosGetBasisSpeAtk(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_BASE_STATS);
    if (stats == 0) return 0;
    return stats->mSpecialAttack;
}

u16 pokemonDataBiosGetBasisPhyDef(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_BASE_STATS);
    if (stats == 0) return 0;
    return stats->mPhysicalDefense;
}

u16 pokemonDataBiosGetBasisPhyAtk(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_BASE_STATS);
    if (stats == 0) return 0;
    return stats->mPhysicalAttack;
}

u16 pokemonDataBiosGetBasisMaxHp(PokemonData *data)
{
    PokemonBaseStats *stats = pokemonDataBiosGetBasisStatusPtr(data, POKEMON_DATA_BASE_STATS);
    if (stats == 0) return 0;
    return stats->mMaxHp;
}

PokemonBaseStats *pokemonDataBiosGetBasisStatusPtr(PokemonData *data, u16 index)
{
    if (data == 0) return 0;
    if (index >= POKEMON_DATA_STAT_SET_COUNT) return 0;
    return &data->mBaseStatsAndEffortYield[index];
}

u32 pokemonDataBiosGetName(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mNameMessageId;
}

}
