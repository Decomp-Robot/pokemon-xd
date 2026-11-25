#include <fightFloor.h>
#include <fightEncount.h>
#include <fightEncountBios.h>
#include <fightKindDataBios.h>
#include <fightMenu.h>
#include <fight.h>
#include <fightSeq.h>
#include <fightSide.h>
#include <fightTarget.h>
#include <app/fight/fightTrainer.h>
#include <app/fight/fightType.h>
#include <GSmsg.h>
#include <Pokemon.h>
#include <msgctrl.h>
#include <pokemonBios.h>
#include <itemData.h>
#include <pokemonDB.h>

u32 encount_floor_id;
FIGHT_FLOOR fight_floor;

s16 _fightFloorSetFightOutPokemonPtrAry(FightOutPokemon **pokemonArray, FightOutPokemon *pokemon);
void _fightFloorInitFightOutPokemonPtrAry(FightOutPokemon **pokemonArray);

void fightFloorInitFightTarget(FIGHT_FLOOR *floor)
{
    u16 targetReset[13] = {
        FIGHT_FLOOR_STATUS_ATTACK_POKEMON_PTR,
        FIGHT_FLOOR_STATUS_DEFENSE_POKEMON_PTR,
        FIGHT_FLOOR_STATUS_ESCAPE_POKEMON_PTR,
        FIGHT_FLOOR_STATUS_IREKAE_POKEMON_PTR,
        FIGHT_FLOOR_STATUS_KIZETU_POKEMON_PTR,
        FIGHT_FLOOR_STATUS_TUIKAKOUKA_POKEMON_PTR,
        FIGHT_FLOOR_STATUS_TOKUSEI_POKEMON_PTR,
        FIGHT_FLOOR_STATUS_ITEM_POKEMON_PTR,
        FIGHT_FLOOR_STATUS_APPOINT_POKEMON_PTR,
        FIGHT_FLOOR_STATUS_APPOINT_TRAINER_PTR,
        FIGHT_FLOOR_STATUS_APPOINT_WAZA_PTR,
        FIGHT_FLOOR_STATUS_APPOINT_SIDE_PTR,
        FIGHT_FLOOR_STATUS_APPOINT_ITEM_PTR
    };

    for (u16 i = 0; i < 13; i++) {
        fightFloorSetStatus(floor, 0, targetReset[i], 0, 0);
    }
    fightFloor_SetAppointMsgPtr(floor, 0);
    fightFloor_SetAttackMsgId(floor, 0);
    fightFloor_SetCriticakMsgId(floor, 0);
    fightFloor_SetWazakoukaMsgId(floor, 0);
    fightFloor_SetAppointPokemonDataId(floor, 0);
    fightFloor_SetAppointPokemonWazaDataId(floor, 0);
    fightFloor_SetAppointItemDataId(floor, 0);
    fightFloor_SetAppointTokuseiDataId(floor, 0);
}

