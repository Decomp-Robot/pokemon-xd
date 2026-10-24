/* fightSeqBasis - waza action sequencer. */
#include <types.h>
#include <fightSeq.h>
#include <fightTarget.h>
#include <fightFloor.h>
#include <fightMenu.h>
#include <pokemonDB.h>
#include <waza.h>
#include <sequence.h>


/* Ordered post-move processing stages. */
enum FightSeqAfterMoveStep {
    FIGHT_SEQ_AFTER_MOVE_RAGE = 0,
    FIGHT_SEQ_AFTER_MOVE_THAW = 1,
    FIGHT_SEQ_AFTER_MOVE_SYNCHRONIZE_DEFENDER = 2,
    FIGHT_SEQ_AFTER_MOVE_HIT_ABILITY = 3,
    FIGHT_SEQ_AFTER_MOVE_ALL_HIT_ABILITIES = 4,
    FIGHT_SEQ_AFTER_MOVE_SYNCHRONIZE_ATTACKER = 5,
    FIGHT_SEQ_AFTER_MOVE_CHOICE_LOCK = 6,
    FIGHT_SEQ_AFTER_MOVE_STOCK_ITEM = 7,
    FIGHT_SEQ_AFTER_MOVE_ALL_HIT_ITEMS = 8,
    FIGHT_SEQ_AFTER_MOVE_HIT_ITEM = 9,
    FIGHT_SEQ_AFTER_MOVE_ATTACKER_VISIBILITY = 10,
    FIGHT_SEQ_AFTER_MOVE_ATTACKER_STATUS = 11,
    FIGHT_SEQ_AFTER_MOVE_DEFENDER_VISIBILITY = 12,
    FIGHT_SEQ_AFTER_MOVE_SUBSTITUTE = 13,
    FIGHT_SEQ_AFTER_MOVE_HISTORY = 14,
    FIGHT_SEQ_AFTER_MOVE_COUNTER_MOVE = 15,
    FIGHT_SEQ_AFTER_MOVE_OTHER_TARGET = 16,
    FIGHT_SEQ_AFTER_MOVE_FINISH = 17
};
enum FightSeqAfterMoveMode {
    FIGHT_SEQ_AFTER_MOVE_MODE_ALL = 0,
    FIGHT_SEQ_AFTER_MOVE_MODE_UNTIL_NO_SEQUENCE = 1,
    FIGHT_SEQ_AFTER_MOVE_MODE_UNTIL_STAGE = 2
};
enum { FIGHT_SEQ_AFTER_MOVE_TERMINAL = FIGHT_SEQ_AFTER_MOVE_FINISH };

enum { MODEL_SEQUENCE_STATUS_FLYING = 3 };

/* Cursor into the move routine script of the running waza sequence. */
struct MOVE_ROUTINE_STEP {
    u8 opcode;
    u8 mode;
    u8 value;
};



static u8 _WsWkcActSubSetStockItem(void *data, u16, void *)
{
    FightOutPokemon *pokemon = static_cast<FightOutPokemon *>(data);
    u16 item = fightOutPokemon_GetStockItemDataId(pokemon);
    if (item != 0) {
        fightOutPokemonDoItemSoubi(pokemon, item, 1, 0);
        fightOutPokemon_SetStockItemDataId(pokemon, 0);
        fightSeqCheckSetHensinBuffToMotoPokemon(pokemon, POKEMON_STATUS_FIELD_HELD_ITEM);
    }
    return 1;
}


static u8 _WsWkcActSubJoutaiMigawari(void *data, u16, void *)
{
    FightOutPokemon *pokemon = static_cast<FightOutPokemon *>(data);
    if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_SUBSTITUTE) == 1 &&
        fightOutPokemonGetJoutaiMigawariHp(pokemon) <= 0)
        fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_SUBSTITUTE);
    return 1;
}


