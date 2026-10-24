#include <Pokemon.h>
#include <fightSeq.h>
#include <fightFloor.h>
#include <fightSeq.h>
#include <fightTrainer.h>
#include <hsdRandom.h>
#include <fightMenu.h>
#include <fightTarget.h>
#include <pokemonDB.h>
#include <fightPokemon.h>
#include <waza.h>
#include <itemBios.h>
#include <GSmsg.h>
#include <string.h>

extern "C" {
// Sequence scripts referenced by the add-on (tsuika) action dispatch table.
extern u8 tsuikaSeqScriptDefault[];
extern u8 tsuikaSeqScriptPoison[];
extern u8 tsuikaSeqScriptBurn[];
extern u8 tsuikaSeqScriptFreeze[];
extern u8 tsuikaSeqScriptParalysis[];
extern u8 tsuikaSeqScript10[];
extern u8 tsuikaSeqScriptBadPoison[];
extern u8 tsuikaSeqScriptCoinPayout[];
extern u8 tsuikaSeqScript13[];
extern u8 tsuikaSeqScriptConfusion[];
extern u8 tsuikaSeqScriptRecoil[];

// Add-on action id -> sequence script, indexed by the add-on action id.
u8 *TSUIKASeqTbl[39] = {
    tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptPoison, tsuikaSeqScriptBurn,
    tsuikaSeqScriptFreeze, tsuikaSeqScriptParalysis, tsuikaSeqScriptBadPoison, tsuikaSeqScriptConfusion,
    tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScript10, tsuikaSeqScriptCoinPayout,
    tsuikaSeqScriptDefault, tsuikaSeqScript13, tsuikaSeqScriptRecoil, tsuikaSeqScriptDefault,
    tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault,
    tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault,
    tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault,
    tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault,
    tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptDefault,
    tsuikaSeqScriptDefault, tsuikaSeqScriptDefault, tsuikaSeqScriptRecoil
};
}

extern "C" {
extern u8 fightSeqSynchronizeStatus;
extern u16 fightSeqWhiteHerbStatusIds[8];
extern u8 seqSinkuroStatus[];
extern u8 seqTurnCureParalysis[];
extern u8 seqCureParalysis[];
extern u8 seqTurnCurePoison[];
extern u8 seqCurePoison[];
extern u8 seqTurnCureBurn[];
extern u8 seqCureBurn[];
extern u8 seqTurnCureFreeze[];
extern u8 seqCureFreeze[];
extern u8 seqTurnCureSleep[];
extern u8 seqCureSleep[];
extern u8 seqTurnCureConfusion[];
extern u8 seqCureConfusion[];
extern u8 seqTurnCureInfatuation[];
extern u8 seqCureInfatuation[];
extern u8 seqTurnCureWhiteHerb[];
extern u8 seqCureWhiteHerb[];
extern u8 seqSoubiRecoverHp[];
extern u8 seqSoubiRecoverPp[];
extern u8 seqSoubiRecoverHpTurn[];
extern u8 seqSoubiRecoverHpMsg[];
extern u8 seqSoubiRecoverHpDislike[];
extern u8 seqSoubiStatUp[];
extern u8 seqSoubiFocusEnergy[];
}

u8 _fightSeqSoubiItemCheckWazaHitAfterAllSub(void *pokemon, u16 target, void *argument);
u8 *_fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(FightOutPokemon *pokemon, u16 status,
    u8 *sequence);
u8 *_fightSeqSoubiItemCheckTurnSubAbiCntUp(FightOutPokemon *pokemon, u8 mode, long divisor,
    u8 type, u8 actionBase, u8 message);
u8 *_fightSeqSoubiItemCheckTurnSubTasteHpkaihuku(FightOutPokemon *pokemon, u16 taste,
    u8 mode, u16 hp, u16 maxHp, long divisor, FIGHT_WAZA *waza);
u8 _fightSeqFightOutPokemonTokuseiCheckWazaHitAfterAllSub(void *pokemon, u16 target,
    void *argument);

enum {
    ITEM_CHECK_NONE = 0,
    ITEM_CHECK_STATUS = 1,
    ITEM_CHECK_VOLATILE_STATUS = 2,
    ITEM_CHECK_PP = 3,
    ITEM_CHECK_HP = 4,
    ITEM_CHECK_STAT = 5
};

enum {
    SOUBI_HP = 1,
    SOUBI_PARALYSIS = 2,
    SOUBI_SLEEP = 3,
    SOUBI_POISON = 4,
    SOUBI_BURN = 5,
    SOUBI_FREEZE = 6,
    SOUBI_PP = 7,
    SOUBI_CONFUSION = 8,
    SOUBI_ALL_STATUS = 9,
    SOUBI_SPICY_HP = 10,
    SOUBI_DRY_HP = 11,
    SOUBI_SWEET_HP = 12,
    SOUBI_BITTER_HP = 13,
    SOUBI_SOUR_HP = 14,
    SOUBI_ATTACK = 15,
    SOUBI_DEFENSE = 16,
    SOUBI_SPEED = 17,
    SOUBI_SPECIAL_ATTACK = 18,
    SOUBI_SPECIAL_DEFENSE = 19,
    SOUBI_CRITICAL = 20,
    SOUBI_RANDOM_STAT = 21,
    SOUBI_WHITE_HERB = 23,
    SOUBI_INFATUATION = 28,
    SOUBI_KINGS_ROCK = 30,
    SOUBI_AMULET_COIN = 32,
    SOUBI_LEFTOVERS = 43,
    SOUBI_SHELL_BELL = 62
};

