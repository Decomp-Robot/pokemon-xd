#include <fightFloor.h>
#include <fightTrainerAi.h>
#include <waza.h>

// AI scoring adjustments stored in the trainer AI add/sub database.
enum AI_ADDSUB_ID {
    AI_ADDSUB_NONE = 0,
    AI_ADDSUB_ATTACK_UP_STAT_ATTACK_LOW = 556,
    AI_ADDSUB_ATTACK_UP_ALLY_FOLLOW_ME = 557,
    AI_ADDSUB_ATTACK_UP_STAT_ATTACK_HIGH = 558,
    AI_ADDSUB_ATTACK_UP_STAGE = 559,

    AI_ADDSUB_DEFENSE_UP_STAT_DEFENSE_LOW = 560,
    AI_ADDSUB_DEFENSE_UP_ALLY_FOLLOW_ME = 561,
    AI_ADDSUB_DEFENSE_UP_STAT_DEFENSE_HIGH = 562,
    AI_ADDSUB_DEFENSE_UP_STAGE = 563,

    AI_ADDSUB_SPEED_UP_STAT_SPEED_LOW = 564,
    AI_ADDSUB_SPEED_UP_ALLY_FOLLOW_ME = 565,
    AI_ADDSUB_SPEED_UP_STAT_SPEED_HIGH = 566,
    AI_ADDSUB_SPEED_UP_STAGE = 567,

    AI_ADDSUB_SP_ATK_UP_STAT_SP_ATK_LOW = 568,
    AI_ADDSUB_SP_ATK_UP_ALLY_FOLLOW_ME = 569,
    AI_ADDSUB_SP_ATK_UP_STAT_SP_ATK_HIGH = 570,
    AI_ADDSUB_SP_ATK_UP_STAGE = 571,

    AI_ADDSUB_SP_DEF_UP_STAT_SP_DEF_LOW = 572,
    AI_ADDSUB_SP_DEF_UP_ALLY_FOLLOW_ME = 573,
    AI_ADDSUB_SP_DEF_UP_STAT_SP_DEF_HIGH = 574,
    AI_ADDSUB_SP_DEF_UP_STAGE = 575,

    AI_ADDSUB_EVASION_UP_STAT_EVASION_LOW = 576,
    AI_ADDSUB_EVASION_UP_ALLY_FOLLOW_ME = 577,
    AI_ADDSUB_EVASION_UP_STAT_EVASION_HIGH = 578,
    AI_ADDSUB_EVASION_UP_STAGE = 579,

    AI_ADDSUB_COSMIC_POWER_STAT_LOW = 580,
    AI_ADDSUB_COSMIC_POWER_ALLY_FOLLOW_ME = 581,
    AI_ADDSUB_COSMIC_POWER_STAT_HIGH = 582,
    AI_ADDSUB_COSMIC_POWER_STAGE = 583,

    AI_ADDSUB_BULK_UP_STAT_LOW = 584,
    AI_ADDSUB_BULK_UP_ALLY_FOLLOW_ME = 585,
    AI_ADDSUB_BULK_UP_STAT_HIGH = 586,
    AI_ADDSUB_BULK_UP_STAGE = 587,

    AI_ADDSUB_DRAGON_DANCE_STAT_LOW = 588,
    AI_ADDSUB_DRAGON_DANCE_ALLY_FOLLOW_ME = 589,
    AI_ADDSUB_DRAGON_DANCE_STAT_HIGH = 590,
    AI_ADDSUB_DRAGON_DANCE_STAGE = 591,

    AI_ADDSUB_CALM_MIND_STAT_LOW = 592,
    AI_ADDSUB_CALM_MIND_ALLY_FOLLOW_ME = 593,
    AI_ADDSUB_CALM_MIND_STAT_HIGH = 594,
    AI_ADDSUB_CALM_MIND_STAGE = 595,

