#include <fightFloor.h>
#include <fightSeq.h>
#include <fightTarget.h>
#include <fightTrainerAi.h>
#include <fightTrainerStatus.h>
#include <pokemonDB.h>
#include <waza.h>

enum { FIGHT_AI_HP_RATE_MAX = 100 };

// Moves that can inflict a status or confusion, scanned when scoring protective moves.
extern const u16 joutaiIzyouWazaTbl[];
extern const u16 konranWazaTbl[];

static const u8 FIGHT_ABICNT_EVASION_LIMIT = 5;
static const u8 FIGHT_ABICNT_NEUTRAL = 6;
static const s8 FIGHT_AI_HP_COMPARE_AT_MOST = -1;

typedef bool (*FightAiStatPredicate)(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon);

s32 _fightTrainerAiWazaValueSubNouryokuKyouka(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender, s32 value, u8 abiCnt,
    u16 statLowId, u16 allyFollowMeId, u16 statHighId, u16 stageId,
    FightAiStatPredicate predicate);
bool _fightTrainerAiWazaValueIsAllyHaveWazaDataId(FIGHT_TRAINER *trainer,
    FightOutPokemon *pokemon, u16 wazaDataId);

s32 fightTrainerAiWazaValueMeromero(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;

    value = 0;
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, trainer, 1, 1,
            WAZA_DATA_ID_ATTRACT, defender) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(0, trainer, AI_ADDSUB_ATTRACT_ALLY_ATTRACT);
    }
    return value;
}

s32 fightTrainerAiWazaValueDarkPanic(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;

    value = 0;
    if (fightTrainerAiGetFightOutPokemonAbiCntAvd(trainer, defender) <= FIGHT_ABICNT_EVASION_LIMIT) {
        value = fightTrainerAiAddValueByAddsubDataId(0, trainer, AI_ADDSUB_SHADOW_PANIC_AVOID_DOWN);
    }
    return value;
}

s32 fightTrainerAiWazaValueDarkMist(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;
    FightOutPokemon *allies[8];
    u16 wazaDataIdAry[8];
    u16 allyCount;
    u16 wazaCount;
    u16 i;
    u16 j;
    u8 wazaAvgValue;
    u8 foundWeakWaza;

    value = 0;
    allyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, allies, 1, 1);
    wazaAvgValue = fightTrainerAiDB_GetWazaAvgValue(fightTrainer_GetFightTrainerDataId(trainer));
    foundWeakWaza = 0;
    for (i = 0; i < allyCount; i++) {
        wazaCount = fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, allies[i], wazaDataIdAry, 0, 1);
        if (wazaCount != 0) {
            for (j = 0; j < wazaCount; j++) {
                if (wazaDB_GetAvg(wazaDataIdAry[j]) < wazaAvgValue) {
                    foundWeakWaza = 1;
                    break;
                }
            }
        }
    }
    if (foundWeakWaza) {
        value = fightTrainerAiAddValueByAddsubDataId(0, trainer, AI_ADDSUB_SHADOW_MIST_WEAK_ALLY_MOVE);
    }
    if (fightTrainerAiGetFightOutPokemonAbiCntAvd(trainer, defender) <= FIGHT_ABICNT_EVASION_LIMIT) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_MIST_AVOID_DOWN);
    }
    return value;
}

s32 fightTrainerAiWazaValueDarkHalf(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    FightOutPokemon *allies[8];
    FightOutPokemon *enemies[8];
    u16 allyCount;
    u16 enemyCount;
    u16 allyHpTotal;
    u16 enemyHpTotal;
    u16 maxHp;
    u16 nowHp;
    u8 i;
    s32 value;

    allyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, allies, 1, 1);
    enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, enemies, 0, 1);
    enemyHpTotal = 0;
    for (i = 0; i < enemyCount; i++) {
        maxHp = fightOutPokemonMaxHpWaruValue(enemies[i], 1);
        nowHp = fightOutPokemonNowHpWaruValue(enemies[i], 2);
        enemyHpTotal += maxHp * nowHp / 100;
    }
    allyHpTotal = 0;
    for (i = 0; i < allyCount; i++) {
        maxHp = fightOutPokemonMaxHpWaruValue(allies[i], 1);
        nowHp = fightOutPokemonNowHpWaruValue(allies[i], 2);
        allyHpTotal += maxHp * nowHp / 100;
    }
    value = fightTrainerAiAddValue(0, (enemyHpTotal / allyHpTotal - 1)
        * fightTrainerGetStatus(0, AI_ADDSUB_SHADOW_HALF_HP_RATIO,
            FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0));
    value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HALF_BASE);
    if (fightTrainerAiGetFightOutPokemonAbiCntAvd(trainer, defender) <= FIGHT_ABICNT_EVASION_LIMIT) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HALF_AVOID_DOWN);
    }
    return value;
}