enum {
    SEQ_WORK_STAT_MESSAGE_MODE = 0x160A5,
    TOKUSEI_SYNCHRONIZE = 28,
    SEQ_SYNCHRONIZE_PENDING = 0x4000,
    SEQ_SYNCHRONIZE_ACTIVE = 0x2000,
    SEQ_STATUS_MASK = 0x3F,
    SEQ_DEFENDER_STATUS = 0x40,
    SEQ_STATUS_BAD_POISON = 6,
    SEQ_STATUS_POISON = 2,
    SEQ_STATUS_FLINCH = 8,
    STAT_STAGE_NORMAL = 6,
    STAT_STAGE_MAX = 12,
    STAT_ACTION_RAISE_ONE = 0x10,
    STAT_ACTION_RAISE_TWO = 0x20,
    STAT_MESSAGE_ATTACK = 0x0F,
    STAT_MESSAGE_DEFENSE = 0x10,
    STAT_MESSAGE_SPEED = 0x11,
    STAT_MESSAGE_SPECIAL_ATTACK = 0x12,
    STAT_MESSAGE_SPECIAL_DEFENSE = 0x13,
    STAT_MESSAGE_RAISE_TWO_BASE = 0x26
};

extern "C" {
u8 wazaDB_GetDageki(u16 move);
u8 wazaCheckDarkWaza(u16 move);
u16 fightOutPokemonGetWazaZokuseiDataId(FightOutPokemon *pokemon);
u8 fightOutPokemon_GetKonranMyselfAttackFlag(FightOutPokemon *pokemon);
u8 fightOutPokemonGetSex(FightOutPokemon *pokemon);
void fightOutPokemonSetZokuseiDataId(FightOutPokemon *pokemon, u8 slot, u16 type);
u32 zokuseiBiosGetName(u16 type);
extern u8 seqTokuseiColorChange[];
extern u8 seqTokuseiRoughSkin[];
extern u8 seqTokuseiContactStatus[];
extern u8 seqTokuseiCuteCharm[];
extern u8 seqTokuseiCureStatus[];
}

enum {
    TOKUSEI_LIMBER = 7,
    TOKUSEI_STATIC = 9,
    TOKUSEI_OBLIVIOUS = 12,
    TOKUSEI_INSOMNIA = 15,
    TOKUSEI_COLOR_CHANGE = 16,
    TOKUSEI_IMMUNITY = 17,
    TOKUSEI_OWN_TEMPO = 20,
    TOKUSEI_ROUGH_SKIN = 24,
    TOKUSEI_EFFECT_SPORE = 27,
    TOKUSEI_POISON_POINT = 38,
    TOKUSEI_MAGMA_ARMOR = 40,
    TOKUSEI_WATER_VEIL = 41,
    TOKUSEI_FLAME_BODY = 49,
    TOKUSEI_CUTE_CHARM = 56,
    TOKUSEI_VITAL_SPIRIT = 72,
    SEQ_STATUS_SLEEP = 1,
    SEQ_STATUS_BURN = 3,
    SEQ_STATUS_PARALYSIS = 5,
    WAZA_STRUGGLE = 165,
    TOKUSEI_CURE_NONE = 0,
    TOKUSEI_CURE_STATUS = 1,
    TOKUSEI_CURE_CONFUSION = 2,
    TOKUSEI_CURE_INFATUATION = 3
};