    AI_ADDSUB_DEFENSE_CURL_STAT_DEFENSE_LOW = 596,
    AI_ADDSUB_DEFENSE_CURL_ROLLOUT_SYNERGY = 597,
    AI_ADDSUB_DEFENSE_CURL_STAT_DEFENSE_HIGH = 598,
    AI_ADDSUB_DEFENSE_CURL_STAGE = 599,

    AI_ADDSUB_BELLY_DRUM_STAT_ATTACK_LOW = 600,
    AI_ADDSUB_BELLY_DRUM_ALLY_FOLLOW_ME = 601,
    AI_ADDSUB_BELLY_DRUM_STAT_ATTACK_HIGH = 602,
    AI_ADDSUB_BELLY_DRUM_LOW_HP = 603,
};

static const u8 FIGHT_ABICNT_NEUTRAL = 6;
static const s8 FIGHT_AI_HP_COMPARE_AT_MOST = -1;

typedef bool (*FightAiStatPredicate)(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);

s32 _fightTrainerAiWazaValueSubNouryokuKyouka(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender, s32 value, u8 abiCnt,
    u16 statLowId, u16 allyFollowMeId, u16 statHighId, u16 stageId,
    FightAiStatPredicate predicate);
bool _fightTrainerAiWazaValueIsAllyHaveWazaDataId(FIGHT_TRAINER *trainer,
    FightOutPokemon *pokemon, u16 wazaDataId);

s32 fightTrainerAiWazaValueHaradaiko(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;

    value = _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, 0, AI_ADDSUB_BELLY_DRUM_STAT_ATTACK_LOW, AI_ADDSUB_BELLY_DRUM_ALLY_FOLLOW_ME,
        AI_ADDSUB_BELLY_DRUM_STAT_ATTACK_HIGH, AI_ADDSUB_NONE, fightTrainerAiIsFightOutPokemonPhyAtkCntTakai);
    if (fightTrainerAiIsFightOutPokemonNokoriHp(trainer, attacker, 0.5f, FIGHT_AI_HP_COMPARE_AT_MOST) == true) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_BELLY_DRUM_LOW_HP);
    }
    return value;
}

s32 fightTrainerAiWazaValueMarukunaru(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    u16 wazaDataIdAry[8];
    u16 wazaCount;
    u16 i;
    s32 value;

    wazaCount = fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, attacker, wazaDataIdAry, 0, 1);
    value = _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntPhyDef(trainer, attacker),
        AI_ADDSUB_DEFENSE_CURL_STAT_DEFENSE_LOW, AI_ADDSUB_NONE,
        AI_ADDSUB_DEFENSE_CURL_STAT_DEFENSE_HIGH, AI_ADDSUB_DEFENSE_CURL_STAGE, fightTrainerAiIsFightOutPokemonPhyDefCntTakai);
    for (i = 0; i < wazaCount; i++) {
        if (wazaDataIdAry[i] == WAZA_DATA_ID_ROLLOUT
            || wazaDataIdAry[i] == WAZA_DATA_ID_ICE_BALL) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_DEFENSE_CURL_ROLLOUT_SYNERGY);
            break;
        }
    }
    return value;
}

s32 fightTrainerAiWazaValueMeisou(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;

    value = _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntSpeDef(trainer, attacker),
        AI_ADDSUB_CALM_MIND_STAT_LOW, AI_ADDSUB_CALM_MIND_ALLY_FOLLOW_ME,
        AI_ADDSUB_CALM_MIND_STAT_HIGH, AI_ADDSUB_CALM_MIND_STAGE, fightTrainerAiIsFightOutPokemonSpeAtkCntTakai);
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        value, fightTrainerAiGetFightOutPokemonAbiCntSpeDef(trainer, attacker),
        AI_ADDSUB_CALM_MIND_STAT_LOW, AI_ADDSUB_NONE, AI_ADDSUB_CALM_MIND_STAT_HIGH, AI_ADDSUB_NONE,
        fightTrainerAiIsFightOutPokemonSpeDefCntTakai);
}

