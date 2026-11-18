#include <fightTrainer.h>
#include <fightTrainerDB.h>
#include <deck.h>
#include <app/fight/fightTrainerEnemy.h>

// The trainer record is read and written through the adapters of this module:
// the AI additional-value records the debug menus edit and the field queries of
// one battle-side trainer record.  A missing record is left alone when writing
// and reads as an empty one.

extern "C" {

void fightTrainer_SetEnemyPokemonRegist(FIGHT_TRAINER *trainer, s16 entryId)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainer_GetFightTrainerEnemyPokemonAryPtr(trainer, 0);
    if (pokemon == 0) {
        return;
    }
    fightTrainerEnemyPokemonRegistAry(pokemon, 12, entryId);
}

void fightTrainer_SetEnemyParam1bantakaiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 value)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) {
        return;
    }
    pokemon->param1bantakaiFlag = value;
}

void fightTrainer_SetEnemyBadwazaHaveFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 value)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) {
        return;
    }
    pokemon->badwazaHaveFlag = value;
}

void fightTrainer_SetEnemyDefense1banhikuiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 value)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) {
        return;
    }
    pokemon->defense1banhikuiFlag = value;
}

void fightTrainer_SetEnemyLv1banhikuiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 value)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) {
        return;
    }
    pokemon->lv1banhikuiFlag = value;
}

void fightTrainer_SetEnemyNowhp1banhikuiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 value)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) {
        return;
    }
    pokemon->nowhp1banhikuiFlag = value;
}

void fightTrainer_SetEnemyStoreTokuseiData(FIGHT_TRAINER *trainer, s16 entryId, u16 value)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) {
        return;
    }
    pokemon->storeTokuseiData = value;
}

void fightTrainer_SetEnemyTokuseiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 value)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) {
        return;
    }
    pokemon->tokuseiFlag = value;
}

void fightTrainer_SetEnemyExpectWazaData(FIGHT_TRAINER *trainer, s16 entryId, u8 index, u16 dataId)
{
    FightTrainerEnemyPokemon *pokemon;
    if (trainer == 0) return;
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) return;
    pokemon->expectWazaDataId[index] = dataId;
}

void fightTrainer_SetEnemyStoreWazaData(FIGHT_TRAINER *trainer, s16 entryId, u8 index, u16 dataId)
{
    FightTrainerEnemyPokemon *pokemon;
    if (trainer == 0) return;
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) return;
    pokemon->storeWazaDataId[index] = dataId;
}

void fightTrainer_SetEnemyExpectWazaNum(FIGHT_TRAINER *trainer, s16 entryId, u8 count)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) {
        return;
    }
    pokemon->expectWazaNum = count;
}

void fightTrainer_SetEnemyStoreWazaNum(FIGHT_TRAINER *trainer, s16 entryId, u8 count)
{
    FightTrainerEnemyPokemon *pokemon;

    if (trainer == 0) {
        return;
    }
    pokemon = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, entryId);
    if (pokemon == 0) {
        return;
    }
    pokemon->storeWazaNum = count;
}

void fightTrainer_SetSequencePtr(FIGHT_TRAINER *trainer, struct NullSequence *sequence)
{
    if (trainer == 0) return;
    trainer->mSequence = sequence;
}

void fightTrainer_SetControllerId(FIGHT_TRAINER *trainer, s32 controllerId)
{
    if (trainer == 0) return;
    trainer->mControllerId = controllerId;
}

void fightTrainer_SetNigeruCount(FIGHT_TRAINER *trainer, s32 count)
{
    if (trainer == 0) return;
    trainer->mNigeruCount = count;
}

void fightTrainer_SetKoban(FIGHT_TRAINER *trainer, s32 koban)
{
    if (trainer == 0) return;
    trainer->mKoban = koban;
}

void fightTrainer_SetOkaneBai(FIGHT_TRAINER *trainer, s32 multiplier)
{
    if (trainer == 0) return;
    trainer->mOkaneBai = multiplier;
}

void fightTrainer_SetFightTrainerDataId(FIGHT_TRAINER *trainer, s32 dataId)
{
    if (trainer == 0) return;
    trainer->mDataId = dataId;
}