extern "C" u8 fightSeqFightOutPokemonTokuseiCheckWazaHitAfter(FightOutPokemon *attacker,
    FightOutPokemon *defender)
{
    s16 physicalDamage = fightOutPokemon_GetMyselfDamageAtkValue(defender);
    s16 specialDamage = fightOutPokemon_GetMyselfDamageSpeValue(defender);
    u16 attackerAbility = fightOutPokemonGetTokuseiDataId(attacker);
    u16 defenderAbility = fightOutPokemonGetTokuseiDataId(defender);
    u16 move = fightOutPokemon_GetUseWazaDataId(attacker);
    u16 power = wazaDB_GetIryoku(move);
    u16 type = fightOutPokemonGetWazaZokuseiDataId(attacker);
    u8 contact = wazaDB_GetDageki(move);
    FIGHT_WAZA *waza = fightOutPokemon_GetFightWazaPtr(attacker);
    u8 hit = fightWazaIsHit(waza);
    u8 selfAttack = fightOutPokemon_GetKonranMyselfAttackFlag(attacker);
    u8 effectSporeStatuses[3] = {SEQ_STATUS_SLEEP, SEQ_STATUS_POISON, SEQ_STATUS_PARALYSIS};
    u8 result = 0;
    u8 attackerSex = fightOutPokemonGetSex(attacker);
    u8 defenderSex = fightOutPokemonGetSex(defender);
    u32 fightType = fightFloor_GetFightType(0);
    u16 defenderTarget = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(defender, fightType);

    switch (defenderAbility) {
    case TOKUSEI_COLOR_CHANGE:
        if (hit == 1 && move != WAZA_STRUGGLE && power && !wazaCheckDarkWaza(move) &&
            (physicalDamage || specialDamage) && !fightOutPokemonIsZokuseiDataId(defender, type) &&
            fightOutPokemonCheckFightOut(defender) == 1) {
            for (u8 slot = 0; slot < 2; slot++)
                fightOutPokemonSetZokuseiDataId(defender, slot, type);
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(zokuseiBiosGetName(type)));
            fightSeq_CallWazaSeq(seqTokuseiColorChange);
            result = 1;
        }
        break;
    case TOKUSEI_ROUGH_SKIN:
        if (hit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 && !selfAttack &&
            (physicalDamage || specialDamage) && contact) {
            fightWaza_SetDamage(waza, fightOutPokemonMaxHpWaruValue(attacker, 16));
            fightSeq_CallWazaSeq(seqTokuseiRoughSkin);
            result = 1;
        }
        break;
    case TOKUSEI_EFFECT_SPORE:
        if (hit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 && !selfAttack &&
            (physicalDamage || specialDamage) && contact && (u16)HSD_Rand() % 10 == 0) {
            u8 choice = (u16)HSD_Rand() % 3U;
            ServerStatusFlag |= SEQ_SYNCHRONIZE_ACTIVE;
            fightSeqWazaWork[SEQ_STATUS_PARAMETER] = effectSporeStatuses[choice];
            fightSeqWazaWork[SEQ_STATUS_PARAMETER] += SEQ_DEFENDER_STATUS;
            fightSeq_CallWazaSeq(seqTokuseiContactStatus);
            result = 1;
        }
        break;
    case TOKUSEI_POISON_POINT:
        if (hit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 && !selfAttack &&
            (physicalDamage || specialDamage) && contact && (u16)HSD_Rand() % 3 == 0) {
            fightSeqWazaWork[SEQ_STATUS_PARAMETER] = SEQ_STATUS_POISON + SEQ_DEFENDER_STATUS;
            ServerStatusFlag |= SEQ_SYNCHRONIZE_ACTIVE;
            fightSeq_CallWazaSeq(seqTokuseiContactStatus);
            result = 1;
        }
        break;
    case TOKUSEI_STATIC:
        if (hit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 && !selfAttack &&
            (physicalDamage || specialDamage) && contact && (u16)HSD_Rand() % 3 == 0) {
            fightSeqWazaWork[SEQ_STATUS_PARAMETER] = SEQ_STATUS_PARALYSIS + SEQ_DEFENDER_STATUS;
            ServerStatusFlag |= SEQ_SYNCHRONIZE_ACTIVE;
            fightSeq_CallWazaSeq(seqTokuseiContactStatus);
            result = 1;
        }
        break;
    case TOKUSEI_FLAME_BODY:
        if (hit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 && !selfAttack &&
            contact && (physicalDamage || specialDamage) && (u16)HSD_Rand() % 3 == 0) {
            fightSeqWazaWork[SEQ_STATUS_PARAMETER] = SEQ_STATUS_BURN + SEQ_DEFENDER_STATUS;
            ServerStatusFlag |= SEQ_SYNCHRONIZE_ACTIVE;
            fightSeq_CallWazaSeq(seqTokuseiContactStatus);
            result = 1;
        }
        break;
    case TOKUSEI_CUTE_CHARM:
        if (hit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 && !selfAttack &&
            contact && (physicalDamage || specialDamage) &&
            fightOutPokemonCheckFightOut(defender) == 1 && (u16)HSD_Rand() % 3 == 0 &&
            attackerAbility != TOKUSEI_OBLIVIOUS && attackerSex != defenderSex &&
            fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_INFATUATION) == JOUTAI_WRITE_ACCEPT &&
            attackerSex != POKEMON_SEX_NONE && defenderSex != POKEMON_SEX_NONE) {
            fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_INFATUATION, defenderTarget);
            fightSeq_CallWazaSeq(seqTokuseiCuteCharm);
            result = 1;
        }
        break;
    }
    return result;
}


u8 _fightSeqFightOutPokemonTokuseiCheckWazaHitAfterAllSub(void *target, u16, void *)
{
    FightOutPokemon *pokemon = (FightOutPokemon *)target;
    for (;;) {
        if (!fightOutPokemonCheckFightOut(pokemon)) return 1;
        u16 ability = fightOutPokemonGetTokuseiDataId(pokemon);
        Pokemon *record = fightOutPokemonGetPokemonPtr(pokemon);
        u8 category = TOKUSEI_CURE_NONE;
        switch (ability) {
        case TOKUSEI_IMMUNITY:
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_POISON) == 1 ||
                fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_BAD_POISON) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_POISON)));
                category = TOKUSEI_CURE_STATUS;
            }
            break;
        case TOKUSEI_OWN_TEMPO:
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_CONFUSION) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_CONFUSION)));
                category = TOKUSEI_CURE_CONFUSION;
            }
            break;
        case TOKUSEI_LIMBER:
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_PARALYSIS) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_PARALYSIS)));
                category = TOKUSEI_CURE_STATUS;
            }
            break;
        case TOKUSEI_INSOMNIA:
        case TOKUSEI_VITAL_SPIRIT:
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_SLEEP) == 1) {
                fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_AKUMU);
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_SLEEP)));
                category = TOKUSEI_CURE_STATUS;
            }
            break;
        case TOKUSEI_WATER_VEIL:
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_BURN) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_BURN)));
                category = TOKUSEI_CURE_STATUS;
            }
            break;
        case TOKUSEI_MAGMA_ARMOR:
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_FREEZE) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_FREEZE)));
                category = TOKUSEI_CURE_STATUS;
            }
            break;
        case TOKUSEI_OBLIVIOUS:
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_INFATUATION) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_INFATUATION)));
                category = TOKUSEI_CURE_INFATUATION;
            }
            break;
        }
        if (category) {
            switch (category) {
            case TOKUSEI_CURE_STATUS:
                pokemonInitJoutai(record);
                fightOutPokemonResetSeqStatus(pokemon, 0);
                break;
            case TOKUSEI_CURE_CONFUSION:
                fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_CONFUSION);
                break;
            case TOKUSEI_CURE_INFATUATION:
                fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_INFATUATION);
                break;
            }
            fightSeqCheckSetHensinBuffToMotoPokemon(pokemon, POKEMON_STATUS_CONDITION);
            fightFloor_SetAppointPokemonPtr(0, pokemon);
            fightSeq_CallWazaSeq(seqTokuseiCureStatus);
        } else break;
    }
    return 1;
}


