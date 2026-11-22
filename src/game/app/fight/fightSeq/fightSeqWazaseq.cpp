#include <Pokemon.h>
#include <PokemonStatus.h>
#include <fightSeq.h>
#include <fightFloor.h>
#include <fightTarget.h>
#include <fightSide.h>
#include <fightAction.h>
#include <fightPokemon.h>
#include <fightPokemonBios.h>
#include <fightWaza.h>
#include <fightMenu.h>
#include <menuFightStatus.h>
#include <hero.h>
#include <fightTrainer.h>
#include <pokemonDB.h>
#include <waza.h>
#include <joutai.h>
#include <GSmsg.h>
#include <hsdRandom.h>
#include <app/tikei/tikeiBios.h>

// Command scripts called directly by the move commands in this file. The
// shared enums in fightSeq.h and waza.h carry the move, status and message ids.
extern "C" {
extern u8 fightSeqPresentDamageWazaSeq[];
extern u8 fightSeqContinueMoveMissWazaSeq[];
extern u8 fightSeqPresentHealWazaSeq[];
extern u8 fightSeqPresentFullHpWazaSeq[];
extern u8 fightSeqAttackMessageWazaSeq[];
extern u8 fightSeqRapidSpinTrapMsgSeq[];
extern u8 fightSeqRapidSpinLeechSeedMsgSeq[];
extern u8 fightSeqRapidSpinSpikesMsgSeq[];
}

// Maximum number of Pokemon a trainer brings to a fight.
enum FightTemotiPokemon { TEMOTI_POKEMON_MAX = 6 };

// Fight slots on one side of the battlefield.
enum FightPokemonSlot { FIGHT_POKEMON_SLOT_COUNT = 8 };

// Which active Pokemon kept its status because of Soundproof.
enum HealBellStatus {
    HEAL_BELL_ATTACKER_SOUNDPROOF = 1,
    HEAL_BELL_PARTNER_SOUNDPROOF = 2,
    HEAL_BELL_AROMATHERAPY = 4
};

u8 _fightSeqWsHorobinoutaSub(void *pokemon, u16 index, void *arg);

// Number of Spikes layers already lying on a side.
static inline s16 fightSideGetSpikesCount(FIGHT_SIDE *side)
{
    if (fightSideIsJoutaiDataId(side, JOUTAI_SPIKES) == 1) {
        return fightSideGetCountAsJoutaiDataId(side, JOUTAI_SPIKES);
    }
    return 0;
}

// Heal Bell and Aromatherapy cure the whole party of the user.  Soundproof
// Pokemon do not hear the bell.
extern "C" void WS_IYASHINOSUZU(void)
{
    u16 move;
    FightOutPokemon *attacker;
    FightOutPokemon *partner;
    u32 trainerNum;
    FIGHT_SIDE *side;
    FightPokemon *fightPokemon;
    u32 i;
    u32 j;
    FIGHT_TRAINER *trainer;
    Pokemon *pokemon;
    Hero *hero;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    move = fightOutPokemon_GetUseWazaDataId(attacker);
    partner = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE, attacker);
    side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, attacker);
    trainerNum = fightFloor_GetFightTypeTrainerNum(0);
    if (move == WAZA_DATA_ID_HEAL_BELL) {
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
        if (fightOutPokemonGetTokuseiDataId(attacker) != TOKUSEI_DATA_ID_SOUNDPROOF) {
            pokemon = fightOutPokemonGetPokemonPtr(attacker);
            pokemonInitJoutai(pokemon);
            fightOutPokemonResetSeqStatus(attacker, 0);
            fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_AKUMU);
            fightSeqCheckSetHensinBuffToMotoPokemon(attacker, POKEMON_STATUS_CONDITION);
        } else {
            fightSeqWazaWork[SEQ_MULTIPLE_STATUS] |= HEAL_BELL_ATTACKER_SOUNDPROOF;
        }
        if (partner != NULL && fightOutPokemonCheckValid(partner) == 1) {
            fightFloor_SetAppointPokemonPtr(0, partner);
            if (fightOutPokemonGetTokuseiDataId(partner) != TOKUSEI_DATA_ID_SOUNDPROOF) {
                pokemon = fightOutPokemonGetPokemonPtr(partner);
                pokemonInitJoutai(pokemon);
                fightOutPokemonResetSeqStatus(partner, 0);
                fightOutPokemonInitJoutaiDataId(partner, JOUTAI_AKUMU);
                fightSeqCheckSetHensinBuffToMotoPokemon(partner, POKEMON_STATUS_CONDITION);
            } else {
                fightSeqWazaWork[SEQ_MULTIPLE_STATUS] |= HEAL_BELL_PARTNER_SOUNDPROOF;
            }
        }
        for (i = 0; i < trainerNum; i++) {
            trainer = fightSideGetValidFightTrainerPtr(side, i);
            if (trainer != NULL) {
                hero = fightTrainer_GetHeroPtr(trainer);
                for (j = 0; j < TEMOTI_POKEMON_MAX; j++) {
                    fightPokemon = fightTrainerCheckTemotiPokemonFightEntry(
                        trainer, (Pokemon *)heroGetStatus(hero, HERO_STATUS_POKEMON, j));
                    if (fightPokemon != NULL && fightPokemonCheckValid(fightPokemon)) {
                        pokemon = fightPokemonGetPokemonPtr(fightPokemon);
                        if (pokemonGetTokuseiDataId(pokemon) != TOKUSEI_DATA_ID_SOUNDPROOF) {
                            pokemonInitJoutai(pokemon);
                        }
                    }
                }
            }
        }
    } else {
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = HEAL_BELL_AROMATHERAPY;
        pokemon = fightOutPokemonGetPokemonPtr(attacker);
        pokemonInitJoutai(pokemon);
        fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_AKUMU);
        fightSeqCheckSetHensinBuffToMotoPokemon(attacker, POKEMON_STATUS_CONDITION);
        if (partner != NULL && fightOutPokemonCheckValid(partner) == 1) {
            fightFloor_SetAppointPokemonPtr(0, partner);
            pokemon = fightOutPokemonGetPokemonPtr(partner);
            pokemonInitJoutai(pokemon);
            fightOutPokemonResetSeqStatus(partner, 0);
            fightOutPokemonInitJoutaiDataId(partner, JOUTAI_AKUMU);
            fightSeqCheckSetHensinBuffToMotoPokemon(partner, POKEMON_STATUS_CONDITION);
        }
        for (i = 0; i < trainerNum; i++) {
            trainer = fightSideGetValidFightTrainerPtr(side, i);
            if (trainer != NULL) {
                hero = fightTrainer_GetHeroPtr(trainer);
                for (j = 0; j < TEMOTI_POKEMON_MAX; j++) {
                    fightPokemon = fightTrainerCheckTemotiPokemonFightEntry(
                        trainer, (Pokemon *)heroGetStatus(hero, HERO_STATUS_POKEMON, j));
                    if (fightPokemon != NULL && fightPokemonCheckValid(fightPokemon)) {
                        pokemon = fightPokemonGetPokemonPtr(fightPokemon);
                        pokemonInitJoutai(pokemon);
                    }
                }
            }
        }
    }
    fightSeq_NextWazaSeq(1);
}