s32 fightTrainerAiWazaValueDarkEnd(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    u16 maxHp;
    s32 nowHp;
    s32 value;
    u16 damageToDefender;
    u16 attackerHp;

    maxHp = fightOutPokemonMaxHpWaruValue(defender, 1);
    damageToDefender = maxHp
        * fightTrainerAiWazaDoWazaDamageFunc(trainer, attacker, wazaDataId, defender, 1) / 100;
    maxHp = fightOutPokemonMaxHpWaruValue(attacker, 1);
    nowHp = fightOutPokemonNowHpWaruValue(attacker, 2);
    attackerHp = maxHp * nowHp / 100;
    value = fightTrainerAiAddValue(0,
        (damageToDefender / attackerHp - 1)
        * fightTrainerGetStatus(0, AI_ADDSUB_SHADOW_END_HP_RATIO,
            FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0));
    if (fightTrainerAiGetFightOutPokemonAbiCntAvd(trainer, defender) <= FIGHT_ABICNT_EVASION_LIMIT) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_END_AVOID_DOWN);
    }
    return value;
}

s32 fightTrainerAiWazaValueDarkRemove(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;
    FIGHT_SIDE *side;

    value = 0;
    side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, defender);
    if (fightSideIsJoutaiDataId(side, JOUTAI_LIGHT_SCREEN) == 1
        || fightSideIsJoutaiDataId(side, JOUTAI_REFLECT) == 1
        || fightSideIsJoutaiDataId(side, JOUTAI_SAFEGUARD) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(0, trainer, AI_ADDSUB_SHED_SCREEN_UP);
    }
    return value;
}

s32 fightTrainerAiWazaValueDarkHold(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    FightOutPokemon *enemies[8];
    s32 value = 0;
    u8 irekae;
    u8 i;
    u16 enemyCount;

    irekae = fightTrainerAiCheckFightFloorCheckFightActionFightOutPokemonIrekaeSelect(
        trainer, 0, defender, 0);
    enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, enemies, 0, 1);
    for (i = 0; i < enemyCount; i++) {
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_POISON) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HOLD_POISON);
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_BAD_POISON) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HOLD_BAD_POISON);
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_BURN) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HOLD_BURN);
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_PARALYSIS) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HOLD_PARALYSIS);
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_CONFUSION) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HOLD_CONFUSION);
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_INFATUATION) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HOLD_INFATUATION);
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_CURSE) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HOLD_CURSE);
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_PERISH_SONG) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HOLD_PERISH_SONG);
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_LEECH_SEED) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_HOLD_LEECH_SEED);
        }
    }
    if (fightTrainerAiGetFightFloorNowTenkouDataId(trainer, 0, 1) == FIGHT_WEATHER_SHADOW_SKY) {
        for (i = 0; i < enemyCount; i++) {
            if (!pokemonIsDarkPokemon(fightOutPokemonGetPokemonPtr(enemies[i]))) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                    AI_ADDSUB_SHADOW_HOLD_NON_DARK_ENEMY);
            }
        }
    }
    if (irekae) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_SHADOW_HOLD_SWITCH_ELIGIBLE);
    }
    return value;
}

s32 fightTrainerAiWazaValueDarkWeather(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    FightPokemon *party[24];
    s32 value;
    u8 tenkou;
    u16 count;
    u16 i;

    value = 0;
    tenkou = fightTrainerAiGetFightFloorNowTenkouDataId(trainer, 0, 0);
    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, trainer, party, 1, 1);
    if (tenkou != FIGHT_WEATHER_SHADOW_SKY) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_DARK_WEATHER_NOT_ACTIVE);
    }
    for (i = 0; i < count; i++) {
        if (pokemonIsDarkPokemon(fightPokemonGetPokemonPtr(party[i])) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_DARK_WEATHER_DARK_ALLY);
            break;
        }
    }
    for (i = 0; i < count; i++) {
        if (fightTrainerAiGetFightPokemonPokemonDataId(trainer, party[i]) == POKEMON_DATA_ID_CASTFORM) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_DARK_WEATHER_CASTFORM);
            break;
        }
    }
    if (tenkou == FIGHT_WEATHER_SHADOW_SKY) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_DARK_WEATHER_ALREADY_ACTIVE);
    }
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
            0, trainer, 1, 1, WAZA_DATA_ID_DARK_SKY, 0) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_DARK_WEATHER_ALLY_DARK_SKY);
    }
    return value;
}