extern "C" void fightSeqFightOutPokemonTokuseiCheckWazaHitAfterAll(void)
{
    fightFloorLoopValidFightOutPokemon(0, _fightSeqFightOutPokemonTokuseiCheckWazaHitAfterAllSub,
        0, 0);
}


extern "C" void fightSeqFightOutPokemonTokuseiCheckSinkuroDef(FightOutPokemon *,
    FightOutPokemon *defender)
{
    u16 ability = fightOutPokemonGetTokuseiDataId(defender);
    if (!fightOutPokemonCheckFightOut(defender)) return;
    if (ability != TOKUSEI_SYNCHRONIZE) return;
    u32 flags = ServerStatusFlag;
    if (!(flags & SEQ_SYNCHRONIZE_PENDING)) return;
    ServerStatusFlag = flags & ~SEQ_SYNCHRONIZE_PENDING;
    fightSeqSynchronizeStatus &= SEQ_STATUS_MASK;
    if (fightSeqSynchronizeStatus == SEQ_STATUS_BAD_POISON) fightSeqSynchronizeStatus = SEQ_STATUS_POISON;
    fightSeqWazaWork[SEQ_STATUS_PARAMETER] = fightSeqSynchronizeStatus + SEQ_DEFENDER_STATUS;
    fightFloor_SetAppointPokemonPtr(0, defender);
    ServerStatusFlag |= SEQ_SYNCHRONIZE_ACTIVE;
    fightSeq_CallWazaSeq(seqSinkuroStatus);
}


extern "C" void fightSeqFightOutPokemonTokuseiCheckSinkuroAtk(FightOutPokemon *attacker,
    FightOutPokemon *)
{
    u16 ability = fightOutPokemonGetTokuseiDataId(attacker);
    if (!fightOutPokemonCheckFightOut(attacker)) return;
    if (ability != TOKUSEI_SYNCHRONIZE) return;
    u32 flags = ServerStatusFlag;
    if (!(flags & SEQ_SYNCHRONIZE_PENDING)) return;
    ServerStatusFlag = flags & ~SEQ_SYNCHRONIZE_PENDING;
    fightSeqSynchronizeStatus &= SEQ_STATUS_MASK;
    if (fightSeqSynchronizeStatus == SEQ_STATUS_BAD_POISON) fightSeqSynchronizeStatus = SEQ_STATUS_POISON;
    fightSeqWazaWork[SEQ_STATUS_PARAMETER] = fightSeqSynchronizeStatus;
    fightFloor_SetAppointPokemonPtr(0, attacker);
    ServerStatusFlag |= SEQ_SYNCHRONIZE_ACTIVE;
    fightSeq_CallWazaSeq(seqSinkuroStatus);
}


extern "C" u8 fightSeqPokemonSoubiItemCheckAppear(FightOutPokemon *pokemon)
{
    u8 category = ITEM_CHECK_NONE;
    u16 item = fightOutPokemonGetSoubiItemDataId(pokemon);
    u16 effect = fightOutPokemonGetSoubiItemSoubiDataId(pokemon);
    s32 parameter = figthOutPokemonGetSoubiItemBuff(pokemon);
    FIGHT_TRAINER *trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, pokemon);
    if (!fightOutPokemonCheckFightOut(pokemon)) return 0;
    fightFloor_SetAppointItemDataId(0, item);
    switch (effect) {
    case SOUBI_AMULET_COIN:
        fightTrainer_SetOkaneBai(trainer, 2);
        break;
    case SOUBI_WHITE_HERB:
        for (u8 index = 0; index < 7; ++index) {
            if (pokemonGetStatus(pokemon, 0, fightSeqWhiteHerbStatusIds[index], 0) < STAT_STAGE_NORMAL) {
                pokemonSetStatus(pokemon, 0, fightSeqWhiteHerbStatusIds[index], 0, STAT_STAGE_NORMAL);
                category = ITEM_CHECK_STAT;
            }
        }
        if (category) {
            fightFloor_SetAppointPokemonPtr(0, pokemon);
            fightFloor_SetAttackPokemonPtr(0, pokemon);
            fightFloor_SetItemPokemonPtr(0, pokemon);
            fightSeq_CallWazaSeq(seqTurnCureWhiteHerb);
        }
        break;
    }
    return category;
}