// A Ghost's Curse costs the user half of its HP and curses the target.
extern "C" void WS_NOROI(void)
{
    u8 *sequence;
    FightOutPokemon *attacker;
    FightOutPokemon *defender;
    FIGHT_WAZA *waza;

    sequence = fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    if (fightOutPokemonCheckWriteJoutaiDataId(defender, JOUTAI_CURSE) != JOUTAI_WRITE_ACCEPT) {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
    } else {
        fightOutPokemonWriteJoutaiDataId(defender, JOUTAI_CURSE, 0);
        fightWaza_SetDamage(waza, fightOutPokemonMaxHpWaruValue(attacker, 2));
        fightSeq_NextWazaSeq(5);
    }
}


// Spikes adds a layer to the opposing side until the limit is reached.
extern "C" void WS_MAKIBISHI(void)
{
    u8 *sequence;
    FightOutPokemon *attacker;
    FIGHT_SIDE *side;

    sequence = fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OPPOSING_SIDE, attacker);
    if (fightSideCheckWriteJoutaiDataId(side, JOUTAI_SPIKES) != JOUTAI_WRITE_ACCEPT ||
        fightSideGetSpikesCount(side) == joutaiDataBiosGetMaxCount(JOUTAI_SPIKES)) {
        fightOutPokemon_SetNoPressureFlag(attacker, 1);
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
    } else {
        fightSideWriteJoutaiDataId(side, JOUTAI_SPIKES, 0);
        fightSeq_NextWazaSeq(5);
    }
}


// Foresight marks the target as identified.
extern "C" void WS_MIYABURU(void)
{
    FightOutPokemon *defender;

    fightSeq_GetWazaSeq();
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    if (fightOutPokemonCheckWriteJoutaiDataId(defender, JOUTAI_FORESIGHT) == JOUTAI_WRITE_ACCEPT) {
        fightOutPokemonWriteJoutaiDataId(defender, JOUTAI_FORESIGHT, 0);
    }
    fightSeq_NextWazaSeq(1);
}


// Perish Song starts the count on every Pokemon that can hear it.
u8 _fightSeqWsHorobinoutaSub(void *pokemon, u16 index, void *arg)
{
    FightOutPokemon *target = (FightOutPokemon *)pokemon;
    s32 *count = (s32 *)arg;
    u16 ability;

    if (fightOutPokemonCheckFightOut(target) == 0) {
        return 1;
    }
    ability = fightOutPokemonGetTokuseiDataId(target);
    if (fightOutPokemonCheckWriteJoutaiDataId(target, JOUTAI_ID_PERISH_SONG) != JOUTAI_WRITE_ACCEPT || ability == TOKUSEI_DATA_ID_SOUNDPROOF) {
        return 1;
    }
    fightOutPokemonWriteJoutaiDataId(target, JOUTAI_ID_PERISH_SONG, 0);
    (*count)++;
    return 1;
}


