#ifndef FIGHT_TRAINER_H
#define FIGHT_TRAINER_H

#include <hero.h>
#include <types.h>
#include <fightAction.h>
#include <fightOutPokemon.h>
#include <fightPokemon.h>
#include <app/fight/fightTrainerBios.h>

typedef struct FIGHT_TRAINER FIGHT_TRAINER;

/* What fightTrainerCheckCanIrekaeFightPokemon makes of swapping a fight Pokemon
   out; the fight packet carries the same codes to the GBA, one per slot. */
enum FightTrainerIrekaeState {
    FIGHT_TRAINER_IREKAE_SWAPPABLE = 0,    /* may be swapped out. */
    FIGHT_TRAINER_IREKAE_CANNOT_FIGHT = 1, /* cannot take part in the fight. */
    FIGHT_TRAINER_IREKAE_FIGHT_OUT = 2,    /* out on the field right now. */
    FIGHT_TRAINER_IREKAE_REFUSED = 3,      /* the reserve check turns the swap down. */
};

// Battle-side trainer record: the data IDs the trainer plays, the hero record
// the battle is fought with, and the party and action work that follows it.
#ifdef __cplusplus
struct FIGHT_TRAINER {
    FIGHT_TRAINER() {}

    u16 mDataId;                                  // data ID of the trainer.
    u16 mReserved;
    Hero mHero;                                   // hero record the fight is fought with.
    FightPokemon mFightPokemon[6];                // party slots, one pokemon per slot.
    FightOutPokemon mFightoutPokemon[2];          // the pokemon out on the field.
    FightOutPokemon mFightoutPokemonBuff;         // working copy of the one that is out.
    u8 mOkaneBai;                                 // prize money multiplier of the trainer.
    u8 mNigeruCount;                              // times the trainer has run away.
    u8 _353A[2];
    u32 mKoban;                                   // prize money the fight is worth.
    u8 mControllerId;                             // controller the fight is fought with.
    u8 _3541[3];
    struct NullSequence *mSequence;               // sequence that drives the trainer model.
    FIGHT_ACTION mFightActionBuff;                // action work of the fight.
    FightTrainerEnemyPokemon mEnemyPokemon[12];   // fight status of the party.
    u8 _3740[4];
};
#else
struct FIGHT_TRAINER {
    u16 mDataId;                                  // data ID of the trainer.
    u16 mReserved;
    Hero mHero;                                   // hero record the fight is fought with.
    FightPokemon mFightPokemon[6];                // party slots, one pokemon per slot.
    FightOutPokemon mFightoutPokemon[2];          // the pokemon out on the field.
    FightOutPokemon mFightoutPokemonBuff;         // working copy of the one that is out.
    u8 mOkaneBai;                                 // prize money multiplier of the trainer.
    u8 mNigeruCount;                              // times the trainer has run away.
    u8 _353A[2];
    u32 mKoban;                                   // prize money the fight is worth.
    u8 mControllerId;                             // controller the fight is fought with.
    u8 _3541[3];
    struct NullSequence *mSequence;               // sequence that drives the trainer model.
    FIGHT_ACTION mFightActionBuff;                // action work of the fight.
    FightTrainerEnemyPokemon mEnemyPokemon[12];   // fight status of the party.
    u8 _3740[4];
};
#endif

// Trainer AI additional-value record: the value the AI adds and the message
// IDs that name it in the debug menus.
typedef struct FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA {
    s32 mValue;
    u32 mKoudouName;
    u32 mPrefixName;
    u32 mName;
    u32 _10;
} FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA;

// Trainer pokemon part record: the message ID the record is listed with and
// the revision value of each move type.
typedef struct FIGHT_TRAINER_POKEMON_PART_DATA {
    u32 mName;
    u32 _04;
    u8 mWazaTypeRevise[14];
    u8 _16[2];
} FIGHT_TRAINER_POKEMON_PART_DATA;