u8 *_fightSeqSoubiItemCheckTurnSubTasteHpkaihuku(FightOutPokemon *pokemon, u16 taste,
    u8 mode, u16 hp, u16 maxHp, long divisor, FIGHT_WAZA *waza)
{
    u8 *sequence = 0;
    if ((fightOutPokemonIsNokoriHpFollowing(pokemon, 2) && mode == 0) ||
        (!fightOutPokemonIsHpMantan(pokemon) && mode == 2)) {
        TasteData *tasteData = tasteDataGetPtr(taste);
        fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(tasteDataGetNigateMsgDataId(tasteData)));
        s32 recovery = fightOutPokemonMaxHpWaruValue(pokemon, divisor);
        if (hp + recovery > maxHp) recovery = maxHp - hp;
        fightWaza_SetDamage(waza, -recovery);
        if (fightOutPokemonGetTasteLike(pokemon, taste) == -1) {
            sequence = seqSoubiRecoverHpDislike;
        } else {
            sequence = seqSoubiRecoverHp;
        }
    }
    return sequence;
}


u8 *_fightSeqSoubiItemCheckTurnSubAbiCntUp(FightOutPokemon *pokemon, u8 mode, long divisor,
    u8 type, u8 actionBase, u8 message)
{
    u8 *sequence = 0;
    if (mode == 0 || mode == 2) {
        s32 statusId = fightSeqCondChgActTypeToPokemonStatusId(type);
        u8 stage = pokemonGetStatus(pokemon, 0, statusId, 0);
        if (fightOutPokemonIsNokoriHpFollowing(pokemon, (u16)divisor) && stage < STAT_STAGE_MAX) {
            UserWork[SEQ_WORK_STAT_ACTION] = type + actionBase;
            UserWork[SEQ_WORK_STAT_MESSAGE] = message;
            UserWork[SEQ_WORK_STAT_MESSAGE_MODE] = 0;
            sequence = seqSoubiStatUp;
        }
    }
    return sequence;
}


u8 *_fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(FightOutPokemon *pokemon, u16 status,
    u8 *sequence)
{
    u8 *result = 0;
    if (fightOutPokemonIsJoutaiDataId(pokemon, status) == 1) {
        fightOutPokemonInitJoutaiDataId(pokemon, status);
        if (status == JOUTAI_SLEEP) {
            fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_AKUMU);
        } else if (status == JOUTAI_BAD_POISON) {
            fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_POISON);
        } else if (status == JOUTAI_POISON) {
            fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_BAD_POISON);
        }
        result = sequence;
    }
    return result;
}