// Perish Song starts the count on every Pokemon in battle that can hear it.
extern "C" void WS_HOROBINOUTA(void)
{
    s32 count = 0;
    u8 *sequence;
    FightOutPokemon *attacker;
    u16 move;
    FightOutPokemon *defender;

    sequence = fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    move = fightOutPokemon_GetUseWazaDataId(attacker);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    if (fightSeqRegulationCheck(move, attacker, defender) == 1) {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
        return;
    }
    fightFloorLoopValidFightOutPokemon(0, _fightSeqWsHorobinoutaSub, &count, 0);
    fightSeqPressurePpdecCheckHorobinouta(attacker);
    if (count == 0) {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
    } else {
        fightSeq_NextWazaSeq(5);
    }
}


// Rollout keeps the user rolling and doubles its power every turn.
extern "C" void WS_KOROGARU(void)
{
    s32 maxCount;
    FightOutPokemon *attacker;
    FIGHT_WAZA *waza;
    FIGHT_WAZA *keepWaza;
    u16 move;
    s16 count;
    u16 power;
    u16 i;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    keepWaza = fightOutPokemon_GetKeepFightWazaPtr(attacker);
    move = fightOutPokemon_GetUseWazaDataId(attacker);
    if (fightWazaIsHit(waza) == 0) {
        fightOutPokemonInitJoutaiKeep(attacker);
        fightSeq_JumpWazaSeq(fightSeqContinueMoveMissWazaSeq);
        return;
    }
    if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_ID_ROLLOUT) == JOUTAI_WRITE_ACCEPT) {
        fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_ID_ROLLOUT, 0);
        if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_CONTINUING_MOVE) == 0) {
            fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_CONTINUING_MOVE, 0);
            fightWazaBiosCopy(keepWaza, waza);
        }
    }
    count = 1;
    if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_ROLLOUT) == 1) {
        maxCount = joutaiDataBiosGetMaxCount(JOUTAI_ID_ROLLOUT);
        count = fightOutPokemonGetCountAsJoutaiDataId(attacker, JOUTAI_ID_ROLLOUT);
        if (count == maxCount) {
            fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_ID_ROLLOUT);
            fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_CONTINUING_MOVE);
        }
    }
    power = wazaDB_GetIryoku(move);
    for (i = 1; i < count; i++) {
        power *= 2;
    }
    fightWaza_SetIryoku(waza, power);
    if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_DEFENSE_CURL) == 1) {
        power = fightWaza_GetIryoku(waza);
        fightWaza_SetIryoku(waza, power * 2);
    }
    fightSeq_NextWazaSeq(1);
}


// Swagger fails when the target is already confused and its stat is at the limit.
extern "C" void WS_IBARU(void)
{
    u8 *sequence;
    u8 statusKind;
    FightOutPokemon *defender;
    u16 statusId;
    s8 stage;

    sequence = fightSeq_GetWazaSeq();
    statusKind = sequence[1];
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    statusId = fightSeqCondChgActTypeToPokemonStatusId(statusKind);
    stage = pokemonGetStatus(defender, 0, statusId, 0);
    if (fightOutPokemonIsJoutaiDataId(defender, JOUTAI_CONFUSION) == 1 && stage >= FIGHT_STAT_STAGE_LIMIT) {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 2));
    } else {
        fightSeq_NextWazaSeq(6);
    }
}


// Fury Cutter doubles its power for every hit in a row.
extern "C" void WS_RENZOKUGIRI(void)
{
    FightOutPokemon *attacker;
    FIGHT_WAZA *waza;
    u16 move;
    s16 count;
    u32 power;
    int i;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    move = fightOutPokemon_GetUseWazaDataId(attacker);
    if (fightWazaIsHit(waza) == 0) {
        if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_FURY_CUTTER) == 1) {
            fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_ID_FURY_CUTTER);
        }
        fightSeq_JumpWazaSeq(fightSeqContinueMoveMissWazaSeq);
        return;
    }
    if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_ID_FURY_CUTTER) == JOUTAI_WRITE_ACCEPT) {
        fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_ID_FURY_CUTTER, 0);
    }
    count = fightOutPokemonGetCountAsJoutaiDataId(attacker, JOUTAI_ID_FURY_CUTTER);
    power = (u16)wazaDB_GetIryoku(move);
    for (i = 1; i < count; i++) {
        power *= 2;
    }
    fightWaza_SetIryoku(waza, power);
    fightSeq_NextWazaSeq(1);
}


// Power of Return: it grows with the user's friendship.
extern "C" u16 fightSeqGetOngaesiIryoku(u16 friendship)
{
    return friendship * 10 / 25;
}


// Power of Frustration: it grows as the user's friendship drops.
extern "C" u16 fightSeqGetYatuatariIryoku(u16 friendship)
{
    return (255 - friendship) * 10 / 25;
}


