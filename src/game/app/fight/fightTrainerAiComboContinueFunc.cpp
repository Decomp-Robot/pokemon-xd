#include <fightTrainerAiCombo.h>

#include <fightFloor.h>
#include <pokemonDB.h>
#include <fightSeq.h>
#include <fightTarget.h>
#include <hsdRandom.h>
#include <waza.h>

// Initial score given to the moves a continuing combo may still pick.
enum { FIGHT_AI_COMBO_CONTINUE_WAZA_VALUE = 50 };

// Ability count a Pokemon needs before its continuing combo acts.
enum { FIGHT_AI_COMBO_CONTINUE_ABICNT_HIGH = 7 };

// Number of moves a trainer AI Pokemon can hold.
enum { FIGHT_AI_COMBO_CONTINUE_WAZA_NUM = 4 };

u8 fightAiCombo_Batontatti_Zikoanzi_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon,
    FightOutPokemon **enemies, u16 enemyCount)
{
    u16 wazaDataIdAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    s16 wazaBanmeAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    u8 valueAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    u8 i;
    u16 wazaCount;
    u16 prevWazaDataId;
    u16 wazaDataId = 0;
    u32 fightType;
    u8 phyAtkCnt;
    u8 speedCnt;
    u8 targetSpeedCnt;
    FightOutPokemon *target;
    FightOutPokemon *defender;
    s8 index;

    fightType = fightFloor_GetFightType(0);
    wazaCount = fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, pokemon, wazaDataIdAry,
        wazaBanmeAry, 1);
    prevWazaDataId = fightTrainerAiGetFightOutPokemonLastUseWazaDataId(trainer, pokemon);
    target = (FightOutPokemon *)fightTargetGetPtr(FIGHT_TARGET_ID_OTHER_POKEMON_ON_SIDE, pokemon,
        fightType);
    phyAtkCnt = fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(trainer, pokemon);
    speedCnt = fightTrainerAiGetFightOutPokemonAbiCntSubayasa(trainer, pokemon);
    targetSpeedCnt = fightTrainerAiGetFightOutPokemonAbiCntSubayasa(trainer, target);
    switch (prevWazaDataId) {
    case WAZA_DATA_ID_PROTECT:
        if (speedCnt >= FIGHT_AI_COMBO_CONTINUE_ABICNT_HIGH) {
            wazaDataId = WAZA_DATA_ID_BATON_PASS;
        }
        break;
    case WAZA_DATA_ID_PSYCH_UP:
        if (speedCnt >= FIGHT_AI_COMBO_CONTINUE_ABICNT_HIGH) {
            wazaDataId = WAZA_DATA_ID_BATON_PASS;
        }
        break;
    case WAZA_DATA_ID_FAKE_OUT:
        if (targetSpeedCnt >= FIGHT_AI_COMBO_CONTINUE_ABICNT_HIGH) {
            wazaDataId = WAZA_DATA_ID_PSYCH_UP;
        }
        break;
    default:
        if (phyAtkCnt >= FIGHT_AI_COMBO_CONTINUE_ABICNT_HIGH
            && speedCnt >= FIGHT_AI_COMBO_CONTINUE_ABICNT_HIGH) {
            wazaDataId = WAZA_DATA_ID_PROTECT;
        }
        break;
    }

    for (i = 0; i < wazaCount; i++) {
        valueAry[wazaBanmeAry[i]] = FIGHT_AI_COMBO_CONTINUE_WAZA_VALUE;
        if (fightAi_CanMoveAppointWaza(trainer, pokemon, wazaDataIdAry[i]) == 0) {
            valueAry[wazaBanmeAry[i]] = 0;
        }
        if (wazaDataId != wazaDataIdAry[i] || wazaDataId == 0) {
            valueAry[wazaBanmeAry[i]] = 0;
        }
    }

    index = _getWazaMaxValueIndex(valueAry, wazaBanmeAry, wazaCount, 0);
    if (index < 0) {
        return 0;
    }
    defender = (wazaDataId == WAZA_DATA_ID_PSYCH_UP) ? target : pokemon;
    if (defender == 0) {
        return 0;
    }
    fightAi_CreateFightActionAttckWaza(pokemon, defender, wazaDataIdAry[index],
        wazaBanmeAry[index]);
    return 1;
}

u8 fightAiCombo_Batontatti_Nekodamasi_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon,
    FightOutPokemon **enemies, u16 enemyCount)
{
    u16 wazaDataIdAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    s16 wazaBanmeAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    u8 valueAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    u8 i;
    u16 wazaCount;
    u16 prevWazaDataId;
    u16 wazaDataId = 0;
    u8 phyAtkCnt;
    u8 speedCnt;
    s8 index;

    wazaCount = fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, pokemon, wazaDataIdAry,
        wazaBanmeAry, 1);
    prevWazaDataId = fightTrainerAiGetFightOutPokemonLastUseWazaDataId(trainer, pokemon);
    phyAtkCnt = fightTrainerAiGetFightOutPokemonAbiCntPhyAtk(trainer, pokemon);
    speedCnt = fightTrainerAiGetFightOutPokemonAbiCntSubayasa(trainer, pokemon);
    if (prevWazaDataId == WAZA_DATA_ID_PROTECT
        && speedCnt >= FIGHT_AI_COMBO_CONTINUE_ABICNT_HIGH) {
        wazaDataId = WAZA_DATA_ID_BATON_PASS;
    }
    if (prevWazaDataId != WAZA_DATA_ID_PROTECT
        && phyAtkCnt >= FIGHT_AI_COMBO_CONTINUE_ABICNT_HIGH
        && speedCnt >= FIGHT_AI_COMBO_CONTINUE_ABICNT_HIGH) {
        wazaDataId = WAZA_DATA_ID_PROTECT;
    }

    for (i = 0; i < wazaCount; i++) {
        valueAry[wazaBanmeAry[i]] = FIGHT_AI_COMBO_CONTINUE_WAZA_VALUE;
        if (fightAi_CanMoveAppointWaza(trainer, pokemon, wazaDataIdAry[i]) == 0) {
            valueAry[wazaBanmeAry[i]] = 0;
        }
        if (wazaDataId != wazaDataIdAry[i] || wazaDataId == 0) {
            valueAry[wazaBanmeAry[i]] = 0;
        }
    }

    index = _getWazaMaxValueIndex(valueAry, wazaBanmeAry, wazaCount, 0);
    if (index < 0) {
        return 0;
    }
    if (pokemon == 0) {
        return 0;
    }
    fightAi_CreateFightActionAttckWaza(pokemon, pokemon, wazaDataIdAry[index],
        wazaBanmeAry[index]);
    return 1;
}