extern "C" u8 fightSeqSoubiItemCheckTurn(FightOutPokemon *pokemon, u8 mode)
{
    u16 hp;
    u8 pp;
    Pokemon *originalRecord;
    s32 parameter;
    u8 *sequence;
    u16 maxHp;
    u16 slot;
    u16 move;
    u16 count;
    Pokemon *record;
    u8 category = ITEM_CHECK_NONE;
    sequence = 0;
    u8 types[5] = {FIGHT_STAT_ATTACK, FIGHT_STAT_DEFENSE, FIGHT_STAT_SPEED,
        FIGHT_STAT_SPECIAL_ATTACK, FIGHT_STAT_SPECIAL_DEFENSE};
    s32 candidates[5] = {-1, -1, -1, -1, -1};

    if (!fightOutPokemonCheckFightOut(pokemon)) return 0;

    u16 item = fightOutPokemonGetSoubiItemDataId(pokemon);
    u16 effect = fightOutPokemonGetSoubiItemSoubiDataId(pokemon);
    parameter = figthOutPokemonGetSoubiItemBuff(pokemon);
    FIGHT_WAZA *waza = fightOutPokemon_GetFightWazaPtr(pokemon);
    record = fightOutPokemonGetPokemonPtr(pokemon);
    hp = pokemon_GetHp(record);
    maxHp = pokemon_GetMaxHp(record);
    fightFloor_SetAppointItemDataId(0, item);

    switch (effect) {
    case SOUBI_HP:
        if ((fightOutPokemonIsNokoriHpFollowing(pokemon, 2) && mode == 0) ||
            (!fightOutPokemonIsHpMantan(pokemon) && mode == 2)) {
            s32 recovery = parameter;
            if (hp + parameter > maxHp) recovery = maxHp - hp;
            fightWaza_SetDamage(waza, -recovery);
            sequence = seqSoubiRecoverHp;
            category = ITEM_CHECK_HP;
        }
        break;
    case SOUBI_PP:
        if (mode == 0 || mode == 2) {
            FightPokemon *original = fightOutPokemon_GetMotoFightPokemonPtr(pokemon);
            originalRecord = fightPokemonGetPokemonPtr(original);
            slot = 0;
            for (; slot < 4; ++slot) {
                if (pokemonWazaCheckValid(originalRecord, slot)) {
                    move = pokemon_GetPokemonWazaDataId(originalRecord, slot);
                    pp = pokemon_GetPokemonWazaPp(originalRecord, slot);
                    if (pp == 0) break;
                }
            }
            if (slot == 4) break;
            u8 maxPp = pokemonWazaGetMaxPP(originalRecord, slot);
            u8 recoveredPp = pp + parameter;
            if (pp + parameter > maxPp) recoveredPp = maxPp;
            pokemon_SetPokemonWazaPp(originalRecord, slot, recoveredPp);
            fightSeqPpdecMonomane(pokemon, (u8)slot, 1);
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(wazaDB_GetName(move)));
            sequence = seqSoubiRecoverPp;
            category = ITEM_CHECK_PP;
        }
        break;
    case SOUBI_WHITE_HERB:
        for (u16 index = 0; index < 7; ++index) {
            if (pokemonGetStatus(pokemon, 0, fightSeqWhiteHerbStatusIds[index], 0) < STAT_STAGE_NORMAL) {
                pokemonSetStatus(pokemon, 0, fightSeqWhiteHerbStatusIds[index], 0, STAT_STAGE_NORMAL);
                category = ITEM_CHECK_STAT;
            }
        }
        if (category) sequence = seqTurnCureWhiteHerb;
        break;
    case SOUBI_LEFTOVERS:
        if ((!fightOutPokemonIsHpMantan(pokemon) && mode == 0) ||
            (!fightOutPokemonIsHpMantan(pokemon) && mode == 2)) {
            s32 recovery = fightOutPokemonMaxHpWaruValue(pokemon, 16);
            if (hp + recovery > maxHp) recovery = maxHp - hp;
            fightWaza_SetDamage(waza, -recovery);
            sequence = seqSoubiRecoverHpTurn;
            category = ITEM_CHECK_HP;
        }
        break;
    case SOUBI_SPICY_HP:
        sequence = _fightSeqSoubiItemCheckTurnSubTasteHpkaihuku(pokemon, 0, mode, hp,
            maxHp, parameter, waza);
        if (sequence) category = ITEM_CHECK_HP;
        break;
    case SOUBI_DRY_HP:
        sequence = _fightSeqSoubiItemCheckTurnSubTasteHpkaihuku(pokemon, 1, mode, hp,
            maxHp, parameter, waza);
        if (sequence) category = ITEM_CHECK_HP;
        break;
    case SOUBI_SWEET_HP:
        sequence = _fightSeqSoubiItemCheckTurnSubTasteHpkaihuku(pokemon, 2, mode, hp,
            maxHp, parameter, waza);
        if (sequence) category = ITEM_CHECK_HP;
        break;
    case SOUBI_BITTER_HP:
        sequence = _fightSeqSoubiItemCheckTurnSubTasteHpkaihuku(pokemon, 3, mode, hp,
            maxHp, parameter, waza);
        if (sequence) category = ITEM_CHECK_HP;
        break;
    case SOUBI_SOUR_HP:
        sequence = _fightSeqSoubiItemCheckTurnSubTasteHpkaihuku(pokemon, 4, mode, hp,
            maxHp, parameter, waza);
        if (sequence) category = ITEM_CHECK_HP;
        break;
    case SOUBI_ATTACK:
        sequence = _fightSeqSoubiItemCheckTurnSubAbiCntUp(pokemon, mode, parameter, FIGHT_STAT_ATTACK, STAT_ACTION_RAISE_ONE, STAT_MESSAGE_ATTACK);
        if (sequence) category = ITEM_CHECK_STAT;
        break;
    case SOUBI_DEFENSE:
        sequence = _fightSeqSoubiItemCheckTurnSubAbiCntUp(pokemon, mode, parameter, FIGHT_STAT_DEFENSE, STAT_ACTION_RAISE_ONE, STAT_MESSAGE_DEFENSE);
        if (sequence) category = ITEM_CHECK_STAT;
        break;
    case SOUBI_SPEED:
        sequence = _fightSeqSoubiItemCheckTurnSubAbiCntUp(pokemon, mode, parameter, FIGHT_STAT_SPEED, STAT_ACTION_RAISE_ONE, STAT_MESSAGE_SPEED);
        if (sequence) category = ITEM_CHECK_STAT;
        break;
    case SOUBI_SPECIAL_ATTACK:
        sequence = _fightSeqSoubiItemCheckTurnSubAbiCntUp(pokemon, mode, parameter, FIGHT_STAT_SPECIAL_ATTACK, STAT_ACTION_RAISE_ONE, STAT_MESSAGE_SPECIAL_ATTACK);
        if (sequence) category = ITEM_CHECK_STAT;
        break;
    case SOUBI_SPECIAL_DEFENSE:
        sequence = _fightSeqSoubiItemCheckTurnSubAbiCntUp(pokemon, mode, parameter, FIGHT_STAT_SPECIAL_DEFENSE, STAT_ACTION_RAISE_ONE, STAT_MESSAGE_SPECIAL_DEFENSE);
        if (sequence) category = ITEM_CHECK_STAT;
        break;
    case SOUBI_CRITICAL:
        if ((mode == 0 || mode == 2) &&
            fightOutPokemonIsNokoriHpFollowing(pokemon, (u16)parameter) &&
            fightOutPokemonCheckWriteJoutaiDataId(pokemon, JOUTAI_FOCUS_ENERGY) == JOUTAI_WRITE_ACCEPT) {
            fightOutPokemonWriteJoutaiDataId(pokemon, JOUTAI_FOCUS_ENERGY, 0);
            sequence = seqSoubiFocusEnergy;
            category = ITEM_CHECK_VOLATILE_STATUS;
        }
        break;
    case SOUBI_RANDOM_STAT:
        if ((mode == 0 || mode == 2) &&
            fightOutPokemonIsNokoriHpFollowing(pokemon, (u16)parameter)) {
            count = 0;
            u8 type;
            for (u16 index = 0; index < 5; ++index) candidates[index] = -1;
            for (u16 index = 0; index < 5; ++index) {
                type = types[index];
                if ((u8)pokemonGetStatus(pokemon, 0,
                    fightSeqCondChgActTypeToPokemonStatusId(type), 0) < STAT_STAGE_MAX) {
                    candidates[count++] = type;
                }
            }
            if (count < 1) break;
            u16 selected = (u16)HSD_Rand() % count;
            const s32 *candidateList = candidates;
            if (candidateList[selected] == -1) break;
            type = candidateList[selected];
            sequence = _fightSeqSoubiItemCheckTurnSubAbiCntUp(pokemon, mode, parameter,
                type, STAT_ACTION_RAISE_TWO, type + STAT_MESSAGE_RAISE_TWO_BASE);
            if (sequence) category = ITEM_CHECK_STAT;
        }
        break;
    case SOUBI_PARALYSIS:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_PARALYSIS,
            seqTurnCureParalysis);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_POISON:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_POISON,
            seqTurnCurePoison);
        if (sequence) {
            category = ITEM_CHECK_STATUS;
            break;
        }
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_BAD_POISON,
            seqTurnCurePoison);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_BURN:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_BURN,
            seqTurnCureBurn);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_FREEZE:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_FREEZE,
            seqTurnCureFreeze);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_SLEEP:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_SLEEP,
            seqTurnCureSleep);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_CONFUSION:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_CONFUSION,
            seqTurnCureConfusion);
        if (sequence) category = ITEM_CHECK_VOLATILE_STATUS;
        break;
    case SOUBI_ALL_STATUS:
        if (!fightOutPokemonIsJoutaiNormal(pokemon) ||
            fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_CONFUSION) == 1) {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = count = 0;
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_POISON) == 1 ||
                fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_BAD_POISON) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_POISON)));
                count = 1;
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_SLEEP) == 1) {
                fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_AKUMU);
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_SLEEP)));
                ++count;
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_PARALYSIS) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_PARALYSIS)));
                ++count;
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_BURN) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_BURN)));
                ++count;
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_FREEZE) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_FREEZE)));
                ++count;
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_CONFUSION) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_CONFUSION)));
                ++count;
            }
            if (count >= 2) fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 1;
            pokemonInitJoutai(record);
            fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_CONFUSION);
            fightOutPokemonResetSeqStatus(pokemon, 0);
            sequence = seqTurnCureInfatuation;
            category = ITEM_CHECK_STATUS;
        }
        break;
    case SOUBI_INFATUATION:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_INFATUATION,
            seqTurnCureInfatuation);
        if (sequence) {
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_INFATUATION)));
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
            category = ITEM_CHECK_VOLATILE_STATUS;
        }
        break;
    }

    if (category) {
        fightFloor_SetAppointPokemonPtr(0, pokemon);
        fightFloor_SetAttackPokemonPtr(0, pokemon);
        fightFloor_SetItemPokemonPtr(0, pokemon);
        switch (category) {
        case ITEM_CHECK_PP:
        case ITEM_CHECK_HP:
        case ITEM_CHECK_STAT:
            break;
        case ITEM_CHECK_STATUS:
            fightSeqCheckSetHensinBuffToMotoPokemon(pokemon, POKEMON_STATUS_CONDITION);
            break;
        }
        if (sequence) fightSeq_CallWazaSeq(sequence);
    }
    return category;
}


