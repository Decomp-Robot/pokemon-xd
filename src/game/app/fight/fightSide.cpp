#include <FightResult.h>
#include <fightFloor.h>
#include <fightPokemonBios.h>
#include <fightSide.h>
#include <pokemonDB.h>

extern "C" {

void fightSideRegistFightSideEnemyPokemonFightAll(
    FIGHT_SIDE *side, FIGHT_TRAINER *enemyTrainer, u16 fightType,
    u16 trainerNum, u16 fightoutPokemonNum)
{
    FIGHT_TRAINER *trainer;
    FightOutPokemon *pokemon;
    u16 trainerIndex;
    u16 pokemonIndex;
    (void)fightType;

    for (trainerIndex = 0; trainerIndex < trainerNum; trainerIndex++) {
        trainer = fightSide_GetFightTrainerPtr(side, trainerIndex);

        if (fightTrainerCheckValid(trainer)) {
            for (pokemonIndex = 0; pokemonIndex < fightoutPokemonNum; pokemonIndex++) {
                pokemon = fightTrainer_GetFightoutPokemonPtr(trainer, pokemonIndex);

                if (fightOutPokemonCheckFightOut(pokemon)) {
                    s16 entryId = fightPokemon_GetEntryId(
                        fightOutPokemon_GetMotoFightPokemonPtr(pokemon));

                    if (entryId >= 0) {
                        fightTrainer_SetEnemyPokemonRegist(enemyTrainer, entryId);
                    }
                }
            }
        }
    }
}

u16 fightSideGetJoutaiUserFightTargetId(FIGHT_SIDE *side, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) != FIGHT_SIDE_STATUS_JOUTAI_PTR) {
        return 0;
    }
    return joutaiGetUserFightTargetId(side, dataId);
}

void fightSideSetNowKaisuuJoutaiDataId(FIGHT_SIDE *side, u16 dataId, u8 nowKaisuu)
{
    if (joutaiDataBiosGetStatus(dataId) == FIGHT_SIDE_STATUS_JOUTAI_PTR) {
        joutaiSetNowKaisuuJoutaiDataId(side, dataId, nowKaisuu);
    }
}

s32 fightSideGetNowKaisuuJoutaiDataId(FIGHT_SIDE *side, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) != FIGHT_SIDE_STATUS_JOUTAI_PTR) {
        return -1;
    }
    return joutaiGetNowKaisuuJoutaiDataId(side, dataId);
}

s32 fightSideGetKaisuuJoutaiDataId(FIGHT_SIDE *side, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) != FIGHT_SIDE_STATUS_JOUTAI_PTR) {
        return -1;
    }
    return joutaiGetKaisuuJoutaiDataId(side, dataId);
}

s32 fightSideGetCountAsJoutaiDataId(FIGHT_SIDE *side, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) != FIGHT_SIDE_STATUS_JOUTAI_PTR) {
        return -1;
    }
    return joutaiGetCountAsJoutaiDataId(side, dataId);
}

void fightSideWriteJoutaiDataId(void *side, u16 dataId, u16 targetId)
{
    if (joutaiDataBiosGetStatus(dataId) == FIGHT_SIDE_STATUS_JOUTAI_PTR) {
        joutaiWriteJoutaiDataId(side, dataId, targetId);
    }
}

u8 fightSideCheckWriteJoutaiDataId(void *side, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) != FIGHT_SIDE_STATUS_JOUTAI_PTR) {
        return 0;
    }
    return joutaiCheckWriteJoutaiDataId(side, dataId);
}

u8 fightSideIsJoutaiDataId(FIGHT_SIDE *side, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) != FIGHT_SIDE_STATUS_JOUTAI_PTR) {
        return 0;
    }
    return joutaiIsJoutaiDataId(side, dataId);
}

void fightSideInitJoutaiDataId(FIGHT_SIDE *side, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == FIGHT_SIDE_STATUS_JOUTAI_PTR) {
        joutaiInitJoutaiDataId(side, dataId);
    }
}

u32 fightSideGetHikaeFightPokemonNum(
    FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum, u16 fightoutPokemonNum)
{
    FIGHT_TRAINER *trainer;
    u16 trainerIndex;
    u32 total = 0;

    for (trainerIndex = 0; trainerIndex < trainerNum; trainerIndex++) {
        trainer = fightSide_GetFightTrainerPtr(side, trainerIndex);

        if (fightTrainerCheckValid(trainer)) {
            total += fightTrainerGetHikaeFightPokemonNum(
                trainer, pokemonNum, fightoutPokemonNum);
        }
    }
    return total;
}

u32 fightSideGetNokoriFightPokemonNum(
    FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum)
{
    return fightSideGetFightPokemonNum(side, trainerNum, pokemonNum, 1);
}

