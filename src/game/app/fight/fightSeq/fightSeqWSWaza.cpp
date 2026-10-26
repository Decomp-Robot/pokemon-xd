#include <types.h>
#include <fightSeq.h>
#include <fightAction.h>
#include <Pokemon.h>
#include <GSgfx.h>
#include <fightTrainer.h>
#include <heroItem.h>
#include <fightTarget.h>
#include <fightFloor.h>
#include <fightSide.h>
#include <fight.h>
#include <fightMenu.h>
#include <fightEncountData.h>
#include <fightPokemon.h>
#include <pokemonDB.h>
#include <pokemonBios.h>
#include <itemData.h>
#include <itemFightItem.h>
#include <fightPokemonBios.h>
#include <wazaSequenceSys.h>
#include <itemParam.h>
#include <GSmsg.h>
#include <waza.h>
#include <fightWaza.h>
#include <hsdRandom.h>
// Item-effect sequence selectors.
enum FightSeqItemType {
    FIGHT_SEQ_ITEM_TYPE_NONE = 7,
    FIGHT_SEQ_ITEM_TYPE_SPECIAL = 1,
    FIGHT_SEQ_ITEM_TYPE_HP_UP = 2,
    FIGHT_SEQ_ITEM_TYPE_STATUS_CURE = 3,
    FIGHT_SEQ_ITEM_TYPE_CRITICAL = 4,
    FIGHT_SEQ_ITEM_TYPE_STAT_UP = 5,
    FIGHT_SEQ_ITEM_TYPE_GUARD = 6
};
enum { FIGHT_BATTLE_TYPE_ESCAPE = 12 };

/*
 * fightSeqWSWaza.cpp
 *
 * The waza phase of the fight sequence: the calls the message and menu layers
 * make while a turn is played out. fightSeqInit resets the sequence work and
 * the fight sides, fightSeqPre/fightSeqExec step the sequence table of the
 * waza the attacker chose, and the SCA helpers end a sequence and report its
 * result. The damage entry points evaluate a waza on a reserve Pokemon without
 * disturbing the state of the turn that is being played.
 */


// Weather codes used by the fight floor.
enum FightWeatherId { FIGHT_WEATHER_SUNLIGHT = 1 };

// Operations accepted by the move-sequence helpers.
enum FightSequenceOperation {
    FIGHT_SEQUENCE_OPERATION_LOAD = 0,
    FIGHT_SEQUENCE_OPERATION_START = 1,
    FIGHT_SEQUENCE_OPERATION_WAIT = 2,
    FIGHT_SEQUENCE_OPERATION_FREE = 3
};

enum FightEscapeResult {
    FIGHT_ESCAPE_ALLOWED = 0,
    FIGHT_ESCAPE_BLOCKED_BY_STATUS = 1,
    FIGHT_ESCAPE_BLOCKED_BY_ABILITY = 2
};
enum { FIGHT_STAT_SELECTOR_MASK = 0x0F };

// Selectors stored in the item-effect sequence work slot.
enum FightSeqItemEffectSelector {
    FIGHT_SEQ_ITEM_EFFECT_CONFUSION = 0,
    FIGHT_SEQ_ITEM_EFFECT_PARALYSIS = 1,
    FIGHT_SEQ_ITEM_EFFECT_FREEZE = 2,
    FIGHT_SEQ_ITEM_EFFECT_BURN = 3,
    FIGHT_SEQ_ITEM_EFFECT_POISON = 4,
    FIGHT_SEQ_ITEM_EFFECT_SLEEP = 5,
    FIGHT_SEQ_ITEM_EFFECT_CRITICAL = 5,
    FIGHT_SEQ_ITEM_EFFECT_STAT_CHANGE = 4
};
enum FightStatMessageResource {
    FIGHT_STAT_TEXT_ONE_STAGE = 20380,
    FIGHT_STAT_TEXT_LOWER_MULTIPLE = 20241,
    FIGHT_STAT_TEXT_RAISE_MULTIPLE = 20239,
    FIGHT_STAT_TEXT_LOWER_DIRECTION = 20242,
    FIGHT_STAT_TEXT_RAISE_DIRECTION = 20240
};

enum FightSeqItemDataId {
    ITEM_DATA_ID_FULL_RESTORE = 19,
    ITEM_DATA_ID_POKE_DOLL = 80,
    ITEM_DATA_ID_FLUFFY_TAIL = 81
};
enum FightSeqMoveResultId { JOUTAI_MOVE_RESULT_FIRST = 63, JOUTAI_MOVE_RESULT_LAST = 71 };
enum FightSeqCallMessageId {
    FIGHT_CALL_MSG_CAME_TO_SENSES = 20434,
    FIGHT_CALL_MSG_EAGER_RESPONSE = 20435,
    FIGHT_CALL_MSG_IGNORING_ORDERS = 20436,
    FIGHT_CALL_MSG_TRAINER_CALLS_POKEMON = 20432,
    FIGHT_CALL_MSG_CALLED_POKEMON_NAME = 20433,
    FIGHT_CALL_MSG_STAT_RAISE = 20243,
    FIGHT_CALL_MSG_STAT_LIMIT = 20248,
    FIGHT_CALL_MSG_ACCURACY = 20345
};
enum { POKEMON_RELIVE_FILTER_CALL = 2 };

// Sequence flag bits an ended sequence clears.
#define FIGHT_SEQ_END_CLEAR_BITS 0x0E176D50

extern u8 fightSeqWazaEffectFlag;
extern u8 fightSeqWazaHitFlag;
extern u8 fightSeqAttackJoutaiStep;
extern FIGHT_ACTION_DATA_dd_ fight_action_data_waza[2];
extern u8 fightSeqEndFlag;
extern FIGHT_ACTION_DATA_dd_ fight_action_data_turn[8];
extern u8 fightSeqTurnWazaSeq[];
extern u8 fightSeqIrekaeWazaSeq[];
extern void (*WazaSequenceTbl[])(void);
extern u32 fight_waza_critical_data_number;
extern u8 *fightSeqItemWazaSeqTbl[13];
extern u8 *fightSeqItemTypeWazaSeqTbl[8];
extern u8 *fightSeqItemSpecialWazaSeqTbl[];
extern s32 AbiCntMsgTbl[9];



// File local loops run once per fight side Pokemon.
static u8 _fightSeqInitSubFightOutPokemon(void *pokemon, u16 index, void *arg);
static u8 _fightSeqPreSub(void *pokemon, u16 index, void *arg);
static u8 _fightSeqTurnCheckSubFightOutPokemon(void *pokemon, u16 index, void *arg);
static u8 _fightSeqClearOneTurnCountSub(void *pokemon, u16 index, void *arg);
static u8 _fightSeqClearOneSelfTurnCountSub(void *pokemon, u16 index, void *arg);




static u8 _fightSeqClearOneSelfTurnCountSub(void *pokemon, u16 index, void *arg)
{
    fightOutPokemonInitOneSelfTurn((FightOutPokemon *)pokemon);
    return 1;
}

extern "C" void fightSeqClearOneSelfTurnCount(void) {
    fightFloorLoopValidFightOutPokemon(0, _fightSeqClearOneSelfTurnCountSub, 0, 0);
}