s32 fightTrainerAiWazaValueRyuunomai(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;

    value = _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntSubayasa(trainer, attacker),
        AI_ADDSUB_DRAGON_DANCE_STAT_LOW, AI_ADDSUB_DRAGON_DANCE_ALLY_FOLLOW_ME,
        AI_ADDSUB_DRAGON_DANCE_STAT_HIGH, AI_ADDSUB_DRAGON_DANCE_STAGE, fightTrainerAiIsFightOutPokemonPhyAtkCntTakai);
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        value, fightTrainerAiGetFightOutPokemonAbiCntSubayasa(trainer, attacker),
        AI_ADDSUB_DRAGON_DANCE_STAT_LOW, AI_ADDSUB_NONE, AI_ADDSUB_DRAGON_DANCE_STAT_HIGH, AI_ADDSUB_NONE,
        fightTrainerAiIsFightOutPokemonNimblenessCntTakai);
}

s32 fightTrainerAiWazaValueBirudoAppu(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;

    value = _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(trainer, attacker),
        AI_ADDSUB_BULK_UP_STAT_LOW, AI_ADDSUB_BULK_UP_ALLY_FOLLOW_ME,
        AI_ADDSUB_BULK_UP_STAT_HIGH, AI_ADDSUB_BULK_UP_STAGE, fightTrainerAiIsFightOutPokemonPhyAtkCntTakai);
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        value, fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(trainer, attacker),
        AI_ADDSUB_BULK_UP_STAT_LOW, AI_ADDSUB_NONE, AI_ADDSUB_BULK_UP_STAT_HIGH, AI_ADDSUB_NONE,
        fightTrainerAiIsFightOutPokemonPhyDefCntTakai);
}

s32 fightTrainerAiWazaValueKosumopawaa(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;

    value = _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntSpeDef(trainer, attacker),
        AI_ADDSUB_COSMIC_POWER_STAT_LOW, AI_ADDSUB_COSMIC_POWER_ALLY_FOLLOW_ME,
        AI_ADDSUB_COSMIC_POWER_STAT_HIGH, AI_ADDSUB_COSMIC_POWER_STAGE, fightTrainerAiIsFightOutPokemonPhyDefCntTakai);
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        value, fightTrainerAiGetFightOutPokemonAbiCntSpeDef(trainer, attacker),
        AI_ADDSUB_COSMIC_POWER_STAT_LOW, AI_ADDSUB_NONE, AI_ADDSUB_COSMIC_POWER_STAT_HIGH, AI_ADDSUB_NONE,
        fightTrainerAiIsFightOutPokemonSpeDefCntTakai);
}

s32 fightTrainerAiWazaValueKaihirituAppu(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntAvd(trainer, attacker),
        AI_ADDSUB_EVASION_UP_STAT_EVASION_LOW, AI_ADDSUB_EVASION_UP_ALLY_FOLLOW_ME,
        AI_ADDSUB_EVASION_UP_STAT_EVASION_HIGH, AI_ADDSUB_EVASION_UP_STAGE,
        fightTrainerAiIsFightOutPokemonAvoidCntTakai);
}

s32 fightTrainerAiWazaValueDowasure(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntSpeDef(trainer, attacker),
        AI_ADDSUB_SP_DEF_UP_STAT_SP_DEF_LOW, AI_ADDSUB_SP_DEF_UP_ALLY_FOLLOW_ME,
        AI_ADDSUB_SP_DEF_UP_STAT_SP_DEF_HIGH, AI_ADDSUB_SP_DEF_UP_STAGE, fightTrainerAiIsFightOutPokemonSpeDefCntTakai);
}

