#ifndef FIGHT_TRAINER_AI_H
#define FIGHT_TRAINER_AI_H

#include <types.h>
#include <fightTrainer.h>

typedef struct FIGHT_FLOOR FIGHT_FLOOR;

#ifdef __cplusplus
extern "C" {
#endif

enum { FIGHT_AI_MAX_COMBINATIONS = 64 };
enum { FIGHT_AI_COMBO_ID_NONE = 0 };

typedef struct AI_ALLWAZA_VALID_TBL {
    FightOutPokemon *mAttacker[2];
    FightOutPokemon *mTarget[2];
    u16 mWazaDataId[2];
    s8 mStockWaza[2];
    u8 mDefenseSelection[2];
    u8 mSuccessProbability[2];
    u8 mCombinedDamageValue;
    u16 mDamage[2][2];
    u8 mAttackResult[2];
    u8 _26;
    u16 _28;
} AI_ALLWAZA_VALID_TBL;

enum FIGHT_AI_COMBO_MATCH {
    FIGHT_AI_COMBO_MATCH_ANY = 0,
    FIGHT_AI_COMBO_MATCH_WAZA = 1,
    FIGHT_AI_COMBO_MATCH_WAZA_TYPE = 2,
    FIGHT_AI_COMBO_MATCH_ZOKUSEI = 3
};

enum { FIGHT_AI_COMBO_MOVE_FIRST = 1 };

enum FIGHT_AI_TOKUSEI_HAVE_FLAG {
    FIGHT_AI_TOKUSEI_ABSENT = 0,
    FIGHT_AI_TOKUSEI_POSSIBLE = 1,
    FIGHT_AI_TOKUSEI_KNOWN = 2
};

typedef struct AI_ALLWAZA_VALID_WORK {
    u8 mPokemonCount;
    u8 _01[3];
} AI_ALLWAZA_VALID_WORK;

extern AI_ALLWAZA_VALID_WORK ai_allwaza_valid_work;
void fightAi_InitAllWazaValidSub(AI_ALLWAZA_VALID_TBL *record);
u8 fightAi_GetTokuseiHaveFlag(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 tokuseiDataId);
void fightAi_AllWazaValid_AddComboList(FIGHT_TRAINER *trainer, AI_ALLWAZA_VALID_TBL *records, u8 *count,
    u8 firstKind, s32 firstValue, u8 secondKind, s32 secondValue);
void fightAi_AllWazaValid_DelComboList(FIGHT_TRAINER *trainer, AI_ALLWAZA_VALID_TBL *records, u8 *count,
    u8 firstKind, s32 firstValue, u8 secondKind, s32 secondValue, u8 keepMatches);
void fightAi_AllWazaValid_DelListFromSpeed(FIGHT_TRAINER *trainer, AI_ALLWAZA_VALID_TBL *records,
    u8 *count, u16 wazaDataId, u8 requireMoveFirst);
s32 fightTrainerAiAddValueByAddsubDataId(s32 value, FIGHT_TRAINER *trainer, u16 addsubDataId);

u8 fightTrainerAiGetFightFloorNowTenkouDataId(FIGHT_TRAINER *trainer, FIGHT_FLOOR *floor, u8 checkAbilities);
u8 fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiGetFightOutPokemonAbiCntPhyDef(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiGetFightOutPokemonAbiCntSpeAtk(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiGetFightOutPokemonAbiCntSpeDef(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiGetFightOutPokemonAbiCntSubayasa(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiIsFightOutPokemonZokuseiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 typeId);
s16 fightTrainerAiGetWazaIryoku(FIGHT_TRAINER *trainer, u16 wazaDataId);
u16 fightTrainerAiGetFightOutPokemonZokuseiDataIdWazaJoutaiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 typeId, s16 power);

u8 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId(FIGHT_TRAINER *trainer, u16 joutaiDataId, u16 wazaDataId);
u16 fightTrainerAiCheckHorobinouta(FIGHT_TRAINER *trainer);
u8 fightTrainerAiCheckTextureZokusei(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
// Flags that select how the trainer AI checks a stat-stage change.
enum FIGHT_TRAINER_AI_CHG_FLAGS {
    FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT = 1,
    FIGHT_TRAINER_AI_CHG_KIND_MASK = 0x1F,
    FIGHT_TRAINER_AI_CHG_SKIP_GUARD = 0x20,
    FIGHT_TRAINER_AI_CHG_ON_ATTACKER = 0x40,
    FIGHT_TRAINER_AI_CHG_IGNORE_MIST_AND_STAT_DOWN_PREVENTION = 0x80
};

u8 fightTrainerAiCheckAbiCnt(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, FightOutPokemon *defender,
                             u16 wazaDataId, u8 paraId, u8 actType, u16 flags);
u8 fightTrainerAiCheckGuard(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 wazaDataId);
u16 fightTrainerAiCheckOumu(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiCheckSimerike(FIGHT_TRAINER *trainer);
u8 fightTrainerAiCheckSawagu(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);
u8 fightTrainerAiIsFightOutPokemonJoutaiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 joutaiDataId);
u8 fightTrainerAiIsFightOutPokemonTokuseiDataId(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 tokuseiDataId);

#ifdef __cplusplus
}
#endif

#endif
