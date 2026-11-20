#include <GSflag.h>
#include <GSmsg.h>
#include <pokemonBios.h>
#include <pokemonDataBios.h>
#include <relglobal.h>
#include <types.h>
#include <waza.h>

static const u8 POKEMON_PP_UP_COUNT_MAX = 3;
static const u8 POKEMON_PP_UP_BASE_PP_CUTOFF = 4;
static const u8 POKEMON_LEVEL_MAX = 100;

extern "C" {

void pokemonBiosSetNimblenessEffort(Pokemon *pokemon, u16 effort)
{
    if (pokemon == 0) return;
    pokemon->setNimblenessEffort(effort);
}

void pokemonBiosSetSpeDefEffort(Pokemon *pokemon, u16 effort)
{
    if (pokemon == 0) return;
    pokemon->setSpeDefEffort(effort);
}

void pokemonBiosSetSpeAtkEffort(Pokemon *pokemon, u16 effort)
{
    if (pokemon == 0) return;
    pokemon->setSpeAtkEffort(effort);
}

void pokemonBiosSetPhyDefEffort(Pokemon *pokemon, u16 effort)
{
    if (pokemon == 0) return;
    pokemon->setPhyDefEffort(effort);
}

void pokemonBiosSetPhyAtkEffort(Pokemon *pokemon, u16 effort)
{
    if (pokemon == 0) return;
    pokemon->setPhyAtkEffort(effort);
}

void pokemonBiosSetMaxHpEffort(Pokemon *pokemon, u16 effort)
{
    if (pokemon == 0) return;
    pokemon->setMaxHpEffort(effort);
}

void pokemonBiosSetNimbleness(Pokemon *pokemon, u16 stat)
{
    if (pokemon == 0) return;
    pokemon->setNimbleness(stat);
}

void pokemonBiosSetSpeDef(Pokemon *pokemon, u16 stat)
{
    if (pokemon == 0) return;
    pokemon->setSpeDef(stat);
}

void pokemonBiosSetSpeAtk(Pokemon *pokemon, u16 stat)
{
    if (pokemon == 0) return;
    pokemon->setSpeAtk(stat);
}

void pokemonBiosSetPhyDef(Pokemon *pokemon, u16 stat)
{
    if (pokemon == 0) return;
    pokemon->setPhyDef(stat);
}

void pokemonBiosSetPhyAtk(Pokemon *pokemon, u16 stat)
{
    if (pokemon == 0) return;
    pokemon->setPhyAtk(stat);
}

void pokemonBiosSetMaxHp(Pokemon *pokemon, u16 stat)
{
    if (pokemon == 0) return;
    pokemon->setMaxHp(stat);
}

void pokemonBiosSetHp(Pokemon *pokemon, u16 hp)
{
    if (pokemon == 0) return;
    pokemon->mHp = hp;
    u16 maxHp = pokemonBiosGetMaxHp(pokemon);
    if (maxHp < pokemon->mHp) {
        pokemon->mHp = maxHp;
    }
}

void pokemonBiosSetItemDataId(Pokemon *pokemon, u16 itemDataId)
{
    if (pokemon == 0) return;
    pokemon->mItemDataId = itemDataId;
}

void pokemonBiosSetPokemonWazaPpCount(Pokemon *pokemon, u16 slot, u8 count)
{
    PokemonWaza *waza = pokemonBiosGetPokemonWazaPtr(pokemon, slot, false);
    if (waza == 0) return;
    if (count > POKEMON_PP_UP_COUNT_MAX) {
        count = POKEMON_PP_UP_COUNT_MAX;
    }
    // Leave the stored count alone for a valid move with four or fewer base PP.
    if (pokemonWazaCheckValid(pokemon, slot) == 1) {
        WazaData *data = wazaDataBiosGetPtr(pokemonBiosGetPokemonWazaDataId(pokemon, slot));
        if (wazaDataBiosGetPp(data) <= POKEMON_PP_UP_BASE_PP_CUTOFF) {
            return;
        }
    }
    waza->ppCount = count;
}

void pokemonBiosSetPokemonWazaPp(Pokemon *pokemon, u16 slot, u8 pp)
{
    PokemonWaza *waza = pokemonBiosGetPokemonWazaPtr(pokemon, slot, false);
    if (waza == 0) return;
    u8 maxPp = pokemonWazaGetMaxPP(pokemon, slot);
    if (maxPp < pp) {
        pp = maxPp;
    }
    waza->pp = pp;
}

void pokemonBiosSetPokemonWazaDataId(Pokemon *pokemon, u16 slot, u16 wazaDataId)
{
    PokemonWaza *waza = pokemonBiosGetPokemonWazaPtr(pokemon, slot, false);
    if (waza != 0) {
        waza->dataId = wazaDataId;
    }
}

void pokemonBiosSetConditionAmari(Pokemon *pokemon, u32 amari)
{
    if (pokemon == 0) return;
    pokemon->mConditionAmari = amari;
}

void pokemonBiosSetLevel(Pokemon *pokemon, u8 level)
{
    if (pokemon == 0) return;
    if (level > POKEMON_LEVEL_MAX) {
        level = POKEMON_LEVEL_MAX;
    }
    pokemon->mLevel = level;
}

void pokemonBiosSetPoolFriend(Pokemon *pokemon, u16 friendLevel)
{
    if (pokemon == 0) return;
    pokemon->setPoolFriendLevel(friendLevel);
}

void pokemonBiosSetPoolExp(Pokemon *pokemon, u32 exp)
{
    if (pokemon == 0) return;
    pokemon->setPoolExp(exp);
}

void pokemonBiosSetExp(Pokemon *pokemon, u32 exp)
{
    if (pokemon == 0) return;
    pokemon->mExp = exp;
}

void pokemonBiosSetNicknameOrgPtr(Pokemon *pokemon, const u16 *name)
{
    if (pokemon == 0 || name == 0) return;
    GScharLenCpy(pokemon->mOriginalNickname, name,
                 sizeof(pokemon->mOriginalNickname) / sizeof(pokemon->mOriginalNickname[0]));
}

void pokemonBiosSetNicknamePtr(Pokemon *pokemon, const u16 *name)
{
    if (pokemon == 0 || name == 0) return;
    GScharLenCpy(pokemon->mNickname, name,
                 sizeof(pokemon->mNickname) / sizeof(pokemon->mNickname[0]));
    pokemonBiosSetNicknameOrgPtr(pokemon, name);
}

void pokemonBiosSetCatchTrainerNamePtr(Pokemon *pokemon, const u16 *name)
{
    if (pokemon == 0 || name == 0) return;
    GScharLenCpy(pokemon->mTrainerName, name,
                 sizeof(pokemon->mTrainerName) / sizeof(pokemon->mTrainerName[0]));
}

void pokemonBiosSetCatchTrainerRnd(Pokemon *pokemon, u32 rnd)
{
    if (pokemon == 0) return;
    pokemon->mCatchTrainerRnd = rnd;
}

void pokemonBiosSetCatchTrainerSex(Pokemon *pokemon, u8 sex)
{
    if (pokemon == 0) return;
    pokemon->mCatchTrainerSex = sex;
}

void pokemonBiosSetCatchBallId(Pokemon *pokemon, u8 ballId)
{
    if (pokemon == 0) return;
    pokemon->mCatchBallId = ballId;
}

void pokemonBiosSetCatchLevel(Pokemon *pokemon, u8 catchLevel)
{
    if (pokemon == 0) return;
    pokemon->mCatchLevel = catchLevel;
}

void pokemonBiosSetCatchFloorId(Pokemon *pokemon, u16 catchFloorId)
{
    if (pokemon == 0) return;
    pokemon->mCatchFloorId = catchFloorId;
}

void pokemonBiosSetRnd(Pokemon *pokemon, u32 rnd)
{
    if (pokemon == 0) return;
    pokemon->mPersonality = rnd;
}

void pokemonBiosSetPokemonDataId(Pokemon *pokemon, u16 pokemonDataId)
{
    if (pokemon == 0) return;
    pokemon->mPokemonDataId = pokemonDataId;
}

u16 pokemonDataBiosGetKowaza(PokemonData *data, u16 index)
{
    if (data == 0) return 0;
    if (index >= POKEMON_DATA_KOWAZA_COUNT) return 0;
    return data->mKowaza[index];
}

u32 pokemonDataBiosGetPkxDataId(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mPkxDataId;
}

u16 pokemonDataBiosGetNumPokemon(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mNumPokemon;
}

u16 pokemonDataBiosGetNumZukan(PokemonData *data)
{
    if (data == 0) return 0;
    return data->mNumZukan;
}

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

// Species record lookup for a species ID. The pokemon data count and the
// species table pointer live in the relglobal data area; a species ID inside
// the count addresses the table directly.
PokemonData *pokemonDataBiosGetPtr(u16 species)
{
    if (species >= *pokemon_data_number) return 0;
    return &pokemon_data[species];
}

// Event acquisition flag of the record.
bool pokemonBiosGetEventGetFlag(Pokemon *pokemon)
{
    if (pokemon == 0) return false;
    return pokemon->mEventGetFlag;
}

// Trainer Pokemon record ID the Pokemon was created from.
u16 pokemonBiosGetFightTrainerPokemonDataId(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->mFightTrainerPokemonDataId;
}

// Spare field of the record.
u16 pokemonBiosGetPara1Amari(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getPara1Amari();
}

u16 pokemonBiosGetAmari(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getAmari();
}

// Mail the record holds.
u8 pokemonBiosGetMailId(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->mMailId;
}

// PC box mark of the record.
u8 pokemonBiosGetPcboxMark(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->mPcBoxMark;
}

// Spare flag byte of the record.
u8 pokemonBiosGetFlagAmari(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->mFlagAmari;
}

// Whether the record is marked invalid.
u8 pokemonBiosGetFuseiFlag(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->mFuseiFlag != 0;
}

// Whether the record uses the second ability of its species. A species
// without a second ability has none to choose, so the lookup fails first.
u8 pokemonBiosGetTokuseiFlag(Pokemon *pokemon)
{
    PokemonData *data;

    if (pokemon == 0) return 0;
    data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon));
    if (pokemonDataBiosGetTokuseiDataId(data, POKEMON_DATA_ABILITY_SECOND) == 0) return 0;
    return pokemon->mTokuseiFlag != 0;
}

