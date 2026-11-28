#include <GSmsg.h>
#include <fightPokemon.h>
#include <fightFloor.h>
#include <fightPokemonBios.h>
#include <fightPokemonEnemy.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTarget.h>
#include <fightTrainer.h>
#include <pokemonDB.h>
#include <waza.h>

extern "C" {

void fightPokemonSetBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId, u32 buff)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return;
    }
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetBuff(pokemon, dataId, buff);
    }
}

u32 fightPokemonGetBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        // These statuses have no buff value in the fight record.
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiGetBuff(pokemon, dataId);
}

void fightPokemonCopyJoutaiDataId(FightPokemon *destination, FightPokemon *source, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonCopyJoutaiDataId(fightPokemonGetPokemonPtr(destination),
                               fightPokemonGetPokemonPtr(source), dataId);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiCopy(destination, source, dataId);
    }
}

void fightPokemonSetIdBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId, u16 idBuff)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return;
    }
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetIdBuff(pokemon, dataId, idBuff);
    }
}

u16 fightPokemonGetIdBuffJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        // These statuses have no buff value in the fight record.
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiGetIdBuff(pokemon, dataId);
}

u16 fightPokemonGetUserFightTargetIdJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        // These statuses have no buff value in the fight record.
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiGetUserFightTargetId(pokemon, dataId);
}

void fightPokemonSetKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId, s8 kaisuu)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonSetKaisuuJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId, kaisuu);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetKaisuuJoutaiDataId(pokemon, dataId, kaisuu);
    }
}

void fightPokemonSetNowKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId, s8 nowKaisuu)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonSetNowKaisuuJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId, nowKaisuu);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetNowKaisuuJoutaiDataId(pokemon, dataId, nowKaisuu);
    }
}

s8 fightPokemonGetNowKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonGetNowKaisuuJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return -1;
    }
    return joutaiGetNowKaisuuJoutaiDataId(pokemon, dataId);
}

s8 fightPokemonGetKaisuuJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonGetKaisuuJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return -1;
    }
    return joutaiGetKaisuuJoutaiDataId(pokemon, dataId);
}

void fightPokemonSetCountAsJoutaiDataId(FightPokemon *pokemon, u16 dataId, s16 count)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonSetCountAsJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId, count);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiSetCountAsJoutaiDataId(pokemon, dataId, count);
    }
}

s16 fightPokemonGetCountAsJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonGetCountAsJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return -1;
    }
    return joutaiGetCountAsJoutaiDataId(pokemon, dataId);
}

void fightPokemonWriteJoutaiDataId(FightPokemon *pokemon, u16 dataId, u16 targetId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonWriteJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId, targetId);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiWriteJoutaiDataId(pokemon, dataId, targetId);
    }
}

u8 fightPokemonCheckWriteJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonCheckWriteJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiCheckWriteJoutaiDataId(pokemon, dataId);
}

u8 fightPokemonIsJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (pokemon == 0) {
        return 0;
    }

    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        return pokemonIsJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    }
    if (joutaiDataBiosGetStatus(dataId) != POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        return 0;
    }
    return joutaiIsJoutaiDataId(pokemon, dataId);
}

void fightPokemonInitJoutaiDataId(FightPokemon *pokemon, u16 dataId)
{
    if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_CONDITION ||
        joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_REVERSE_MODE) {
        pokemonInitJoutaiDataId(fightPokemonGetPokemonPtr(pokemon), dataId);
    } else if (joutaiDataBiosGetStatus(dataId) == POKEMON_STATUS_FIELD_FIGHT_JOUTAI) {
        joutaiInitJoutaiDataId(pokemon, dataId);
    }
}

u8 fightOutPokemonIsAlly(FightOutPokemon *pokemon, FightOutPokemon *other)
{
    u16 fightType = fightFloor_GetFightType(0);
    void *pokemonSide;
    void *otherSide;

    if (pokemon == 0) {
        return 0;
    }
    if (other == 0) {
        return 0;
    }
    pokemonSide = fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE, pokemon, fightType);
    otherSide = fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE, other, fightType);
    return pokemonSide == otherSide;
}

void fightOutPokemonCreateFightPokemonEnemyAryEnemySideAll(FightOutPokemon *pokemon, FIGHT_SIDE *side)
{
    FIGHT_TRAINER *trainer;
    FightOutPokemon *target;
    u16 i;
    u16 j;
    u16 trainerNum;
    u16 pokemonNum;

    fightFloor_GetFightType(0);
    trainerNum = fightFloor_GetFightTypeTrainerNum(0);
    pokemonNum = fightFloor_GetFightTypeFightoutPokemonNum(0);
    for (i = 0; i < trainerNum; i++) {
        trainer = fightSide_GetFightTrainerPtr(side, i);
        if (fightTrainerCheckValid(trainer)) {
            for (j = 0; j < pokemonNum; j++) {
                target = fightTrainer_GetFightoutPokemonPtr(trainer, j);
                if (fightOutPokemonCheckFightOut(target)) {
                    fightOutPokemonCreateFightPokemonEnemyAry(pokemon, target);
                }
            }
        }
    }
}

