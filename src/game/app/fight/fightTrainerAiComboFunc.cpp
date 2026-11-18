#include <fightTrainerAiCombo.h>
#include <fightFloor.h>
#include <fightPokemon.h>
#include <fightSeq.h>
#include <hsdRandom.h>
#include <pokemonDB.h>
#include <waza.h>

u8 fightAiCombo_Kinokonohousi_Kiaipanti_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;

    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    u8 awakeCount = 0;
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonJoutaiDataId(trainer, enemies[i], JOUTAI_SLEEP) == 0) {
                awakeCount++;
            }
        }
    }
    if (awakeCount == 0) {
        return 0;
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_SPORE, FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_FOCUS_PUNCH);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, -1);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

enum FIGHT_AI_WEATHER_COMBO_ADDSUB_DATA_ID {
    AI_ADDSUB_SUN_SOLAR_BEAM_ENEMY_FIRE = 668,
    AI_ADDSUB_SUN_SOLAR_BEAM_ENEMY_CHLOROPHYLL = 669,
    AI_ADDSUB_SUN_FIRE_ENEMY_FIRE = 670,
    AI_ADDSUB_SUN_FIRE_ENEMY_CHLOROPHYLL = 671,
    AI_ADDSUB_RAIN_THUNDER_ENEMY_WATER = 672,
    AI_ADDSUB_RAIN_THUNDER_ENEMY_RAIN_DISH = 673,
    AI_ADDSUB_RAIN_THUNDER_ENEMY_SWIFT_SWIM = 674,
    AI_ADDSUB_RAIN_WATER_ENEMY_WATER = 675,
    AI_ADDSUB_RAIN_WATER_ENEMY_RAIN_DISH = 676,
    AI_ADDSUB_RAIN_WATER_ENEMY_SWIFT_SWIM = 677,
    AI_ADDSUB_SUN_WEATHER_BALL_ENEMY_FIRE = 678,
    AI_ADDSUB_SUN_WEATHER_BALL_ENEMY_CHLOROPHYLL = 679,
    AI_ADDSUB_RAIN_WEATHER_BALL_ENEMY_WATER = 680,
    AI_ADDSUB_RAIN_WEATHER_BALL_ENEMY_RAIN_DISH = 681,
    AI_ADDSUB_RAIN_WEATHER_BALL_ENEMY_SWIFT_SWIM = 682,
    AI_ADDSUB_SAND_WEATHER_BALL_ENEMY_IMMUNE = 683,
    AI_ADDSUB_SAND_WEATHER_BALL_ENEMY_SAND_VEIL = 684,
    AI_ADDSUB_HAIL_WEATHER_BALL_ENEMY_ICE = 685,
    AI_ADDSUB_SHADOW_WEATHER_BALL_ENEMY_SHADOW = 686,
    AI_ADDSUB_SHADOW_WEATHER_BALL_ALLY_SHADOW = 687,
};

u8 fightAiCombo_DarkWeather_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;

    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    if (fightFloorGetNowTenkouDataId(0, false) == FIGHT_WEATHER_SUNLIGHT) {
        return 0;
    }
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_AIR_LOCK) == FIGHT_AI_TOKUSEI_KNOWN
                || fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_CLOUD_NINE) == FIGHT_AI_TOKUSEI_KNOWN) {
                return 0;
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_AIR_LOCK) >= FIGHT_AI_TOKUSEI_POSSIBLE
                || fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CLOUD_NINE) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                return 0;
            }
        }
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_DARK_SKY, FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_WEATHER_BALL);
    fightAi_AllWazaValid_DelListFromSpeed(trainer, records, &count, WAZA_DATA_ID_DARK_SKY, FIGHT_AI_COMBO_MOVE_FIRST);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (pokemonIsDarkPokemon(fightOutPokemonGetPokemonPtr(allies[i])) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_WEATHER_BALL_ALLY_SHADOW);
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (pokemonIsDarkPokemon(fightOutPokemonGetPokemonPtr(enemies[i])) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SHADOW_WEATHER_BALL_ENEMY_SHADOW);
            }
        }
    }
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, FIGHT_WEATHER_SHADOW_SKY);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