// Message ID printed in front of the name of the record.
void fightTrainerAiValueAddsubDB_SetPrefixName(u16 index, u32 prefixName)
{
    FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *addsubData = fightTrainerAiValueAddsubDataBiosGetPtr(index);

    if (addsubData != 0) {
        addsubData->mPrefixName = prefixName;
    }
}

// Message ID of the action the record applies to.
void fightTrainerAiValueAddsubDB_SetKoudouName(u16 index, u32 koudouName)
{
    FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *addsubData = fightTrainerAiValueAddsubDataBiosGetPtr(index);

    if (addsubData != 0) {
        addsubData->mKoudouName = koudouName;
    }
}

// Message ID naming the record.
void fightTrainerAiValueAddsubDB_SetName(u16 index, u32 name)
{
    FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *addsubData = fightTrainerAiValueAddsubDataBiosGetPtr(index);

    if (addsubData != 0) {
        addsubData->mName = name;
    }
}

// Value the AI adds to (or subtracts from) the decision weight.
void fightTrainerAiValueAddsubDB_SetValue(u16 index, s32 value)
{
    FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *addsubData = fightTrainerAiValueAddsubDataBiosGetPtr(index);

    if (addsubData != 0) {
        addsubData->mValue = value;
    }
}

// Has the highest parameter of the trainer's team.
u8 fightTrainer_GetEnemyParam1bantakaiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    return enemy->param1bantakaiFlag;
}

// Holds a bad move.
u8 fightTrainer_GetEnemyBadwazaHaveFlag(FIGHT_TRAINER *trainer, s16 fightEntryId)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    return enemy->badwazaHaveFlag;
}

// Has the lowest defense of the trainer's team.
u8 fightTrainer_GetEnemyDefense1banhikuiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    return enemy->defense1banhikuiFlag;
}

// Has the lowest level of the trainer's team.
u8 fightTrainer_GetEnemyLv1banhikuiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    return enemy->lv1banhikuiFlag;
}

// Has the lowest current HP of the trainer's team.
u8 fightTrainer_GetEnemyNowhp1banhikuiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    return enemy->nowhp1banhikuiFlag;
}

// Ability data the enemy pokemon stores in its fight status block.
u16 fightTrainer_GetEnemyStoreTokuseiData(FIGHT_TRAINER *trainer, s16 fightEntryId)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    return enemy->storeTokuseiData;
}

// Ability the enemy pokemon fights with.
u8 fightTrainer_GetEnemyTokuseiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    return enemy->tokuseiFlag;
}

// Move data id stored in the expected move slot of the enemy pokemon.
u16 fightTrainer_GetEnemyExpectWazaData(FIGHT_TRAINER *trainer, s16 fightEntryId, u8 index)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    if (index > 8) {
        return 0;
    }
    return enemy->expectWazaDataId[index];
}

// Move data id stored in the stored move slot of the enemy pokemon.
u16 fightTrainer_GetEnemyStoreWazaData(FIGHT_TRAINER *trainer, s16 fightEntryId, u8 index)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    if (index >= 4) {
        return 0;
    }
    return enemy->storeWazaDataId[index];
}

// Move slots the enemy pokemon stores.
u8 fightTrainer_GetEnemyStoreWazaNum(FIGHT_TRAINER *trainer, s16 fightEntryId)
{
    FightTrainerEnemyPokemon *enemy;

    if (trainer == 0) {
        return 0;
    }
    enemy = fightTrainerGetFightEntryIdToEnemyPokemonPtr(trainer, fightEntryId);
    if (enemy == 0) {
        return 0;
    }
    return enemy->storeWazaNum;
}

// Fight status of one of the trainer's pokemon.
FightTrainerEnemyPokemon *fightTrainer_GetFightTrainerEnemyPokemonAryPtr(FIGHT_TRAINER *trainer,
                                                                        u8 index)
{
    if (trainer == 0) {
        return 0;
    }
    if (index >= 12) {
        return 0;
    }
    return &trainer->mEnemyPokemon[index];
}

// Sequence that drives the trainer model.
struct NullSequence *fightTrainer_GetSequencePtr(FIGHT_TRAINER *trainer)
{
    if (trainer == 0) {
        return 0;
    }
    return trainer->mSequence;
}