u32 fightFloorSetStatus(FIGHT_FLOOR *floor, u16 floorDataId, u16 status, u16 index, u32 value)
{
    u8 result = 0;
    u8 fightType = (u8)fightFloor_GetFightType(floor);

    if (status == FIGHT_FLOOR_STATUS_FIGHT_FLOOR_PTR || status >= FIGHT_FLOOR_STATUS_SELECTOR_LIMIT) {
        return 0;
    }
    if (status < FIGHT_FLOOR_STATUS_FIGHT_FLOOR_DATA_LIMIT) {
        floor = (FIGHT_FLOOR *)fightFloorDataBiosGetPtr(floorDataId);
        if (floor == 0) {
            return 0;
        }
    } else if (status < FIGHT_FLOOR_STATUS_RESERVED && floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    switch (status) {
    case FIGHT_FLOOR_STATUS_NAME:
        fightFloorDataBiosSetName((FIGHT_FLOOR_DATA *)floor, value);
        break;
    case FIGHT_FLOOR_STATUS_FLOOR_DATA_ID:
        fightFloorDataBiosSetFloorDataId((FIGHT_FLOOR_DATA *)floor, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_FIGHT_SIDE_DATA_ID:
        fightFloorDataBiosSetFightSideDataId((FIGHT_FLOOR_DATA *)floor, (u8)index, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_TIKEI_DATA_ID:
        fightFloorDataBiosSetTikeiDataId((FIGHT_FLOOR_DATA *)floor, (u8)value);
        break;
    case FIGHT_FLOOR_STATUS_SYOUKAI_WZX_DATA_ID:
        fightFloorDataBiosSetSyoukaiWzxDataId((FIGHT_FLOOR_DATA *)floor, value);
        break;
    case FIGHT_FLOOR_STATUS_BGM_SND_ID:
        fightFloorDataBiosSetBgmSndId((FIGHT_FLOOR_DATA *)floor, value);
        break;
    case FIGHT_FLOOR_STATUS_ENV_SND_ID:
        fightFloorDataBiosSetEnvSndId((FIGHT_FLOOR_DATA *)floor, value);
        break;
    case FIGHT_FLOOR_STATUS_JOUTAI_DATA_ID:
        result = fightFloorCheckWriteJoutaiDataId(floor, (u16)value);
        if (result == 2) {
            fightFloorWriteJoutaiDataId(floor, (u16)value, index);
        }
        break;
    case FIGHT_FLOOR_STATUS_TURN_COUNT:
        fightFloorBiosSetTurnCount(floor, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_ENCOUNT_DATA_ID:
        fightFloorBiosSetEncountDataId(floor, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_ATTACK_POKEMON_PTR:
        if (value != 0) {
            if (fightOutPokemonCheckValid((FightOutPokemon *)value) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON, value);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_ABILITY_NAME, (unsigned long)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(fightOutPokemonGetTokuseiDataId((FightOutPokemon *)value)))));
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON_SECOND, value);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON_FOURTH, value);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON_THIRD, value);
            } else {
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_ABILITY_NAME, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON_SECOND, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON_FOURTH, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON_THIRD, 0);
            }
        } else {
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_ABILITY_NAME, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON_SECOND, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON_FOURTH, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_ATTACK_POKEMON_THIRD, 0);
        }
        fightFloorBiosSetAttackPokemonPtr(floor, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_USE_WAZA_DATA_ID:
        fightOutPokemon_SetUseWazaDataId(fightFloorBiosGetAttackPokemonPtr(floor), (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_MOTO_WAZA_DATA_ID:
        fightOutPokemon_SetMotoWazaDataId(fightFloorBiosGetAttackPokemonPtr(floor), (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_WAZA_DAMAGE_VALUE:
        fightOutPokemon_SetWazaDamageValue(fightFloorBiosGetAttackPokemonPtr(floor), (u8)value);
        break;
    case FIGHT_FLOOR_STATUS_WAZA_CRITICAL:
        fightOutPokemon_SetWazaCritical(fightFloorBiosGetAttackPokemonPtr(floor), (u8)value);
        break;
    case FIGHT_FLOOR_STATUS_WAZA_JOUTAI_DATA_ID:
        fightOutPokemon_SetWazaJoutaiDataId(fightFloorBiosGetAttackPokemonPtr(floor), (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_WAZA_INIT_JOUTAI_DATA_ID:
        fightOutPokemon_SetWazaInitJoutaiDataId(fightFloorBiosGetAttackPokemonPtr(floor), (u8)value);
        break;
    case FIGHT_FLOOR_STATUS_IS_HIT:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(floor), 0, POKEMON_STATUS_FIELD_IS_HIT, 0, value);
        break;
    case FIGHT_FLOOR_STATUS_WAZA_DAMAGE:
        fightOutPokemon_SetWazaDamage(fightFloorBiosGetAttackPokemonPtr(floor), value);
        break;
    case FIGHT_FLOOR_STATUS_WAZA_IRYOKU:
        fightOutPokemon_SetWazaIryoku(fightFloorBiosGetAttackPokemonPtr(floor), (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_WAZA_TARGET_ID:
        fightOutPokemon_SetWazaTargetId(fightFloorBiosGetAttackPokemonPtr(floor), (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_WAZA_HIT_DAMAGE:
        fightOutPokemon_SetWazaHitDamage(fightFloorBiosGetAttackPokemonPtr(floor), value);
        break;
    case FIGHT_FLOOR_STATUS_DEFENSE_POKEMON_PTR:
        if (value != 0) {
            if (fightOutPokemonCheckValid((FightOutPokemon *)value) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON, value);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_ABILITY_NAME, (unsigned long)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(fightOutPokemonGetTokuseiDataId((FightOutPokemon *)value)))));
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON_SECOND, value);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON_FOURTH, value);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON_THIRD, value);
            } else {
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_ABILITY_NAME, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON_SECOND, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON_FOURTH, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON_THIRD, 0);
            }
        } else {
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_ABILITY_NAME, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON_SECOND, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON_FOURTH, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_DEFENSE_POKEMON_THIRD, 0);
        }
        fightFloorBiosSetDefensePokemonPtr(floor, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_DEFENSE_POKEMON_PTR_AND_WAZA_TARGET:
        fightFloor_SetDefensePokemonPtr(floor, (FightOutPokemon *)value);
        fightFloor_SetAttackPokemonFightWazaTargetId(floor, fightTargetGetTragetPtrToRelativeHostSideFightTargetId((void *)value, fightType));
        break;
    case FIGHT_FLOOR_STATUS_ESCAPE_POKEMON_PTR:
        fightFloorBiosSetEscapePokemonPtr(floor, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_IREKAE_POKEMON_PTR:
        fightFloorBiosSetIrekaePokemonPtr(floor, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_KIZETU_POKEMON_PTR:
        fightFloorBiosSetKizetuPokemonPtr(floor, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_TUIKAKOUKA_POKEMON_PTR:
        if (value != 0) {
            if (fightOutPokemonCheckValid((FightOutPokemon *)value) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_TUIKAKOUKA_POKEMON, value);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_TUIKAKOUKA_ABILITY_NAME, (unsigned long)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(fightOutPokemonGetTokuseiDataId((FightOutPokemon *)value)))));
            } else {
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_TUIKAKOUKA_POKEMON, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_TUIKAKOUKA_ABILITY_NAME, 0);
            }
        } else {
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_TUIKAKOUKA_POKEMON, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_TUIKAKOUKA_ABILITY_NAME, 0);
        }
        fightFloorBiosSetTuikakoukaPokemonPtr(floor, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_TOKUSEI_POKEMON_PTR:
        fightFloorBiosSetTokuseiPokemonPtr(floor, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_ITEM_POKEMON_PTR:
        if (value != 0 && fightOutPokemonCheckValid((FightOutPokemon *)value) == 1) {
            fightFloor_SetAppointItemDataId(floor, fightOutPokemonGetSoubiItemDataId((FightOutPokemon *)value));
        }
        fightFloorBiosSetItemPokemonPtr(floor, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_ENCOUNT_FLOOR_ID:
        fightFloorBiosSetEncountFloorId(value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_POKEMON_PTR:
        if (value != 0) {
            if (fightOutPokemonCheckValid((FightOutPokemon *)value) == 1) {
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_POKEMON, value);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_ABILITY_NAME, (unsigned long)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr(fightOutPokemonGetTokuseiDataId((FightOutPokemon *)value)))));
            } else {
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_POKEMON, 0);
                fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_ABILITY_NAME, 0);
            }
        } else {
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_POKEMON, 0);
            fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_ABILITY_NAME, 0);
        }
        fightFloorBiosSetAppointPokemonPtr(floor, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_TRAINER_PTR:
        fightFloorBiosSetAppointTrainerPtr(floor, (FIGHT_TRAINER *)value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_WAZA_PTR:
        fightFloorBiosSetAppointWazaPtr(floor, (FIGHT_WAZA *)value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_SIDE_PTR:
        fightFloorBiosSetAppointSidePtr(floor, (FIGHT_SIDE *)value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_ITEM_PTR:
        fightFloorBiosSetAppointItemPtr(floor, (void *)value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_MSG_ID:
        fightFloorBiosSetAppointMsgId(floor, value);
        break;
    case FIGHT_FLOOR_STATUS_ATTACK_MSG_ID:
        fightFloorBiosSetAttackMsgId(floor, value);
        break;
    case FIGHT_FLOOR_STATUS_CRITICAL_MSG_ID:
        fightFloorBiosSetCriticalMsgId(floor, value);
        break;
    case FIGHT_FLOOR_STATUS_WAZAKOUKA_MSG_ID:
        fightFloorBiosSetWazakoukaMsgId(floor, value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_POKEMON_DATA_ID:
        fightFloorBiosSetAppointPokemonDataId(floor, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_WAZA_DATA_ID:
        fightFloorBiosSetAppointWazaDataId(floor, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_ITEM_DATA_ID:
        fightMsgctrlSetValue(FIGHT_MSG_SLOT_ITEM_NAME, (unsigned long)GSmsgGetStringPtr(itemDB_GetName((u16)value)));
        fightFloorBiosSetAppointItemDataId(floor, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_APPOINT_TOKUSEI_DATA_ID:
        fightMsgctrlSetValue(FIGHT_MSG_SLOT_BLOCKING_ABILITY_NAME, (unsigned long)GSmsgGetStringPtr(pokemonTokuseiDataBiosGetName(pokemonTokuseiDataBiosGetPtr((u16)value))));
        fightFloorBiosSetAppointTokuseiDataId(floor, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_FIGHT_POKEMON_ENTRY_CNT:
        fightFloorBiosSetFightPokemonEntryCnt(floor, (s16)value);
        break;
    case FIGHT_FLOOR_STATUS_FIGHT_OUT_POKEMON_PTR:
        fightFloorBiosSetFightOutPokemonPtrAry(floor, index, (FightOutPokemon *)value);
        break;
    case FIGHT_FLOOR_STATUS_FIGHT_OUT_POKEMON_PTR_ARY_PTR:
        fightFloorBiosSetFightOutPokemonPtrAryPtr(floor, (FightOutPokemon **)value);
        break;
    case FIGHT_FLOOR_STATUS_FIRST_ATTACK_RND:
        fightFloorBiosSetFirstAttackRnd(floor, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_FIGHT_RESULT_ID:
        fightFloorSetFightResultId(floor, (u16)value);
        break;
    case FIGHT_FLOOR_STATUS_ITEM_QUANTITY_MSG_VALUE:
        fightMsgctrlSetValue(MSGCTRL_VALUE_ITEM_QUANTITY, value);
        break;
    }
    return result;
}

u32 fightFloorGetStatus(FIGHT_FLOOR *floor, u16 dataId, u16 selector, u16 index)
{
    if (selector >= FIGHT_FLOOR_STATUS_SELECTOR_LIMIT) {
        return 0;
    }
    if (selector == FIGHT_FLOOR_STATUS_FIGHT_FLOOR_PTR) {
        return (u32)fightFloorBiosGetFightFloorPtr();
    }
    if (selector < FIGHT_FLOOR_STATUS_FIGHT_FLOOR_DATA_LIMIT) {
        floor = (FIGHT_FLOOR *)fightFloorDataBiosGetPtr(dataId);
        if (floor == 0) {
            return 0;
        }
    } else if (selector < FIGHT_FLOOR_STATUS_RESERVED && floor == 0) {
        floor = (FIGHT_FLOOR *)fightFloorBiosGetFightFloorPtr();
        if (floor == 0) {
            return 0;
        }
    }
    switch (selector) {
    case FIGHT_FLOOR_STATUS_NAME:
        return (u32)fightFloorDataBiosGetName((FIGHT_FLOOR_DATA *)floor);
    case FIGHT_FLOOR_STATUS_FLOOR_DATA_ID:
        return fightFloorDataBiosGetFloorDataId((FIGHT_FLOOR_DATA *)floor);
    case FIGHT_FLOOR_STATUS_FIGHT_SIDE_DATA_ID:
        return fightFloorDataBiosGetFightSideDataId((FIGHT_FLOOR_DATA *)floor, (u8)index);
    case FIGHT_FLOOR_STATUS_TIKEI_DATA_ID:
        return fightFloorDataBiosGetTikeiDataId((FIGHT_FLOOR_DATA *)floor);
    case FIGHT_FLOOR_STATUS_SYOUKAI_WZX_DATA_ID:
        return fightFloorDataBiosGetSyoukaiWzxDataId((FIGHT_FLOOR_DATA *)floor);
    case FIGHT_FLOOR_STATUS_BGM_SND_ID:
        return fightFloorDataBiosGetBgmSndId((FIGHT_FLOOR_DATA *)floor);
    case FIGHT_FLOOR_STATUS_ENV_SND_ID:
        return fightFloorDataBiosGetEnvSndId((FIGHT_FLOOR_DATA *)floor);
    case FIGHT_FLOOR_STATUS_JOUTAI_PTR:
        return (u32)fightFloorBiosGetJoutaiPtr(floor, index);
    case FIGHT_FLOOR_STATUS_JOUTAI_DATA_ID:
        return fightFloorIsJoutaiDataId(floor, index);
    case FIGHT_FLOOR_STATUS_CHECK_WRITE_JOUTAI_DATA_ID:
        return fightFloorCheckWriteJoutaiDataId(floor, index);
    case FIGHT_FLOOR_STATUS_TURN_COUNT:
        return fightFloorBiosGetTurnCount(floor);
    case FIGHT_FLOOR_STATUS_ENCOUNT_DATA_ID:
        return fightFloorBiosGetEncountDataId(floor);
    case FIGHT_FLOOR_STATUS_ENCOUNT_FLOOR_DATA_ID:
        return fightEncountDataBiosGetFightFloorDataId(fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(floor)));
    case FIGHT_FLOOR_STATUS_NOW_TIKEI_DATA_ID:
        return fightFloorDB_GetTikeiDataId(fightFloor_GetFightFloorDataId(floor));
    case FIGHT_FLOOR_STATUS_NOW_SYOUKAI_WZX_DATA_ID:
        return fightFloorDB_GetSyoukaiWzxDataId(fightFloor_GetFightFloorDataId(floor));
    case FIGHT_FLOOR_STATUS_NOW_BGM_SND_DATA_ID:
        return fightFloorGetBgmSndDataId(floor);
    case FIGHT_FLOOR_STATUS_NOW_ENV_SND_DATA_ID:
        return fightEncountGetEnvSndDataId(fightFloor_GetEncountDataId(floor));
    case FIGHT_FLOOR_STATUS_NOW_TENKOU_DATA_ID:
        return fightFloorGetNowTenkouDataId(floor, index != 0);
    case FIGHT_FLOOR_STATUS_FIGHT_TYPE:
        return fightEncountDataBiosGetFightType(fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_TYPE_NAME:
        return fightTypeDataBiosGetName(fightTypeDataBiosGetPtr(fightFloor_GetFightType(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_TYPE_TRAINER_NUM:
        return fightTypeDataBiosGetTrainerNum(fightTypeDataBiosGetPtr(fightFloor_GetFightType(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_TYPE_ENTRY_POKEMON_NUM:
        return fightTypeDataBiosGetEntryPokemonNum(fightTypeDataBiosGetPtr(fightFloor_GetFightType(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_TYPE_FIGHTOUT_POKEMON_NUM:
        return fightTypeDataBiosGetFightoutPokemonNum(fightTypeDataBiosGetPtr(fightFloor_GetFightType(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_SIDE_FIGHTOUT_POKEMON_MAX:
        return fightTypeGetFightSideFightOutPokemonMax(fightFloor_GetFightType(floor));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND:
        return (u8)fightEncountDataBiosGetFightKind(fightEncountDataBiosGetPtr(fightFloor_GetEncountDataId(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_NAME:
        return fightKindDataBiosGetName(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_BACK_SAVE_DATA_FLAG:
        return fightKindDataBiosGetBackSaveDataFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_BADGE_CHECK_FLAG:
        return fightKindDataBiosGetDoBadgeCheckFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_ZUKAN_MITA_FLAG:
        return fightKindDataBiosGetDoZukanMitaFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_ZUKAN_TUKAMAETA_FLAG:
        return fightKindDataBiosGetDoZukanTukamaetaFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_USE_ITEM_FLAG:
        return fightKindDataBiosGetUseItemFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_CALL_FLAG:
        return fightKindDataBiosGetCallFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_NIGERU_FLAG:
        return fightKindDataBiosGetNigeruFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DRAW_FLAG:
        return fightKindDataBiosGetDrawFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_GET_EXP_FLAG:
        return fightKindDataBiosGetGetExpFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_OKANE_POOL_FLAG:
        return fightKindDataBiosGetOkanePoolFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_GET_OKANE_FLAG:
        return fightKindDataBiosGetGetOkaneFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_GET_FRIEND_FLAG:
        return fightKindDataBiosGetGetFriendFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_GET_INFECT_POKERUS_FLAG:
        return fightKindDataBiosGetGetInfectPokerusFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_CRITICAL_ATTACK_FLAG:
        return fightKindDataBiosGetDoCriticalAttackFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_HIZUKI_AI_FLAG:
        return fightKindDataBiosGetDoHizukiAiFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_HIZUKI_MIYABURI_FLAG:
        return fightKindDataBiosGetDoHizukiMiyaburiFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_ITEM_SOUBI_TOKUKOUTOKUBOUP_FLAG:
        return fightKindDataBiosGetDoItemSoubiTokukoutokubouupFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_KEIKENTIHUERU_FLAG:
        return fightKindDataBiosGetKeikentihueruFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_BOSS_FLAG:
        return fightKindDataBiosGetBossFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DOROBOU_FLAG:
        return fightKindDataBiosGetDorobouFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_MONOHIROI_FLAG:
        return fightKindDataBiosGetMonohiroiFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_DARKPOKEMON_HYPERMODE_FLAG:
        return fightKindDataBiosGetDarkpokemonHypermodeFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_POKEMON_STATUS_MENU_SUBBAR_FLAG:
        return fightKindDataBiosGetPokemonStatusMenuSubbarFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_HOST_ENEMY_MSG_FLAG:
        return fightKindDataBiosGetHostEnemyMsgFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_KIND_REGULATION_FLAG:
        return fightKindDataBiosGetRegulationFlag(fightKindDataBiosGetPtr((u8)fightFloor_GetFightKind(floor)));
    case FIGHT_FLOOR_STATUS_FIGHT_SIDE_PTR:
        return (u32)fightFloorBiosGetFightSidePtr(floor, index);
    case FIGHT_FLOOR_STATUS_ATTACK_POKEMON_PTR:
        return (u32)fightFloorBiosGetAttackPokemonPtr(floor);
    case FIGHT_FLOOR_STATUS_USE_WAZA_DATA_ID:
        return fightOutPokemonGetUseWazaDataId(fightFloorBiosGetAttackPokemonPtr(floor));
    case FIGHT_FLOOR_STATUS_MOTO_WAZA_DATA_ID:
        return fightOutPokemonGetMotoWazaDataId(fightFloorBiosGetAttackPokemonPtr(floor));
    case FIGHT_FLOOR_STATUS_WAZA_DAMAGE_VALUE:
        return fightOutPokemon_GetDamageValue(fightFloorBiosGetAttackPokemonPtr(floor));
    case FIGHT_FLOOR_STATUS_WAZA_CRITICAL:
        return fightOutPokemon_GetCritical(fightFloorBiosGetAttackPokemonPtr(floor));
    case FIGHT_FLOOR_STATUS_WAZA_JOUTAI_DATA_ID:
        return fightOutPokemon_GetJoutaiDataId(fightFloorBiosGetAttackPokemonPtr(floor), index);
    case FIGHT_FLOOR_STATUS_IS_HIT:
        return fightOutPokemon_GetIsHit(fightFloorBiosGetAttackPokemonPtr(floor));
    case FIGHT_FLOOR_STATUS_WAZA_DAMAGE:
        return fightOutPokemon_GetDamage(fightFloorBiosGetAttackPokemonPtr(floor));
    case FIGHT_FLOOR_STATUS_WAZA_IRYOKU:
        return fightOutPokemon_GetIryoku(fightFloorBiosGetAttackPokemonPtr(floor));
    case FIGHT_FLOOR_STATUS_WAZA_TARGET_ID:
        return fightOutPokemon_GetTargetId(fightFloorBiosGetAttackPokemonPtr(floor));
    case FIGHT_FLOOR_STATUS_WAZA_HIT_DAMAGE:
        return fightOutPokemon_GetHitDamage(fightFloorBiosGetAttackPokemonPtr(floor));
    case FIGHT_FLOOR_STATUS_DEFENSE_POKEMON_PTR:
        return (u32)fightFloorBiosGetDefensePokemonPtr(floor);
    case FIGHT_FLOOR_STATUS_ESCAPE_POKEMON_PTR:
        return (u32)fightFloorBiosGetEscapePokemonPtr(floor);
    case FIGHT_FLOOR_STATUS_IREKAE_POKEMON_PTR:
        return (u32)fightFloorBiosGetIrekaePokemonPtr(floor);
    case FIGHT_FLOOR_STATUS_KIZETU_POKEMON_PTR:
        return (u32)fightFloorBiosGetKizetuPokemonPtr(floor);
    case FIGHT_FLOOR_STATUS_TUIKAKOUKA_POKEMON_PTR:
        return (u32)fightFloorBiosGetTuikakoukaPokemonPtr(floor);
    case FIGHT_FLOOR_STATUS_TOKUSEI_POKEMON_PTR:
        return (u32)fightFloorBiosGetTokuseiPokemonPtr(floor);
    case FIGHT_FLOOR_STATUS_ITEM_POKEMON_PTR:
        return (u32)fightFloorBiosGetItemPokemonPtr(floor);
    case FIGHT_FLOOR_STATUS_ENCOUNT_FLOOR_ID:
        return fightFloorBiosGetEncountFloorId();
    case FIGHT_FLOOR_STATUS_APPOINT_POKEMON_PTR:
        return (u32)fightFloorBiosGetAppointPokemonPtr(floor);
    case FIGHT_FLOOR_STATUS_APPOINT_TRAINER_PTR:
        return (u32)fightFloorBiosGetAppointTrainerPtr(floor);
    case FIGHT_FLOOR_STATUS_APPOINT_WAZA_PTR:
        return (u32)fightFloorBiosGetAppointWazaPtr(floor);
    case FIGHT_FLOOR_STATUS_APPOINT_SIDE_PTR:
        return (u32)fightFloorBiosGetAppointSidePtr(floor);
    case FIGHT_FLOOR_STATUS_APPOINT_ITEM_PTR:
        return (u32)fightFloorBiosGetAppointItemPtr(floor);
    case FIGHT_FLOOR_STATUS_APPOINT_MSG_ID:
        return fightFloorBiosGetAppointMsgId(floor);
    case FIGHT_FLOOR_STATUS_ATTACK_MSG_ID:
        return fightFloorBiosGetAttackMsgId(floor);
    case FIGHT_FLOOR_STATUS_CRITICAL_MSG_ID:
        return fightFloorBiosGetCriticalMsgId(floor);
    case FIGHT_FLOOR_STATUS_WAZAKOUKA_MSG_ID:
        return fightFloorBiosGetWazakoukaMsgId(floor);
    case FIGHT_FLOOR_STATUS_APPOINT_POKEMON_DATA_ID:
        return fightFloorBiosGetAppointPokemonDataId(floor);
    case FIGHT_FLOOR_STATUS_APPOINT_WAZA_DATA_ID:
        return fightFloorBiosGetAppointWazaDataId(floor);
    case FIGHT_FLOOR_STATUS_APPOINT_ITEM_DATA_ID:
        return fightFloorBiosGetAppointItemDataId(floor);
    case FIGHT_FLOOR_STATUS_APPOINT_TOKUSEI_DATA_ID:
        return fightFloorBiosGetAppointTokuseiDataId(floor);
    case FIGHT_FLOOR_STATUS_FIGHT_POKEMON_ENTRY_CNT:
        return fightFloorBiosGetFightPokemonEntryCnt(floor);
    case FIGHT_FLOOR_STATUS_FIGHT_OUT_POKEMON_PTR:
        return (u32)fightFloorBiosGetFightOutPokemonPtrAry(floor, index);
    case FIGHT_FLOOR_STATUS_FIGHT_OUT_POKEMON_PTR_ARY_PTR:
        return (u32)fightFloorBiosGetFightOutPokemonPtrAryPtr(floor);
    case FIGHT_FLOOR_STATUS_FIRST_ATTACK_RND:
        return fightFloorBiosGetFirstAttackRnd(floor);
    case FIGHT_FLOOR_STATUS_FIGHT_RESULT_ID:
        return fightGetFightResultId();
    case FIGHT_FLOOR_STATUS_FIGHT_OUT_BANME_POKEMON_PTR:
        return (u32)fightFloorGetFightOutBanmeToValidFightOutPokemon(floor, index);
    }
    return 0;
}

void *fightFloorGetNowPtr(void)
{
    return (void *)fightFloorGetStatus(0, 0, 0, 0);
}

s32 _fightFloorCreateFightOutPokemonPtrAry(FIGHT_FLOOR *floor, FightOutPokemon **pokemonArray, bool deployedOnly, u8 sideFilter, FightOutPokemon *referencePokemon)
{
    _fightFloorInitFightOutPokemonPtrAry(pokemonArray);
    FIGHT_SIDE *fightSide;
    FIGHT_SIDE *targetSide;
    u16 trainer;
    u16 pokemon;
    u16 side;
    u16 trainerNum;
    u16 pokemonNum;
    s32 count;
    u16 fightType;
    count = 0;
    fightType = fightFloor_GetFightType(floor);
    trainerNum = fightFloor_GetFightTypeTrainerNum(floor);
    pokemonNum = fightFloor_GetFightTypeFightoutPokemonNum(floor);

    if (sideFilter != FIGHT_FLOOR_SIDE_FILTER_ALL) {
        if (fightOutPokemonCheckValid(referencePokemon) == 0) {
            return 0;
        }
        targetSide = (FIGHT_SIDE *)fightTargetGetPtr(FIGHT_TARGET_ID_OWN_SIDE, referencePokemon, fightType);
    } else {
        targetSide = 0;
    }
    for (side = 0; side < FIGHT_FLOOR_SIDE_COUNT; side++) {
        fightSide = fightFloorGetValidFightSidePtr(floor, side);
        if (fightSide == 0) {
            continue;
        }
        if (sideFilter == FIGHT_FLOOR_SIDE_FILTER_OWN_SIDE) {
            if (targetSide != fightSide) {
                continue;
            }
        } else if (sideFilter == FIGHT_FLOOR_SIDE_FILTER_OPPOSING_SIDE) {
            if (targetSide == fightSide) {
                continue;
            }
        }
        for (trainer = 0; trainer < trainerNum; trainer++) {
            FIGHT_TRAINER *fightTrainer = fightSideGetValidFightTrainerPtr(fightSide, trainer);
            if (fightTrainer == 0) {
                continue;
            }
            for (pokemon = 0; pokemon < pokemonNum; pokemon++) {
                FightOutPokemon *fightOutPokemon = fightTrainerGetValidFightOutPokemonPtr(fightTrainer, pokemon);
                if (fightOutPokemon == 0) {
                    continue;
                }
                if (deployedOnly == 1 && !fightOutPokemonCheckFightOut(fightOutPokemon)) {
                    continue;
                }
                if (sideFilter == FIGHT_FLOOR_SIDE_FILTER_EXCLUDE_REFERENCE && fightOutPokemon == referencePokemon) {
                    continue;
                }
                if (_fightFloorSetFightOutPokemonPtrAry(pokemonArray, fightOutPokemon) >= 0) {
                    count++;
                }
            }
        }
    }
    return count;
}

s16 _fightFloorSetFightOutPokemonPtrAry(FightOutPokemon **pokemonArray, FightOutPokemon *pokemon)
{
    for (u16 index = 0; index < FIGHT_OUT_POKEMON_COUNT; index++) {
        if (pokemonArray[index] == 0) {
            pokemonArray[index] = pokemon;
            return index;
        }
    }
    return -1;
}

void _fightFloorInitFightOutPokemonPtrAry(FightOutPokemon **pokemonArray)
{
    for (u16 index = 0; index < FIGHT_OUT_POKEMON_COUNT; index++) {
        pokemonArray[index] = 0;
    }
}