// Return and Frustration work out their power from the attacker's friendship.
extern "C" void WS_NATSUKI(void)
{
    FightOutPokemon *attacker;
    FIGHT_WAZA *waza;
    Pokemon *pokemon;
    u16 sequenceId;
    u16 friendship;
    u16 iryoku;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    pokemon = fightOutPokemonGetPokemonPtr(attacker);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    sequenceId = wazaDB_GetSeqId(fightOutPokemon_GetUseWazaDataId(attacker));
    friendship = pokemon_GetFriend(pokemon);
    if (sequenceId == WAZA_SEQUENCE_ID_RETURN) {
        iryoku = fightSeqGetOngaesiIryoku(friendship);
    } else {
        iryoku = fightSeqGetYatuatariIryoku(friendship);
    }
    fightWaza_SetIryoku(waza, iryoku);
    fightSeq_NextWazaSeq(1);
}


// Present: the damage of the gift, or the healing it hands out instead.
extern "C" void WS_PRESENT(void)
{
    FightOutPokemon *defender;
    FIGHT_WAZA *waza;
    u8 roll;

    fightSeq_GetWazaSeq();
    waza = fightOutPokemon_GetFightWazaPtr((FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0));
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    roll = HSD_Rand();
    if (roll < 102) {
        fightWaza_SetIryoku(waza, 40);
    } else if (roll < 178) {
        fightWaza_SetIryoku(waza, 80);
    } else if (roll < 204) {
        fightWaza_SetIryoku(waza, 120);
    } else {
        fightWaza_SetDamage(waza, -fightOutPokemonMaxHpWaruValue(defender, 4));
    }
    if (roll < 204) {
        fightSeq_JumpWazaSeq(fightSeqPresentDamageWazaSeq);
    } else if (fightOutPokemonIsHpMantan(defender) == 1) {
        fightSeq_JumpWazaSeq(fightSeqPresentFullHpWazaSeq);
    } else {
        fightWazaInitJoutaiDataId(waza, JOUTAI_MOVE_RESULT_NO_EFFECT);
        fightSeq_JumpWazaSeq(fightSeqPresentHealWazaSeq);
    }
}


// Safeguard: keeps the status of the side safe for five turns.
extern "C" void WS_SHINPINOMAMORI(void)
{
    FightOutPokemon *attacker;
    FIGHT_SIDE *side;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, attacker);
    if (fightSideCheckWriteJoutaiDataId(side, JOUTAI_SAFEGUARD) != JOUTAI_WRITE_ACCEPT) {
        fightFloor_SetAttackPokemonFightWazaJoutaiDataId(0, JOUTAI_MOVE_RESULT_MISSED);
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
    } else {
        fightSideWriteJoutaiDataId(side, JOUTAI_SAFEGUARD, 0);
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 5;
    }
    fightSeq_NextWazaSeq(1);
}


// Magnitude: the strength of the move, and the number the message announces.
extern "C" void WS_MAGNITUDE(void)
{
    u8 magnitude;
    FIGHT_WAZA *waza;
    u16 iryoku;
    u16 random;
    u8 roll;

    fightSeq_GetWazaSeq();
    waza = fightOutPokemon_GetFightWazaPtr((FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0));
    random = HSD_Rand();
    roll = random % 100;
    if (roll < 5) {
        iryoku = 10;
        magnitude = 4;
    } else if (roll < 15) {
        iryoku = 30;
        magnitude = 5;
    } else if (roll < 35) {
        iryoku = 50;
        magnitude = 6;
    } else if (roll < 65) {
        iryoku = 70;
        magnitude = 7;
    } else if (roll < 85) {
        iryoku = 90;
        magnitude = 8;
    } else if (roll < 95) {
        iryoku = 110;
        magnitude = 9;
    } else {
        iryoku = 150;
        magnitude = 10;
    }
    fightWaza_SetIryoku(waza, iryoku);
    fightMsgctrlSetValue(FIGHT_MSG_SLOT_MAGNITUDE, magnitude);
    fightSeq_NextWazaSeq(1);
}


