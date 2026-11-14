#include <types.h>
#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTarget.h>
#include <fightTrainer.h>
#include <fightTrainerAi.h>
#include <fightTrainerDB.h>
#include <hsdRandom.h>
#include <pokemonDB.h>
#include <waza.h>

// Flags that select how the trainer AI checks a stat-stage change.
enum {
    FIGHT_TRAINER_AI_CHG_KIND_MASK = 0x1F,
    FIGHT_TRAINER_AI_CHG_SKIP_GUARD = 0x20,
    FIGHT_TRAINER_AI_CHG_ON_ATTACKER = 0x40,
    FIGHT_TRAINER_AI_CHG_IGNORE_MIST_AND_STAT_DOWN_PREVENTION = 0x80
};

// Opponents that Perish Song can still be sung on, counted by the fight-out
// traversal below.  The trainer supplies the AI data the eligibility queries
// are answered with.
typedef struct {
    FIGHT_TRAINER *mTrainer;
    u32 mCount;
} HOROBINOUTA_CHECK_WORK;

// Trainer used for the battle-wide Damp check.
typedef struct {
    FIGHT_TRAINER *mTrainer;
} SIMERIKE_CHECK_WORK;

// Trainer and the pokemon whose sleep is being checked.
typedef struct {
    FIGHT_TRAINER *mTrainer;
    FightOutPokemon *mPokemon;
} SAWAGU_CHECK_WORK;

extern "C" u8 _fightTrainerAiSawaguCheckSub(void *target, u16 fightType, void *argument)
{
    FIGHT_TRAINER *trainer = ((SAWAGU_CHECK_WORK *)argument)->mTrainer;
    FightOutPokemon *sleepTarget = ((SAWAGU_CHECK_WORK *)argument)->mPokemon;
    FightOutPokemon *pokemon = (FightOutPokemon *)target;

    if (!fightOutPokemonCheckFightOut(pokemon)) {
        return 1;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_ID_UPROAR) == 1
        && fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, sleepTarget, TOKUSEI_DATA_ID_SOUNDPROOF) == 0) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiCheckSawagu(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon)
{
    SAWAGU_CHECK_WORK work;

    work.mTrainer = trainer;
    work.mPokemon = pokemon;
    if (fightFloorLoopValidFightOutPokemon(0, _fightTrainerAiSawaguCheckSub, &work, 0) == 1) {
        return 0;
    }
    return 1;
}

extern "C" u8 _fightTrainerAiSimerikeCheckSub(void *target, u16 fightType, void *argument)
{
    FIGHT_TRAINER *trainer = ((SIMERIKE_CHECK_WORK *)argument)->mTrainer;
    FightOutPokemon *pokemon = (FightOutPokemon *)target;

    if (!fightOutPokemonCheckFightOut(pokemon)) {
        return 1;
    }
    if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, pokemon, TOKUSEI_DATA_ID_DAMP) == 1) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiCheckSimerike(FIGHT_TRAINER *trainer)
{
    SIMERIKE_CHECK_WORK work;

    work.mTrainer = trainer;
    if (fightFloorLoopValidFightOutPokemon(0, _fightTrainerAiSimerikeCheckSub, &work, 0) == 1) {
        return 0;
    }
    return 1;
}

u16 fightTrainerAiCheckOumu(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon)
{
    u16 oumuWazaDataIdAry[4];
    u16 oumuWazaDataId;
    u16 wazaDataId;
    u8 count;

    oumuWazaDataId = fightOutPokemon_GetOumuWazaDataId(pokemon);
    if (oumuWazaDataId != WAZA_DATA_ID_NONE && oumuWazaDataId != WAZA_SHADOW_SPECIAL && oumuWazaDataId != WAZA_DATA_ID_INVALID) {
        return oumuWazaDataId;
    }
    count = fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry(pokemon, oumuWazaDataIdAry);
    if (count != 0) {
        wazaDataId = oumuWazaDataIdAry[(u8)((u16)HSD_Rand() % count)];
        if (wazaDataId != WAZA_DATA_ID_NONE && wazaDataId != WAZA_SHADOW_SPECIAL) {
            return wazaDataId;
        }
    }
    return 0;
}

u8 fightTrainerAiCheckGuard(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, u16 wazaDataId)
{
    if (fightTrainerAiDB_GetPokemonJoutaiFlag(fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer)))
            == 1
        && fightSeqIsGuard(pokemon, wazaDataId) == 1) {
        return 1;
    }
    return 0;
}