extern "C" u8 fightSeqSoubiItemCheckWazaHitAfter(FightOutPokemon *attacker,
    FightOutPokemon *defender)
{
    FIGHT_WAZA *waza = fightOutPokemon_GetFightWazaPtr(attacker);
    u16 item = fightOutPokemonGetSoubiItemDataId(attacker);
    u16 effect = fightOutPokemonGetSoubiItemSoubiDataId(attacker);
    s32 parameter = figthOutPokemonGetSoubiItemBuff(attacker);
    u16 defenderItem = fightOutPokemonGetSoubiItemDataId(defender);
    u16 defenderEffect = fightOutPokemonGetSoubiItemSoubiDataId(defender);
    s32 defenderParameter = figthOutPokemonGetSoubiItemBuff(defender);
    u8 result = 0;
    if (fightWaza_GetDamage(waza) == 0) return 0;

    u16 move = fightOutPokemon_GetUseWazaDataId(attacker);
    u8 hit = fightWazaIsHit(waza);
    u8 kingsRockMove = wazaDB_GetOujanosirusi(move);
    s16 physicalDamage = fightOutPokemon_GetMyselfDamageAtkValue(defender);
    s16 specialDamage = fightOutPokemon_GetMyselfDamageSpeValue(defender);
    s32 shellBellDamage = fightOutPokemon_GetKaigaraDamageValue(defender);
    switch (effect) {
    case SOUBI_KINGS_ROCK:
        if (hit == 1 && (physicalDamage != 0 || specialDamage != 0) && kingsRockMove == 1 &&
            (u16)HSD_Rand() % 100 < parameter &&
            fightOutPokemonCheckFightOut(defender) == 1) {
            fightSeqWazaWork[SEQ_STATUS_PARAMETER] = SEQ_STATUS_FLINCH;
            u8 *savedSequence = fightSeq_GetWazaSeq();
            fightSeqTuikaAct(0, 0);
            fightSeq_JumpWazaSeq(savedSequence);
        }
        break;
    case SOUBI_SHELL_BELL:
        if (hit == 1 && shellBellDamage != 0 && shellBellDamage != 0xFFFF &&
            attacker != defender && !fightOutPokemonIsHpMantan(attacker) &&
            fightOutPokemonCheckFightOut(attacker) == 1) {
            fightFloor_SetAppointItemDataId(0, item);
            fightFloor_SetItemPokemonPtr(0, attacker);
            fightFloor_SetAppointPokemonPtr(0, attacker);
            s32 recovery = -(shellBellDamage / parameter);
            if (recovery == 0) recovery = -1;
            fightWaza_SetDamage(waza, recovery);
            fightOutPokemon_SetKaigaraDamageValue(defender, 0);
            fightSeq_CallWazaSeq(seqSoubiRecoverHpMsg);
            result = 1;
        }
        break;
    }
    return result;
}