static u8 _fightSeqClearOneTurnCountSub(void *pokemon, u16 index, void *arg)
{
    u8 flag;
    u16 count;
    s8 num;
    s8 next;

    flag = *(u8 *)arg;
    if (flag == 1) {
        fightOutPokemonInitJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_GUARD);
        fightOutPokemonInitJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_ENDURE);
    } else {
        fightOutPokemonInitOneTurn((FightOutPokemon *)pokemon);
        fightOutPokemonInitJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_ID_HELPING_HAND);
        fightOutPokemonInitJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_ID_MAGIC_COAT);
        fightOutPokemonInitJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_ID_SNATCH);
        count = fightOutPokemon_GetFightoutTurnCount((FightOutPokemon *)pokemon);
        if (count != 0) {
            fightOutPokemon_SetFightoutTurnCount((FightOutPokemon *)pokemon, count - 1);
        }
        if (fightOutPokemonIsJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_TIMED_NO_ACTION) == 1) {
            num = fightOutPokemonGetNowKaisuuJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_TIMED_NO_ACTION);
            if ((next = (s8)num + 1) >= fightOutPokemonGetKaisuuJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_TIMED_NO_ACTION)) {
                fightOutPokemonInitJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_TIMED_NO_ACTION);
            } else {
                fightOutPokemonSetNowKaisuuJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_TIMED_NO_ACTION, next);
            }
        }
    }
    if (fightOutPokemonIsJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_SUBSTITUTE) == 1 && fightOutPokemonGetJoutaiMigawariHp((FightOutPokemon *)pokemon) <= 0) {
        fightOutPokemonInitJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_SUBSTITUTE);
    }
    return 1;
}

extern "C" void fightSeqClearOneTurnCount(u8 oneTurn) {
    fightFloorLoopValidFightOutPokemon(0, _fightSeqClearOneTurnCountSub, &oneTurn, 0);

    for (u16 i = 0; i < 2; i++) {
        FIGHT_SIDE *side = fightFloorGetValidFightSidePtr(0, i);

        if (side != 0) {
            fightSideInitJoutaiDataId(side, JOUTAI_ID_FOLLOW_ME);
        }
    }
}

static u8 _fightSeqTurnCheckSubFightOutPokemon(void *pokemon, u16 index, void *arg)
{
    if (fightOutPokemonCheckFightOut((FightOutPokemon *)pokemon) == 0) {
        return 1;
    }
    fightOutPokemonInitJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_FLINCH);
    if (fightOutPokemonIsJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_SLEEP) == 1 && fightOutPokemonIsJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_CONTINUING_MOVE) == 1) {
        fightOutPokemonInitJoutaiKeep((FightOutPokemon *)pokemon);
    }
    return 1;
}

extern "C" void fightSeqTurnCheck(void)
{
    u8 i;
    u16 turn;

    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fightMenuCloseMsg();
    for (i = 0; i < 8; i++) {
        fightSeqWazaWork[i] = 0;
    }
    fightFloorLoopValidFightOutPokemon(0, _fightSeqTurnCheckSubFightOutPokemon, 0, 0);
    fightFloorInitFightOutPokemonPtrAryFightWazaJoutai(0);
    fightSeqClearOneTurnCount(1);
    fightSeqFloorJoutaiCheck();
    fightSeqSideJoutaiCheck();
    fightSeqGetExpCheck();
    fightSeqSideJoutaiCheck2();
    fightSeqClearOneTurnCount(0);
    ServerStatusFlag &= ~FIGHT_SEQ_FLAG_REPLAYING_MOVE_ON_OTHER_BATTLER;
    ServerStatusFlag &= ~FIGHT_SEQ_FLAG_BADGE_CHECK_ABORTED;
    ServerStatusFlag &= ~FIGHT_SEQ_FLAG_FAINTED_POKEMON_HANDLED;
    ServerStatusFlag &= ~FIGHT_SEQ_FLAG_SKIP_DAMAGE_ACCOUNTING;
    UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION] = 0;
    UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_INVOCATION] = 0;
    for (i = 0; i < 5; i++) {
        fightSeqWazaWork[i] = 0;
    }
    if (fightGetFightResultId() == 0) {
        turn = fightFloor_GetTurnCount(0);
        turn++;
        if (turn > 255) {
            turn = 255;
        }
        fightFloor_SetTurnCount(0, turn);
        fightFloor_SetFisstAttackRnd(0, (u16)HSD_Rand());
        wazaSequenceSysFreeAllWaza();
        fightMenuAllFightTrainerCloseStatusMenu(0);
        fightMenuAllFightOutPokemonCloseStatusMenu(0);
        fightMenuCloseMsg();
    }
}

extern "C" u8 EscapeNGCheck(FightOutPokemon *fightOutPokemon)
{
    u16 itemDataId;
    u16 ability;
    u16 holdItemId;
    u8 cnt;
    s32 i;
    Pokemon *pokemon;
    void *fightData;
    FightOutPokemon *ary[2];

    pokemon = fightOutPokemonGetPokemonPtr(fightOutPokemon);
    fightData = fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(0));
    itemDataId = pokemonGetSoubiItemDataId(pokemon);
    ability = pokemonGetTokuseiDataId(pokemon);
    cnt = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, fightOutPokemon), ary, 0, 1);
    holdItemId = pokemonGetSoubiItemSoubiDataId((Pokemon *)pokemon);
    fightMsgctrlSetValue(FIGHT_MSG_SLOT_ITEM_NAME, (u32)(unsigned long)GSmsgGetStringPtr(itemDB_GetName(itemDataId)));
    if ((u8)fightEncountDataBiosGetFightKind(fightData) != FIGHT_BATTLE_TYPE_ESCAPE) {
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
        return FIGHT_ESCAPE_BLOCKED_BY_STATUS;
    }
    if (holdItemId == SOUBI_ESCAPE_OK
        || fightFloorIsNowEncountTuusinTaisen(0) == 1
        || ability == TOKUSEI_DATA_ID_RUN_AWAY) {
        return 0;
    }
    for (i = 0; i < cnt; i++) {
        u16 tokusei = fightOutPokemon_GetTokuseiDataId(ary[i]);

        if (tokusei == TOKUSEI_DATA_ID_SHADOW_TAG) {
            UserWork[FIGHT_SEQ_WORK_ESCAPE_ABILITY_HOLDER_INDEX] = i;
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_ABILITY_NAME, (u32)(unsigned long)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(tokusei))));
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_POKEMON, (u32)(unsigned long)ary[i]);
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_ESCAPE_BLOCKED_BY_ABILITY;
            return FIGHT_ESCAPE_BLOCKED_BY_ABILITY;
        }
        if (tokusei == TOKUSEI_DATA_ID_ARENA_TRAP && ability != TOKUSEI_DATA_ID_LEVITATE
            && fightOutPokemonIsZokuseiDataId(fightOutPokemon, ZOKUSEI_DATA_ID_FLYING) == 0) {
            UserWork[FIGHT_SEQ_WORK_ESCAPE_ABILITY_HOLDER_INDEX] = i;
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_ABILITY_NAME, (u32)(unsigned long)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(tokusei))));
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_POKEMON, (u32)(unsigned long)ary[i]);
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_ESCAPE_BLOCKED_BY_ABILITY;
            return FIGHT_ESCAPE_BLOCKED_BY_ABILITY;
        }
        if (tokusei == TOKUSEI_DATA_ID_MAGNET_PULL && fightOutPokemonIsZokuseiDataId(fightOutPokemon, ZOKUSEI_DATA_ID_STEEL) == 1) {
            UserWork[FIGHT_SEQ_WORK_ESCAPE_ABILITY_HOLDER_INDEX] = i - 1;
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_ABILITY_NAME, (u32)(unsigned long)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(tokusei))));
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_POKEMON, (u32)(unsigned long)ary[i]);
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_ESCAPE_BLOCKED_BY_ABILITY;
            return FIGHT_ESCAPE_BLOCKED_BY_ABILITY;
        }
    }
    if (fightOutPokemonIsJoutaiDataId(fightOutPokemon, JOUTAI_PARTIAL_TRAPPING) == 1
        || fightOutPokemonIsJoutaiDataId(fightOutPokemon, JOUTAI_TRAPPING) == 1
        || fightOutPokemonIsJoutaiDataId(fightOutPokemon, JOUTAI_ID_INGRAIN) == 1) {
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
        return FIGHT_ESCAPE_BLOCKED_BY_STATUS;
    }
    return FIGHT_ESCAPE_ALLOWED;
}

