#include <types.h>
#include <fightFloor.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTarget.h>
#include <fightTrainerAi.h>
#include <pokemonDB.h>
#include <app/tikei/tikeiBios.h>
#include <waza.h>

enum AiStatChange {
    AI_STAT_RAISE_ONE = 0x10,
    AI_STAT_LOWER_ONE = 0x90
};

enum AiHitCheckMode {
    AI_HIT_CHECK_NORMAL = 0,
    AI_HIT_CHECK_MODE_LOCK_ON = 0xFFFF
};

enum AiHitResult {
    AI_HIT_RESULT_GUARANTEED = -1,
    AI_HIT_RESULT_NO_HIT = 0,
    AI_HIT_RESULT_CAN_HIT = 1
};

enum AiWazaBufferCapacity {
    AI_ASSIST_WAZA_CAPACITY = 24,
    AI_IMPRISON_WAZA_CAPACITY = 8
};

enum AiImprisonLookupFlags {
    AI_IMPRISON_CHECK_CAN_USE = false,
    AI_IMPRISON_SELECT_ALLIES = false,
    AI_IMPRISON_ONLY_DEPLOYED = true
};

enum { AI_NO_USED_ITEM_DATA_ID = 0 };

extern "C" {
u8 fightTrainerAiWazaHit000(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender);
u8 fightTrainerAiWazaHit002(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender);
u8 _fightTrainerAiWazaHitAbicnt(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, FightOutPokemon *defender, u16 wazaDataId, u8 paraId, u8 actType, u16 flags);
AiHitResult _fightTrainerAiWazaHitCheck(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender, u32 checkMode);
u8 _fightTrainerAiWazaHitCheckTokusei(FIGHT_TRAINER *trainer, FightOutPokemon *defender, u16 joutaiDataId);

u8 fightTrainerAiWazaHit180(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u16 wazaDataIdAry[AI_ASSIST_WAZA_CAPACITY];
    return fightSeqGetTemotiPokemonNekonoteWazaDataAry(trainer, wazaDataIdAry, AI_ASSIST_WAZA_CAPACITY, attacker) > 0;
}

u8 fightTrainerAiWazaHit181(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, attacker, JOUTAI_ID_INGRAIN) == 1) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit182(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit183(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return 1;
}

u8 fightTrainerAiWazaHit184(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    if (fightTrainerAiGetFightOutPokemonUsedItemDataId(trainer, attacker) == AI_NO_USED_ITEM_DATA_ID) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit185(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit186(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit187(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    AiHitResult hit;
    if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, defender, TOKUSEI_DATA_ID_VITAL_SPIRIT) == 1) {
        return 0;
    }
    if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, defender, TOKUSEI_DATA_ID_INSOMNIA) == 1) {
        return 0;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, defender, JOUTAI_SUBSTITUTE) == 1) {
        return 0;
    }
    if (fightSideIsJoutaiDataId((FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, defender), JOUTAI_SAFEGUARD) == 1) {
        return 0;
    }
    hit = _fightTrainerAiWazaHitCheck(trainer, attacker, wazaDataId, defender, AI_HIT_CHECK_NORMAL);
    if (fightTrainerAiCheckSawagu(trainer, defender) == 1) {
        return 0;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, defender, JOUTAI_ID_DROWSY) == 1) {
        return 0;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiNormal(trainer, defender) == 0) {
        return 0;
    }
    if (fightSeqRegulationOverTwoCheck(JOUTAI_SLEEP, defender) == 1) {
        return 0;
    }
    if (hit == AI_HIT_RESULT_NO_HIT) {
        return 0;
    } else if (hit == AI_HIT_RESULT_GUARANTEED) {
        return 1;
    }
    return 1;
}

u8 fightTrainerAiWazaHit188(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit189(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u16 attackerHp;
    AiHitResult hit;
    u16 typeId = fightTrainerAiGetWazaZokuseiDataId(trainer, wazaDataId, attacker);
    attackerHp = fightTrainerAiGetFightOutPokemonNowHp(trainer, attacker);
    if (fightTrainerAiGetFightOutPokemonNowHp(trainer, defender) <= attackerHp) {
        return 0;
    }
    hit = _fightTrainerAiWazaHitCheck(trainer, attacker, wazaDataId, defender, AI_HIT_CHECK_NORMAL);
    if (fightTrainerAiGetFightOutPokemonZokuseiDataIdWazaJoutaiDataId(trainer, defender, typeId,
            fightTrainerAiGetWazaIryoku(trainer, wazaDataId)) == JOUTAI_MOVE_RESULT_NO_EFFECT) {
        hit = AI_HIT_RESULT_NO_HIT;
    }
    if (hit == AI_HIT_RESULT_NO_HIT) {
        return 0;
    } else if (hit == AI_HIT_RESULT_GUARANTEED) {
        return 1;
    }
    return 1;
}

u8 fightTrainerAiWazaHit190(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit191(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    AiHitResult hit;
    hit = _fightTrainerAiWazaHitCheck(trainer, attacker, wazaDataId, defender, AI_HIT_CHECK_MODE_LOCK_ON);
    if (fightSeqRegulationCheck(wazaDataId, attacker, defender) == 1) {
        return 0;
    }
    if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, attacker, TOKUSEI_DATA_ID_WONDER_GUARD) == 1) {
        return 0;
    }
    if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, defender, TOKUSEI_DATA_ID_WONDER_GUARD) == 1) {
        return 0;
    }
    if (hit == AI_HIT_RESULT_NO_HIT) {
        return 0;
    } else if (hit == AI_HIT_RESULT_GUARANTEED) {
        return 1;
    }
    return 1;
}