// Trainer AI data record: the decision switches and the weights the AI of one
// of the trainer's pokemon fights with. The debug menu keeps a copy of the
// whole record while it is edited.
typedef struct FIGHT_TRAINER_AI_DATA {
    u8 mSelectRandomPokemon : 1;
    u8 mNotSameWeekPoint : 1;
    u8 mOrder : 1;
    u8 mSelectAceBoss : 1;
    u8 mRespectKeyPlayer : 1;
    u8 mAttrDetermination : 1;
    u8 mAbilityDetermination : 1;
    u8 mCalcWazaDamage : 1;
    u8 mSelectRandomDefence : 1;
    u8 mSelectRandomWaza : 1;
    u8 mRespectWazaAssmntVal : 1;
    u8 mRespectRole : 1;
    u8 mRespectWazaHit : 1;
    u8 mRespectWazaRiskJudge : 1;
    u8 mRespectPokemonCondJuge : 1;
    u8 _01 : 1;
    u8 _02[2];
    u8 mStatusDetermVal[8];  // Minimum in the low nibble, maximum in the high one.
    u8 mStatusDetermValTotalMax;
    u8 mStatusDetermValTotalMin;
    u8 mPokeChgRate;
    u8 mItemUseRate;
    u8 mHitRate;
    u8 mAnnihilateDetermRate;
    u8 mLowHPDetermRate;
    u8 mLastPPDetermRate;
    u8 mAttrTypeID[2];
    u8 mAttrTypeParam[2];
    u16 mWazaKindID[2];
    u8 mWazaKindParam[2];
    u8 mTotalCorrection;
} FIGHT_TRAINER_AI_DATA;

#ifdef __cplusplus
extern "C" {
#endif
u8 fightTrainerCheckValid(FIGHT_TRAINER *trainer);
u8 fightTrainerCheckDoFight(FIGHT_TRAINER *trainer);
s32 fightTrainerGetHikaeFightPokemonNum(
    FIGHT_TRAINER *trainer, u16 pokemonNum, u16 fightoutPokemonNum);
void fightTrainerInitAry(FIGHT_TRAINER *trainerAry, u16 trainerNum);
void fightTrainer_SetEnemyPokemonRegist(FIGHT_TRAINER *trainer, s16 entryId);
void fightTrainer_SetEnemyParam1bantakaiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 flag);
void fightTrainer_SetEnemyBadwazaHaveFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 flag);
void fightTrainer_SetEnemyDefense1banhikuiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 flag);
void fightTrainer_SetEnemyLv1banhikuiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 flag);
void fightTrainer_SetEnemyNowhp1banhikuiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 flag);
void fightTrainer_SetEnemyStoreTokuseiData(FIGHT_TRAINER *trainer, s16 entryId, u16 data);
void fightTrainer_SetEnemyTokuseiFlag(FIGHT_TRAINER *trainer, s16 entryId, u8 flag);
void fightTrainer_SetEnemyExpectWazaData(FIGHT_TRAINER *trainer, s16 entryId, u8 index, u16 dataId);
void fightTrainer_SetEnemyStoreWazaData(FIGHT_TRAINER *trainer, s16 entryId, u8 index, u16 dataId);
void fightTrainer_SetEnemyExpectWazaNum(FIGHT_TRAINER *trainer, s16 entryId, u8 count);
void fightTrainer_SetEnemyStoreWazaNum(FIGHT_TRAINER *trainer, s16 entryId, u8 count);
void fightTrainer_SetSequencePtr(FIGHT_TRAINER *trainer, struct NullSequence *sequence);
void fightTrainer_SetControllerId(FIGHT_TRAINER *trainer, s32 controllerId);
void fightTrainer_SetNigeruCount(FIGHT_TRAINER *trainer, s32 count);
void fightTrainer_SetKoban(FIGHT_TRAINER *trainer, s32 koban);
void fightTrainer_SetOkaneBai(FIGHT_TRAINER *trainer, s32 multiplier);
void fightTrainer_SetFightTrainerDataId(FIGHT_TRAINER *trainer, s32 dataId);
s32 fightTrainerGetStatus(FIGHT_TRAINER *trainer, u16 id, u16 field, u16 sub);
void fightTrainerSetStatus(FIGHT_TRAINER *trainer, u16 id, u16 field, u16 sub, s32 value);