extern "C" void SCA_EndCheck(void) {
    fightSeqGetExpCheck();
    fightSeqEndFlag = FIGHT_SEQ_END_FLAG_FINISHED;
}

extern "C" void SCA_End(void)
{
    FightOutPokemon *attackPokemon;
    FIGHT_WAZA *fightWaza;
    FightOutPokemon *defender;
    FIGHT_WAZA *defenderWaza;
    u8 id;

    attackPokemon = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    fightWaza = fightOutPokemon_GetFightWazaPtr(attackPokemon);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    defenderWaza = 0;
    if (defender != 0) {
        defenderWaza = fightOutPokemon_GetFightWazaPtr(defender);
    }
    fightSeqClearOneSelfTurnCount();
    ServerStatusFlag &= ~FIGHT_SEQ_END_CLEAR_BITS;
    fightSeqWazaWork[SEQ_STATUS_PARAMETER] = 0;
    fightSeqWazaEffectFlag = 0;
    fightSeqWazaWork[4] = 0;
    UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION] = 0;
    UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_INVOCATION] = 0;
    fightOutPokemon_SetHitWazaDataId(attackPokemon, 0);
    fightOutPokemon_SetHitWazaZokuseiDataId(attackPokemon, ZOKUSEI_DATA_ID_MYSTERY);
    fightWaza_SetDamage(fightWaza, 0);
    for (id = JOUTAI_MOVE_RESULT_FIRST; id <= JOUTAI_MOVE_RESULT_LAST; id++) {
        fightWazaInitJoutaiDataId(fightWaza, id);
        if (defenderWaza != 0) {
            fightWazaInitJoutaiDataId(defenderWaza, id);
        }
    }
}

extern "C" u8 fightSeqOpenMsg(s32 msgId) {
    u8 result = fightMenuOpenMsg(msgId);
    if (result == 1) {
        fightSeqWazaWork[SEQ_MESSAGE_SHOWN] = 1;
    }
    return result;
}

extern "C" void fightSeqIrekaeExec(FIGHT_ACTION *sequence) {
    FightOutPokemon *pokemon = fightFloor_GetIrekaePokemonPtr(0);

    fightFloor_SetAttackPokemonPtr(0, pokemon);
    fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (u32)fightOutPokemonGetNicknamePtr(pokemon));
    fightSeqEndFlag = 0;
    fightSeqSpecificationActionCounterInit(pokemon);
    fightSeq_CallWazaSeqCustom(sequence, fightSeqIrekaeWazaSeq, 1);
    wazaSequenceSysFreeAllWaza();
    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fightMenuCloseMsg();
}