s32 fightTrainerAiWazaValueTokukouAppu(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntSpeAtk(trainer, attacker),
        AI_ADDSUB_SP_ATK_UP_STAT_SP_ATK_LOW, AI_ADDSUB_SP_ATK_UP_ALLY_FOLLOW_ME,
        AI_ADDSUB_SP_ATK_UP_STAT_SP_ATK_HIGH, AI_ADDSUB_SP_ATK_UP_STAGE, fightTrainerAiIsFightOutPokemonSpeAtkCntTakai);
}

s32 fightTrainerAiWazaValueKousokuidou(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntSubayasa(trainer, attacker),
        AI_ADDSUB_SPEED_UP_STAT_SPEED_LOW, AI_ADDSUB_SPEED_UP_ALLY_FOLLOW_ME,
        AI_ADDSUB_SPEED_UP_STAT_SPEED_HIGH, AI_ADDSUB_SPEED_UP_STAGE, fightTrainerAiIsFightOutPokemonNimblenessCntTakai);
}

s32 fightTrainerAiWazaValueBougyoAppu(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntPhyDef(trainer, attacker),
        AI_ADDSUB_DEFENSE_UP_STAT_DEFENSE_LOW, AI_ADDSUB_DEFENSE_UP_ALLY_FOLLOW_ME,
        AI_ADDSUB_DEFENSE_UP_STAT_DEFENSE_HIGH, AI_ADDSUB_DEFENSE_UP_STAGE, fightTrainerAiIsFightOutPokemonPhyDefCntTakai);
}

s32 fightTrainerAiWazaValueKougekiAppu(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    return _fightTrainerAiWazaValueSubNouryokuKyouka(trainer, attacker, wazaDataId, defender,
        0, fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(trainer, attacker),
        AI_ADDSUB_ATTACK_UP_STAT_ATTACK_LOW, AI_ADDSUB_ATTACK_UP_ALLY_FOLLOW_ME,
        AI_ADDSUB_ATTACK_UP_STAT_ATTACK_HIGH, AI_ADDSUB_ATTACK_UP_STAGE, fightTrainerAiIsFightOutPokemonPhyAtkCntTakai);
}

s32 _fightTrainerAiWazaValueSubNouryokuKyouka(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender, s32 value, u8 abiCnt,
    u16 statLowId, u16 allyFollowMeId, u16 statHighId, u16 stageId,
    FightAiStatPredicate predicate)
{
    s32 stageBonus;

    if (statLowId != AI_ADDSUB_NONE && !predicate(trainer, attacker)) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, statLowId);
    }
    if (allyFollowMeId != AI_ADDSUB_NONE
        && _fightTrainerAiWazaValueIsAllyHaveWazaDataId(trainer, attacker, WAZA_DATA_ID_FOLLOW_ME) == true) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, allyFollowMeId);
    }
    if (statHighId != AI_ADDSUB_NONE && predicate(trainer, attacker) == true) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, statHighId);
    }
    if (stageId != AI_ADDSUB_NONE) {
        stageBonus = abiCnt - FIGHT_ABICNT_NEUTRAL;
        if (stageBonus < 0) {
            stageBonus = 0;
        }
        value = fightTrainerAiAddValue(value,
            stageBonus * fightTrainerAiValueAddsubDB_GetValue(stageId));
    }
    return value;
}

bool _fightTrainerAiWazaValueIsAllyHaveWazaDataId(FIGHT_TRAINER *trainer,
    FightOutPokemon *pokemon, u16 wazaDataId)
{
    FightOutPokemon *allies[8];
    u16 wazaDataIdAry[8];
    u16 allyCount;
    u16 i;
    u16 wazaCount;
    u16 j;

    allyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, allies, 1, 1);
    for (i = 0; i < allyCount; i++) {
        if (pokemon == allies[i]) {
            continue;
        }
        wazaCount = fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, allies[i],
            wazaDataIdAry, 0, 1);
        if (wazaCount != 0) {
            for (j = 0; j < wazaCount; j++) {
                if (wazaDataIdAry[j] == wazaDataId) {
                    return true;
                }
            }
        }
    }
    return false;
}
