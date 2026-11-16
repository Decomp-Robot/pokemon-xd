#include <fightEncountBios.h>
#include <app/fight/fightType.h>
#include <deckDataVirtualBios.h>

extern "C" {

/* The fight type table loaded for the running fight and the number of records
 * it holds. */
extern u32 *fight_type_data_number;

/* The encounter table loaded for the running fight and the number of records
 * it holds. */
extern FightEncountRecord *fight_encount_data;

}

/* Sets the battle disk an encounter record is fought as; 0 keeps the decks of
 * the record's own trainer entries. */
extern "C" void fightEncountDataBiosSetDiskId(FightEncountRecord *data, s32 id)
{
    if (data == 0) {
        return;
    }
    data->mDiskId = id;
}

extern "C" u16 fightEncountDataBiosGetDiskId(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mDiskId;
}

/* Sound effect played with a battle wipe transition. */
extern "C" u16 fightEncountDataBiosGetWipeEffectSndID(FightEncountWipeData *wipeData)
{
    if (wipeData == 0) {
        return 0;
    }
    return wipeData->mEffectSndID;
}

/* How long a battle wipe transition runs. */
extern "C" f32 fightEncountDataBiosGetWipeEffectTime(FightEncountWipeData *wipeData)
{
    if (wipeData == 0) {
        return 0.0f;
    }
    return wipeData->mTime;
}

/* Whether a battle wipe transition keeps the picture as a snapshot. */
extern "C" bool fightEncountDataBiosGetWipeSnapshotUse(FightEncountWipeData *wipeData)
{
    if (wipeData == 0) {
        return false;
    }
    return wipeData->mSnapshotUse;
}

/* Fade effect that draws a battle wipe transition. */
extern "C" FadeFunction fightEncountDataBiosGetWipeFunction(FightEncountWipeData *wipeData)
{
    if (wipeData == 0) {
        return 0;
    }
    return wipeData->mFunction;
}

/* Transition record for a battle wipe id, or 0 when the id is out of range. */
extern "C" FightEncountWipeData *fightEncountWipeDataBiosGetPtr(enum FIGHT_WIPE_ID wipeId)
{
    if (wipeId >= fight_encount_wipe_data_number) {
        return 0;
    }
    return &fight_encount_wipe_data[wipeId];
}

extern "C" u32 fightEncountDataBiosGetFightName(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFightName;
}

extern "C" void fightEncountDataBiosSetSyoukaiWzxDataId(FightEncountRecord *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mSyoukaiWzxDataId = value;
}

extern "C" void fightEncountDataBiosSetZenmetuFlag(FightEncountRecord *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mZenmetuFlag = value;
}

extern "C" void fightEncountDataBiosSetBgmSndId(FightEncountRecord *data, u32 value)
{
    if (data == 0) {
        return;
    }
    data->mBgmSndId = value;
}

extern "C" void fightEncountDataBiosSetGSInputDevice(FightEncountRecord *data, u8 player, s32 devId)
{
    FightEncountTrainerInfo *trainerInfo = fightEncountDataBiosGetFightTrainerInfoPtr(data, player);
    if (trainerInfo == 0) {
        return;
    }
    trainerInfo->mGSInputDevice = devId;
}

/* Sets the trainer data a trainer entry plays. */
extern "C" void fightEncountDataBiosSetFightTrainerDataId(FightEncountRecord *data, u8 player, u16 trainerDataId)
{
    FightEncountTrainerInfo *trainerInfo = fightEncountDataBiosGetFightTrainerInfoPtr(data, player);
    if (trainerInfo == 0) {
        return;
    }
    trainerInfo->mDeckGrpAndTrainerDataId =
        (trainerInfo->mDeckGrpAndTrainerDataId & 0xFFFF0000) | trainerDataId;
}

extern "C" void fightEncountDataBiosSetFightFloorDataId(FightEncountRecord *data, u16 value)
{
    if (data == 0) {
        return;
    }
    data->mFightFloorDataId = value;
}

extern "C" void fightEncountDataBiosSetEntry(FightEncountRecord *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mEntry = value;
}

extern "C" void fightEncountDataBiosSetPokemon(FightEncountRecord *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mPokemon = value;
}

extern "C" void fightEncountDataBiosSetTrainer(FightEncountRecord *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mTrainer = value;
}

/* The fight type of an encounter record cannot be set: it is derived from the
 * record's trainer, Pokemon and entry counts by fightEncountDataBiosGetFightType. */
extern "C" void fightEncountDataBiosSetFightType(FightEncountRecord *data, u8 value)
{
    if (data == 0) {
        return;
    }
}

extern "C" void fightEncountDataBiosSetFightKind(FightEncountRecord *data, u8 value)
{
    if (data == 0) {
        return;
    }
    data->mFightKind = value;
}