extern "C" void fightSeqCallExec(void)
{
    FightOutPokemon *attacker;
    FIGHT_TRAINER *trainer;
    s32 msgId;
    u8 wokeFromSleep;
    u8 endedHyperMode;
    u8 raisedAccuracy;
    u16 fightType;
    u8 callProbability;
    u16 rand;

    wokeFromSleep = 0;
    endedHyperMode = 0;
    raisedAccuracy = 0;
    fightType = fightFloor_GetFightType(0);
    attacker = fightFloor_GetAttackPokemonPtr(0);
    trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, attacker);

    fightSeqSpecificationActionCounterInit(attacker);

    if (pokemonIsDarkPokemon(fightOutPokemonGetPokemonPtr(attacker)) == 1) {
        callProbability = pokemonGetDarkPokemonCallKakuritu(fightOutPokemonGetPokemonPtr(attacker));

        if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_DARK_HYPER_MODE) == 1) {
            rand = HSD_Rand();
            if (rand % 100 < (u8)callProbability) {
                msgId = FIGHT_CALL_MSG_EAGER_RESPONSE;
                endedHyperMode = 1;
                if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_SLEEP) == 1) {
                    wokeFromSleep = 1;
                }
            } else {
                msgId = FIGHT_CALL_MSG_IGNORING_ORDERS;
                if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_SLEEP) == 1) {
                    rand = HSD_Rand();
                    if (rand % 100 < ((callProbability >> 1) & 0x7F)) {
                        wokeFromSleep = 1;
                    }
                }
            }
        } else {
            if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_SLEEP) == 1) {
                msgId = FIGHT_CALL_MSG_CAME_TO_SENSES;
                wokeFromSleep = 1;
            } else {
                raisedAccuracy = 1;
            }
        }
    } else {
        if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_SLEEP) == 1) {
            msgId = FIGHT_CALL_MSG_CAME_TO_SENSES;
            wokeFromSleep = 1;
        } else {
            raisedAccuracy = 1;
        }
    }

    if (raisedAccuracy == 1) {
        u8 abicnt = fightOutPokemon_GetAbicntAverage(attacker);

        if (abicnt < FIGHT_STAT_STAGE_LIMIT) {
            msgId = FIGHT_CALL_MSG_STAT_RAISE;
            GSmsgGetStringPtr(wazaGetStatus(0, WAZA_DATA_ID_SAFEGUARD, WAZA_FIELD_NAME, 0));
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (u32)GSmsgGetStringPtr(FIGHT_CALL_MSG_ACCURACY));
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_DEGREE, (u32)GSmsgGetStringPtr(FIGHT_STAT_TEXT_ONE_STAGE));
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_DIRECTION, (u32)GSmsgGetStringPtr(FIGHT_STAT_TEXT_RAISE_DIRECTION));
            raisedAccuracy = 1;
            fightOutPokemon_SetAbicntAverage(attacker, abicnt + 1);
        } else {
            msgId = FIGHT_CALL_MSG_STAT_LIMIT;
            raisedAccuracy = 0;
        }
    }

    fightTrainerCallEffect(trainer, FIGHT_SEQUENCE_OPERATION_LOAD);
    fightOutPokemonDarkPokemonKaijoEffect(attacker, wokeFromSleep, endedHyperMode, raisedAccuracy, 0, FIGHT_SEQUENCE_OPERATION_LOAD);
    fightMsgctrlSetValue(FIGHT_MSG_SLOT_TRAINER_NAME, (u32)fightTrainerGetNamePtr(trainer));
    fightMainAllCamera(1, 0);
    fightMenuOpenMsg(FIGHT_CALL_MSG_TRAINER_CALLS_POKEMON);
    fightMainWaitFrame(64);
    fightMenuCloseMsg();
    fightTrainerCallEffect(trainer, FIGHT_SEQUENCE_OPERATION_START);
    fightMenuFightOutPokemonOpenStatusMenu(attacker, fightType, 1);
    fightMenuFightOutPokemonCloseStatusMenuAnotherMine(0, attacker, fightType, 0, 1);
    fightMenuOpenMsg(FIGHT_CALL_MSG_CALLED_POKEMON_NAME);
    fightTrainerCallEffect(trainer, FIGHT_SEQUENCE_OPERATION_WAIT);
    fightMenuCloseMsg();
    if (wokeFromSleep == 1) {
        fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_SLEEP);
        fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_AKUMU);
        fightSeqCheckSetHensinBuffToMotoPokemon(attacker, POKEMON_STATUS_CONDITION);
    }
    if (endedHyperMode == 1) {
        fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_DARK_HYPER_MODE);
        fightSeqCheckSetHensinBuffToMotoPokemon(attacker, POKEMON_STATUS_FIELD_REVERSE_MODE);
        if (fightOutPokemonIsGcHeroFightOutPokemon(attacker) == 1
            && pokemon_GetPokemonDataId(fightOutPokemonGetPokemonPtr(attacker)) != POKEMON_DATA_ID_LUGIA) {
            pokemonAddDpFormPokemonDpFilterId(fightOutPokemonGetPokemonPtr(attacker), 0, POKEMON_RELIVE_FILTER_CALL);
            fightSeqCheckSetHensinBuffToMotoPokemon(attacker, POKEMON_STATUS_FIELD_DARK_POINTS);
        }
    }
    fightOutPokemonDarkPokemonKaijoEffect(attacker, wokeFromSleep, endedHyperMode, raisedAccuracy, msgId, FIGHT_SEQUENCE_OPERATION_START);
    fightOutPokemonDarkPokemonKaijoEffect(attacker, wokeFromSleep, endedHyperMode, raisedAccuracy, 0, FIGHT_SEQUENCE_OPERATION_WAIT);
    fightMenuCloseMsg();
    fightMenuFightOutPokemonCloseStatusMenu(attacker, fightType, 0);
    fightTrainerCallEffect(trainer, FIGHT_SEQUENCE_OPERATION_FREE);
    fightOutPokemonDarkPokemonKaijoEffect(attacker, wokeFromSleep, endedHyperMode, raisedAccuracy, 0, FIGHT_SEQUENCE_OPERATION_FREE);
    wazaSequenceSysFreeAllWaza();
    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fightMenuCloseMsg();
}

extern "C" void fightSeqWazaExec(FIGHT_ACTION *seqData, u16 waza)
{
    FIGHT_SIDE *attackSide;
    u16 seqId;
    u8 layout;
    FightOutPokemon *attacker;
    FightOutPokemon *defender;
    FIGHT_SIDE *defenseSide;
    FightOutPokemon *selected;

    layout = (u8)fightFloor_GetFightType(0);
    seqId = wazaDB_GetSeqId(waza);
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    attackSide = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, attacker);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    defenseSide = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, defender);


    if (fightOutPokemonCheckFightOut(attacker) == 0) {
        return;
    }
    if (fightActionCheckValid(fightOutPokemon_GetFightActionBuffPtr(attacker)) == 0) {
        return;
    }

    fightFloor_SetAttackMsgId(0, 0);
    fightFloor_SetCriticakMsgId(0, 0);
    fightFloor_SetWazakoukaMsgId(0, 0);
    fightSeqWazaWork[SEQ_MOVE_MISS_REASON] = 0;
    fightSeqWazaHitFlag = 0;
    fightSeqAttackJoutaiStep = 0;

    if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_ENCORE) == 1 && waza != WAZA_DATA_ID_STRUGGLE) {
        u32 stockWaza;
        u16 joutaiWaza;
        u16 pokemonWaza;
        joutaiWaza = fightOutPokemonGetIdBuffJoutaiDataId(attacker, JOUTAI_ID_ENCORE);
        stockWaza = fightOutPokemonGetBuffJoutaiDataId(attacker, JOUTAI_ID_ENCORE);
        pokemonWaza = pokemon_GetPokemonWazaDataId(fightOutPokemonGetPokemonPtr(attacker), stockWaza);

        if (joutaiWaza != pokemonWaza) {
            fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_ID_ENCORE);
        }
        selected = fightSeqSelectDefensePokemonPtr(attacker, pokemonWaza, layout, 0, 1, 1, -1);
        fightOutPokemonCreateFightActionAttackWaza(attacker, 0, FIGHT_ACTION_KIND_FIGHT_OUT_POKEMON_WAZA, 0, fight_action_data_waza, pokemonWaza,
                                                  fightTargetGetTragetPtrToRelativeHostSideFightTargetId(selected, layout), (s8)stockWaza, 1);
        waza = pokemonWaza;
        fightFloor_SetDefensePokemonPtr(0, selected);
        defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
        defenseSide = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, defender);
    }

    if (waza == WAZA_DATA_ID_STRUGGLE || wazaCheckDarkWaza(waza) == 1) {
        ServerStatusFlag |= FIGHT_SEQ_FLAG_SKIP_PP_DECREMENT;
    }

    if (fightSeqTameWazaCheck(waza) == 1 && fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_CONTINUING_MOVE) == 0) {
        if (seqId == WAZA_SEQUENCE_ID_SOLAR_BEAM && fightFloorGetNowTenkouDataId(0, true) == FIGHT_WEATHER_SUNLIGHT) {
            selected = fightSeqSelectDefensePokemonPtr(attacker, waza, layout, 0, 0, 1, -1);
            if (selected != 0) {
                defender = selected;
                fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, selected);
                defenseSide = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, selected);
            }
        }
    } else {
        selected = fightSeqSelectDefensePokemonPtr(attacker, waza, layout, 0, 0, 1, -1);
        if (selected != 0) {
            defender = selected;
            fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, selected);
            defenseSide = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, selected);
        }
    }

    if (fightOutPokemonCheckFightOut(defender) == 0) {
        if (attackSide != defenseSide ||
            fightOutPokemonCheckFightOut(defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OPPOSING_SIDE_FIRST_POKEMON, attacker)) == 0) {
            defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE, defender);
            if (defender != 0) {
                if (fightOutPokemonCheckFightOut(defender) == 0) {
                    return;
                }
            } else {
                return;
            }
        }
        fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, defender);
        fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, defender);
    }

    seqId = wazaDB_GetSeqId(waza);
    fightSeqEndFlag = 0;
    fightMsgctrlSetValue(FIGHT_MSG_SLOT_MOVE_NAME, (u32)GSmsgGetStringPtr(wazaDB_GetName(waza)));
    fightSeq_CallWazaSeqCustom(seqData, wazaSeqJumpTbl[seqId], 1);
    wazaSequenceSysFreeAllWaza();
    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fightMenuCloseMsg();
}

