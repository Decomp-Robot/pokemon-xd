#include <types.h>
#include <fightFloor.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTarget.h>
#include <fightTrainerAi.h>
#include <app/tikei/tikeiBios.h>
#include <waza.h>

enum AiStatChange {
    AI_STAT_RAISE_ONE = 0x10,
    AI_STAT_LOWER_ONE = 0x90
};

extern "C" {
u8 fightTrainerAiWazaHit000(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender);
u8 fightTrainerAiWazaHit002(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender);
u8 _fightTrainerAiWazaHitAbicnt(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, FightOutPokemon *defender, u16 wazaDataId, u8 paraId, u8 actType, u16 flags);
s32 _fightTrainerAiWazaHitCheck(FIGHT_TRAINER *trainer, FightOutPokemon *attacker, u16 wazaDataId, FightOutPokemon *defender, u32 checkMode);

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
            fightTrainerAiGetWazaIryoku(trainer, wazaDataId)) == JOUTAI_TYPE_IMMUNITY) {
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