void fightOutPokemonInitFightPokemonEnemyAry(FightOutPokemon *pokemon, FightOutPokemon *target)
{
    FightOutPokemonEnemy *enemy;
    u16 wazaDataId;

    if (pokemon == 0) {
        return;
    }
    enemy = fightOutPokemonEnemySearchAry(fightOutPokemon_GetFightOutPokemonEnemyPtr(pokemon, 0), 4,
                                         target);
    if (enemy == 0) {
        return;
    }
    wazaDataId = fightOutPokemonEnemyBiosGetOumuWazaDataId(enemy);
    fightOutPokemonEnemyInit(enemy);
    if (wazaDataId == WAZA_DATA_ID_NONE) {
        return;
    }
    if (wazaDataId == WAZA_SHADOW_SPECIAL) {
        return;
    }
    if (wazaDataId == WAZA_DATA_ID_INVALID) {
        return;
    }
    if (fightOutPokemon_GetOumuWazaDataId(pokemon) != WAZA_DATA_ID_NONE) {
        return;
    }
    if (fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry(pokemon, 0)) {
        return;
    }
    fightOutPokemon_SetOumuWazaDataId(pokemon, wazaDataId);
}

FightOutPokemonEnemy *fightOutPokemonCreateFightPokemonEnemyAry(FightOutPokemon *pokemon,
                                                                FightOutPokemon *target)
{
    FightOutPokemonEnemy *enemyAry;
    FightOutPokemonEnemy *enemy;
    u16 i;

    if (pokemon == 0) {
        return 0;
    }
    if (!fightOutPokemonCheckValid(target)) {
        return 0;
    }
    enemyAry = fightOutPokemon_GetFightOutPokemonEnemyPtr(pokemon, 0);
    enemy = fightOutPokemonEnemySearchAry(enemyAry, 4, target);
    if (enemy != 0) {
        return enemy;
    }
    for (i = 0; i < 4; i++) {
        enemy = &enemyAry[i];
        if (!fightOutPokemonEnemyCheckValid(enemy)) {
            fightOutPokemonEnemyCreate(enemy, target);
            return enemy;
        }
    }
    return 0;
}

void fightPokemonGetFriendFormPokemonFriendFilterId(FightPokemon *pokemon, u32 filterID)
{
    Pokemon *record = fightPokemonGetPokemonPtr(pokemon);
    u16 heldEffectID;

    if (record == 0) {
        return;
    }
    heldEffectID = fightPokemonGetSoubiItemSoubiDataId(pokemon);
    pokemonGetFriendFormPokemonFriendFilterId(record, heldEffectID, filterID);
}

void fightPokemonGetEffortFromPokemon(FightPokemon *pokemon, u8 pokerus, u16 speciesID)
{
    Pokemon *record = fightPokemonGetPokemonPtr(pokemon);
    u16 heldEffectID;

    if (record == 0) {
        return;
    }
    heldEffectID = fightPokemonGetSoubiItemSoubiDataId(pokemon);
    pokemonGetEffortFromPokemon(record, heldEffectID, pokerus, speciesID);
}

s32 fightPokemonGetLevelToExp(FightPokemon *pokemon, u8 level)
{
    Pokemon *record = fightPokemonGetPokemonPtr(pokemon);

    if (record == 0) {
        return 0;
    }
    return pokemonGetLevelToExp(record, level);
}

void figthPokemonSetExp(FightPokemon *pokemon, u32 exp)
{
    Pokemon *record = fightPokemonGetPokemonPtr(pokemon);

    if (record == 0) {
        return;
    }
    pokemon_SetExp(record, exp);
}

u32 figthPokemonGetExp(FightPokemon *pokemon)
{
    Pokemon *record = fightPokemonGetPokemonPtr(pokemon);

    if (record == 0) {
        return 0;
    }
    return pokemon_GetExp(record);
}

void fightPokemonGrowBasisStatus(FightPokemon *pokemon, u32 exp)
{
    Pokemon *record = fightPokemonGetPokemonPtr(pokemon);

    if (record == 0) {
        return;
    }
    pokemonGrowBasisStatus(record, exp);
}

u32 fightOutPokemonGetVoiceSndId(FightOutPokemon *pokemon)
{
    return pokemonDB_GetVoice(pokemon_GetPokemonDataId(fightOutPokemonGetPokemonPtr(pokemon)));
}

u16 *fightOutPokemonGetNamePtr(FightOutPokemon *pokemon)
{
    return fightPokemonGetNamePtr(fightOutPokemon_GetFightPokemonPtr(pokemon));
}

u16 *fightPokemonGetNamePtr(FightPokemon *pokemon)
{
    Pokemon *record = fightPokemonGetPokemonPtr(pokemon);

    if (record == 0) {
        return 0;
    }
    return GSmsgGetStringPtr(pokemonDB_GetName(pokemon_GetPokemonDataId(record)));
}

u16 *fightOutPokemonGetNicknamePtr(FightOutPokemon *pokemon)
{
    return fightPokemonGetNicknamePtr(fightOutPokemon_GetFightPokemonPtr(pokemon));
}

u16 *fightPokemonGetNicknamePtr(FightPokemon *pokemon)
{
    Pokemon *record = fightPokemonGetPokemonPtr(pokemon);

    if (record == 0) {
        return 0;
    }
    return pokemon_GetName(record);
}

} // extern "C"