s32 fightTrainerAiWazaValueHuuin(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    FightOutPokemon *enemies[8];
    s32 value;
    u16 enemyCount;
    s32 i;

    value = 0;
    enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, enemies, 0, 1);
    if (fightOutPokemon_GetFightoutTurnCount(attacker) != 0) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_IMPRISON_SELF_ACTED);
    }
    for (i = 0; i < enemyCount; i++) {
        if (fightOutPokemon_GetFightoutTurnCount(enemies[i]) != 0) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_IMPRISON_ENEMY_ACTED);
        }
        break;
    }
    return value;
}

s32 fightTrainerAiWazaValueMahi(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;
    u8 i;

    value = 0;
    for (i = 0; i < 18; i++) {
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, trainer, 1, 1, joutaiIzyouWazaTbl[i], defender) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_ALLY_STATUS_MOVE_PLANNED);
            break;
        }
    }
    return value;
}

s32 fightTrainerAiWazaValueKonran(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;
    u8 i;

    value = 0;
    for (i = 0; i < 5; i++) {
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, trainer, 1, 1, konranWazaTbl[i], defender) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_ALLY_STATUS_MOVE_PLANNED);
            break;
        }
    }
    return value;
}

s32 fightTrainerAiWazaValueYumekui(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    u16 maxHp;
    s32 damage;
    s32 hpRate;
    s32 value;

    fightTrainerAiGetFightOutPokemonNowHp(trainer, attacker);
    maxHp = fightTrainerAiGetFightOutPokemonMaxHp(trainer, attacker);
    damage = fightTrainerAiWazaDoWazaDamageFunc(trainer, attacker, wazaDataId, defender, 1);
    hpRate = damage / 2 * 100 / maxHp;
    if (hpRate > FIGHT_AI_HP_RATE_MAX) {
        hpRate = FIGHT_AI_HP_RATE_MAX;
    }
    value = fightTrainerAiAddValue(0,
        hpRate / fightTrainerGetStatus(0, AI_ADDSUB_DREAM_EATER_HP_DIVISOR,
                                       FIGHT_TRAINER_QUERY_AI_VALUE_ADDSUB_VALUE, 0));
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
            0, trainer, 1, 1, WAZA_DATA_ID_HELPING_HAND, attacker) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_DREAM_EATER_ALLY_HELPING_HAND);
    }
    return value;
}

s32 fightTrainerAiWazaValueKawarawari(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;
    FIGHT_SIDE *side;

    value = 0;
    side = (FIGHT_SIDE *)fightTargetGetPtrAsNowFightType(FIGHT_TARGET_ID_OWN_SIDE, defender);
    if (fightSideIsJoutaiDataId(side, JOUTAI_LIGHT_SCREEN) == 1
        || fightSideIsJoutaiDataId(side, JOUTAI_REFLECT) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_BRICK_BREAK_SCREEN_UP);
    }
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
            0, trainer, 1, 1, WAZA_DATA_ID_HELPING_HAND, attacker) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_BRICK_BREAK_ALLY_HELPING_HAND);
    }
    return value;
}

s32 fightTrainerAiWazaValueTeiryouDameeji(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;

    value = 0;
    if (fightTrainerAiGetFightOutPokemonPokemonDataId(trainer, defender) == POKEMON_DATA_ID_SHUCKLE) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_FIXED_DAMAGE_SHUCKLE);
    }
    return value;
}

s32 fightTrainerAiWazaValueHikarinokabe(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;
    u16 speAtk;
    u16 phyAtk;
    u16 lastUseWazaDataId;

    value = 0;
    speAtk = fightTrainerAiGetFightOutPokemonSpeAtk(trainer, defender);
    phyAtk = fightTrainerAiGetFightOutPokemonPhyAtk(trainer, defender);
    lastUseWazaDataId = fightTrainerAiGetFightOutPokemonLastUseWazaDataId(trainer, defender);
    if (fightTrainerAiWazaDoWazaHitFunc(trainer, attacker, wazaDataId, defender) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_LIGHT_SCREEN_HIT);
    }
    if (phyAtk < speAtk) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_LIGHT_SCREEN_SPECIAL_TARGET);
    }
    if (lastUseWazaDataId == WAZA_DATA_ID_BRICK_BREAK) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_LIGHT_SCREEN_TARGET_BRICK_BREAK);
    }
    return value;
}