extern "C" void fightSeqSpecificationActionCounterInit(FightOutPokemon *pokemon) {
    if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_ID_FURY_CUTTER) == 1) {
        fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_ID_FURY_CUTTER);
    }
    if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_ID_DESTINY_BOND) == 1) {
        fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_ID_DESTINY_BOND);
    }
    if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_ID_GRUDGE) == 1) {
        fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_ID_GRUDGE);
    }
}

extern "C" u8 fightSeqGetItemType(u16 itemID, FightOutPokemon *pokemon)
{
    ITEM_PARAM *param = (ITEM_PARAM *)itemParamGetPtr(itemDataBiosGetItemEffectParam(itemDataBiosGetPtr(itemID)));

    if (param == 0) {
        return FIGHT_SEQ_ITEM_TYPE_NONE;
    }
    if (itemID == ITEM_DATA_ID_FULL_RESTORE) {
        return FIGHT_SEQ_ITEM_TYPE_SPECIAL;
    }
    if (itemParamGetHPUp(param) != 0) {
        return FIGHT_SEQ_ITEM_TYPE_HP_UP;
    }
    if (itemParamGetSleepFlag(param) == 1 || itemParamGetPoisonFlag(param) == 1 ||
        itemParamGetBurnFlag(param) == 1 || itemParamGetFreezeFlag(param) == 1 ||
        itemParamGetParalyzeFlag(param) == 1 || itemParamGetConfuseFlag(param) == 1) {
        return FIGHT_SEQ_ITEM_TYPE_STATUS_CURE;
    }
    if (itemParamGetCriticalFlag(param) == 1) {
        return FIGHT_SEQ_ITEM_TYPE_CRITICAL;
    }
    if (itemParamGetAttackUp(param) != 0 || itemParamGetDefenceUp(param) != 0 ||
        itemParamGetQuickUp(param) != 0 || itemParamGetHitUp(param) != 0 ||
        itemParamGetSpAttackUp(param) != 0) {
        return FIGHT_SEQ_ITEM_TYPE_STAT_UP;
    }
    if (itemParamGetGuardFlag(param) == 1) {
        return FIGHT_SEQ_ITEM_TYPE_GUARD;
    }
    return FIGHT_SEQ_ITEM_TYPE_NONE;
}

extern "C" void fightSeqItemExec(FIGHT_ACTION *action, u16 itemID)
{
    s32 layout = (u8)fightFloor_GetFightType(0);
    FightOutPokemon *user = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    FightOutPokemon *target = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    FIGHT_TRAINER *trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, user);
    Hero *itemOwner = fightTrainer_GetHeroPtr(trainer);
    FIGHT_ITEM *fightItem = fightOutPokemon_GetFightItemPtr(user);
    u8 *info;
    ITEM_PARAM *param;
    s32 count;
    u8 type;
    u8 savedStatSelector;
    u8 savedMessageEffectId;
    s16 up;
    u8 flag;

    if (fightItem == 0) {
        return;
    }
    fightItem_GetTargetDataId(fightItem);
    count = fightItem_GetCount(fightItem);
    flag = (fightItem_GetBuff(fightItem) != 0);
    param = (ITEM_PARAM *)itemParamGetPtr(itemDataBiosGetItemEffectParam(itemDataBiosGetPtr(itemID)));
    savedStatSelector = UserWork[SEQ_WORK_STAT_ACTION];
    savedMessageEffectId = UserWork[SEQ_WORK_STAT_MESSAGE];
    fightSeqSpecificationActionCounterInit(user);

    if (flag == 1) {
        info = fightSeqItemTypeWazaSeqTbl[0];
        goto skip;
    }
    if (itemDB_GetKind(itemID) == ITEM_KIND_POKEMON_BALL) {
        info = fightSeqItemWazaSeqTbl[itemID];
        goto skip;
    }
    if (itemID == ITEM_DATA_ID_POKE_DOLL || itemID == ITEM_DATA_ID_FLUFFY_TAIL) {
        info = fightSeqItemSpecialWazaSeqTbl[0];
        goto skip;
    }
    type = fightSeqGetItemType(itemID, user);
    if (type == FIGHT_SEQ_ITEM_TYPE_NONE) {
        info = fightSeqItemTypeWazaSeqTbl[0];
        goto skip;
    }
    fightFloor_SetAppointPokemonPtr(0, user);
    fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_CONFUSION;
    switch (type) {
    case FIGHT_SEQ_ITEM_TYPE_SPECIAL:
    case FIGHT_SEQ_ITEM_TYPE_HP_UP:
        break;
    case FIGHT_SEQ_ITEM_TYPE_STATUS_CURE:
        if (itemParamGetSleepFlag(param) == 1 && itemParamGetPoisonFlag(param) == 1 &&
            itemParamGetBurnFlag(param) == 1 && itemParamGetFreezeFlag(param) == 1 &&
            itemParamGetParalyzeFlag(param) == 1) {
            if (fightOutPokemonIsJoutaiDataId(user, JOUTAI_SLEEP) == 1) {
                fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_SLEEP;
            } else if (fightOutPokemonIsJoutaiDataId(user, JOUTAI_POISON) == 1) {
                fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_POISON;
            } else if (fightOutPokemonIsJoutaiDataId(user, JOUTAI_BAD_POISON) == 1) {
                fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_POISON;
            } else if (fightOutPokemonIsJoutaiDataId(user, JOUTAI_BURN) == 1) {
                fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_BURN;
            } else if (fightOutPokemonIsJoutaiDataId(user, JOUTAI_FREEZE) == 1) {
                fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_FREEZE;
            } else if (fightOutPokemonIsJoutaiDataId(user, JOUTAI_PARALYSIS) == 1) {
                fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_PARALYSIS;
            }
        } else if (itemParamGetSleepFlag(param) == 1) {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_SLEEP;
        } else if (itemParamGetPoisonFlag(param) == 1) {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_POISON;
        } else if (itemParamGetBurnFlag(param) == 1) {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_BURN;
        } else if (itemParamGetFreezeFlag(param) == 1) {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_FREEZE;
        } else if (itemParamGetParalyzeFlag(param) == 1) {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_PARALYSIS;
        } else if (itemParamGetConfuseFlag(param) == 1) {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_CONFUSION;
        }
        break;
    case FIGHT_SEQ_ITEM_TYPE_CRITICAL:
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_CRITICAL;
        break;
    case FIGHT_SEQ_ITEM_TYPE_STAT_UP:
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = FIGHT_SEQ_ITEM_EFFECT_STAT_CHANGE;
        up = itemParamGetAttackUp(param);
        if (up > 0) {
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (u32)GSmsgGetStringPtr(AbiCntMsgTbl[FIGHT_STAT_ATTACK]));
            UserWork[SEQ_WORK_STAT_ACTION] = FIGHT_STAT_ATTACK;
        } else {
            up = itemParamGetDefenceUp(param);
            if (up > 0) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (u32)GSmsgGetStringPtr(AbiCntMsgTbl[FIGHT_STAT_DEFENSE]));
                UserWork[SEQ_WORK_STAT_ACTION] = FIGHT_STAT_DEFENSE;
            } else {
                up = itemParamGetQuickUp(param);
                if (up > 0) {
                    fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (u32)GSmsgGetStringPtr(AbiCntMsgTbl[FIGHT_STAT_SPEED]));
                    UserWork[SEQ_WORK_STAT_ACTION] = FIGHT_STAT_SPEED;
                } else {
                    up = itemParamGetHitUp(param);
                    if (up > 0) {
                        fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (u32)GSmsgGetStringPtr(AbiCntMsgTbl[FIGHT_STAT_ACCURACY]));
                        UserWork[SEQ_WORK_STAT_ACTION] = FIGHT_STAT_ACCURACY;
                    } else {
                        up = itemParamGetSpAttackUp(param);
                        if (up > 0) {
                            fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (u32)GSmsgGetStringPtr(AbiCntMsgTbl[FIGHT_STAT_SPECIAL_ATTACK]));
                            UserWork[SEQ_WORK_STAT_ACTION] = FIGHT_STAT_SPECIAL_ATTACK;
                        }
                    }
                }
            }
        }
        if (up < 0) {
            if (up == 1 || up == -1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_DEGREE, (u32)GSmsgGetStringPtr(FIGHT_STAT_TEXT_ONE_STAGE));
                UserWork[SEQ_WORK_STAT_ACTION] = (UserWork[SEQ_WORK_STAT_ACTION] & FIGHT_STAT_SELECTOR_MASK) + FIGHT_STAT_MESSAGE_LOWER_ONE;
            } else {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_DEGREE, (u32)GSmsgGetStringPtr(FIGHT_STAT_TEXT_LOWER_MULTIPLE));
                UserWork[SEQ_WORK_STAT_MESSAGE] = (UserWork[SEQ_WORK_STAT_ACTION] & FIGHT_STAT_SELECTOR_MASK) + FIGHT_STAT_MESSAGE_LOWER_MULTIPLE;
            }
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_DIRECTION, (u32)GSmsgGetStringPtr(FIGHT_STAT_TEXT_LOWER_DIRECTION));
        } else {
            if (up == 1 || up == -1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_DEGREE, (u32)GSmsgGetStringPtr(FIGHT_STAT_TEXT_ONE_STAGE));
                UserWork[SEQ_WORK_STAT_MESSAGE] = (UserWork[SEQ_WORK_STAT_ACTION] & FIGHT_STAT_SELECTOR_MASK) + FIGHT_STAT_MESSAGE_RAISE_ONE;
            } else {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_DEGREE, (u32)GSmsgGetStringPtr(FIGHT_STAT_TEXT_RAISE_MULTIPLE));
                UserWork[SEQ_WORK_STAT_MESSAGE] = (UserWork[SEQ_WORK_STAT_ACTION] & FIGHT_STAT_SELECTOR_MASK) + FIGHT_STAT_MESSAGE_RAISE_MULTIPLE;
            }
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_DIRECTION, (u32)GSmsgGetStringPtr(FIGHT_STAT_TEXT_RAISE_DIRECTION));
        }
        break;
    case FIGHT_SEQ_ITEM_TYPE_GUARD:
        break;
    }
    info = fightSeqItemTypeWazaSeqTbl[type];