// Whether the record is still an egg.
u8 pokemonBiosGetTamagoFlag(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->mTamagoFlag != 0;
}

// Pokerus infection of the record.
u8 pokemonBiosGetPokerus(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->mPokerus;
}

// Spare ribbon field of the record.
u8 pokemonBiosGetAmariRibbon(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getRibbon()->amari;
}

// Ribbon queries. Each ribbon takes one flag of the record's ribbon word,
// read through the ribbon word accessor; a record the accessor cannot resolve
// counts as having no ribbons.

u8 pokemonBiosGetWorldRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->world != 0;
}

u8 pokemonBiosGetEarthRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->earth != 0;
}

u8 pokemonBiosGetNationalRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->national != 0;
}

u8 pokemonBiosGetCountryRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->country != 0;
}

u8 pokemonBiosGetSkyRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->sky != 0;
}

u8 pokemonBiosGetLandRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->land != 0;
}

u8 pokemonBiosGetMarineRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->marine != 0;
}

u8 pokemonBiosGetGanbaRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->ganba != 0;
}

u8 pokemonBiosGetBromideRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->bromide != 0;
}

u8 pokemonBiosGetVictoryRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->victory != 0;
}

u8 pokemonBiosGetWinningRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->winning != 0;
}

u8 pokemonBiosGetChampRibbon(Pokemon *pokemon)
{
    PokemonRibbon *ribbon;

    ribbon = pokemonBiosGetRibbonPtr(pokemon);
    if (ribbon == 0) return 0;
    return ribbon->champ != 0;
}