s32 fightTrainerAiWazaValueRihurekutaa(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;
    u16 speAtk;
    u16 phyAtk;
    u16 lastUseWazaDataId;

    value = 0;
    speAtk = fightTrainerAiGetFightOutPokemonSpeAtk(trainer, defender);
    phyAtk = fightTrainerAiGetFightOutPokemonPhyAtk(trainer, defender);
    lastUseWazaDataId = fightTrainerAiGetFightOutPokemonLastUseWazaDataId(trainer, defender);
    if (fightTrainerAiWazaDoWazaHitFunc(trainer, attacker, wazaDataId, defender) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_REFLECT_HIT);
    }
    if (phyAtk > speAtk) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_REFLECT_PHYSICAL_TARGET);
    }
    if (lastUseWazaDataId == WAZA_DATA_ID_BRICK_BREAK) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_REFLECT_TARGET_BRICK_BREAK);
    }
    return value;
}

s32 fightTrainerAiWazaValueOnibi(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    FightOutPokemon *enemies[8];
    s32 value = 0;
    u16 speAtk;
    u16 phyAtk;
    u16 lastUseWazaDataId;
    u16 enemyCount;
    u16 j;
    u16 i;
    u16 species;

    speAtk = fightTrainerAiGetFightOutPokemonSpeAtk(trainer, defender);
    phyAtk = fightTrainerAiGetFightOutPokemonPhyAtk(trainer, defender);
    lastUseWazaDataId = fightTrainerAiGetFightOutPokemonLastUseWazaDataId(trainer, defender);
    enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, enemies, 0, 1);
    for (i = 0; i < enemyCount; i++) {
        species = fightTrainerAiGetFightOutPokemonPokemonDataId(trainer, enemies[i]);
        if (species == POKEMON_DATA_ID_WOBBUFFET
            || species == POKEMON_DATA_ID_WYNAUT
            || species == POKEMON_DATA_ID_SHEDINJA
            || species == POKEMON_DATA_ID_SHUCKLE) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_BURN_STALL_TARGET);
            break;
        }
    }
    if (phyAtk > speAtk) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_BURN_PHYSICAL_TARGET);
    }
    if (lastUseWazaDataId == WAZA_DATA_ID_REFRESH) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_BURN_TARGET_REFRESH);
    }
    for (j = 0; j < 18; j++) {
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, trainer, 1, 1, joutaiIzyouWazaTbl[j], defender) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_BURN_ALLY_STATUS_MOVE);
            break;
        }
    }
    return value;
}

s32 fightTrainerAiWazaValueDoku(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    FightOutPokemon *enemies[8];
    s32 value = 0;
    u16 lastUseWazaDataId;
    u16 enemyCount;
    u16 j;
    u16 i;
    u16 species;

    enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, enemies, 0, 1);
    lastUseWazaDataId = fightTrainerAiGetFightOutPokemonLastUseWazaDataId(trainer, defender);
    for (i = 0; i < enemyCount; i++) {
        species = fightTrainerAiGetFightOutPokemonPokemonDataId(trainer, enemies[i]);
        if (species == POKEMON_DATA_ID_WOBBUFFET
            || species == POKEMON_DATA_ID_WYNAUT
            || species == POKEMON_DATA_ID_SHEDINJA
            || species == POKEMON_DATA_ID_SHUCKLE) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_POISON_STALL_TARGET);
            break;
        }
    }
    if (lastUseWazaDataId == WAZA_DATA_ID_REFRESH) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_POISON_TARGET_REFRESH);
    }
    for (j = 0; j < 18; j++) {
        if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                0, trainer, 1, 1, joutaiIzyouWazaTbl[j], defender) == 1) {
            value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
                AI_ADDSUB_ALLY_STATUS_MOVE_PLANNED);
            break;
        }
    }
    return value;
}

s32 fightTrainerAiWazaValueBatontatti(FIGHT_TRAINER *trainer, FightOutPokemon *attacker,
    u16 wazaDataId, FightOutPokemon *defender)
{
    s32 value;

    value = 0;
    if (fightTrainerAiIsFightOutPokemonAllAbiCntTakai(trainer, attacker) == 1) {
        value = fightTrainerAiAddValueByAddsubDataId(value, trainer,
            AI_ADDSUB_BATON_PASS_ALL_STATS_HIGH);
    }
    return value;
}

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