u8 fightTrainerAiWazaHit192(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u16 wazaDataIdAry[AI_IMPRISON_WAZA_CAPACITY];
    FightOutPokemon *enemyPokemonPtrAry[FIGHT_OUT_POKEMON_COUNT];
    // Gather all of the attacker's moves, including unusable ones.
    fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, attacker, wazaDataIdAry, NULL, AI_IMPRISON_CHECK_CAN_USE);
    // Gather deployed opponents rather than allies.
    fightFloorGetFightTrainerFightOutPokemonPtrAry(NULL, trainer, enemyPokemonPtrAry, AI_IMPRISON_SELECT_ALLIES, AI_IMPRISON_ONLY_DEPLOYED);
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, attacker, JOUTAI_ID_IMPRISON) == 1) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit193(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u8 result;
    if (fightTrainerAiIsFightOutPokemonJoutaiKaragenki(trainer, attacker) == 1) {
        result = 1;
    } else {
        return 0;
    }
    return result;
}

u8 fightTrainerAiWazaHit194(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, defender, JOUTAI_ID_GRUDGE) == 1) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit195(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return 1;
}

u8 fightTrainerAiWazaHit196(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit197(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit198(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit199(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    FIGHT_SIDE *side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, defender);
    AiHitResult hit;
    if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, defender, TOKUSEI_DATA_ID_OWN_TEMPO) == 1) {
        return 0;
    }
    if (_fightTrainerAiWazaHitCheckTokusei(trainer, defender, JOUTAI_CONFUSION) == 0) {
        return 0;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, defender, JOUTAI_SUBSTITUTE) == 1) {
        return 0;
    }
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, defender, JOUTAI_CONFUSION) == 1) {
        return 0;
    }
    hit = _fightTrainerAiWazaHitCheck(trainer, attacker, wazaDataId, defender, AI_HIT_CHECK_NORMAL);
    if (fightSideIsJoutaiDataId(side, JOUTAI_SAFEGUARD) == 1) {
        return 0;
    }
    if (hit == AI_HIT_RESULT_NO_HIT) {
        return 0;
    } else if (hit == AI_HIT_RESULT_GUARANTEED) {
        return 1;
    }
    return 1;
}

