#ifndef FIGHT_SEQ_H
#define FIGHT_SEQ_H

#include <types.h>
#include <bool.h>
#include <fightPokemon.h>
#include <fightAction.h>

struct FIGHT_TRAINER;
typedef struct FIGHT_SIDE FIGHT_SIDE;
typedef FightOutPokemon *(*FightPokemonSelectCallback)(FightOutPokemon *pokemon,
    u16 waza, u16 fightType);

typedef struct FIGHT_ACTION FIGHT_ACTION;
typedef u8 (*FightSeqCallback)(void *pokemon, u16 index, void *arg);
typedef void (*FightSeqWazaFunc)(FIGHT_TRAINER *trainer, u16 wazaDataId, FightOutPokemon *attackPokemon,
                                 FightPokemon *defensePokemon);

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SOUBI_ESCAPE_OK = 37,
    SOUBI_SCOPE_LENS = 41,
    SOUBI_LUCKY_PUNCH = 63,
    SOUBI_STICK = 66,
    SOUBI_CHOICE_BAND = 29
};
enum {
    FIGHT_TARGET_ID_OWN_SIDE = 2,
    FIGHT_TARGET_ID_OPPOSING_SIDE = 3,
    FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE = 14,
    FIGHT_TARGET_ID_ATTACKER = 17,
    FIGHT_TARGET_ID_DEFENDER = 18,
    FIGHT_TARGET_ID_HOST_TRAINER_1 = 11,
    FIGHT_TARGET_ID_OWN_SIDE_TRAINER_2 = 7,
    FIGHT_TARGET_ID_OPPONENT_TRAINER_1 = 9,
    FIGHT_TARGET_ID_OPPONENT_TRAINER_2 = 10
};
enum {
    JOUTAI_ID_DESTINY_BOND = 21,
    JOUTAI_ID_CHARGE = 36,
    JOUTAI_ID_INGRAIN = 37,
    JOUTAI_ID_GRUDGE = 40,
    JOUTAI_ID_ENCORE = 42,
    JOUTAI_ID_FURY_CUTTER = 46,
    JOUTAI_ID_HELPING_HAND = 50,
    JOUTAI_ID_SNATCH = 51,
    JOUTAI_ID_MAGIC_COAT = 55,
    JOUTAI_ID_FOLLOW_ME = 77,
    JOUTAI_ID_FLYING = 31
};
enum {
    FIGHT_SEQ_FLAG_ATTACKER_DEFENDER_SWAPPED = 0x1000,
    SEQ_WORK_STAT_ACTION = 90142,
    SEQ_WORK_STAT_MESSAGE = 90276,
    FIGHT_SEQ_WORK_WAZA_EFFECT_INVOCATION = 90273,
    FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION = 90114,
    FIGHT_MSG_STATUS_NAME = 13,
    FIGHT_MSG_STATUS_DEGREE = 14,
    FIGHT_MSG_STATUS_DIRECTION = 65,
    FIGHT_SEQ_FLAG_SKIP_DAMAGE_ACCOUNTING = 0x00100000
};
enum { FIGHT_STAT_STAGE_LIMIT = 12, FIGHT_STAT_ATTACK=1, FIGHT_STAT_DEFENSE=2, FIGHT_STAT_SPEED=3, FIGHT_STAT_SPECIAL_ATTACK=4, FIGHT_STAT_SPECIAL_DEFENSE=5, FIGHT_STAT_ACCURACY=6 };
enum { FIGHT_STAT_MESSAGE_LOWER_ONE = 21, FIGHT_STAT_MESSAGE_LOWER_MULTIPLE = 45, FIGHT_STAT_MESSAGE_RAISE_ONE = 14, FIGHT_STAT_MESSAGE_RAISE_MULTIPLE = 38 };
enum {
    WAZA_SEQUENCE_ID_BATON_PASS = 127,
    WAZA_SEQUENCE_ID_HIGH_CRITICAL = 43,
    WAZA_SEQUENCE_ID_SKY_ATTACK = 75,
    WAZA_SEQUENCE_ID_BLAZE_KICK = 200,
    WAZA_SEQUENCE_ID_POISON_TAIL = 209,
    WAZA_SEQUENCE_ID_SOLAR_BEAM = 151
};
enum { ITEM_KIND_POKEMON_BALL = 1, WAZA_RANGE_OTHER_POKEMON_ON_SIDE = 4 };
enum {
    JOUTAI_ID_CHOICE_ITEM_LOCK = 54,
    JOUTAI_PARTIAL_TRAPPING = 14,
    JOUTAI_FLINCH = 17,
    JOUTAI_TIMED_NO_ACTION = 18,
    JOUTAI_RAGE_ATTACK_BOOST = 19,
    JOUTAI_TRAPPING = 22,
    JOUTAI_CONTINUING_MOVE = 34,
    JOUTAI_DARK_HYPER_MODE = 62
};
enum { JOUTAI_POISON=3, JOUTAI_BAD_POISON=4, JOUTAI_PARALYSIS=5, JOUTAI_BURN=6, JOUTAI_FREEZE=7, JOUTAI_SLEEP=8, JOUTAI_CONFUSION=9, JOUTAI_INFATUATION=10, JOUTAI_FOCUS_ENERGY=15, JOUTAI_AKUMU=23, JOUTAI_LEECH_SEED=28, JOUTAI_GUARD=43, JOUTAI_ENDURE=44, JOUTAI_SUBSTITUTE=20, JOUTAI_MOVE_FAILED=64, JOUTAI_REFLECT=72 };
enum {
    SEQ_STATUS_PARAMETER = 3,
    SEQ_MULTIPLE_STATUS = 5,
    SEQ_MESSAGE_SHOWN = 7,
    SEQ_MOVE_MISS_REASON = 6,
    SEQ_ATTACK_MESSAGE_SHOWN = 0x400,
    FIGHT_SEQ_EFFECT_ANIMATION_FLAG = 0x80,
    FIGHT_SEQ_FLAG_ACTION_IN_PROGRESS = 0x02000000,
    FIGHT_SEQ_FLAG_SKIP_PP_DECREMENT = 0x800,
    FIGHT_SEQ_FLAG_BADGE_CHECK_ABORTED = 0x80000,
    FIGHT_SEQ_FLAG_REPLAYING_MOVE_ON_OTHER_BATTLER = 0x200,
    FIGHT_SEQ_FLAG_FAINTED_POKEMON_HANDLED = 0x400000
};
enum {
    FIGHT_SEQ_END_FLAG_RUNNING_END_CHECK = 1,
    FIGHT_SEQ_END_FLAG_FINISHED = 2
};
enum {
    FIGHT_MSG_SLOT_TRAINER_NAME = 19,
    FIGHT_MSG_SLOT_BLOCKING_ABILITY_NAME = 28,
    FIGHT_MSG_SLOT_BLOCKING_POKEMON = 30,
    FIGHT_MSG_SLOT_TRAINER_PREFIX = 34,
    FIGHT_MSG_SLOT_ITEM_TRAINER_NAME = 35,
    FIGHT_MSG_SLOT_MOVE_NAME = 40,
    FIGHT_MSG_SLOT_ITEM_NAME = 41
};
enum {
    FIGHT_SEQ_WORK_ESCAPE_ABILITY_HOLDER_INDEX = 90115,
    FIGHT_TARGET_ID_OPPOSING_SIDE_FIRST_POKEMON = 15,
    JOUTAI_ID_REGULATION_REJECTED_MOVE = 69
};
extern u8 fightSeqWazaWork[8];
extern u8 *UserWork;
extern u8 *_wazaSeqAdrs;
extern u32 ServerStatusFlag;
extern u8 *wazaSeqJumpTbl[];
extern u8 fightSeqWkcActStep;
extern u8 fightSeqJoutaiWazaSeq[];
extern u8 fightSeqHensinWazaSeq[];
extern u8 fightSeqHitWazaSeq[];
extern FIGHT_ACTION *fight_seq_now_fight_action_ptr;
u8 *fightSeq_GetWazaSeq(void);
void fightSeq_CallWazaSeq(u8 *sequence);
void fightSeq_CallWazaSeqCustom(FIGHT_ACTION *action, u8 *script, u8 mode);
u8 fightSeqFightActionCreate(FIGHT_ACTION *action, struct FIGHT_ACTION_DATA_dd_ *currentActionData, void *pokemon,
                             u16 kind, s32 bufferSelector, void *nextActionData, void *wazaSequence);