struct FightPokemon *fightTrainerBiosGetFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot);
FightPokemon *fightTrainerGetValidFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot);
u8 fightTrainerCheckCanIrekaeFightPokemon(FIGHT_TRAINER *trainer, FightPokemon *pokemon);
FightOutPokemon *fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(FIGHT_TRAINER *trainer, FightPokemon *pokemon);
FightTrainerEnemyPokemon *fightTrainerGetFightEntryIdToEnemyPokemonPtr(FIGHT_TRAINER *trainer, s16 fightEntryId);
Hero *fightTrainerBiosGetHeroPtr(FIGHT_TRAINER *trainer);
FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *fightTrainerAiValueAddsubDataBiosGetPtr(u16 index);
FIGHT_TRAINER_POKEMON_PART_DATA *fightTrainerPokemonPartDataBiosGetPtr(u16 index);
FIGHT_TRAINER_AI_DATA *fightTrainerAiDataBiosGetPtr(u16 index);

enum FightTrainerMenuBallStatus {
    FIGHT_TRAINER_MENU_BALL_STATUS_NONE = 0,
    FIGHT_TRAINER_MENU_BALL_STATUS_NORMAL = 1,
    FIGHT_TRAINER_MENU_BALL_STATUS_ABNORMAL = 2,
    FIGHT_TRAINER_MENU_BALL_STATUS_FIGHT_OUT = 3
};

void fightTrainerSetNameHearFlag(u16 encountNo);
u16 fightTrainerGetTemotiNormalItemDataIdAry(FIGHT_TRAINER *trainer,
                                             u16 *itemDataIdAry,
                                             u16 capacity,
                                             u8 excludeSelected);
u8 fightTrainerIsUsedItem(FIGHT_TRAINER *trainer);
FightPokemon *fightTrainerSearchInvalidFightPokemonPtr(FIGHT_TRAINER *trainer,
                                                       u16 entryPokemonNum);
void fightTrainerCreateFightTrainerDataIdToHero(u16 trainerDataId, s32 controllerId, Hero *hero, u16 battleId);
u8 fightTrainerIsSelectedItemBall(FIGHT_TRAINER *trainer);
u8 fightTrainerIsGcHero(FIGHT_TRAINER *trainer);
extern u32 *fight_trainer_pokemon_part_data_number;
extern u32 *fight_trainer_ai_value_addsub_data_number;

u16 *fightTrainerGetPrefixNamePtr(FIGHT_TRAINER *trainer);
u16 *fightTrainerGetNamePtr(FIGHT_TRAINER *trainer);
void fightTrainerCallEffect(FIGHT_TRAINER *trainer, u8 operation);
u8 fightTrainerIsAllyFightTargetPtr(FIGHT_TRAINER *trainer, FightOutPokemon *target, u16 fightType);
void fightTrainerToMenuBallStatus(FIGHT_TRAINER *trainer, u8 *ballStatus);

/* Field queries of one battle-side trainer record.  A missing record reads as
   an empty value. */
u16 fightTrainer_GetFightTrainerDataId(FIGHT_TRAINER *trainer);
u8 fightTrainer_GetControllerId(FIGHT_TRAINER *trainer);
u8 fightTrainer_GetNigeruCount(FIGHT_TRAINER *trainer);
u32 fightTrainer_GetKoban(FIGHT_TRAINER *trainer);
u8 fightTrainer_GetOkaneBai(FIGHT_TRAINER *trainer);
struct NullSequence *fightTrainer_GetSequencePtr(FIGHT_TRAINER *trainer);
FIGHT_ACTION *fightTrainer_GetFightActionBuffPtr(FIGHT_TRAINER *trainer);
Hero *fightTrainer_GetHeroPtr(FIGHT_TRAINER *trainer);
FightPokemon *fightTrainer_GetFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot);
FightOutPokemon *fightTrainer_GetFightoutPokemonPtr(FIGHT_TRAINER *trainer, u16 slot);
FightOutPokemon *fightTrainer_GetFightoutPokemonBuffPtr(FIGHT_TRAINER *trainer);
FightTrainerEnemyPokemon *fightTrainer_GetFightTrainerEnemyPokemonAryPtr(FIGHT_TRAINER *trainer,
                                                                        u8 index);