u32 fightSideGetFightPokemonNum(
    FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum, u8 fightOutOnly)
{
    FIGHT_TRAINER *trainer;
    u16 trainerIndex;
    u16 pokemonIndex;
    u32 total = 0;

    for (trainerIndex = 0; trainerIndex < trainerNum; trainerIndex++) {
        trainer = fightSide_GetFightTrainerPtr(side, trainerIndex);

        if (fightTrainerCheckValid(trainer)) {
            for (pokemonIndex = 0; pokemonIndex < pokemonNum; pokemonIndex++) {
                FightPokemon *pokemon = fightTrainerGetValidFightPokemonPtr(
                    trainer, pokemonIndex);

                if (pokemon != 0 &&
                    (fightOutOnly != 1 || fightPokemonCheckFightOut(pokemon))) {
                    total++;
                }
            }
        }
    }
    return total;
}

u32 fightSideGetFightPokemonMaxHp(FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum)
{
    FIGHT_TRAINER *trainer;
    FightPokemon *fightPokemon;
    Pokemon *pokemon;
    u16 trainerIndex;
    u16 pokemonIndex;
    u32 total = 0;

    for (trainerIndex = 0; trainerIndex < trainerNum; trainerIndex++) {
        trainer = fightSide_GetFightTrainerPtr(side, trainerIndex);

        if (fightTrainerCheckValid(trainer)) {
            for (pokemonIndex = 0; pokemonIndex < pokemonNum; pokemonIndex++) {
                fightPokemon = fightTrainerGetValidFightPokemonPtr(trainer, pokemonIndex);

                if (fightPokemon != 0) {
                    pokemon = fightPokemonGetPokemonPtr(fightPokemon);

                    if (pokemon != 0 && fightPokemonCheckFightOut(fightPokemon)) {
                        total += pokemon_GetMaxHp(pokemon);
                    }
                }
            }
        }
    }
    return total;
}

u32 fightSideGetFightPokemonNokoriHp(FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum)
{
    FIGHT_TRAINER *trainer;
    FightPokemon *fightPokemon;
    Pokemon *pokemon;
    u16 trainerIndex;
    u16 pokemonIndex;
    u32 total = 0;

    for (trainerIndex = 0; trainerIndex < trainerNum; trainerIndex++) {
        trainer = fightSide_GetFightTrainerPtr(side, trainerIndex);

        if (fightTrainerCheckValid(trainer)) {
            for (pokemonIndex = 0; pokemonIndex < pokemonNum; pokemonIndex++) {
                fightPokemon = fightTrainerGetValidFightPokemonPtr(trainer, pokemonIndex);

                if (fightPokemon != 0) {
                    pokemon = fightPokemonGetPokemonPtr(fightPokemon);

                    if (pokemon != 0 && fightPokemonCheckFightOut(fightPokemon)) {
                        total += pokemon_GetHp(pokemon);
                    }
                }
            }
        }
    }
    return total;
}

FIGHT_TRAINER *fightSideGetValidFightTrainerPtr(FIGHT_SIDE *side, u16 index)
{
    FIGHT_TRAINER *trainer = fightSide_GetFightTrainerPtr(side, index);

    if (fightTrainerCheckValid(trainer) == 0) {
        return 0;
    } else {
        return trainer;
    }
}

void fightSideGetFightTrainerGridParam(
    u16 sideDataId, u16 trainerNum, u16 trainerIndex, u8 *reverse, s8 *offset)
{
    if (reverse == 0) {
        return;
    }
    if (offset == 0) {
        return;
    }

    *reverse = (u8)(fightSideDB_GetYrot(sideDataId) != 0);
    *offset = 0;

    if (trainerNum == 1) {
        return;
    } else if (trainerNum == 2) {
        if (*reverse == 1) {
            if (trainerIndex == 0) {
                *offset = 1;
                return;
            }
            if (trainerIndex == 1) {
                *offset = -1;
            } else {
                return;
            }
        } else {
            if (trainerIndex == 0) {
                *offset = -1;
            } else if (trainerIndex == 1) {
                *offset = 1;
            }
        }
    }
}

u32 fightSideGetDoFightTrainerCount(FIGHT_SIDE *side)
{
    u8 trainerIndex;
    u32 count = 0;

    for (trainerIndex = 0; trainerIndex < FIGHT_SIDE_TRAINER_COUNT; trainerIndex++) {
        if (fightTrainerCheckDoFight(fightSide_GetFightTrainerPtr(side, trainerIndex))) {
            count++;
        }
    }
    return count;
}

u8 fightSideCheckValid(FIGHT_SIDE *side)
{
    if (side == 0) {
        return 0;
    }
    if (fightGetFightResultId() == kFightResultCommunicationError) {
        return 0;
    }
    if (fightSide_GetFightSideDataId(side) == 0) {
        return 0;
    }
    return 1;
}

void fightSideCreate(FIGHT_SIDE *side, u16 dataId)
{
    if (side != 0) {
        fightSideInit(side);
        fightSide_SetFightSideDataId(side, dataId);
    }
}