u8 fightSeqFightActionCreateAndFlowFifo(struct FIGHT_ACTION_DATA_dd_ *currentActionData, void *pokemon,
                                        u16 kind, s32 bufferSelector, void *nextActionData, void *wazaSequence);
void fightSeqTypeCheck(s32 modifyTypeDamage, s32 advanceSequence, s32 updateMoveResult,
                       s32 skipZeroPowerMoves, s32 scriptBytesToAdvance);
void fightSeqExec(FIGHT_ACTION *action, u8 *sequence, u8 mode);
void fightSeqPpdecMonomane(FightOutPokemon *pokemon, u8 slot, u8 mode);
s32 fightSeqWazaDamageCalcHikaePokemon(FightOutPokemon *attacker, FightPokemon *reservePokemon,
                                      FIGHT_SIDE *defenderSide, u16 wazaDataId, u16 iryoku, s32 zokusei);
u8 fightSeqSoubiItemCheckWazaHitAfter(FightOutPokemon *attacker, FightOutPokemon *defender);
void fightSeqFightOutPokemonTokuseiCheckSinkuroAtk(FightOutPokemon *attacker, FightOutPokemon *defender);
void fightSeqFightOutPokemonTokuseiCheckSinkuroDef(FightOutPokemon *attacker, FightOutPokemon *defender);
u8 fightSeqFightOutPokemonTokuseiCheckWazaHitAfter(FightOutPokemon *attacker, FightOutPokemon *defender);
void fightSeqFightOutPokemonTokuseiCheckWazaHitAfterAll(void);
void fightSeqSoubiItemCheckWazaHitAfterAll(void);
s32 fightSeqCondChgActTypeToPokemonStatusId(u8 type);
void fightSeq_NextWazaSeq(s32 count);
void fightSeq_JumpWazaSeq(u8 *sequence);
void fightSeqCheckSetHensinBuffToMotoPokemon(FightOutPokemon *pokemon, u32 field);
void fightSeqTuikaAct(u8 action, u8 mode);
bool fightSeqGetEffectAminFlag(void);
void fightSeqSetEffectAminFlag(u8 enabled);
u8 fightSeqCheckYubiwohuruWazaDataId(u16 wazaId);