/* Field queries of one of the enemy pokemon records of a trainer. */
u8 fightTrainer_GetEnemyParam1bantakaiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId);
u8 fightTrainer_GetEnemyBadwazaHaveFlag(FIGHT_TRAINER *trainer, s16 fightEntryId);
u8 fightTrainer_GetEnemyDefense1banhikuiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId);
u8 fightTrainer_GetEnemyLv1banhikuiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId);
u8 fightTrainer_GetEnemyNowhp1banhikuiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId);
u16 fightTrainer_GetEnemyStoreTokuseiData(FIGHT_TRAINER *trainer, s16 fightEntryId);
u8 fightTrainer_GetEnemyTokuseiFlag(FIGHT_TRAINER *trainer, s16 fightEntryId);
u16 fightTrainer_GetEnemyExpectWazaData(FIGHT_TRAINER *trainer, s16 fightEntryId, u8 index);
u16 fightTrainer_GetEnemyStoreWazaData(FIGHT_TRAINER *trainer, s16 fightEntryId, u8 index);
u8 fightTrainer_GetEnemyStoreWazaNum(FIGHT_TRAINER *trainer, s16 fightEntryId);

/* One word of the trainer AI additional-value records the debug menus edit; a
   missing record reads as zero and ignores the update. */
u32 fightTrainerAiValueAddsubDB_GetPrefixName(u16 index);
u32 fightTrainerAiValueAddsubDB_GetKoudouName(u16 index);
u32 fightTrainerAiValueAddsubDB_GetName(u16 index);
s32 fightTrainerAiValueAddsubDB_GetValue(u16 index);
void fightTrainerAiValueAddsubDB_SetPrefixName(u16 index, u32 prefixName);
void fightTrainerAiValueAddsubDB_SetKoudouName(u16 index, u32 koudouName);
void fightTrainerAiValueAddsubDB_SetName(u16 index, u32 name);
void fightTrainerAiValueAddsubDB_SetValue(u16 index, s32 value);

