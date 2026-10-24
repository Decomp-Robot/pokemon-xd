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
    SOUBI_CHOICE_BAND = 0x1D
};
enum {
    FIGHT_TARGET_ID_OWN_SIDE = 2,
    FIGHT_TARGET_ID_OPPOSING_SIDE = 3,
    FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE = 0xE,
    FIGHT_TARGET_ID_ATTACKER = 0x11,
    FIGHT_TARGET_ID_DEFENDER = 0x12
};
enum {
    JOUTAI_ID_DESTINY_BOND = 0x15,
    JOUTAI_ID_CHARGE = 0x24,
    JOUTAI_ID_INGRAIN = 0x25,
    JOUTAI_ID_GRUDGE = 0x28,
    JOUTAI_ID_ENCORE = 0x2A,
    JOUTAI_ID_FURY_CUTTER = 0x2E,
    JOUTAI_ID_HELPING_HAND = 0x32,
    JOUTAI_ID_SNATCH = 0x33,
    JOUTAI_ID_MAGIC_COAT = 0x37,
    JOUTAI_ID_FOLLOW_ME = 0x4D,
    JOUTAI_ID_FLYING = 0x1F
};
enum {
    FIGHT_SEQ_FLAG_ATTACKER_DEFENDER_SWAPPED = 0x1000,
    SEQ_WORK_STAT_ACTION = 0x1601E,
    SEQ_WORK_STAT_MESSAGE = 0x160A4,
    FIGHT_SEQ_WORK_WAZA_EFFECT_INVOCATION = 0x160A1,
    FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION = 0x16002,
    FIGHT_MSG_STATUS_NAME = 13,
    FIGHT_MSG_STATUS_DEGREE = 0x0E,
    FIGHT_MSG_STATUS_DIRECTION = 0x41,
    FIGHT_SEQ_FLAG_SKIP_DAMAGE_ACCOUNTING = 0x00100000
};
enum { FIGHT_STAT_STAGE_LIMIT = 0x0C, FIGHT_STAT_ATTACK=1, FIGHT_STAT_DEFENSE=2, FIGHT_STAT_SPEED=3, FIGHT_STAT_SPECIAL_ATTACK=4, FIGHT_STAT_SPECIAL_DEFENSE=5, FIGHT_STAT_ACCURACY=6 };
enum { FIGHT_STAT_MESSAGE_LOWER_ONE = 0x15, FIGHT_STAT_MESSAGE_LOWER_MULTIPLE = 0x2D, FIGHT_STAT_MESSAGE_RAISE_ONE = 0x0E, FIGHT_STAT_MESSAGE_RAISE_MULTIPLE = 0x26 };
enum {
    WAZA_SEQUENCE_ID_BATON_PASS = 0x7F,
    WAZA_SEQUENCE_ID_HIGH_CRITICAL = 0x2B,
    WAZA_SEQUENCE_ID_SKY_ATTACK = 0x4B,
    WAZA_SEQUENCE_ID_BLAZE_KICK = 0xC8,
    WAZA_SEQUENCE_ID_POISON_TAIL = 0xD1,
    WAZA_SEQUENCE_ID_SOLAR_BEAM = 0x97
};
enum { ITEM_KIND_POKEMON_BALL = 1, WAZA_RANGE_OTHER_POKEMON_ON_SIDE = 4 };
enum {
    JOUTAI_ID_CHOICE_ITEM_LOCK = 0x36,
    JOUTAI_PARTIAL_TRAPPING = 0x0E,
    JOUTAI_FLINCH = 0x11,
    JOUTAI_TIMED_NO_ACTION = 0x12,
    JOUTAI_RAGE_ATTACK_BOOST = 0x13,
    JOUTAI_TRAPPING = 0x16,
    JOUTAI_CONTINUING_MOVE = 0x22,
    JOUTAI_DARK_HYPER_MODE = 0x3E
};
enum { JOUTAI_POISON=3, JOUTAI_BAD_POISON=4, JOUTAI_PARALYSIS=5, JOUTAI_BURN=6, JOUTAI_FREEZE=7, JOUTAI_SLEEP=8, JOUTAI_CONFUSION=9, JOUTAI_INFATUATION=10, JOUTAI_FOCUS_ENERGY=15, JOUTAI_AKUMU=23, JOUTAI_LEECH_SEED=0x1C, JOUTAI_GUARD=0x2B, JOUTAI_ENDURE=0x2C, JOUTAI_SUBSTITUTE=0x14, JOUTAI_MOVE_FAILED=0x40, JOUTAI_REFLECT=0x48 };
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
    FIGHT_MSG_SLOT_TRAINER_NAME = 0x13,
    FIGHT_MSG_SLOT_BLOCKING_ABILITY_NAME = 0x1C,
    FIGHT_MSG_SLOT_BLOCKING_POKEMON = 0x1E,
    FIGHT_MSG_SLOT_TRAINER_PREFIX = 0x22,
    FIGHT_MSG_SLOT_ITEM_TRAINER_NAME = 0x23,
    FIGHT_MSG_SLOT_MOVE_NAME = 0x28,
    FIGHT_MSG_SLOT_ITEM_NAME = 0x29
};
enum {
    FIGHT_SEQ_WORK_ESCAPE_ABILITY_HOLDER_INDEX = 0x16003,
    FIGHT_TARGET_ID_OPPOSING_SIDE_FIRST_POKEMON = 0xF,
    JOUTAI_ID_REGULATION_REJECTED_MOVE = 0x45
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