u8 fightAiCombo_Arare_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;

    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    if (fightFloorGetNowTenkouDataId(0, false) == FIGHT_WEATHER_SUNLIGHT) {
        return 0;
    }
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_AIR_LOCK) == FIGHT_AI_TOKUSEI_KNOWN
                || fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_CLOUD_NINE) == FIGHT_AI_TOKUSEI_KNOWN) {
                return 0;
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_AIR_LOCK) >= FIGHT_AI_TOKUSEI_POSSIBLE
                || fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CLOUD_NINE) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                return 0;
            }
        }
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_HAIL, FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_WEATHER_BALL);
    fightAi_AllWazaValid_DelListFromSpeed(trainer, records, &count, WAZA_DATA_ID_HAIL, FIGHT_AI_COMBO_MOVE_FIRST);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_ICE) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_HAIL_WEATHER_BALL_ENEMY_ICE);
            }
        }
    }
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, FIGHT_WEATHER_HAIL);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

u8 fightAiCombo_Sunaarasi_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;

    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    if (fightFloorGetNowTenkouDataId(0, false) == FIGHT_WEATHER_SUNLIGHT) {
        return 0;
    }
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_AIR_LOCK) == FIGHT_AI_TOKUSEI_KNOWN
                || fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_CLOUD_NINE) == FIGHT_AI_TOKUSEI_KNOWN) {
                return 0;
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_AIR_LOCK) >= FIGHT_AI_TOKUSEI_POSSIBLE
                || fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CLOUD_NINE) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                return 0;
            }
        }
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_SANDSTORM, FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_WEATHER_BALL);
    fightAi_AllWazaValid_DelListFromSpeed(trainer, records, &count, WAZA_DATA_ID_SANDSTORM, FIGHT_AI_COMBO_MOVE_FIRST);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_ROCK) == 1
                || fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_STEEL) == 1
                || fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_GROUND) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SAND_WEATHER_BALL_ENEMY_IMMUNE);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_SAND_VEIL) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SAND_WEATHER_BALL_ENEMY_SAND_VEIL);
            }
        }
    }
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, FIGHT_WEATHER_SANDSTORM);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

u8 fightAiCombo_Amagoi_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;

    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    if (fightFloorGetNowTenkouDataId(0, false) == FIGHT_WEATHER_RAIN) {
        return 0;
    }
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_AIR_LOCK) == FIGHT_AI_TOKUSEI_KNOWN
                || fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_CLOUD_NINE) == FIGHT_AI_TOKUSEI_KNOWN) {
                return 0;
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_AIR_LOCK) >= FIGHT_AI_TOKUSEI_POSSIBLE
                || fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CLOUD_NINE) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                return 0;
            }
        }
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_RAIN_DANCE, FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_WEATHER_BALL);
    fightAi_AllWazaValid_DelListFromSpeed(trainer, records, &count, WAZA_DATA_ID_RAIN_DANCE, FIGHT_AI_COMBO_MOVE_FIRST);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_WATER) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_RAIN_WEATHER_BALL_ENEMY_WATER);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_RAIN_DISH) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_RAIN_WEATHER_BALL_ENEMY_RAIN_DISH);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_SWIFT_SWIM) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_RAIN_WEATHER_BALL_ENEMY_SWIFT_SWIM);
            }
        }
    }
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, FIGHT_WEATHER_RAIN);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

u8 fightAiCombo_Nihonbare_Whezaabooru_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;

    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    if (fightFloorGetNowTenkouDataId(0, false) == FIGHT_WEATHER_SUNLIGHT) {
        return 0;
    }
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_AIR_LOCK) == FIGHT_AI_TOKUSEI_KNOWN
                || fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_CLOUD_NINE) == FIGHT_AI_TOKUSEI_KNOWN) {
                return 0;
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_AIR_LOCK) >= FIGHT_AI_TOKUSEI_POSSIBLE
                || fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CLOUD_NINE) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                return 0;
            }
        }
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_SUNNY_DAY, FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_WEATHER_BALL);
    fightAi_AllWazaValid_DelListFromSpeed(trainer, records, &count, WAZA_DATA_ID_SUNNY_DAY, FIGHT_AI_COMBO_MOVE_FIRST);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_FIRE) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SUN_WEATHER_BALL_ENEMY_FIRE);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CHLOROPHYLL) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SUN_WEATHER_BALL_ENEMY_CHLOROPHYLL);
            }
        }
    }
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, FIGHT_WEATHER_SUNLIGHT);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