// Pursuit: the check whether another foe that picked Pursuit this turn still
// lies in wait for the Pokemon the user switches out.
extern "C" void WS_OIUCHI_CHECK(void)
{
    u16 targetId;
    u8 *sequence;
    FightOutPokemon *target;
    FightOutPokemon *candidate;
    FightOutPokemon *defender;
    FIGHT_ACTION *action;
    FIGHT_WAZA *waza;

    sequence = fightSeq_GetWazaSeq();
    target = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_IREKAE_POKEMON, 0);
    targetId = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(target, fightFloor_GetFightType(0));
    if (fightOutPokemonCheckFightOut(target) == 0) {
        fightSeqJibakuIndex = fightSeqJibakuCount;
    } else {
        candidate = 0;
        defender = 0;
        while (fightSeqJibakuIndex < fightSeqJibakuCount) {
            candidate = fightFloor_GetFightOutPokemonFromDefaultSortData(fightSeqJibakuIndex);
            if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0 && target != candidate &&
                fightOutPokemonIsAlly(target, candidate) != 1) {
                action = fightOutPokemon_GetFightActionBuffPtr(candidate);
                if (action != 0 && fightActionCheckValid(action) != 0 &&
                    fightActionGetKindDataId(action) == FIGHT_ACTION_KIND_FIGHT_OUT_POKEMON_WAZA) {
                    waza = fightOutPokemon_GetFightWazaPtr(candidate);
                    if (fightOutPokemon_GetUseWazaDataId(candidate) == WAZA_DATA_ID_PURSUIT &&
                        targetId == fightWaza_GetTargetDataId(waza) &&
                        fightOutPokemonIsJoutaiDataId(candidate, JOUTAI_SLEEP) != 1 &&
                        fightOutPokemonIsJoutaiDataId(candidate, JOUTAI_FREEZE) != 1 &&
                        fightOutPokemon_GetNamakeFlag(candidate) != 1) {
                        defender = candidate;
                        break;
                    }
                }
            }
            fightSeqJibakuIndex++;
        }
        if (defender != 0) {
            fightFloor_SetDefensePokemonPtr(0, defender);
            UserWork[FIGHT_SEQ_WORK_WAZA_EFFECT_VARIATION] = 1;
            ServerStatusFlag &= ~SEQ_ATTACK_MESSAGE_SHOWN;
            fightOutPokemonBiosSetFightActionFlag(candidate, 1);
            fightSeq_NextWazaSeq(5);
            return;
        }
    }
    fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
}


// Belly Drum: the user trades half of its HP for a full attack boost.
extern "C" void WS_HARADAIKO(void)
{
    u16 halfHp;
    u16 hp;
    u8 *sequence;
    FightOutPokemon *attacker;
    FIGHT_WAZA *waza;
    u16 actType;
    s8 count;

    sequence = fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    hp = pokemon_GetHp(fightOutPokemonGetPokemonPtr(attacker));
    halfHp = fightOutPokemonMaxHpWaruValue(attacker, 2);
    actType = fightSeqCondChgActTypeToPokemonStatusId(FIGHT_STAT_ATTACK);
    count = pokemonGetStatus(attacker, 0, actType, 0);
    if (count < FIGHT_STAT_STAGE_LIMIT && hp > halfHp) {
        pokemonSetStatus(attacker, 0, actType, 0, FIGHT_STAT_STAGE_LIMIT);
        fightWaza_SetDamage(waza, halfHp);
        fightSeq_NextWazaSeq(5);
    } else {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
    }
}


// Psych Up: the user copies the stat changes of its target.
extern "C" void WS_JIKOANJI(void)
{
    FightOutPokemon *attacker;
    FightOutPokemon *defender;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    fightOutPokemonCopyAllAbiCnt(attacker, defender);
    fightSeq_NextWazaSeq(5);
}


// Rapid Spin: the user blows its own trapping move, Leech Seed and Spikes away.
extern "C" void WS_KOUSOKUSPIN(void)
{
    u16 fightType = fightFloor_GetFightType(0);
    FightOutPokemon *attacker;
    FIGHT_SIDE *side;
    FightOutPokemon *defender;
    u16 trapperId;
    FightOutPokemon *trapper;
    u16 wazaDataId;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, attacker);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_PARTIAL_TRAPPING) == 1) {
        trapperId = fightOutPokemonGetUserFightTargetIdJoutaiDataId(attacker,
                                                                                      JOUTAI_PARTIAL_TRAPPING);
        if (trapperId != 0) {
            trapper = (FightOutPokemon *)fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(trapperId,
                                                                                               fightType);
            if (trapper != 0) {
                fightFloor_SetDefensePokemonPtr(0, trapper);
                wazaDataId = fightOutPokemonGetIdBuffJoutaiDataId(attacker, JOUTAI_PARTIAL_TRAPPING);
                fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME,
                                     (unsigned long)GSmsgGetStringPtr(wazaDB_GetName(wazaDataId)));
                fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_PARTIAL_TRAPPING);
                fightSeq_CallWazaSeq(fightSeqRapidSpinTrapMsgSeq);
                fightFloor_SetDefensePokemonPtr(0, defender);
            }
        }
    }
    if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_LEECH_SEED) == 1) {
        fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_LEECH_SEED);
        fightSeq_CallWazaSeq(fightSeqRapidSpinLeechSeedMsgSeq);
    }
    if (fightSideIsJoutaiDataId(side, JOUTAI_SPIKES) == 1) {
        fightSideInitJoutaiDataId(side, JOUTAI_SPIKES);
        fightSeq_CallWazaSeq(fightSeqRapidSpinSpikesMsgSeq);
    }
    fightSeq_NextWazaSeq(1);
}