extern "C" void WS_WKC_ACT(void)
{
    u16 fightType = fightFloor_GetFightType(0);
    u16 heldItem;
    u16 zokusei;
    u16 useMove;
    u16 seqId;
    u16 useWaza;
    FIGHT_WAZA *waza;
    u8 posMode;
    u8 targets;
    u8 bSeqIssued;
    u8 hit;
    s16 dmgAtk;
    s16 dmgSpe;
    u16 iryoku;
    u16 wazaId;
    FightOutPokemon *atk;
    FightOutPokemon *def;
    u8 posValue;

    MOVE_ROUTINE_STEP *pos;

    bSeqIssued = 0;
    pos = reinterpret_cast<MOVE_ROUTINE_STEP *>(fightSeq_GetWazaSeq());
    posMode = pos->mode;
    posValue = pos->value;
    atk = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);

    if (atk != 0) {
        heldItem = fightOutPokemonGetSoubiItemSoubiDataId(atk);
        waza = fightOutPokemon_GetFightWazaPtr(atk);
        hit = fightWazaIsHit(waza);
        useMove = fightOutPokemon_GetUseWazaDataId(atk);
        useWaza = fightOutPokemon_GetMotoWazaDataId(atk);
        wazaId = (useWaza == WAZA_DATA_ID_INVALID) ? WAZA_DATA_ID_NONE : useWaza;
        zokusei = fightWaza_GetZokusei(waza);
        targets = wazaDB_GetRangeId(useMove);
        seqId = wazaDB_GetSeqId(useMove);
        iryoku = (u16)wazaDB_GetIryoku(useMove);
    } else {
        heldItem = 0;
        waza = 0;
        hit = 0;
        useMove = 0;
        useWaza = 0;
        wazaId = 0;
        zokusei = ZOKUSEI_DATA_ID_MYSTERY;
        targets = 1;
        seqId = 0;
        iryoku = 0;
    }

    def = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    if (def != 0) {
        dmgAtk = fightOutPokemon_GetMyselfDamageAtkValue(def);
        dmgSpe = fightOutPokemon_GetMyselfDamageSpeValue(def);
    } else {
        dmgAtk = 0;
        dmgSpe = 0;
    }

    do {
        switch (fightSeqWkcActStep) {
        case FIGHT_SEQ_AFTER_MOVE_RAGE:
            if (fightOutPokemonIsJoutaiDataId(def, JOUTAI_RAGE_ATTACK_BOOST) == 1 &&
                fightOutPokemonCheckFightOut(def) == 1 &&
                atk != def &&
                !fightOutPokemonIsAlly(atk, def) &&
                hit == 1 &&
                (dmgAtk != 0 || dmgSpe != 0) &&
                iryoku != 0) {
                s32 actType = fightSeqCondChgActTypeToPokemonStatusId(FIGHT_STAT_ATTACK);
                s8 count = (s8)pokemonGetStatus(def, 0, actType, 0);
                if (count < FIGHT_STAT_STAGE_LIMIT) {
                    pokemonSetStatus(def, 0, actType, 0, (s8)(count + 1));
                    fightSeq_CallWazaSeq(fightSeqJoutaiWazaSeq);
                    bSeqIssued = 1;
                }
            }
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_THAW:
            if (fightOutPokemonIsJoutaiDataId(def, JOUTAI_FREEZE) == 1 &&
                fightOutPokemonCheckFightOut(def) == 1 &&
                atk != def &&
                fightOutPokemon_GetMyselfDamageSpeValue(def) != 0 &&
                fightWazaIsHit(waza) == 1 &&
                wazaDB_GetZokuseiDataId(useMove) == ZOKUSEI_DATA_ID_FIRE) {
                fightOutPokemonInitJoutaiDataId(def, JOUTAI_FREEZE);
                fightSeqCheckSetHensinBuffToMotoPokemon(def, POKEMON_STATUS_CONDITION);
                fightSeq_CallWazaSeq(fightSeqHensinWazaSeq);
                bSeqIssued = 1;
            }
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_SYNCHRONIZE_DEFENDER:
            fightSeqFightOutPokemonTokuseiCheckSinkuroDef(atk, def);
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_HIT_ABILITY:
            if (fightSeqFightOutPokemonTokuseiCheckWazaHitAfter(atk, def) != 0)
                bSeqIssued = 1;
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_ALL_HIT_ABILITIES:
            fightSeqFightOutPokemonTokuseiCheckWazaHitAfterAll();
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_SYNCHRONIZE_ATTACKER:
            fightSeqFightOutPokemonTokuseiCheckSinkuroAtk(atk, def);
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_CHOICE_LOCK:
            if ((ServerStatusFlag & FIGHT_SEQ_FLAG_ACTION_IN_PROGRESS) != 0 &&
                heldItem == SOUBI_CHOICE_BAND &&
                useWaza != WAZA_DATA_ID_STRUGGLE &&
                fightOutPokemonCheckWriteJoutaiDataId(atk, JOUTAI_ID_CHOICE_ITEM_LOCK) == JOUTAI_WRITE_ACCEPT) {
                if (useWaza == WAZA_DATA_ID_BATON_PASS &&
                    fightWazaIsJoutaiDataId(waza, JOUTAI_ID_REGULATION_REJECTED_MOVE) == 1) {
                    fightSeqWkcActStep++;
                    break;
                }
                fightOutPokemonWriteJoutaiDataId(atk, JOUTAI_ID_CHOICE_ITEM_LOCK, 0);
                fightOutPokemonSetIdBuffJoutaiDataId(atk, JOUTAI_ID_CHOICE_ITEM_LOCK, useWaza);
            }
            if (fightOutPokemonIsJoutaiDataId(atk, JOUTAI_ID_CHOICE_ITEM_LOCK) == 1) {
                u16 param = fightOutPokemonGetIdBuffJoutaiDataId(atk, JOUTAI_ID_CHOICE_ITEM_LOCK);
                if (pokemonSearchWazaDataId(fightOutPokemonGetPokemonPtr(atk), param) < 0)
                    fightOutPokemonInitJoutaiDataId(atk, JOUTAI_ID_CHOICE_ITEM_LOCK);
            }
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_STOCK_ITEM:
            (void)fightFloorLoopValidFightOutPokemon(0, _WsWkcActSubSetStockItem, 0, 0);
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_ALL_HIT_ITEMS:
            fightSeqSoubiItemCheckWazaHitAfterAll();
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_HIT_ITEM:
            fightSeqSoubiItemCheckWazaHitAfter(atk, def);
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_ATTACKER_VISIBILITY:
            if (fightOutPokemonCheckFightOut(atk) == 1) {
                u16 kie = fightOutPokemonIsJoutaiKie(atk);
                if (kie != 0) {
                    if (kie != JOUTAI_ID_FLYING)
                        fightOutPokemonSetVisibility(atk, 0);
                    else
                        Sequence_SetStatus(fightOutPokemon_GetSequencePtr(atk), MODEL_SEQUENCE_STATUS_FLYING);
                }
            }
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_ATTACKER_STATUS:
            if (fightOutPokemonCheckFightOut(atk) == 1) {
                if (!hit ||
                    !fightOutPokemonIsJoutaiKie(atk) ||
                    fightOutPokemonCheckNoAttackFlag(atk) == 1) {
                    fightOutPokemonInitJoutaiKie(atk);
                    if (useMove != WAZA_DATA_ID_SURF && useMove != WAZA_DATA_ID_MUDDY_WATER) {
                        fightOutPokemon_SetVanishoffFlag(atk, 1);
                        fightSeqVanishOff(atk);
                    }
                }
            }
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_DEFENDER_VISIBILITY:
            if (fightOutPokemonCheckFightOut(def) == 1 &&
                !fightOutPokemon_GetVanishoffFlag(def) &&
                !fightOutPokemonIsJoutaiKie(def))
                fightSeqVanishOff(def);
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_SUBSTITUTE:
            (void)fightFloorLoopValidFightOutPokemon(0, _WsWkcActSubJoutaiMigawari, 0, 0);
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_HISTORY:
            if ((ServerStatusFlag & FIGHT_SEQ_FLAG_ATTACKER_DEFENDER_SWAPPED) != 0) {
                fightFloorReplaceFightOutPokemonAttackToDefense(0);
                ServerStatusFlag &= ~FIGHT_SEQ_FLAG_ATTACKER_DEFENDER_SWAPPED;
                atk = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
                if (atk != 0) {
                    heldItem = fightOutPokemonGetSoubiItemSoubiDataId(atk);
                    waza = fightOutPokemon_GetFightWazaPtr(atk);
                    hit = fightWazaIsHit(waza);
                    useMove = fightOutPokemon_GetUseWazaDataId(atk);
                    useWaza = fightOutPokemon_GetMotoWazaDataId(atk);
                    wazaId = (useWaza == WAZA_DATA_ID_INVALID) ? WAZA_DATA_ID_NONE : useWaza;
                    zokusei = fightWaza_GetZokusei(waza);
                    targets = wazaDB_GetRangeId(useMove);
                    seqId = wazaDB_GetSeqId(useMove);
                    iryoku = (u16)wazaDB_GetIryoku(useMove);
                } else {
                    heldItem = 0;
                    waza = 0;
                    hit = 0;
                    useMove = 0;
                    useWaza = 0;
                    wazaId = 0;
                    zokusei = ZOKUSEI_DATA_ID_MYSTERY;
                    targets = 1;
                    seqId = 0;
                    iryoku = 0;
                }
                def = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
                if (def != 0) {
                    dmgAtk = fightOutPokemon_GetMyselfDamageAtkValue(def);
                    dmgSpe = fightOutPokemon_GetMyselfDamageSpeValue(def);
                } else {
                    dmgAtk = 0;
                    dmgSpe = 0;
                }
            }
            if (wazaDB_GetSeqId(wazaId) != WAZA_SEQUENCE_ID_BATON_PASS ||
                (wazaDB_GetSeqId(wazaId) == WAZA_SEQUENCE_ID_BATON_PASS && !hit))
                fightOutPokemon_SetSketchWazaDataId(atk, useWaza);
            if (fightOutPokemonCheckFightOut(atk) == 1) {
                if (wazaDB_GetSeqId(wazaId) != WAZA_SEQUENCE_ID_BATON_PASS) {
                    if ((ServerStatusFlag & FIGHT_SEQ_FLAG_ACTION_IN_PROGRESS) != 0) {
                        fightOutPokemon_SetLastSelectWazaDataId(atk, useWaza);
                        fightOutPokemon_SetLastUseWazaDataId(atk, useMove);
                    } else {
                        fightOutPokemon_SetLastSelectWazaDataId(atk, WAZA_DATA_ID_INVALID);
                        fightOutPokemon_SetLastUseWazaDataId(atk, WAZA_DATA_ID_INVALID);
                    }
                    if (fightOutPokemonCheckFightOut(def) == 1)
                        fightOutPokemon_SetLastReceiveWazaTargetDataId(
                            def,
                            fightTargetGetTragetPtrToRelativeHostSideFightTargetId(atk, fightType));
                    if ((ServerStatusFlag & FIGHT_SEQ_FLAG_ACTION_IN_PROGRESS) != 0 && hit == 1) {
                        if (useWaza == WAZA_DATA_ID_INVALID) {
                            fightOutPokemon_SetHitWazaDataId(def, useWaza);
                        } else {
                            fightOutPokemon_SetHitWazaDataId(def, useMove);
                            fightOutPokemon_SetHitWazaZokuseiDataId(def, zokusei);
                        }
                    } else {
                        fightOutPokemon_SetHitWazaDataId(def, WAZA_DATA_ID_INVALID);
                    }
                }
            }
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_COUNTER_MOVE:
            if (fightOutPokemonCheckValid(atk) == 1 &&
                wazaDB_GetOumugaesi(wazaId) == 1 &&
                (ServerStatusFlag & FIGHT_SEQ_FLAG_ACTION_IN_PROGRESS) != 0 &&
                atk != def &&
                fightOutPokemonCheckValid(def) == 1 &&
                hit == 1)
                fightOutPokemonSetOumuWazaDataId(def, atk, useWaza);
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_OTHER_TARGET:
            if ((ServerStatusFlag & FIGHT_SEQ_FLAG_BADGE_CHECK_ABORTED) == 0 &&
                fightFloor_GetFightSideFightOutPokemonSideMax(0) >= 2 &&
                !fightOutPokemon_GetTameWazaFlag(atk) &&
                targets == WAZA_RANGE_OTHER_POKEMON_ON_SIDE &&
                (ServerStatusFlag & FIGHT_SEQ_FLAG_REPLAYING_MOVE_ON_OTHER_BATTLER) == 0) {
                FightOutPokemon *target = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE, def);
                if (fightOutPokemonCheckFightOut(target) == 1) {
                    fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, target);
                    fightSeqLoopInitAct(waza);
                    ServerStatusFlag |= FIGHT_SEQ_FLAG_REPLAYING_MOVE_ON_OTHER_BATTLER;
                    fightSeqWkcActStep = 0;
                    fightSeq_CallWazaSeq(fightSeqHitWazaSeq);
                    fightSeq_JumpWazaSeq(wazaSeqJumpTbl[seqId]);
                    return;
                }
                ServerStatusFlag |= FIGHT_SEQ_FLAG_REPLAYING_MOVE_ON_OTHER_BATTLER;
            }
            fightSeqWkcActStep++;
            break;
        case FIGHT_SEQ_AFTER_MOVE_FINISH:
            if (posMode == FIGHT_SEQ_AFTER_MOVE_MODE_ALL) {
                fightMenuAllFightTrainerCloseStatusMenu(0);
                fightMenuAllFightOutPokemonCloseStatusMenu(0);
                fightMenuCloseMsg();
            }
            break;
        }

        if (posMode == FIGHT_SEQ_AFTER_MOVE_MODE_UNTIL_NO_SEQUENCE && !bSeqIssued)
            fightSeqWkcActStep = FIGHT_SEQ_AFTER_MOVE_TERMINAL;
        if (posMode == FIGHT_SEQ_AFTER_MOVE_MODE_UNTIL_STAGE && posValue == fightSeqWkcActStep)
            fightSeqWkcActStep = FIGHT_SEQ_AFTER_MOVE_TERMINAL;
    } while (fightSeqWkcActStep != FIGHT_SEQ_AFTER_MOVE_TERMINAL && !bSeqIssued);

    if (fightSeqWkcActStep == FIGHT_SEQ_AFTER_MOVE_TERMINAL && !bSeqIssued)
        fightSeq_NextWazaSeq(3);
}

extern "C" void WS_TYPE_CHECK_NO_HIT(void)
{
    (void)fightSeq_GetWazaSeq();
    fightSeqTypeCheck(0, 1, 1, 0, 1);
}