// Ribbon word of the record.
PokemonRibbon *pokemonBiosGetRibbonPtr(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getRibbon();
}

// Fur sheen of the record.
u8 pokemonBiosGetFur(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->mFur;
}

// Contest medal queries. The medals are the second expand status record, one
// byte per contest category.

u8 pokemonBiosGetStrongMedal(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_MEDAL);
    if (status == 0) return 0;
    return status->strong;
}

u8 pokemonBiosGetCleverMedal(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_MEDAL);
    if (status == 0) return 0;
    return status->clever;
}

u8 pokemonBiosGetCuteMedal(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_MEDAL);
    if (status == 0) return 0;
    return status->cute;
}

u8 pokemonBiosGetBeautifulMedal(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_MEDAL);
    if (status == 0) return 0;
    return status->beautiful;
}

u8 pokemonBiosGetStyleMedal(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_MEDAL);
    if (status == 0) return 0;
    return status->style;
}

// Contest condition queries. The contest values are the first expand status
// record, one byte per contest category in category order.

u8 pokemonBiosGetStrong(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) return 0;
    return status->strong;
}

u8 pokemonBiosGetClever(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) return 0;
    return status->clever;
}

u8 pokemonBiosGetCute(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) return 0;
    return status->cute;
}

u8 pokemonBiosGetBeautiful(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) return 0;
    return status->beautiful;
}