skip:
    fightSeqEndFlag = 0;
    fightMsgctrlSetValue(FIGHT_MSG_SLOT_TRAINER_PREFIX, (u32)fightTrainerGetPrefixNamePtr(trainer));
    fightMsgctrlSetValue(FIGHT_MSG_SLOT_ITEM_TRAINER_NAME, (u32)fightTrainerGetNamePtr(trainer));
    fightMsgctrlSetValue(FIGHT_MSG_SLOT_TRAINER_NAME, (u32)fightTrainerGetNamePtr(trainer));
    fightMsgctrlSetValue(FIGHT_MSG_SLOT_ITEM_NAME, (u32)GSmsgGetStringPtr(itemDB_GetName(itemID)));
    fightMainAllCamera(1, 0);
    if (flag == 0) {
        fightMenuFightOutPokemonOpenStatusMenu(target, layout, 1);
        fightMenuFightOutPokemonCloseStatusMenuAnotherMine(0, target, layout, 0, 1);
    }
    fightSeq_CallWazaSeqCustom(action, info, 1);
    if (flag == 0 && heroItemDecItemDataId(itemOwner, itemID, 1, count) < 0) {
        heroItemDecItemDataId(itemOwner, itemID, 1, -1);
    }
    UserWork[SEQ_WORK_STAT_ACTION] = savedStatSelector;
    UserWork[SEQ_WORK_STAT_MESSAGE] = savedMessageEffectId;
    wazaSequenceSysFreeAllWaza();
    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fightMenuCloseMsg();
}

extern "C" void fightSeqExec(FIGHT_ACTION *action, u8 *position, u8 checkEnd)
{
    FIGHT_ACTION *savedAction;
    u8 savedState;
    u8 *savedPosition;

    savedAction = fight_seq_now_fight_action_ptr;
    savedState = fightSeqEndFlag;
    savedPosition = fightSeq_GetWazaSeq();
    fightSeq_JumpWazaSeq(position);
    if (checkEnd == 0) {
        fightSeqEndFlag = 0;
    }
    fight_seq_now_fight_action_ptr = action;
    while (1) {
        u16 routine;

        routine = *(u8 *)fightSeq_GetWazaSeq();
        (*WazaSequenceTbl[routine])();
        if (checkEnd != 0) {
            if (fightSeqEndFlag == FIGHT_SEQ_END_FLAG_RUNNING_END_CHECK) {
                SCA_EndCheck();
            }
            if (fightSeqEndFlag == FIGHT_SEQ_END_FLAG_FINISHED) {
                SCA_End();
                break;
            }
        } else {
            if (fightSeqEndFlag == FIGHT_SEQ_END_FLAG_RUNNING_END_CHECK || fightSeqEndFlag == FIGHT_SEQ_END_FLAG_FINISHED) {
                break;
            }
        }
    }
    if (checkEnd != 0) {
        ServerStatusFlag &= ~FIGHT_SEQ_FLAG_REPLAYING_MOVE_ON_OTHER_BATTLER;
        ServerStatusFlag &= ~FIGHT_SEQ_FLAG_BADGE_CHECK_ABORTED;
    }
    fight_seq_now_fight_action_ptr = savedAction;
    fightSeqEndFlag = savedState;
    fightSeq_JumpWazaSeq(savedPosition);
}