// Action work of the fight.
FIGHT_ACTION *fightTrainer_GetFightActionBuffPtr(FIGHT_TRAINER *trainer)
{
    if (trainer == 0) {
        return 0;
    }
    return &trainer->mFightActionBuff;
}

// Controller the fight is fought with.
u8 fightTrainer_GetControllerId(FIGHT_TRAINER *trainer)
{
    if (trainer == 0) {
        return 0;
    }
    return trainer->mControllerId;
}

// Times the trainer has run away.
u8 fightTrainer_GetNigeruCount(FIGHT_TRAINER *trainer)
{
    if (trainer == 0) {
        return 0;
    }
    return trainer->mNigeruCount;
}

// Prize money the fight is worth.
u32 fightTrainer_GetKoban(FIGHT_TRAINER *trainer)
{
    if (trainer == 0) {
        return 0;
    }
    return trainer->mKoban;
}

// Prize money multiplier of the trainer.
u8 fightTrainer_GetOkaneBai(FIGHT_TRAINER *trainer)
{
    if (trainer == 0) {
        return 0;
    }
    return trainer->mOkaneBai;
}

// Working copy of the pokemon that is out.
FightOutPokemon *fightTrainer_GetFightoutPokemonBuffPtr(FIGHT_TRAINER *trainer)
{
    if (trainer == 0) {
        return 0;
    }
    return &trainer->mFightoutPokemonBuff;
}

// One of the pokemon out on the field.
FightOutPokemon *fightTrainer_GetFightoutPokemonPtr(FIGHT_TRAINER *trainer, u16 slot)
{
    if (trainer == 0) {
        return 0;
    }
    if (slot >= 2) {
        return 0;
    }
    return &trainer->mFightoutPokemon[slot];
}

// One slot of the trainer's party.
FightPokemon *fightTrainer_GetFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot)
{
    if (trainer == 0) {
        return 0;
    }
    if (slot >= 6) {
        return 0;
    }
    return &trainer->mFightPokemon[slot];
}

// Data ID of the trainer.
u16 fightTrainer_GetFightTrainerDataId(FIGHT_TRAINER *trainer)
{
    if (trainer == 0) {
        return 0;
    }
    return trainer->mDataId;
}

// Hero record the fight is fought with.
Hero *fightTrainer_GetHeroPtr(FIGHT_TRAINER *trainer)
{
    if (trainer == 0) {
        return 0;
    }
    return &trainer->mHero;
}

// Message ID printed in front of the name of the record.
u32 fightTrainerAiValueAddsubDB_GetPrefixName(u16 index)
{
    FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *addsubData = fightTrainerAiValueAddsubDataBiosGetPtr(index);

    if (addsubData == 0) {
        return 0;
    }
    return addsubData->mPrefixName;
}

// Message ID of the action the record applies to.
u32 fightTrainerAiValueAddsubDB_GetKoudouName(u16 index)
{
    FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *addsubData = fightTrainerAiValueAddsubDataBiosGetPtr(index);

    if (addsubData == 0) {
        return 0;
    }
    return addsubData->mKoudouName;
}

// Message ID naming the record.
u32 fightTrainerAiValueAddsubDB_GetName(u16 index)
{
    FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *addsubData = fightTrainerAiValueAddsubDataBiosGetPtr(index);

    if (addsubData == 0) {
        return 0;
    }
    return addsubData->mName;
}

// Value the AI adds to (or subtracts from) the decision weight.
s32 fightTrainerAiValueAddsubDB_GetValue(u16 index)
{
    FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *addsubData = fightTrainerAiValueAddsubDataBiosGetPtr(index);

    if (addsubData == 0) {
        return 0;
    }
    return addsubData->mValue;
}

void fightTrainerAiDB_SetParamStoreFlag(u16 aiDataId, u8 flag)
{

}

u8 fightTrainerAiDB_GetParamStoreFlag(u16 aiDataId)
{
    return 1;
}

void fightTrainerAiDB_SetParamExpectFlag(u16 aiDataId, u8 flag)
{

}

u8 fightTrainerAiDB_GetParamExpectFlag(u16 aiDataId)
{
    return 1;
}