u8 fightAiCombo_Amagoi_Mizu_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;
    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    if (fightFloorGetNowTenkouDataId(0, false) == FIGHT_WEATHER_RAIN) {
        return 0;
    }
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_AIR_LOCK) == FIGHT_AI_TOKUSEI_KNOWN ||
                fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_CLOUD_NINE) == FIGHT_AI_TOKUSEI_KNOWN) {
                return 0;
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_AIR_LOCK) >= FIGHT_AI_TOKUSEI_POSSIBLE ||
                fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CLOUD_NINE) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                return 0;
            }
        }
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_RAIN_DANCE, FIGHT_AI_COMBO_MATCH_ZOKUSEI, ZOKUSEI_DATA_ID_WATER);
    fightAi_AllWazaValid_DelComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_RAIN_DANCE, FIGHT_AI_COMBO_MATCH_WAZA_TYPE,
        WAZA_TYPE_ID_IRYOKU, true);
    fightAi_AllWazaValid_DelListFromSpeed(trainer, records, &count,
        WAZA_DATA_ID_RAIN_DANCE, FIGHT_AI_COMBO_MOVE_FIRST);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_WATER) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_RAIN_WATER_ENEMY_WATER);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_RAIN_DISH) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_RAIN_WATER_ENEMY_RAIN_DISH);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_SWIFT_SWIM) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_RAIN_WATER_ENEMY_SWIFT_SWIM);
            }
        }
    }
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, FIGHT_WEATHER_RAIN);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

u8 fightAiCombo_Amagoi_Kaminari_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;
    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    if (fightFloorGetNowTenkouDataId(0, false) == FIGHT_WEATHER_RAIN) {
        return 0;
    }
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_AIR_LOCK) == FIGHT_AI_TOKUSEI_KNOWN ||
                fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_CLOUD_NINE) == FIGHT_AI_TOKUSEI_KNOWN) {
                return 0;
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_AIR_LOCK) >= FIGHT_AI_TOKUSEI_POSSIBLE ||
                fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CLOUD_NINE) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                return 0;
            }
        }
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_RAIN_DANCE, FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_THUNDER);
    fightAi_AllWazaValid_DelListFromSpeed(trainer, records, &count,
        WAZA_DATA_ID_RAIN_DANCE, FIGHT_AI_COMBO_MOVE_FIRST);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_WATER) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_RAIN_THUNDER_ENEMY_WATER);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_RAIN_DISH) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_RAIN_THUNDER_ENEMY_RAIN_DISH);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_SWIFT_SWIM) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_RAIN_THUNDER_ENEMY_SWIFT_SWIM);
            }
        }
    }
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, FIGHT_WEATHER_RAIN);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

u8 fightAiCombo_Nihonbare_Honoo_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;
    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    if (fightFloorGetNowTenkouDataId(0, false) == FIGHT_WEATHER_SUNLIGHT) {
        return 0;
    }
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_AIR_LOCK) == FIGHT_AI_TOKUSEI_KNOWN ||
                fightAi_GetTokuseiHaveFlag(trainer, allies[i], TOKUSEI_DATA_ID_CLOUD_NINE) == FIGHT_AI_TOKUSEI_KNOWN) {
                return 0;
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_AIR_LOCK) >= FIGHT_AI_TOKUSEI_POSSIBLE ||
                fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CLOUD_NINE) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                return 0;
            }
        }
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_SUNNY_DAY, FIGHT_AI_COMBO_MATCH_ZOKUSEI, ZOKUSEI_DATA_ID_FIRE);
    fightAi_AllWazaValid_DelComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_SUNNY_DAY, FIGHT_AI_COMBO_MATCH_WAZA_TYPE,
        WAZA_TYPE_ID_IRYOKU, true);
    fightAi_AllWazaValid_DelListFromSpeed(trainer, records, &count,
        WAZA_DATA_ID_SUNNY_DAY, FIGHT_AI_COMBO_MOVE_FIRST);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_FIRE) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SUN_FIRE_ENEMY_FIRE);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CHLOROPHYLL) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SUN_FIRE_ENEMY_CHLOROPHYLL);
            }
        }
    }
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, FIGHT_WEATHER_SUNLIGHT);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