static u8 _fightSeqPreSub(void *pokemon, u16 index, void *arg)
{
    FIGHT_ACTION *action;

    if (fightOutPokemonCheckFightOut((FightOutPokemon *)pokemon) == 0) {
        return 1;
    }
    action = (FIGHT_ACTION *)fightOutPokemon_GetFightActionBuffPtr((FightOutPokemon *)pokemon);
    if (action == 0) {
        return 1;
    }
    if (fightActionCheckValid(action) == 0) {
        return 1;
    }
    if (fightOutPokemon_GetMotoWazaDataId((FightOutPokemon *)pokemon) == WAZA_DATA_ID_FOCUS_PUNCH && fightOutPokemonIsJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_SLEEP) == 0 &&
        fightOutPokemon_GetNamakeFlag((FightOutPokemon *)pokemon) == 0) {
        fightSeqFightActionCreateAndFlowFifo(fightActionBiosGetFightActionDataPtr(fight_seq_now_fight_action_ptr), pokemon, FIGHT_ACTION_KIND_FOCUS_PUNCH_PREPARE, 0, fight_action_data_turn,
                                             fightSeqTurnWazaSeq);
    }
    return 1;
}

extern "C" void fightSeqPre(void)
{
    fightFloorLoopValidFightOutPokemon(0, _fightSeqPreSub, 0, 1);
    fightFloorLoopValidFightOutPokemon(0, fightSeqWazaOutBeforeCheckSub, 0, 1);
    fightSeqWazaEffectFlag = 0;
    fightSeqWazaWork[SEQ_STATUS_PARAMETER] = 0;
    fightSeqWazaWork[4] = 0;
    UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION] = 0;
    UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_INVOCATION] = 0;
    wazaSequenceSysFreeAllWaza();
}

extern "C" void fightSeqPost(void) {
    ServerStatusFlag &= ~FIGHT_SEQ_FLAG_SKIP_DAMAGE_ACCOUNTING;
    fightSeqTurnCheck();
}

u8 fightSeqFightActionCreate(FIGHT_ACTION *action, FIGHT_ACTION_DATA_dd_ *currentActionData, void *pokemon, u16 kind, s32 bufferSelector,
                             void *nextActionData, void *wazaSequence)
{
    u8 result;

    result = fightActionCreate(action, currentActionData, pokemon, kind, bufferSelector, nextActionData);
    if (result != 1) {
        return result;
    }
    fightActionBiosSetBuffDataPtr(action, wazaSequence);
    return 1;
}

u8 fightSeqFightActionCreateAndFlowFifo(FIGHT_ACTION_DATA_dd_ *currentActionData, void *pokemon, u16 kind, s32 bufferSelector, void *nextActionData,
                                       void *wazaSequence)
{
    FIGHT_ACTION action;
    u8 result;

    result = fightSeqFightActionCreate(&action, currentActionData, pokemon, kind, bufferSelector, nextActionData, wazaSequence);
    if (result != 1) {
        return result;
    }
    return fightActionFlowFifo(&action);
}

static u8 _fightSeqInitSubFightOutPokemon(void *pokemon, u16 index, void *arg)
{
    if (fightOutPokemonCheckFightOut((FightOutPokemon *)pokemon) == 0) {
        return 1;
    }
    fightOutPokemonInitJoutaiDataId((FightOutPokemon *)pokemon, JOUTAI_FLINCH);
    return 1;
}

extern "C" void fightSeqInit(void)
{
    int i;

    fightSeqClearOneTurnCount(0);
    fightSeqClearOneSelfTurnCount();
    for (i = 0; i < 8; i++) {
        fightSeqWazaWork[i] = 0;
    }
    fight_seq_now_fight_action_ptr = 0;
    fightFloorLoopValidFightOutPokemon(0, _fightSeqInitSubFightOutPokemon, 0, 0);
}

extern "C" void fightSeqSetEffectAminFlag(u8 flag) {
    if (flag == 1) {
        ServerStatusFlag &= ~FIGHT_SEQ_EFFECT_ANIMATION_FLAG;
    } else {
        ServerStatusFlag |= FIGHT_SEQ_EFFECT_ANIMATION_FLAG;
    }
}

extern "C" bool fightSeqGetEffectAminFlag(void) {
    return !(ServerStatusFlag & FIGHT_SEQ_EFFECT_ANIMATION_FLAG);
}

typedef void (*FightSeqNormalWazaFunc)(FIGHT_TRAINER *, u16, FightOutPokemon *, FightOutPokemon *);

extern "C" s32 fightSeqGetNromalWazaDamage(FIGHT_TRAINER *trainer, u16 wazaDataId, FightOutPokemon *attackPokemon,
                                           FightOutPokemon *defensePokemon, u8 criticalCheckFlag, u8 damageLossFlag,
                                           FightSeqNormalWazaFunc preFunc, FightSeqNormalWazaFunc postFunc)
{
    FIGHT_WAZA *wazaBios;
    u8 *savedPosition;
    FightOutPokemon *savedAttackPokemon;
    FightOutPokemon *savedDefensePokemon;
    u32 savedServerStatusFlag;
    u8 savedWork[8];
    FightOutPokemon savedAttackBios;
    FightOutPokemon savedDefenseBios;
    FIGHT_WAZA savedWazaBios;
    s32 damage;

    savedAttackPokemon = fightFloor_GetAttackPokemonPtr(0);
    savedDefensePokemon = fightFloor_GetDefensePokemonPtr(0);
    savedPosition = fightSeq_GetWazaSeq();
    savedServerStatusFlag = ServerStatusFlag;
    for (u32 i = 0; i < 8; i++) {
        savedWork[i] = fightSeqWazaWork[i];
    }

    fightOutPokemonBiosCopy(&savedAttackBios, attackPokemon);
    fightOutPokemonBiosCopy(&savedDefenseBios, defensePokemon);
    fightFloor_SetAttackPokemonPtr(0, attackPokemon);
    fightFloor_SetDefensePokemonPtr(0, defensePokemon);

    wazaBios = fightOutPokemon_GetFightWazaPtr(attackPokemon);
    fightWazaBiosCopy(&savedWazaBios, wazaBios);
    fightWazaInit(wazaBios);
    fightWazaCreate(wazaBios, 0, wazaDataId, 0, 0);

    ServerStatusFlag = 0;
    for (u32 i = 0; i < 8; i++) {
        fightSeqWazaWork[i] = 0;
    }

    if (preFunc != 0) {
        preFunc(trainer, wazaDataId, attackPokemon, defensePokemon);
    }
    if (criticalCheckFlag == 1) {
        WS_CRITICAL_CHECK();
    }
    WS_DAMAGE_CALC();
    WS_TYPE_CHECK();
    if (damageLossFlag == 1) {
        WS_DAMAGE_LOSS();
    }
    if (postFunc != 0) {
        postFunc(trainer, wazaDataId, attackPokemon, defensePokemon);
    }

    damage = (fightWazaIsHit(wazaBios) == 1) ? fightWaza_GetDamage(wazaBios) : 0;

    fightFloor_SetAttackPokemonPtr(0, savedAttackPokemon);
    fightFloor_SetDefensePokemonPtr(0, savedDefensePokemon);
    fightWazaBiosCopy(wazaBios, &savedWazaBios);
    fightSeq_JumpWazaSeq(savedPosition);
    ServerStatusFlag = savedServerStatusFlag;
    for (u32 i = 0; i < 8; i++) {
        fightSeqWazaWork[i] = savedWork[i];
    }
    fightOutPokemonBiosCopy(attackPokemon, &savedAttackBios);
    fightOutPokemonBiosCopy(defensePokemon, &savedDefenseBios);
    return damage;
}