u8 pokemonBiosGetStyle(Pokemon *pokemon)
{
    PokemonExpandStatus *status;

    status = pokemonBiosGetExpandStatusPtr(pokemon, POKEMON_EXPAND_STATUS_CONDITION);
    if (status == 0) return 0;
    return status->style;
}

// Expand status record of a Pokemon. The index selects the contest values
// (0) or the contest medals (1); the record holds two of them.
PokemonExpandStatus *pokemonBiosGetExpandStatusPtr(Pokemon *pokemon, u16 index)
{
    if (pokemon == 0) return 0;
    if (index >= POKEMON_EXPAND_STATUS_COUNT) return 0;
    return &pokemon->mExpandStatus[index];
}

// Shadow points. The initial value comes from the deck's Dark Pokemon
// record for this Pokemon's Dark Pokemon data ID; the current value is
// held by the Pokemon's live Dark Pokemon.

u16 pokemonBiosGetInitDp(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    u16 darkID = pokemonBiosGetDarkpokemonDataId(pokemon);
    if (darkID == 0) return 0;
    return darkPokemonBiosGetDarkPoint(darkID);
}

bool pokemonBiosGetDarkFlag(Pokemon *pokemon)
{
    if (pokemon == 0) return false;
    if (pokemonBiosGetDarkpokemonDataId(pokemon) == 0) return false;
    return !darkPokemonBiosGetReliveFlag(pokemonBiosGetDarkpokemonDataId(pokemon));
}

u16 pokemonBiosGetPoolFriend(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getPoolFriendLevel();
}

u32 pokemonBiosGetPoolExp(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getPoolExp();
}

int pokemonBiosGetDp(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getDarkPointDirect();
}

u16 pokemonBiosGetDarkpokemonDataId(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->mDarkPokemonDataId;
}

u16 pokemonBiosGetFriend(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getFriendLevel();
}

// Individual values, effort values and calculated battle stats, each stored
// in the Pokemon record and queried through its corresponding getter.
u16 pokemonBiosGetNimblenessRnd(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getNimblenessRnd();
}

u16 pokemonBiosGetSpeDefRnd(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getSpeDefRnd();
}

u16 pokemonBiosGetSpeAtkRnd(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getSpeAtkRnd();
}

u16 pokemonBiosGetPhyDefRnd(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getPhyDefRnd();
}

u16 pokemonBiosGetPhyAtkRnd(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getPhyAtkRnd();
}

u16 pokemonBiosGetMaxHpRnd(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getMaxHpRnd();
}

u16 pokemonBiosGetNimblenessEffort(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getNimblenessEffort();
}

u16 pokemonBiosGetSpeDefEffort(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getSpeDefEffort();
}

u16 pokemonBiosGetSpeAtkEffort(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getSpeAtkEffort();
}

u16 pokemonBiosGetPhyDefEffort(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getPhyDefEffort();
}

u16 pokemonBiosGetPhyAtkEffort(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getPhyAtkEffort();
}

u16 pokemonBiosGetMaxHpEffort(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getMaxHpEffort();
}

u16 pokemonBiosGetNimbleness(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getNimbleness();
}

u16 pokemonBiosGetSpeDef(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getSpeDef();
}

u16 pokemonBiosGetSpeAtk(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getSpeAtk();
}

u16 pokemonBiosGetPhyDef(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getPhyDef();
}

u16 pokemonBiosGetPhyAtk(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getPhyAtk();
}

u16 pokemonBiosGetMaxHp(Pokemon *pokemon)
{
    if (pokemon == 0) return 0;
    return pokemon->getMaxHp();
}
}