void fightTrainerAiDB_SetParamStoreFlag(u16 aiDataId, u8 flag);
u8 fightTrainerAiDB_GetParamStoreFlag(u16 aiDataId);
void fightTrainerAiDB_SetParamExpectFlag(u16 aiDataId, u8 flag);
u8 fightTrainerAiDB_GetParamExpectFlag(u16 aiDataId);
void fightTrainerAiDB_SetComboValue(u16 aiDataId, u8 value);
u8 fightTrainerAiDB_GetComboValue(u16 aiDataId);
void fightTrainerAiDB_SetLastValueRevise(u16 aiDataId, u8 correction);
u8 fightTrainerAiDB_GetLastValueRevise(u16 aiDataId);
void fightTrainerAiDB_SetWazaTypeReviseValue(u16 aiDataId, u8 slot, u8 value);
u8 fightTrainerAiDB_GetWazaTypeReviseValue(u16 aiDataId, u8 slot);
void fightTrainerAiDB_SetWazaTypeReviseTypeDataId(u16 aiDataId, u8 slot, u8 typeDataId);
u8 fightTrainerAiDB_GetWazaTypeReviseTypeDataId(u16 aiDataId, u8 slot);
void fightTrainerAiDB_SetZokuseiReviseValue(u16 aiDataId, u8 slot, u8 value);
u8 fightTrainerAiDB_GetZokuseiReviseValue(u16 aiDataId, u8 slot);
void fightTrainerAiDB_SetZokuseiReviseZokuseiDataId(u16 aiDataId, u8 slot, u8 zokuseiDataId);
u8 fightTrainerAiDB_GetZokuseiReviseZokuseiDataId(u16 aiDataId, u8 slot);
void fightTrainerAiDB_SetWazaNokoriPpValue(u16 aiDataId, u8 rate);
u8 fightTrainerAiDB_GetWazaNokoriPpValue(u16 aiDataId);
void fightTrainerAiDB_SetNokoriHpValue(u16 aiDataId, u8 rate);
u8 fightTrainerAiDB_GetNokoriHpValue(u16 aiDataId);
u8 fightTrainerAiDB_GetStrokeValue(u16 aiDataId);
void fightTrainerAiDB_SetWazaAvgValue(u16 aiDataId, u8 rate);
u8 fightTrainerAiDB_GetWazaAvgValue(u16 aiDataId);
void fightTrainerAiDB_SetItemValue(u16 aiDataId, u8 rate);
u8 fightTrainerAiDB_GetItemValue(u16 aiDataId);
void fightTrainerAiDB_SetIrekaeValue(u16 aiDataId, u8 rate);
u8 fightTrainerAiDB_GetIrekaeValue(u16 aiDataId);
void fightTrainerAiDB_SetAbicntMinValue(u16 aiDataId, u8 value);
u8 fightTrainerAiDB_GetAbicntMinValue(u16 aiDataId);
void fightTrainerAiDB_SetAbicntMaxValue(u16 aiDataId, u8 value);
u8 fightTrainerAiDB_GetAbicntMaxValue(u16 aiDataId);
u8 fightTrainerAiDB_GetAvoidMinValue(u16 aiDataId);
u8 fightTrainerAiDB_GetAvoidMaxValue(u16 aiDataId);
u8 fightTrainerAiDB_GetNimblenessMaxValue(u16 aiDataId);
u8 fightTrainerAiDB_GetSpeDefMaxValue(u16 aiDataId);
u8 fightTrainerAiDB_GetSpeAtkMaxValue(u16 aiDataId);
u8 fightTrainerAiDB_GetPhyDefMaxValue(u16 aiDataId);
u8 fightTrainerAiDB_GetPhyAtkMaxValue(u16 aiDataId);
void fightTrainerAiDB_SetPokemonJoutaiFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetPokemonJoutaiFlag(u16 aiDataId);
void fightTrainerAiDB_SetWazaRiskFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetWazaRiskFlag(u16 aiDataId);
void fightTrainerAiDB_SetWazaHitFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetWazaHitFlag(u16 aiDataId);
void fightTrainerAiDB_SetPartFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetPartFlag(u16 aiDataId);
void fightTrainerAiDB_SetWazaInitValueFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetWazaInitValueFlag(u16 aiDataId);
void fightTrainerAiDB_SetWazaRndSelectFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetWazaRndSelectFlag(u16 aiDataId);
void fightTrainerAiDB_SetDefensePokemonRndSelectFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetDefensePokemonRndSelectFlag(u16 aiDataId);
void fightTrainerAiDB_SetWazaDamageFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetWazaDamageFlag(u16 aiDataId);
void fightTrainerAiDB_SetTokuseiCheckFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetTokuseiCheckFlag(u16 aiDataId);
void fightTrainerAiDB_SetZokuseiCheckFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetZokuseiCheckFlag(u16 aiDataId);
void fightTrainerAiDB_SetKeyPlayerFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetKeyPlayerFlag(u16 aiDataId);
void fightTrainerAiDB_SetPokemonDataOrderAceBossFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetPokemonDataOrderAceBossFlag(u16 aiDataId);
void fightTrainerAiDB_SetPokemonDataOrderOutFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetPokemonDataOrderOutFlag(u16 aiDataId);
void fightTrainerAiDB_SetPokemonSelectWeakPointFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetPokemonSelectWeakPointFlag(u16 aiDataId);
void fightTrainerAiDB_SetPokemonSelectRandomFlag(u16 aiDataId, bool value);
bool fightTrainerAiDB_GetPokemonSelectRandomFlag(u16 aiDataId);

#ifdef __cplusplus
}
#endif

#endif