u8 fightAiCombo_Sukirusuwappu_Irekae_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon,
    FightOutPokemon **enemies, u16 enemyCount)
{
    u16 wazaDataIdAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    s16 wazaBanmeAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    u32 fightType;
    s16 index;

    fightType = fightFloor_GetFightType(0);
    fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, pokemon, wazaDataIdAry, wazaBanmeAry, 1);
    if (fightAi_GetTokuseiHaveFlag(trainer, pokemon, TOKUSEI_DATA_ID_TRUANT)
        != FIGHT_AI_TOKUSEI_KNOWN) {
        return 0;
    }
    if (fightTrainerAiIsFightPokemonTokuseiDataId(trainer, pokemon->getFightPokemonPtr(),
            TOKUSEI_DATA_ID_TRUANT) == 1) {
        return 0;
    }
    index = fightTrainerAiSelectIrekaeDasuFightPokemon(trainer, fightType, 1, pokemon);
    if (index < 0) {
        return 0;
    }
    fightAi_CreateFightActionAttckIrekae(pokemon, index);
    return 1;
}

u8 fightAiCombo_Amaikaori_Hubuki_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon,
    FightOutPokemon **enemies, u16 enemyCount)
{
    u16 wazaDataIdAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    s16 wazaBanmeAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    u8 valueAry[FIGHT_AI_COMBO_CONTINUE_WAZA_NUM];
    u16 wazaCount;
    FightOutPokemon *target;
    u8 hikuiNum;
    u8 i;
    s8 index;

    wazaCount = fightTrainerAiGetFightOutPokemonWazaDataIdAry(trainer, pokemon, wazaDataIdAry,
        wazaBanmeAry, 1);
    hikuiNum = 0;
    for (i = 0; i < enemyCount; i++) {
        if (enemies[i] != 0
            && fightTrainerAiIsFightOutPokemonAvoidCntHikui(trainer, enemies[i]) == 1) {
            hikuiNum++;
        }
    }
    if (hikuiNum == 0) {
        return 0;
    }

    for (i = 0; i < wazaCount; i++) {
        valueAry[wazaBanmeAry[i]] = FIGHT_AI_COMBO_CONTINUE_WAZA_VALUE;
        if (fightAi_CanMoveAppointWaza(trainer, pokemon, wazaDataIdAry[i]) == 0) {
            valueAry[wazaBanmeAry[i]] = 0;
        }
        if (wazaDataIdAry[i] != WAZA_DATA_ID_BLIZZARD
            && wazaDataIdAry[i] != WAZA_DATA_ID_ROCK_SLIDE) {
            valueAry[wazaBanmeAry[i]] = 0;
        }
    }

    index = _getWazaMaxValueIndex(valueAry, wazaBanmeAry, wazaCount, 0);
    if (index < 0) {
        return 0;
    }

    if (hikuiNum >= 2) {
        target = fightAi_SelectDefPoke(trainer, pokemon, enemies, enemyCount,
            wazaDataIdAry[index], -1);
    } else {
        for (i = 0; i < enemyCount; i++) {
            if (enemies[i] != 0
                && fightTrainerAiIsFightOutPokemonAvoidCntHikui(trainer, enemies[i]) == 1) {
                target = enemies[i];
                break;
            }
        }
    }
    if (target == 0) {
        return 0;
    }
    fightAi_CreateFightActionAttckWaza(pokemon, target, wazaDataIdAry[index],
        wazaBanmeAry[index]);
    return 1;
}

// The strategy has nothing left to continue, so the combo ends on the next turn.
u8 fightAiCombo_null_Continue(FIGHT_TRAINER *trainer, FightOutPokemon *pokemon, FightOutPokemon **enemies,
    u16 enemyCount)
{
    return 0;
}

s8 _getWazaMaxValueIndex(u8 *valueAry, s16 *banmeAry, u16 wazaCount, u8 comparisonValue)
{
    u8 tieAry[FIGHT_AI_MAX_COMBINATIONS];
    u8 maxValue = comparisonValue;
    u8 tieNum;
    u8 i;
    s8 index;

    for (i = 0; i < wazaCount; i++) {
        if (valueAry[banmeAry[i]] > maxValue) {
            tieAry[0] = i;
            maxValue = valueAry[banmeAry[i]];
            tieNum = 1;
        } else if (maxValue == valueAry[banmeAry[i]] && maxValue > comparisonValue) {
            tieAry[tieNum++] = i;
        }
    }

    index = tieAry[(u16)HSD_Rand() % tieNum];
    if (maxValue > comparisonValue) {
        return index;
    }
    return -1;
}