// Defense Curl: the user curls up, which strengthens its Rollout and Ice Ball.
extern "C" void WS_MARUKUNARU(void)
{
    FightOutPokemon *attacker;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_DEFENSE_CURL) == JOUTAI_WRITE_ACCEPT) {
        fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_DEFENSE_CURL, 0);
    }
    fightSeq_NextWazaSeq(1);
}


// Weather healing moves restore a share of the maximum HP that depends on the
// weather the floor is in.
extern "C" void WS_WEATHERKAIFUKU(void)
{
    u8 *sequence;
    FightOutPokemon *attacker;
    FIGHT_WAZA *waza;
    u16 tenkou;
    s32 heal;

    sequence = fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, attacker);
    tenkou = fightFloorGetNowTenkouDataId(0, 1);
    if (fightOutPokemonIsHpMantan(attacker) == 0) {
        if (tenkou == FIGHT_WEATHER_NONE) {
            heal = fightOutPokemonMaxHpWaruValue(attacker, 2);
        } else if (tenkou == FIGHT_WEATHER_SUNLIGHT) {
            heal = fightOutPokemonMaxHpWaruValue(attacker, 1) * 20 / 30;
        } else {
            heal = fightOutPokemonMaxHpWaruValue(attacker, 4);
        }
        fightWaza_SetDamage(waza, -heal);
        fightSeq_NextWazaSeq(5);
    } else {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
    }
}


// Mezameru Power draws its type and power from the hidden power of the user.
extern "C" void WS_MEZAMERUPOWER(void)
{
    FightOutPokemon *attacker;
    FIGHT_WAZA *waza;
    Pokemon *pokemon;
    u16 iryoku;
    u16 zokusei;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    pokemon = fightOutPokemonGetPokemonPtr(attacker);
    pokemonGetMezamerupower(pokemon, &iryoku, &zokusei);
    fightWaza_SetIryoku(waza, iryoku);
    fightWaza_SetZokusei(waza, zokusei);
    fightSeq_NextWazaSeq(1);
}


// Earthquake picks the other pokemon of the floor the move hits.
extern "C" void WS_JISIN(void)
{
    FightOutPokemon *attacker;
    FightOutPokemon *candidate;
    FightOutPokemon *defender;

    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    fightSeq_GetWazaSeq();
    fightSeqJibakuCount = FIGHT_POKEMON_SLOT_COUNT;
    fightSeqJibakuIndex = 0;
    defender = 0;
    while (fightSeqJibakuIndex < fightSeqJibakuCount) {
        candidate = fightFloor_GetFightOutPokemonFromDefaultSortData(fightSeqJibakuIndex);
        if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0 && attacker != candidate) {
            defender = candidate;
            break;
        }
        fightSeqJibakuIndex++;
    }
    if (defender != 0) {
        fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, defender);
    }
    fightSeq_NextWazaSeq(1);
}


// Future Sight stores the damage it will deal when it lands in the status of
// the target.
extern "C" void WS_MIRAIYOCHI(void)
{
    u16 fightType;
    u8 *sequence;
    FightOutPokemon *attacker;
    FightOutPokemon *defender;
    FIGHT_WAZA *waza;
    u16 targetId;
    u16 wazaDataId;
    u16 iryoku;
    u16 zokusei;
    s32 damage;
    FIGHT_SIDE *defenderSide;

    fightType = fightFloor_GetFightType(0);
    sequence = fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    targetId = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, fightType);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    wazaDataId = fightOutPokemon_GetUseWazaDataId(attacker);
    iryoku = fightWaza_GetIryoku(waza);
    zokusei = fightWaza_GetZokusei(waza);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    defenderSide = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, defender);
    if (fightOutPokemonCheckWriteJoutaiDataId(defender, JOUTAI_FUTURE_SIGHT) != JOUTAI_WRITE_ACCEPT) {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
        return;
    }
    fightOutPokemonWriteJoutaiDataId(defender, JOUTAI_FUTURE_SIGHT, targetId);
    fightOutPokemonSetIdBuffJoutaiDataId(defender, JOUTAI_FUTURE_SIGHT, wazaDataId);
    damage = fightSeqWazaDamageCalc(attacker, defender, defenderSide, wazaDataId, iryoku, zokusei);
    if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_HELPING_HAND) == 1) {
        damage = damage * 15 / 10;
    }
    fightOutPokemonSetBuffJoutaiDataId(defender, JOUTAI_FUTURE_SIGHT, damage);
    if (wazaDataId == WAZA_DATA_ID_DOOM_DESIRE) {
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 1;
    } else {
        fightSeqWazaWork[SEQ_MULTIPLE_STATUS] = 0;
    }
    fightSeq_NextWazaSeq(5);
}


// Power of one Beat Up hit: the base stats of the party member stand in for
// the attacking statistics of the user.
extern "C" s32 fightSeqGetHukuroDatakiDamage(u16 wazaDataId, u16 attackerDataId, u16 level,
                                             u16 defenderDataId, u16 defenderLevel)
{
    s32 damage;
    s32 defense;

    damage = pokemonDB_GetBasisPhyAtk(attackerDataId);
    damage *= (u16)wazaDB_GetIryoku(wazaDataId);
    damage *= level * 2 / 5 + 2;
    defense = pokemonDB_GetBasisPhyDef(defenderDataId);
    if (defense <= 0) {
        defense = 1;
    }
    damage = damage / defense;
    return damage / 50 + 2;
}


