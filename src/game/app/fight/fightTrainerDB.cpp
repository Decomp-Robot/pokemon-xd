#include <fightTrainer.h>

// The trainer record is read and written through the adapters of this module:
// the AI additional-value records the debug menus edit and the field queries of
// one battle-side trainer record.  A missing record is left alone when writing
// and reads as an empty one.

extern "C" {

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

}