void fightTrainerAiDB_SetComboValue(u16 aiDataId, u8 value)
{

}

u8 fightTrainerAiDB_GetComboValue(u16 aiDataId)
{
    return 100;
}

void fightTrainerAiDB_SetLastValueRevise(u16 aiDataId, u8 correction)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setTotalCorrection(correction);
}

u8 fightTrainerAiDB_GetLastValueRevise(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getTotalCorrection();
}

void fightTrainerAiDB_SetWazaTypeReviseValue(u16 aiDataId, u8 slot, u8 value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    if (slot >= 2) {
        return;
    }
    ai->setWazaKindParam(slot, value);
}

u8 fightTrainerAiDB_GetWazaTypeReviseValue(u16 aiDataId, u8 slot)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    if (slot >= 2) {
        return 0;
    }
    return ai->getWazaKindParam(slot);
}

void fightTrainerAiDB_SetWazaTypeReviseTypeDataId(u16 aiDataId, u8 slot, u8 typeDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    if (slot >= 2) {
        return;
    }
    ai->setWazaKindID(slot, typeDataId);
}

u8 fightTrainerAiDB_GetWazaTypeReviseTypeDataId(u16 aiDataId, u8 slot)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    if (slot >= 2) {
        return 0;
    }
    return ai->getWazaKindID(slot);
}

void fightTrainerAiDB_SetZokuseiReviseValue(u16 aiDataId, u8 slot, u8 value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    if (slot >= 2) {
        return;
    }
    ai->setAttrTypeParam(slot, value);
}

u8 fightTrainerAiDB_GetZokuseiReviseValue(u16 aiDataId, u8 slot)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    if (slot >= 2) {
        return 0;
    }
    return ai->getAttrTypeParam(slot);
}

void fightTrainerAiDB_SetZokuseiReviseZokuseiDataId(u16 aiDataId, u8 slot, u8 zokuseiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    if (slot >= 2) {
        return;
    }
    ai->setAttrTypeID(slot, zokuseiDataId);
}

u8 fightTrainerAiDB_GetZokuseiReviseZokuseiDataId(u16 aiDataId, u8 slot)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    if (slot >= 2) {
        return 0;
    }
    return ai->getAttrTypeID(slot);
}

void fightTrainerAiDB_SetWazaNokoriPpValue(u16 aiDataId, u8 rate)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setLastPPDetermRate(rate);
}

u8 fightTrainerAiDB_GetWazaNokoriPpValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getLastPPDetermRate();
}

void fightTrainerAiDB_SetNokoriHpValue(u16 aiDataId, u8 rate)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setLowHPDetermRate(rate);
}

u8 fightTrainerAiDB_GetNokoriHpValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getLowHPDetermRate();
}

u8 fightTrainerAiDB_GetStrokeValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getAnnihilateDetermRate();
}

void fightTrainerAiDB_SetWazaAvgValue(u16 aiDataId, u8 rate)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setHitRate(rate);
}

u8 fightTrainerAiDB_GetWazaAvgValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getHitRate();
}

void fightTrainerAiDB_SetItemValue(u16 aiDataId, u8 rate)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setItemUseRate(rate);
}

u8 fightTrainerAiDB_GetItemValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getItemUseRate();
}

void fightTrainerAiDB_SetIrekaeValue(u16 aiDataId, u8 rate)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setPokeChgRate(rate);
}

u8 fightTrainerAiDB_GetIrekaeValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getPokeChgRate();
}

void fightTrainerAiDB_SetAbicntMinValue(u16 aiDataId, u8 value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setStatusDetermValTotalMin(value);
}

u8 fightTrainerAiDB_GetAbicntMinValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getStatusDetermValTotalMin();
}

void fightTrainerAiDB_SetAbicntMaxValue(u16 aiDataId, u8 value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setStatusDetermValTotalMax(value);
}

u8 fightTrainerAiDB_GetAbicntMaxValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getStatusDetermValTotalMax();
}

u8 fightTrainerAiDB_GetAvoidMinValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getStatusDetermValMin(DECK_STATUS_DETERM_AVOID);
}