u8 fightTrainerAiWazaHit200(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit004(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit201(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, attacker, JOUTAI_ID_MUD_SPORT) == 1) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit202(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit203(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    s32 hit;
    u16 typeId;
    u8 weather = fightTrainerAiGetFightFloorNowTenkouDataId(trainer, 0, 1);
    if (weather == FIGHT_WEATHER_RAIN) {
        typeId = ZOKUSEI_DATA_ID_WATER;
    } else if (weather == FIGHT_WEATHER_SANDSTORM) {
        typeId = ZOKUSEI_DATA_ID_ROCK;
    } else if (weather == FIGHT_WEATHER_SUNLIGHT) {
        typeId = ZOKUSEI_DATA_ID_FIRE;
    } else if (weather == FIGHT_WEATHER_HAIL) {
        typeId = ZOKUSEI_DATA_ID_ICE;
    } else {
        typeId = ZOKUSEI_DATA_ID_NORMAL;
    }

    hit = _fightTrainerAiWazaHitCheck(trainer, attacker, wazaDataId, defender, 0);
    if (fightTrainerAiGetFightOutPokemonZokuseiDataIdWazaJoutaiDataId(trainer, defender, typeId,
            fightTrainerAiGetWazaIryoku(trainer, wazaDataId)) == JOUTAI_MOVE_RESULT_NO_EFFECT) {
        hit = 0;
    }
    if (hit == 0) {
        return 0;
    } else if (hit == -1) {
        return 1;
    }
    return 1;
}

u8 fightTrainerAiWazaHit204(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit205(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u8 attackStage = fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(trainer, defender);
    u8 defenseStage = fightTrainerAiGetFightOutPokemonAbiCntPhyDef(trainer, defender);
    if (attackStage == 0 && defenseStage == 0) {
        return 0;
    }
    if (!fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                 AI_STAT_LOWER_ONE, FIGHT_STAT_ATTACK, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT)
        && !fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                    AI_STAT_LOWER_ONE, FIGHT_STAT_DEFENSE, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT)) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit206(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u8 defenseStage = fightTrainerAiGetFightOutPokemonAbiCntPhyDef(trainer, attacker);
    u8 specialDefenseStage = fightTrainerAiGetFightOutPokemonAbiCntSpeDef(trainer, attacker);
    if (defenseStage >= FIGHT_STAT_STAGE_LIMIT && specialDefenseStage >= FIGHT_STAT_STAGE_LIMIT) {
        return 0;
    }
    if (!fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                 AI_STAT_RAISE_ONE, FIGHT_STAT_DEFENSE, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT | FIGHT_TRAINER_AI_CHG_ON_ATTACKER)
        && !fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                    AI_STAT_RAISE_ONE, FIGHT_STAT_SPECIAL_DEFENSE, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT | FIGHT_TRAINER_AI_CHG_ON_ATTACKER)) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit207(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit208(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u8 attackStage = fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(trainer, attacker);
    u8 defenseStage = fightTrainerAiGetFightOutPokemonAbiCntPhyDef(trainer, attacker);
    if (attackStage >= FIGHT_STAT_STAGE_LIMIT && defenseStage >= FIGHT_STAT_STAGE_LIMIT) {
        return 0;
    }
    if (!fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                 AI_STAT_RAISE_ONE, FIGHT_STAT_ATTACK, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT | FIGHT_TRAINER_AI_CHG_ON_ATTACKER)
        && !fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                    AI_STAT_RAISE_ONE, FIGHT_STAT_DEFENSE, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT | FIGHT_TRAINER_AI_CHG_ON_ATTACKER)) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit209(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit002(trainer, attacker, wazaDataId, defender);
}

u8 fightTrainerAiWazaHit210(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, attacker, JOUTAI_WATER_SPORT) == 1) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit211(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u8 specialAttackStage = fightTrainerAiGetFightOutPokemonAbiCntSpeAtk(trainer, attacker);
    u8 specialDefenseStage = fightTrainerAiGetFightOutPokemonAbiCntSpeDef(trainer, attacker);
    if (specialAttackStage >= FIGHT_STAT_STAGE_LIMIT && specialDefenseStage >= FIGHT_STAT_STAGE_LIMIT) {
        return 0;
    }
    if (!fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                 AI_STAT_RAISE_ONE, FIGHT_STAT_SPECIAL_ATTACK, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT | FIGHT_TRAINER_AI_CHG_ON_ATTACKER)
        && !fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                    AI_STAT_RAISE_ONE, FIGHT_STAT_SPECIAL_DEFENSE, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT | FIGHT_TRAINER_AI_CHG_ON_ATTACKER)) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit212(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u8 attackStage = fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(trainer, attacker);
    u8 speedStage = fightTrainerAiGetFightOutPokemonAbiCntSubayasa(trainer, attacker);
    if (attackStage >= FIGHT_STAT_STAGE_LIMIT && speedStage >= FIGHT_STAT_STAGE_LIMIT) {
        return 0;
    }
    if (!fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                 AI_STAT_RAISE_ONE, FIGHT_STAT_ATTACK, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT | FIGHT_TRAINER_AI_CHG_ON_ATTACKER)
        && !fightTrainerAiCheckAbiCnt(trainer, attacker, defender, wazaDataId,
                                    AI_STAT_RAISE_ONE, FIGHT_STAT_SPEED, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT | FIGHT_TRAINER_AI_CHG_ON_ATTACKER)) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit213(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    u8 typeId = tikeiDataBiosGetZokuseiDataId(fightFloor_GetTikeiDataId(0));
    if (fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, attacker, typeId) == 1) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit214(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return _fightTrainerAiWazaHitAbicnt(trainer, attacker, defender, wazaDataId,
                                        AI_STAT_LOWER_ONE, FIGHT_STAT_EVASION, FIGHT_TRAINER_AI_CHG_KIND_PRIMARY_EFFECT);
}

u8 fightTrainerAiWazaHit215(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    if (fightTrainerAiGetFightFloorNowTenkouDataId(trainer, 0, 1) == FIGHT_WEATHER_SHADOW_SKY) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit216(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    FIGHT_SIDE *side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, defender);
    if (!fightSideIsJoutaiDataId(side, JOUTAI_REFLECT)
        && !fightSideIsJoutaiDataId(side, JOUTAI_LIGHT_SCREEN)
        && !fightSideIsJoutaiDataId(side, JOUTAI_SAFEGUARD)) {
        return 0;
    }
    return 1;
}

u8 fightTrainerAiWazaHit217(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender)
{
    return fightTrainerAiWazaHit000(trainer, attacker, wazaDataId, defender);
}

}