u8 fightTrainerAiCheckAbiCnt(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, FightOutPokemon *defender,
                             u16 wazaDataId, u8 paraId, u8 actType, u16 flags)
{
    FightOutPokemon *target;
    FIGHT_SIDE *side;
    u8 changeFlags;
    s32 statusId;
    u8 ignoreStatDownPrevention = 0;
    u8 skipGuard = 0;
    s8 stage;

    target = (flags & FIGHT_TRAINER_AI_CHG_ON_ATTACKER) ? attacker : defender;
    side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, target);
    changeFlags = flags;
    if ((u8)(changeFlags & (u8)~FIGHT_TRAINER_AI_CHG_ON_ATTACKER) & FIGHT_TRAINER_AI_CHG_IGNORE_MIST_AND_STAT_DOWN_PREVENTION) {
        ignoreStatDownPrevention = 1;
    }
    if (changeFlags & FIGHT_TRAINER_AI_CHG_SKIP_GUARD) {
        skipGuard = 1;
    }
    statusId = fightSeqCondChgActTypeToPokemonStatusId(actType);
    stage = pokemonGetStatus(target, 0, statusId, 0);
    if (fightSeqCondChgActParaIdToValue(paraId) < 0) {
        if (fightSideIsJoutaiDataId(side, JOUTAI_ID_MIST) == 1
            && !ignoreStatDownPrevention
            && wazaDataId != WAZA_DATA_ID_CURSE) {
            return 0;
        }
        if (wazaDataId != WAZA_DATA_ID_CURSE
            && skipGuard != 1
            && fightTrainerAiCheckGuard(trainer, defender, wazaDataId) == 1) {
            return 0;
        }
        if ((fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, target, TOKUSEI_DATA_ID_CLEAR_BODY) == 1
             || fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, target, TOKUSEI_DATA_ID_WHITE_SMOKE) == 1)
            && !ignoreStatDownPrevention
            && wazaDataId != WAZA_DATA_ID_CURSE) {
            return 0;
        }
        if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, target, TOKUSEI_DATA_ID_KEEN_EYE) == 1
            && !ignoreStatDownPrevention
            && (u16)statusId == POKEMON_STATUS_FIELD_ACCURACY_STAGE) {
            return 0;
        }
        if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, target, TOKUSEI_DATA_ID_HYPER_CUTTER) == 1
            && !ignoreStatDownPrevention
            && (u16)statusId == POKEMON_STATUS_FIELD_ATTACK_STAGE) {
            return 0;
        }
        if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, target, TOKUSEI_DATA_ID_SHIELD_DUST) == 1
            && (changeFlags & FIGHT_TRAINER_AI_CHG_KIND_MASK) == 0) {
            return 0;
        }
        if (stage <= 0) {
            return 0;
        }
    } else if (stage >= FIGHT_STAT_STAGE_LIMIT) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiCheckTextureZokusei(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon)
{
    s32 textureZokusei[4];

    if (fightTrainerAiDB_GetZokuseiCheckFlag(fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer)))
        == 1) {
        return fightSeqGetTextureZokuseiAry(pokemon, textureZokusei) >= 1;
    }
    return 1;
}

// Anyone still in the battle on the opposing side who is not counting down
// already and has no soundproof ability can be sung at.  Traversal always
// keeps walking the fight-out list.
extern "C" u8 _fightTrainerAiCheckHorobinoutaSub(void *target, u16 fightType, void *argument)
{
    HOROBINOUTA_CHECK_WORK *work = (HOROBINOUTA_CHECK_WORK *)argument;
    FightOutPokemon *pokemon = (FightOutPokemon *)target;
    FIGHT_TRAINER *trainer = work->mTrainer;

    if (!fightOutPokemonCheckFightOut(pokemon)) {
        return 1;
    }
    if (fightTrainerIsAllyFightTargetPtr(trainer, pokemon, fightType) == 1) {
        return 1;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, pokemon, JOUTAI_ID_PERISH_SONG) != 1
        && fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, pokemon, TOKUSEI_DATA_ID_SOUNDPROOF) != 1) {
        work->mCount++;
    }
    return 1;
}

u16 fightTrainerAiCheckHorobinouta(FIGHT_TRAINER *trainer)
{
    HOROBINOUTA_CHECK_WORK work;

    work.mTrainer = trainer;
    work.mCount = 0;
    fightFloorLoopValidFightOutPokemon(0, _fightTrainerAiCheckHorobinoutaSub, &work, 0);
    return work.mCount;
}

u8 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId(FIGHT_TRAINER *trainer, u16 joutaiDataId, u16 wazaDataId)
{
    u16 seqId = wazaDB_GetSeqId(wazaDataId);

    if (joutaiDataId == JOUTAI_ID_FLYING
        && (seqId == WAZA_SEQUENCE_ID_TWISTER
            || seqId == WAZA_SEQUENCE_ID_GUST
            || seqId == WAZA_SEQUENCE_ID_THUNDER
            || seqId == WAZA_SEQUENCE_ID_SKY_UPPERCUT)) {
        return 1;
    }
    if (joutaiDataId == JOUTAI_ID_DIGGING && seqId == WAZA_SEQUENCE_ID_EARTHQUAKE) {
        return 1;
    }
    if (joutaiDataId == JOUTAI_ID_DIVING
        && (wazaDataId == WAZA_DATA_ID_SURF || wazaDataId == WAZA_DATA_ID_WHIRLPOOL)) {
        return 1;
    }
    if (seqId == WAZA_SEQUENCE_ID_LOCK_ON) {
        return 1;
    }
    return 0;
}
