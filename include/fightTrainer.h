#ifndef FIGHT_TRAINER_H
#define FIGHT_TRAINER_H

#include <hero.h>
#include <types.h>

typedef struct FightPokemon FightPokemon;
typedef struct FightOutPokemon FightOutPokemon;
typedef struct FIGHT_TRAINER FIGHT_TRAINER;

// Battle-side trainer record: the data IDs the trainer plays, the hero record
// the battle is fought with, and the party and action work that follows it.
#ifdef __cplusplus
struct FIGHT_TRAINER {
    FIGHT_TRAINER() {}

    u16 mDataId;
    u16 mReserved;
    Hero mHero;
    u8 mWork[0x2BC8];
    struct NullSequence *mSequence;  // 0x3544: sequence that drives the trainer model.
    u8 _3548[0x1FC];
};
#else
struct FIGHT_TRAINER {
    u16 mDataId;
    u16 mReserved;
    Hero mHero;
    u8 mWork[0x2BC8];
    struct NullSequence *mSequence;  // 0x3544: sequence that drives the trainer model.
    u8 _3548[0x1FC];
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
void fightTrainer_SetOkaneBai(FIGHT_TRAINER *trainer, s32 multiplier);
s32 fightTrainerGetStatus(FIGHT_TRAINER *trainer, u16 id, u16 field, u16 sub);
void fightTrainerSetStatus(FIGHT_TRAINER *trainer, u16 id, u16 field, u16 sub, s32 value);

struct FightPokemon *fightTrainerBiosGetFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot);
FightPokemon *fightTrainerGetValidFightPokemonPtr(FIGHT_TRAINER *trainer, u16 slot);
u8 fightTrainerCheckCanIrekaeFightPokemon(FIGHT_TRAINER *trainer, FightPokemon *pokemon);
FightOutPokemon *fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(FIGHT_TRAINER *trainer, FightPokemon *pokemon);
Hero *fightTrainerBiosGetHeroPtr(FIGHT_TRAINER *trainer);
FIGHT_TRAINER_AI_VALUE_ADDSUB_DATA *fightTrainerAiValueAddsubDataBiosGetPtr(u16 index);
FIGHT_TRAINER_POKEMON_PART_DATA *fightTrainerPokemonPartDataBiosGetPtr(u16 index);
FIGHT_TRAINER_AI_DATA *fightTrainerAiDataBiosGetPtr(u16 index);

void fightTrainerCreateFightTrainerDataIdToHero(u16 trainerDataId, s32 controllerId, Hero *hero, u32 battleId);
u8 fightTrainerIsSelectedItemBall(FIGHT_TRAINER *trainer);
extern u32 *fight_trainer_pokemon_part_data_number;
extern u32 *fight_trainer_ai_value_addsub_data_number;

Hero *fightTrainer_GetHeroPtr(FIGHT_TRAINER *trainer);
u16 *fightTrainerGetPrefixNamePtr(FIGHT_TRAINER *trainer);
u16 *fightTrainerGetNamePtr(FIGHT_TRAINER *trainer);
void fightTrainerCallEffect(FIGHT_TRAINER *trainer, u8 operation);
u8 fightTrainerIsAllyFightTargetPtr(FIGHT_TRAINER *trainer, FightOutPokemon *target, u16 fightType);
void fightTrainerToMenuBallStatus(FIGHT_TRAINER *trainer, u8 *ballStatus);

#ifdef __cplusplus
}
#endif

#endif