void fightSideInitAry(FIGHT_SIDE *sideAry, u16 sideNum)
{
    u16 index;

    if (sideAry != 0) {
        for (index = 0; index < sideNum; index++) {
            fightSideInit(&sideAry[index]);
        }
    }
}

void fightSideInit(FIGHT_SIDE *side)
{
    if (side != 0) {
        fightSide_SetFightSideDataId(side, 0);
        joutaiInitAry(fightSide_GetJoutaiPtr(side, 0), FIGHT_SIDE_JOUTAI_COUNT);
        fightTrainerInitAry(
            fightSide_GetFightTrainerPtr(side, 0), FIGHT_SIDE_TRAINER_COUNT);
        fightSide_SetMakibisiCheckFlag(side, 0);
    }
}

void fightSideSetStatus(
    FIGHT_SIDE *side, u16 dataId, u16 selector, u16 index, u32 value)
{
    if (selector == 0 || selector >= FIGHT_SIDE_STATUS_SELECTOR_LIMIT) {
        return;
    }

    void *target = side;
    if (selector < FIGHT_SIDE_STATUS_RESERVED) {
        target = fightSideDataBiosGetPtr(dataId);
    }
    if (target == 0) {
        return;
    }

    switch (selector) {
    case FIGHT_SIDE_STATUS_NONE:
        break;
    case FIGHT_SIDE_STATUS_YROT:
        fightSideDataBiosSetYrot((FIGHT_SIDE_DATA *)target, (u16)value);
        break;
    case FIGHT_SIDE_STATUS_FIGHT_TRAINER_STATUS_MENU_DATA_ID:
        fightSideDataBiosSetFightTrainerStatusMenuDataId(
            (FIGHT_SIDE_DATA *)target, (u8)index, value);
        break;
    case FIGHT_SIDE_STATUS_FIGHTOUT_POKEMON_STATUS_MENU_DATA_ID:
        fightSideDataBiosSetFightoutPokemonStatusMenuDataId(
            (FIGHT_SIDE_DATA *)target, (u8)index, value);
        break;
    case FIGHT_SIDE_STATUS_RESERVED:
        break;
    case FIGHT_SIDE_STATUS_SIDE_DATA_ID:
        fightSideBiosSetFightSideDataId((FIGHT_SIDE *)target, (u16)value);
        break;
    case FIGHT_SIDE_STATUS_JOUTAI_PTR:
        break;
    case FIGHT_SIDE_STATUS_FIGHT_TRAINER_PTR:
        break;
    case FIGHT_SIDE_STATUS_MAKIBISI_CHECK_FLAG:
        fightSideBiosSetMakibisiCheckFlag((FIGHT_SIDE *)target, (u8)(value != 0));
        break;
    default:
        break;
    }
}

u32 fightSideGetStatus(FIGHT_SIDE *side, u16 dataId, u16 selector, u16 index)
{
    if (selector == 0 || selector >= FIGHT_SIDE_STATUS_SELECTOR_LIMIT) {
        return 0;
    }

    void *target = side;
    if (selector < FIGHT_SIDE_STATUS_RESERVED) {
        target = fightSideDataBiosGetPtr(dataId);
    }
    if (target == 0) {
        return 0;
    }

    switch (selector) {
    case FIGHT_SIDE_STATUS_NONE:
        break;
    case FIGHT_SIDE_STATUS_YROT:
        return fightSideDataBiosGetYrot((FIGHT_SIDE_DATA *)target);
    case FIGHT_SIDE_STATUS_FIGHT_TRAINER_STATUS_MENU_DATA_ID:
        return fightSideDataBiosGetFightTrainerStatusMenuDataId(
            (FIGHT_SIDE_DATA *)target, (u8)index);
    case FIGHT_SIDE_STATUS_FIGHTOUT_POKEMON_STATUS_MENU_DATA_ID:
        return fightSideDataBiosGetFightoutPokemonStatusMenuDataId(
            (FIGHT_SIDE_DATA *)target, (u8)index);
    case FIGHT_SIDE_STATUS_RESERVED:
        break;
    case FIGHT_SIDE_STATUS_SIDE_DATA_ID:
        return fightSideBiosGetFightSideDataId((FIGHT_SIDE *)target);
    case FIGHT_SIDE_STATUS_JOUTAI_PTR:
        return (u32)fightSideBiosGetJoutaiPtr((FIGHT_SIDE *)target, index);
    case FIGHT_SIDE_STATUS_FIGHT_TRAINER_PTR:
        return (u32)fightSideBiosGetFightTrainerPtr((FIGHT_SIDE *)target, index);
    case FIGHT_SIDE_STATUS_MAKIBISI_CHECK_FLAG:
        return fightSideBiosGetMakibisiCheckFlag((FIGHT_SIDE *)target);
    default:
        break;
    }
    return 0;
}

}
