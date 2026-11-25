#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightSeq.h>
#include <fightTrainerAi.h>
#include <fightTrainerDB.h>
#include <hsdRandom.h>
#include <waza.h>

AI_ALLWAZA_VALID_WORK ai_allwaza_valid_work;

u8 fightTrainerAiGetWazaRiskFlag(FIGHT_TRAINER *trainer, u16 wazaDataId)
{
    u16 aiDataId = fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));

    if (fightTrainerAiDB_GetWazaRiskFlag(aiDataId) == 1) {
        return wazaDB_GetRiskFlag(wazaDataId);
    }
    return 0;
}

u8 fightTrainerAiGetWazaPp(FIGHT_TRAINER *trainer, u16 wazaDataId)
{
    fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));

    return wazaDB_GetPp(wazaDataId);
}

u8 fightTrainerAiGetWazaRangeId(FIGHT_TRAINER *trainer, u16 wazaDataId)
{
    fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));

    return wazaDB_GetRangeId(wazaDataId);
}

bool fightTrainerAiIsWazaAvgTakai(FIGHT_TRAINER *trainer, u16 wazaDataId)
{
    u16 aiDataId = fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));
    u8 wazaAvg;
    u8 avgValue;

    wazaAvg = fightTrainerAiGetWazaAvg(trainer, wazaDataId);
    avgValue = fightTrainerAiDB_GetWazaAvgValue(aiDataId);
    if (wazaAvg >= avgValue) {
        return true;
    }
    return false;
}

u8 fightTrainerAiGetWazaAvg(FIGHT_TRAINER *trainer, u16 wazaDataId)
{
    fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));

    if (fightSeqIsTenkouHit(wazaDataId) == 1) {
        return 100;
    }
    if (fightSeqIsAllwaysHit(wazaDataId) == 1) {
        return 100;
    }
    return wazaDB_GetAvg(wazaDataId);
}

u8 fightTrainerAiIsWazaTypeId(FIGHT_TRAINER *trainer, u16 wazaDataId, u8 typeId)
{
    fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));

    return wazaIsWazaTypeId(wazaDataId, typeId);
}

u8 fightTrainerAiGetWazaTypeId(FIGHT_TRAINER *trainer, u16 wazaDataId, u8 index)
{
    fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));

    return wazaDB_GetTypeId(wazaDataId, index);
}

s16 fightTrainerAiGetWazaIryoku(FIGHT_TRAINER *trainer, u16 wazaDataId)
{
    fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));

    return wazaDB_GetIryoku(wazaDataId);
}

u8 fightTrainerAiGetWazaAddKoukaAvg(FIGHT_TRAINER *trainer, u16 wazaDataId)
{
    fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));

    return wazaDB_GetAddFightKoukaAvg(wazaDataId);
}

u16 fightTrainerAiGetWazaZokuseiDataId(FIGHT_TRAINER *trainer, u16 wazaDataId, FightOutPokemon *attacker)
{
    u16 aiDataId = fightTrainerDB_GetAiDataId(fightTrainer_GetFightTrainerDataId(trainer));
    u8 weather = fightTrainerAiGetFightFloorNowTenkouDataId(trainer, 0, 1);
    u16 typeId;

    if (fightTrainerAiDB_GetZokuseiCheckFlag(aiDataId) == 1) {
        if (wazaDataId == WAZA_DATA_ID_STRUGGLE || wazaCheckDarkWaza(wazaDataId) == 1
            || wazaDataId == WAZA_DATA_ID_FUTURE_SIGHT || wazaDataId == WAZA_DATA_ID_DOOM_DESIRE) {
            return ZOKUSEI_DATA_ID_MYSTERY;
        }
        if (wazaDataId == WAZA_DATA_ID_HIDDEN_POWER) {
            pokemonGetMezamerupower(fightOutPokemonGetPokemonPtr(attacker), 0, &typeId);
            return typeId;
        }
        if (wazaDataId == WAZA_DATA_ID_WEATHER_BALL) {
            if (weather == FIGHT_WEATHER_RAIN) {
                typeId = ZOKUSEI_DATA_ID_WATER;
            } else if (weather == FIGHT_WEATHER_SANDSTORM) {
                typeId = ZOKUSEI_DATA_ID_ROCK;
            } else if (weather == FIGHT_WEATHER_SUNLIGHT) {
                typeId = ZOKUSEI_DATA_ID_FIRE;
            } else if (weather == FIGHT_WEATHER_HAIL) {
                typeId = ZOKUSEI_DATA_ID_ICE;
            } else if (weather == FIGHT_WEATHER_SHADOW_SKY) {
                typeId = ZOKUSEI_DATA_ID_NORMAL;
            } else {
                typeId = ZOKUSEI_DATA_ID_NORMAL;
            }
            return typeId;
        }
        return wazaDB_GetZokuseiDataId(wazaDataId);
    }
    return ZOKUSEI_DATA_ID_MYSTERY;
}

s32 fightTrainerAiGetValueAryMaxBanme(s32 *valueAry, u16 count, u8 rndSelectFlag)
{
    s32 banmeAry[FIGHT_AI_MAX_CANDIDATES];
    u16 i;
    u16 j;
    u16 banmeCount = 0;
    u16 banme;
    s32 maxValue = FIGHT_AI_VALUE_MIN;

    for (i = 0; i < FIGHT_AI_MAX_CANDIDATES; i++) {
        banmeAry[i] = -1;
    }
    for (j = 0; j < count; j++) {
        if (maxValue < valueAry[j]) {
            maxValue = valueAry[j];
        }
    }
    for (j = 0; j < count; j++) {
        if (maxValue <= valueAry[j]) {
            banmeAry[banmeCount++] = j;
        }
        if (banmeCount >= FIGHT_AI_MAX_CANDIDATES) {
            break;
        }
    }
    if (banmeCount == 0) {
        return -1;
    }
    if (banmeCount == 1) {
        banme = 0;
    } else if (rndSelectFlag == 1) {
        banme = (u16)HSD_Rand() % banmeCount;
    } else {
        banme = 0;
    }
    return banmeAry[banme];
}

s32 fightTrainerAiReviseValueByAddsubDataId(s32 value, u8 rate, FIGHT_TRAINER *trainer, u16 addsubDataId)
{
    return fightTrainerAiReviseValue(value, rate, fightTrainerAiValueAddsubDB_GetValue(addsubDataId));
}

s32 fightTrainerAiAddValueByAddsubDataId(s32 value, FIGHT_TRAINER *trainer, u16 addsubDataId)
{
    return fightTrainerAiAddValue(value, fightTrainerAiValueAddsubDB_GetValue(addsubDataId));
}

s32 fightTrainerAiReviseValue(s32 value, u8 rate, s32 addValue)
{
    s16 reviseValue;

    reviseValue = rate * FIGHT_AI_RATE_PERCENT / FIGHT_AI_RATE_PERCENT;
    reviseValue = (reviseValue - FIGHT_AI_RATE_NEUTRAL) * addValue / FIGHT_AI_RATE_NEUTRAL;
    return fightTrainerAiAddValue(value, reviseValue);
}

s32 fightTrainerAiAddValue(s32 value, s32 addValue)
{
    if (addValue > 0) {
        if (value + addValue > FIGHT_AI_VALUE_MAX) {
            return FIGHT_AI_VALUE_MAX;
        }
        value += addValue;
    } else if (addValue < 0) {
        if (value + addValue < FIGHT_AI_VALUE_MIN) {
            return FIGHT_AI_VALUE_MIN;
        }
        value += addValue;
    }
    return value;
}