u8 _fightSeqSoubiItemCheckWazaHitAfterAllSub(void *target, u16, void *)
{
    FightOutPokemon *pokemon = (FightOutPokemon *)target;
    u8 category = ITEM_CHECK_NONE;
    Pokemon *record = fightOutPokemonGetPokemonPtr(pokemon);
    u16 item = fightOutPokemonGetSoubiItemDataId(pokemon);
    u16 effect = fightOutPokemonGetSoubiItemSoubiDataId(pokemon);
    s32 parameter = figthOutPokemonGetSoubiItemBuff(pokemon);
    u8 *sequence = 0;

    if (!fightOutPokemonCheckFightOut(pokemon)) {
        return 1;
    }
    fightFloor_SetAppointItemDataId(0, item);
    switch (effect) {
    case SOUBI_PARALYSIS:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_PARALYSIS,
            seqCureParalysis);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_POISON:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_POISON,
            seqCurePoison);
        if (sequence) {
            category = ITEM_CHECK_STATUS;
            break;
        }
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_BAD_POISON,
            seqCurePoison);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_BURN:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_BURN,
            seqCureBurn);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_FREEZE:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_FREEZE,
            seqCureFreeze);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_SLEEP:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_SLEEP,
            seqCureSleep);
        if (sequence) category = ITEM_CHECK_STATUS;
        break;
    case SOUBI_CONFUSION:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_CONFUSION,
            seqCureConfusion);
        if (sequence) category = ITEM_CHECK_VOLATILE_STATUS;
        break;
    case SOUBI_INFATUATION:
        sequence = _fightSeqSoubiItemCheckTurnSubJoutaiKaihuku(pokemon, JOUTAI_INFATUATION,
            seqCureInfatuation);
        if (sequence) {
            fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_INFATUATION)));
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
            category = ITEM_CHECK_VOLATILE_STATUS;
        }
        break;
    case SOUBI_ALL_STATUS:
        if (!fightOutPokemonIsJoutaiNormal(pokemon) ||
            fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_CONFUSION) == 1) {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_POISON) == 1 ||
                fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_BAD_POISON) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_POISON)));
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_SLEEP) == 1) {
                fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_AKUMU);
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_SLEEP)));
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_PARALYSIS) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_PARALYSIS)));
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_BURN) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_BURN)));
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_FREEZE) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_FREEZE)));
            }
            if (fightOutPokemonIsJoutaiDataId(pokemon, JOUTAI_CONFUSION) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)GSmsgGetStringPtr(joutaiDataBiosGetName(JOUTAI_CONFUSION)));
            }
            pokemonInitJoutai(record);
            fightOutPokemonInitJoutaiDataId(pokemon, JOUTAI_CONFUSION);
            fightOutPokemonResetSeqStatus(pokemon, 0);
            sequence = seqCureInfatuation;
            category = ITEM_CHECK_STATUS;
        }
        break;
    case SOUBI_WHITE_HERB:
        for (u16 index = 0; index < 7; ++index) {
            if (pokemonGetStatus(pokemon, 0, fightSeqWhiteHerbStatusIds[index], 0) < STAT_STAGE_NORMAL) {
                pokemonSetStatus(pokemon, 0, fightSeqWhiteHerbStatusIds[index], 0, STAT_STAGE_NORMAL);
                category = ITEM_CHECK_STAT;
            }
        }
        if (category) sequence = seqCureWhiteHerb;
        break;
    }

    if (category) {
        fightFloor_SetAppointPokemonPtr(0, pokemon);
        fightFloor_SetItemPokemonPtr(0, pokemon);
        if (category == ITEM_CHECK_STATUS) {
            fightSeqCheckSetHensinBuffToMotoPokemon(pokemon, POKEMON_STATUS_CONDITION);
        }
        if (sequence) fightSeq_CallWazaSeq(sequence);
    }
    return 1;
}


extern "C" void fightSeqSoubiItemCheckWazaHitAfterAll(void)
{
    fightFloorLoopValidFightOutPokemon(0, _fightSeqSoubiItemCheckWazaHitAfterAllSub, 0, 0);
}


extern "C" u8 *fightSeq_GetWazaSeq(void)
{
    return _wazaSeqAdrs;
}


extern "C" u8 *fightSeq_GetWazaSeqAdr(s32 offset)
{
    return _wazaSeqAdrs + offset;
}


extern "C" void fightSeq_NextWazaSeq(s32 count)
{
    _wazaSeqAdrs += count;
}


extern "C" void fightSeq_JumpWazaSeq(u8 *sequence)
{
    _wazaSeqAdrs = sequence;
}


extern "C" void fightSeq_CallWazaSeq(u8 *sequence)
{
    fightSeqExec(fight_seq_now_fight_action_ptr, sequence, 0);
}


extern "C" void fightSeq_CallWazaSeqCustom(FIGHT_ACTION *action, u8 *sequence, u8 mode)
{
    fightSeqExec(action, sequence, mode);
}


extern "C" u8 *fightSeq_GetWazaSeqPtr_u8(u8 byteOffset)
{
    return *(u8 **)(_wazaSeqAdrs + byteOffset);
}


extern "C" void fightSeq_ClearUserWork(void)
{
    memset(UserWork, 0, 1);
}