FightOutPokemon *fightSeqSelectDefensePokemonPtr(FightOutPokemon *pokemon,
    u16 waza, u16 fightType, FightPokemonSelectCallback selectCallback,
    u8 selectPokemon, u8 checkRedirect, s8 targetType);

void CriticalCheckHikaePokemon(FightPokemon *pokemon);
void fightSeqTypeCheckHikaePokemon(FightPokemon *pokemon, u32 modifyTypeDamage,
                                   u32 updateMoveResult, u32 skipZeroPowerMoves);
u8 fightSeqTameWazaCheck(u16 waza);
void fightSeqFloorJoutaiCheck(void);
void fightSeqSideJoutaiCheck(void);
void fightSeqSideJoutaiCheck2(void);
void fightSeqGetExpCheck(void);
u8 fightSeqWazaOutBeforeCheckSub(void *pokemon, u16 index, void *arg);
void fightSeqClearOneTurnCount(u8 oneTurn);
void fightSeqSpecificationActionCounterInit(FightOutPokemon *pokemon);
void fightSeqTurnCheck(void);
void fightSeqClearOneSelfTurnCount(void);
void SCA_End(void);
void SCA_EndCheck(void);
u8 fightSeqGetItemType(u16 itemID, FightOutPokemon *pokemon);
void fightSeqLoopInitAct(FIGHT_WAZA *waza);
void WS_CRITICAL_CHECK(void);
void WS_DAMAGE_CALC(void);
void WS_TYPE_CHECK(void);
void WS_DAMAGE_LOSS(void);
void fightSeqVanishOff(FightOutPokemon *pokemon);

#ifdef __cplusplus
}
#endif

#endif