u8 fightTrainerAiDB_GetAvoidMaxValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getStatusDetermValMax(DECK_STATUS_DETERM_AVOID);
}

u8 fightTrainerAiDB_GetNimblenessMaxValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getStatusDetermValMax(DECK_STATUS_DETERM_NIMBLENESS);
}

u8 fightTrainerAiDB_GetSpeDefMaxValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getStatusDetermValMax(DECK_STATUS_DETERM_SPE_DEF);
}

u8 fightTrainerAiDB_GetSpeAtkMaxValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getStatusDetermValMax(DECK_STATUS_DETERM_SPE_ATK);
}

u8 fightTrainerAiDB_GetPhyDefMaxValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getStatusDetermValMax(DECK_STATUS_DETERM_PHY_DEF);
}

u8 fightTrainerAiDB_GetPhyAtkMaxValue(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return 0;
    }
    return ai->getStatusDetermValMax(DECK_STATUS_DETERM_PHY_ATK);
}

void fightTrainerAiDB_SetPokemonJoutaiFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setRespectPokemonCondJuge(value);
}

bool fightTrainerAiDB_GetPokemonJoutaiFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getRespectPokemonCondJuge();
}

void fightTrainerAiDB_SetWazaRiskFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setRespectWazaRiskJudge(value);
}

bool fightTrainerAiDB_GetWazaRiskFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getRespectWazaRiskJudge();
}

void fightTrainerAiDB_SetWazaHitFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setRespectWazaHit(value);
}

bool fightTrainerAiDB_GetWazaHitFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getRespectWazaHit();
}

void fightTrainerAiDB_SetPartFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setRespectRole(value);
}

bool fightTrainerAiDB_GetPartFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getRespectRole();
}

void fightTrainerAiDB_SetWazaInitValueFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setRespectWazaAssmntVal(value);
}

bool fightTrainerAiDB_GetWazaInitValueFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getRespectWazaAssmntVal();
}

void fightTrainerAiDB_SetWazaRndSelectFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setSelectRandomWaza(value);
}

bool fightTrainerAiDB_GetWazaRndSelectFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getSelectRandomWaza();
}

void fightTrainerAiDB_SetDefensePokemonRndSelectFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setSelectRandomDefence(value);
}

bool fightTrainerAiDB_GetDefensePokemonRndSelectFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getSelectRandomDefence();
}

void fightTrainerAiDB_SetWazaDamageFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setCalcWazaDamage(value);
}

bool fightTrainerAiDB_GetWazaDamageFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getCalcWazaDamage();
}

void fightTrainerAiDB_SetTokuseiCheckFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setAbilityDetermination(value);
}

bool fightTrainerAiDB_GetTokuseiCheckFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getAbilityDetermination();
}

void fightTrainerAiDB_SetZokuseiCheckFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setAttrDetermination(value);
}

bool fightTrainerAiDB_GetZokuseiCheckFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getAttrDetermination();
}

void fightTrainerAiDB_SetKeyPlayerFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setRespectKeyPlayer(value);
}

bool fightTrainerAiDB_GetKeyPlayerFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getRespectKeyPlayer();
}

void fightTrainerAiDB_SetPokemonDataOrderAceBossFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setSelectAceBoss(value);
}

bool fightTrainerAiDB_GetPokemonDataOrderAceBossFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getSelectAceBoss();
}

void fightTrainerAiDB_SetPokemonDataOrderOutFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setOrder(value);
}

bool fightTrainerAiDB_GetPokemonDataOrderOutFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getOrder();
}

void fightTrainerAiDB_SetPokemonSelectWeakPointFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setNotSameWeekPoint(value);
}

bool fightTrainerAiDB_GetPokemonSelectWeakPointFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getNotSameWeekPoint();
}

void fightTrainerAiDB_SetPokemonSelectRandomFlag(u16 aiDataId, bool value)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return;
    }
    ai->setSelectRandomPokemon(value);
}

bool fightTrainerAiDB_GetPokemonSelectRandomFlag(u16 aiDataId)
{
    DeckTrainerAI *ai = DeckTrainerAI::getDeckTrainerAIObj(aiDataId);

    if (ai == 0) {
        return false;
    }
    return ai->getSelectRandomPokemon();
}

}