// Beat Up hits once for every pokemon of the own party that can still fight.
// The sequence runs once per hit, so the work slot keeps the party slot the
// next hit takes its attacker from.
extern "C" void WS_HUKURODATAKI(void)
{
    u8 *sequence;
    FightOutPokemon *attacker;
    FightOutPokemon *defender;
    FIGHT_TRAINER *trainer;
    Pokemon *pokemon;
    Hero *hero;
    u16 wazaDataId;
    u8 startSlot;
    FIGHT_WAZA *waza;
    FightPokemon *fightPokemon;
    u16 attackerDataId;
    u8 level;
    u16 defenderDataId;
    u8 defenderLevel;
    s32 damage;

    sequence = fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    wazaDataId = fightOutPokemon_GetUseWazaDataId(attacker);
    trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, attacker);
    hero = fightTrainer_GetHeroPtr(trainer);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    if (fightOutPokemonCheckFightOut(defender) == 0) {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
        return;
    }
    startSlot = fightSeqWazaWork[SEQ_HUKURO_DATAKI_SLOT];
    pokemon = 0;
    while (fightSeqWazaWork[SEQ_HUKURO_DATAKI_SLOT] < TEMOTI_POKEMON_MAX) {
        pokemon = (Pokemon *)heroGetStatus(hero, HERO_STATUS_POKEMON, fightSeqWazaWork[SEQ_HUKURO_DATAKI_SLOT]);
        fightPokemon = fightTrainerCheckTemotiPokemonFightEntry(trainer, pokemon);
        if (fightPokemon != 0 && fightPokemonCheckValid(fightPokemon) != 0) {
            pokemon = fightPokemonGetPokemonPtr(fightPokemon);
            if (pokemonCheckFightOut(pokemon) == 1 && pokemonIsJoutaiNormal(pokemon) == 1) {
                break;
            }
        }
        fightSeqWazaWork[SEQ_HUKURO_DATAKI_SLOT]++;
    }
    if (fightSeqWazaWork[SEQ_HUKURO_DATAKI_SLOT] < TEMOTI_POKEMON_MAX) {
        attackerDataId = pokemon_GetPokemonDataId(pokemon);
        level = pokemon_GetLevel(pokemon);
        defenderDataId = figthOutPokemonGetPokemonDataId(defender);
        defenderLevel = figthOutPokemonGetLevel(defender);
        fightMsgctrlSetValue(FIGHT_MSG_STATUS_NAME, (unsigned long)pokemon_GetName(pokemon));
        damage = fightSeqGetHukuroDatakiDamage(wazaDataId, attackerDataId, level, defenderDataId, defenderLevel);
        if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_HELPING_HAND) == 1) {
            damage = damage * 15 / 10;
        }
        fightWaza_SetDamage(waza, damage);
        fightSeqWazaWork[SEQ_HUKURO_DATAKI_SLOT]++;
        fightSeq_NextWazaSeq(9);
    } else if (startSlot != 0) {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
    } else {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 5));
    }
}


// The vanish moves hide the user; the hidden status is dropped again with a
// status write.
extern "C" void WS_KIERUTAME(void)
{
    FightOutPokemon *attacker;
    u16 move;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    move = fightOutPokemon_GetUseWazaDataId(attacker);
    switch (move) {
    case WAZA_DATA_ID_FLY:
    case WAZA_DATA_ID_BOUNCE:
        if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_ID_FLYING) == JOUTAI_WRITE_ACCEPT) {
            fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_ID_FLYING, 0);
        }
        break;
    case WAZA_DATA_ID_DIG:
        if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_ID_DIGGING) == JOUTAI_WRITE_ACCEPT) {
            fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_ID_DIGGING, 0);
        }
        break;
    case WAZA_DATA_ID_DIVE:
        if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_ID_DIVING) == JOUTAI_WRITE_ACCEPT) {
            fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_ID_DIVING, 0);
        }
        break;
    }
    fightSeq_NextWazaSeq(1);
}


// After a vanish move resolves, the hidden status of the user is dropped
// through the status init path.
extern "C" void WS_KIERUTAME_AFTAR(void)
{
    FightOutPokemon *attacker;
    u16 move;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    move = fightOutPokemon_GetUseWazaDataId(attacker);
    switch (move) {
    case WAZA_DATA_ID_FLY:
    case WAZA_DATA_ID_BOUNCE:
        if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_FLYING) == 1) {
            fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_ID_FLYING);
        }
        break;
    case WAZA_DATA_ID_DIG:
        if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_DIGGING) == 1) {
            fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_ID_DIGGING);
        }
        break;
    case WAZA_DATA_ID_DIVE:
        if (fightOutPokemonIsJoutaiDataId(attacker, JOUTAI_ID_DIVING) == 1) {
            fightOutPokemonInitJoutaiDataId(attacker, JOUTAI_ID_DIVING);
        }
        break;
    }
    fightSeq_NextWazaSeq(1);
}