extern "C" void CriticalCheckHikaePokemon(FightPokemon *pokemon)
{
    FightOutPokemon *attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    u8 stickFlag;
    u8 luckyPunchFlag;
    fightOutPokemonGetTokuseiDataId(attacker);
    u16 heldItemId = fightOutPokemonGetSoubiItemSoubiDataId(attacker);
    FIGHT_WAZA *wazaBios = fightOutPokemon_GetFightWazaPtr(attacker);
    u16 wazaDataId = fightOutPokemon_GetUseWazaDataId(attacker);
    u16 species = figthOutPokemonGetPokemonDataId(attacker);
    u16 baseSpecies = pokemon_GetPokemonDataId(fightPokemon_GetPokemonBuffPtr(pokemon));
    u8 abilityIndex = pokemon_GetTokuseiFlag(fightPokemon_GetPokemonBuffPtr(pokemon));
    u16 rand;

    luckyPunchFlag = 0;
    if (heldItemId == SOUBI_LUCKY_PUNCH && species == POKEMON_DATA_ID_CHANSEY) {
        luckyPunchFlag = 1;
    }
    stickFlag = 0;
    if (heldItemId == SOUBI_STICK && species == POKEMON_DATA_ID_FARFETCHD) {
        stickFlag = 1;
    }

    u16 criticalRatio = (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_FOCUS_ENERGY) == 1) * 2
                      + (wazaDB_GetSeqId(wazaDataId) == WAZA_SEQUENCE_ID_HIGH_CRITICAL)
                      + (wazaDB_GetSeqId(wazaDataId) == WAZA_SEQUENCE_ID_SKY_ATTACK)
                      + (wazaDB_GetSeqId(wazaDataId) == WAZA_SEQUENCE_ID_BLAZE_KICK)
                      + (wazaDB_GetSeqId(wazaDataId) == WAZA_SEQUENCE_ID_POISON_TAIL)
                      + (heldItemId == SOUBI_SCOPE_LENS)
                      + luckyPunchFlag * 2
                      + stickFlag * 2;
    if (criticalRatio > fight_waza_critical_data_number - 1) {
        criticalRatio = fight_waza_critical_data_number - 1;
    }

    if (pokemonDB_GetTokuseiDataId(baseSpecies, abilityIndex) != TOKUSEI_DATA_ID_BATTLE_ARMOR
        && pokemonDB_GetTokuseiDataId(baseSpecies, abilityIndex) != TOKUSEI_DATA_ID_SHELL_ARMOR
        && fightFloor_GetFightKindDoCriticalAttackFlag(0) == 1) {
        rand = (u16)HSD_Rand();
        if (rand % fightWazaCriticalDataBiosGetBunbo(fightWazaCriticalDataBiosGetPtr(criticalRatio)) == 0) {
            fightWaza_SetCritical(wazaBios, 2);
        } else {
            fightWaza_SetCritical(wazaBios, 1);
        }
    } else {
        fightWaza_SetCritical(wazaBios, 1);
    }
}

extern "C" void DamageCalcHikaePokemon(FightPokemon *fightPokemon)
{
    FightOutPokemon *attacker;
    FIGHT_SIDE *defenderSide;
    u16 wazaDataId;
    u16 iryoku;
    FIGHT_WAZA *waza;
    u16 zokusei;
    s32 damage;

    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    defenderSide = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OPPOSING_SIDE, attacker);
    wazaDataId = fightOutPokemon_GetUseWazaDataId(attacker);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    iryoku = fightWaza_GetIryoku(waza);
    zokusei = fightWaza_GetZokusei(waza);
    damage = fightSeqWazaDamageCalcHikaePokemon(attacker, fightPokemon, defenderSide, wazaDataId, iryoku, zokusei);
    damage *= fightWaza_GetCritical(waza);
    damage *= fightWaza_GetDamageValue(waza);
    if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_CHARGE) == 1 && zokusei == ZOKUSEI_DATA_ID_ELECTRIC) {
        damage *= 2;
    }
    if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_HELPING_HAND) == 1) {
        damage = damage * 15 / 10;
    }
    fightWaza_SetDamage(waza, damage);
}

extern "C" s32 fightSeqGetWazaDamageHikaePokemon(FIGHT_TRAINER *trainer, u16 wazaDataId,
                                                 FightOutPokemon *fightOutPokemon, FightPokemon *fightPokemon,
                                                 u8 criticalFlag, FightSeqWazaFunc preFunc, FightSeqWazaFunc postFunc)
{
    u8 savedWork[8];
    FightOutPokemon savedFightOutPokemon;
    Pokemon savedPokemon;
    FIGHT_WAZA savedWaza;
    FIGHT_WAZA *waza;
    u8 *savedPosition;
    FightOutPokemon *savedAttackPokemon;
    u32 savedServerStatusFlag;
    s32 damage;

    savedAttackPokemon = fightFloor_GetAttackPokemonPtr(0);
    savedPosition = fightSeq_GetWazaSeq();
    savedServerStatusFlag = ServerStatusFlag;
    for (u32 i = 0; i < 8; i++) {
        savedWork[i] = fightSeqWazaWork[i];
    }

    fightOutPokemonBiosCopy(&savedFightOutPokemon, fightOutPokemon);
    pokemonBiosCopy(&savedPokemon, fightPokemon_GetPokemonBuffPtr(fightPokemon));
    fightFloor_SetAttackPokemonPtr(0, fightOutPokemon);
    waza = fightOutPokemon_GetFightWazaPtr(fightOutPokemon);
    fightWazaBiosCopy(&savedWaza, waza);
    fightWazaInit(waza);
    fightWazaCreate(waza, 0, wazaDataId, 0, 0);
    ServerStatusFlag = 0;
    for (u32 i = 0; i < 8; i++) {
        fightSeqWazaWork[i] = 0;
    }

    if (preFunc != 0) {
        preFunc(trainer, wazaDataId, fightOutPokemon, fightPokemon);
    }
    if (criticalFlag == 1) {
        CriticalCheckHikaePokemon(fightPokemon);
    }
    DamageCalcHikaePokemon(fightPokemon);
    fightSeqTypeCheckHikaePokemon(fightPokemon, 1, 1, 0);
    if (postFunc != 0) {
        postFunc(trainer, wazaDataId, fightOutPokemon, fightPokemon);
    }

    if (fightWazaIsHit(waza) == 1) {
        damage = fightWaza_GetDamage(waza);
    } else {
        damage = 0;
    }

    fightFloor_SetAttackPokemonPtr(0, savedAttackPokemon);
    fightWazaBiosCopy(waza, &savedWaza);
    fightSeq_JumpWazaSeq(savedPosition);
    ServerStatusFlag = savedServerStatusFlag;
    for (u32 i = 0; i < 8; i++) {
        fightSeqWazaWork[i] = savedWork[i];
    }
    fightOutPokemonBiosCopy(fightOutPokemon, &savedFightOutPokemon);
    pokemonBiosCopy(fightPokemon_GetPokemonBuffPtr(fightPokemon), &savedPokemon);
    return damage;
}