u8 fightAiCombo_Nihonbare_Sooraabiimu_2(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    AI_ALLWAZA_VALID_TBL records[FIGHT_AI_MAX_COMBINATIONS];
    u8 count = 0;
    u8 i;
    s16 value;
    s16 score;
    for (i = 0; i < FIGHT_AI_MAX_COMBINATIONS; i++) {
        fightAi_InitAllWazaValidSub(&records[i]);
    }
    if (ai_allwaza_valid_work.mPokemonCount < 2) {
        return 0;
    }
    if (allyCount < 2) {
        return 0;
    }
    if (fightFloorGetNowTenkouDataId(0, false) == FIGHT_WEATHER_SUNLIGHT) {
        return 0;
    }
    for (i = 0; i < allyCount; i++) {
        if (allies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, allies[i], TOKUSEI_DATA_ID_AIR_LOCK) == 1 ||
                fightTrainerAiIsFightOutPokemonTokuseiDataId(trainer, allies[i], TOKUSEI_DATA_ID_CLOUD_NINE) == 1) {
                return 0;
            }
        }
    }
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_AIR_LOCK) >= FIGHT_AI_TOKUSEI_POSSIBLE ||
                fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CLOUD_NINE) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                return 0;
            }
        }
    }
    fightAi_AllWazaValid_AddComboList(trainer, records, &count,
        FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_SUNNY_DAY, FIGHT_AI_COMBO_MATCH_WAZA, WAZA_DATA_ID_SOLAR_BEAM);
    fightAi_AllWazaValid_DelListFromSpeed(trainer, records, &count,
        WAZA_DATA_ID_SUNNY_DAY, FIGHT_AI_COMBO_MOVE_FIRST);
    if (count == 0) {
        return 0;
    }
    value = fightAiCombo_GetValidValue(allies, allyCount, enemies, enemyCount);
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0) {
            if (fightTrainerAiIsFightOutPokemonZokuseiDataId(trainer, enemies[i], ZOKUSEI_DATA_ID_FIRE) == 1) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SUN_SOLAR_BEAM_ENEMY_FIRE);
            }
            if (fightAi_GetTokuseiHaveFlag(trainer, enemies[i], TOKUSEI_DATA_ID_CHLOROPHYLL) >= FIGHT_AI_TOKUSEI_POSSIBLE) {
                value = fightTrainerAiAddValueByAddsubDataId(value, trainer, AI_ADDSUB_SUN_SOLAR_BEAM_ENEMY_CHLOROPHYLL);
            }
        }
    }
    score = fightAiCombo_GetAllWazaMaxValueTable(records, count, value,
        trainer, allies, enemies, output, FIGHT_WEATHER_SUNLIGHT);
    if (score > 0) {
        u16 randomValue = HSD_Rand();
        if (score + activation > (u8)(randomValue % 100)) {
            for (i = 0; i < allyCount; i++) {
                allies[i]->setComboFuncId(FIGHT_AI_COMBO_ID_NONE);
            }
            return 1;
        }
    }
    return 0;
}

u8 fightAiCombo_null(FIGHT_TRAINER *trainer, FightOutPokemon **allies,
    u16 allyCount, FightOutPokemon **enemies, u16 enemyCount,
    AI_ALLWAZA_VALID_TBL *output, u8 activation)
{
    return 0;
}

s8 fightAi_getAnotherPokeNo(u8 pokeNo)
{
    if (pokeNo == 0) {
        return 1;
    }
    if (pokeNo == 1) {
        return 0;
    }
    return -1;
}