// Minimize clears the shrink status while a fight action is running.
extern "C" void WS_CHIISAKUNARU(void)
{
    FightOutPokemon *attacker;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    if ((ServerStatusFlag & FIGHT_SEQ_FLAG_ACTION_IN_PROGRESS) != 0) {
        if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_MINIMIZE) == JOUTAI_WRITE_ACCEPT) {
            fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_MINIMIZE, 0);
        }
    }
    fightSeq_NextWazaSeq(1);
}


// Memento faints the user: it takes its whole HP as damage and its HP bar is
// emptied, then the following steps drop the attacking stages of the defender.
// When neither stage can drop and the move recorded no miss reason the drop
// steps are skipped.
extern "C" void WS_OKIMIYAGE(void)
{
    u8 *sequence;
    FightOutPokemon *attacker;
    FIGHT_WAZA *waza;
    FightOutPokemon *defender;
    u8 statusAttack;
    u32 statusMenuId;
    u8 statusSpecialAttack;
    u16 fightType;

    fightType = fightFloor_GetFightType(0);
    sequence = fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    statusMenuId = fightMenuGetFightOutPokemonPtrToStatusMenuId(attacker, fightType, true);
    defender = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_DEFENDER, 0);
    statusAttack = pokemonGetStatus(defender, 0, fightSeqCondChgActTypeToPokemonStatusId(FIGHT_STAT_ATTACK), 0);
    statusSpecialAttack = pokemonGetStatus(defender, 0,
        fightSeqCondChgActTypeToPokemonStatusId(FIGHT_STAT_SPECIAL_ATTACK), 0);
    if (statusAttack == 0 && statusSpecialAttack == 0 && fightSeqWazaWork[SEQ_MOVE_MISS_REASON] != 1) {
        fightSeq_JumpWazaSeq(*(u8 **)(sequence + 1));
        return;
    }
    fightWaza_SetDamage(waza, (u16)pokemon_GetHp(fightOutPokemonGetPokemonPtr(attacker)));
    menuFightStatusSetHP(statusMenuId, 0);
    fightSeq_NextWazaSeq(5);
}


// Follow Me draws the attacks of the turn to the user, so the own side keeps
// the relative target id of the pokemon that does the drawing.
extern "C" void WS_KONOYUBITOMARE(void)
{
    FightOutPokemon *attacker;
    FIGHT_SIDE *side;
    u16 targetId;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, attacker);
    if (fightSideCheckWriteJoutaiDataId(side, JOUTAI_ID_FOLLOW_ME) == JOUTAI_WRITE_ACCEPT) {
        targetId = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, fightFloor_GetFightType(0));
        fightSideWriteJoutaiDataId(side, JOUTAI_ID_FOLLOW_ME, targetId);
    }
    fightSeq_NextWazaSeq(1);
}


// The charged move has been released, so the charge status of the user goes.
extern "C" void WS_JUUDEN(void)
{
    FightOutPokemon *attacker;

    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    if (fightOutPokemonCheckWriteJoutaiDataId(attacker, JOUTAI_ID_CHARGE) == JOUTAI_WRITE_ACCEPT) {
        fightOutPokemonWriteJoutaiDataId(attacker, JOUTAI_ID_CHARGE, 0);
    }
    fightSeq_NextWazaSeq(1);
}


// Nature Power plays the move the terrain of the battle floor provides, then
// continues with the sequence of that move.
extern "C" void WS_SHIZENNOCHIKARA(void)
{
    u16 sequenceId;
    u16 fightType;
    FightOutPokemon *attacker;
    FIGHT_WAZA *waza;
    u16 move;
    FightOutPokemon *defender;

    fightType = fightFloor_GetFightType(0);
    fightSeq_GetWazaSeq();
    attacker = (FightOutPokemon *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_ATTACKER, 0);
    waza = fightOutPokemon_GetFightWazaPtr(attacker);
    move = tikeiDataBiosGetWazaId(fightFloor_GetTikeiDataId(0));
    sequenceId = wazaDB_GetSeqId(move);
    fightWazaSetUseWazaStatus(waza, move);
    fightMsgctrlSetValue(FIGHT_MSG_SLOT_MOVE_NAME, (unsigned long)GSmsgGetStringPtr(wazaDB_GetName(move)));
    defender = fightSeqSelectDefensePokemonPtr(attacker, move, fightType, NULL, 1, 1, -1);
    fightFloor_SetDefPokemonAndAtkFightWazaTargetId(0, defender);
    ServerStatusFlag &= ~SEQ_ATTACK_MESSAGE_SHOWN;
    fightSeq_CallWazaSeq(fightSeqAttackMessageWazaSeq);
    fightSeq_JumpWazaSeq(wazaSeqJumpTbl[sequenceId]);
}