extern "C" u32 fightEncountDataBiosGetSyoukaiWzxDataId(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mSyoukaiWzxDataId;
}

extern "C" u8 fightEncountDataBiosGetZenmetuFlag(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mZenmetuFlag;
}

extern "C" enum FIGHT_WIPE_ID fightEncountDataBiosGetWipeId(FightEncountRecord *data)
{
    if (data == 0) {
        return FIGHT_WIPE_ID_FADEOUT_IN;
    }
    return data->mWipeId;
}

extern "C" u32 fightEncountDataBiosGetBgmSndId(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mBgmSndId;
}

extern "C" u32 fightEncountDataBiosGetGSInputDevice(FightEncountRecord *data, u8 player)
{
    FightEncountTrainerInfo *trainerInfo = fightEncountDataBiosGetFightTrainerInfoPtr(data, player);
    if (trainerInfo == 0) {
        return 0;
    }
    return trainerInfo->mGSInputDevice;
}

/* Deck group a trainer entry fights with: the deck group stored in the entry
 * for a normal encounter, or the deck group of the battle disk. */
extern "C" u16 fightEncountDataBiosGetFightDeckGrpId(FightEncountRecord *data, u8 player)
{
    FightEncountTrainerInfo *trainerInfo = fightEncountDataBiosGetFightTrainerInfoPtr(data, player);
    if (trainerInfo == 0) {
        return 0;
    }
    if (data->mDiskId == 0) {
        return trainerInfo->mDeckGrpAndTrainerDataId >> 16;
    }
    return FIGHT_DECK_GRP_ID_BATTLE_DISK;
}

/* Trainer data a trainer entry plays: the entry's own trainer data for a normal
 * encounter, or the deck of the battle disk side it fights on. */
extern "C" u16 fightEncountDataBiosGetFightTrainerDataId(FightEncountRecord *data, u8 player)
{
    FightEncountTrainerInfo *trainerInfo = fightEncountDataBiosGetFightTrainerInfoPtr(data, player);
    if (trainerInfo == 0) {
        return 0;
    }
    if (data->mDiskId == 0) {
        return trainerInfo->mDeckGrpAndTrainerDataId;
    }
    DeckDataVirtual *deck = deckDataVirtual_GetPtr(data->mDiskId);
    if (deck == 0) {
        return 0;
    }
    switch (player) {
    case 0:
        return deck->allyDeck;
    case 1:
        return deck->enemyDeck;
    }
    return 0;
}

/* One of the four trainer entries of an encounter record, or 0 when the record
 * is missing or the entry is out of range. */
extern "C" FightEncountTrainerInfo *fightEncountDataBiosGetFightTrainerInfoPtr(FightEncountRecord *data, u8 player)
{
    if (data == 0) {
        return 0;
    }
    if (player >= 4) {
        return 0;
    }
    return &data->mTrainerInfo[player];
}

extern "C" u16 fightEncountDataBiosGetFightFloorDataId(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFightFloorDataId;
}

extern "C" u32 fightEncountDataBiosGetEntry(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mEntry;
}

/* Number of Pokemon the record keeps out at once. */
extern "C" u32 fightEncountDataBiosGetPokemon(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    if (data->mDiskId == 0) {
        return data->mPokemon;
    }
    DeckDataVirtual *deck = deckDataVirtual_GetPtr(data->mDiskId);
    if (deck == 0) {
        return 0;
    }
    return deck->pokemonNum;
}

extern "C" u32 fightEncountDataBiosGetTrainer(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mTrainer;
}

/* Fight type the record is displayed as: the first fight type whose trainer,
 * fightout and entry Pokemon counts match the record, or FIGHT_TYPE_ID_FALLBACK
 * when none does. */
extern "C" u8 fightEncountDataBiosGetFightType(FightEncountRecord *data)
{
    FIGHT_TYPE_DATA *fightTypeData;
    u8 i;
    if (data == 0) {
        return 0;
    }
    for (i = 0; i < *fight_type_data_number; i++) {
        fightTypeData = fightTypeDataBiosGetPtr(i);
        if (data->mTrainer == fightTypeData->mTrainerNum
            && data->mPokemon == fightTypeDataBiosGetFightoutPokemonNum(fightTypeData)
            && data->mEntry == fightTypeDataBiosGetEntryPokemonNum(fightTypeData)) {
            return i;
        }
    }
    return FIGHT_TYPE_ID_FALLBACK;
}

extern "C" u32 fightEncountDataBiosGetFightKind(FightEncountRecord *data)
{
    if (data == 0) {
        return 0;
    }
    return data->mFightKind;
}

/* Encounter record for an encounter id, or 0 when the id is out of range. */
extern "C" FightEncountRecord *fightEncountDataBiosGetPtr(u16 encountNo)
{
    if (encountNo >= *fight_encount_data_number) {
        return 0;
    }
    return &fight_encount_data[encountNo];
}
