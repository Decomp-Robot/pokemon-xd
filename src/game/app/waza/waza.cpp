#include <types.h>
#include <waza.h>
#include <fightWaza.h>

extern "C" u8 wazaCheckDarkWaza(u16 move)
{
    switch (move) {
    case WAZA_DATA_ID_DARK_BLITZ:
    case WAZA_DATA_ID_DARK_RUSH:
    case WAZA_DATA_ID_DARK_BREAK:
    case WAZA_DATA_ID_DARK_END:
    case WAZA_DATA_ID_DARK_WAVE:
    case WAZA_DATA_ID_DARK_RAVE:
    case WAZA_DATA_ID_DARK_STORM:
    case WAZA_DATA_ID_DARK_FIRE:
    case WAZA_DATA_ID_DARK_BOLT:
    case WAZA_DATA_ID_DARK_CHILL:
    case WAZA_DATA_ID_DARK_BLAST:
    case WAZA_DATA_ID_DARK_SKY:
    case WAZA_DATA_ID_DARK_HOLD:
    case WAZA_DATA_ID_DARK_MIST:
    case WAZA_DATA_ID_DARK_PANIC:
    case WAZA_DATA_ID_DARK_DOWN:
    case WAZA_DATA_ID_DARK_SHED:
    case WAZA_DATA_ID_DARK_HALF:
        return 1;
    default:
        return 0;
    }
}

enum {
    MSG_WAZA_TYPE_IRYOKU = 60496,
    MSG_WAZA_TYPE_KAIFUKU = 60497,
    MSG_WAZA_TYPE_NOURYOKU_HENKOU = 60498,
    MSG_WAZA_TYPE_KYOUKA = 60681,
    MSG_WAZA_TYPE_POKEMON_JOUTAI_HENKOU = 60499,
    MSG_WAZA_TYPE_SENJOU_JOUTAI_HENKOU = 60500,
    MSG_WAZA_TYPE_MAMORI = 60501,
    MSG_WAZA_TYPE_ICHIGEKI_HISSATSU = 60502,
    MSG_WAZA_TYPE_TAME = 60503,
    MSG_WAZA_TYPE_TOKUSHU = 60504,
    MSG_WAZA_TYPE_HISENTOU = 60505,
};

extern "C" u32 wazaGetWazaTypeIdName(u8 typeId)
{
    u32 messageId = 0;
    switch (typeId) {
    case WAZA_TYPE_ID_NONE: messageId = 0; break;
    case WAZA_TYPE_ID_IRYOKU: messageId = MSG_WAZA_TYPE_IRYOKU; break;
    case WAZA_TYPE_ID_KAIFUKU: messageId = MSG_WAZA_TYPE_KAIFUKU; break;
    case WAZA_TYPE_ID_NOURYOKU_HENKOU: messageId = MSG_WAZA_TYPE_NOURYOKU_HENKOU; break;
    case WAZA_TYPE_ID_KYOUKA: messageId = MSG_WAZA_TYPE_KYOUKA; break;
    case WAZA_TYPE_ID_POKEMON_JOUTAI_HENKOU: messageId = MSG_WAZA_TYPE_POKEMON_JOUTAI_HENKOU; break;
    case WAZA_TYPE_ID_SENJOU_JOUTAI_HENKOU: messageId = MSG_WAZA_TYPE_SENJOU_JOUTAI_HENKOU; break;
    case WAZA_TYPE_ID_MAMORI: messageId = MSG_WAZA_TYPE_MAMORI; break;
    case WAZA_TYPE_ID_ICHIGEKI_HISSATSU: messageId = MSG_WAZA_TYPE_ICHIGEKI_HISSATSU; break;
    case WAZA_TYPE_ID_TAME: messageId = MSG_WAZA_TYPE_TAME; break;
    case WAZA_TYPE_ID_TOKUSHU: messageId = MSG_WAZA_TYPE_TOKUSHU; break;
    case WAZA_TYPE_ID_HISENTOU: messageId = MSG_WAZA_TYPE_HISENTOU; break;
    }
    return messageId;
}

extern "C" u8 wazaIsWazaTypeId(u16 move, u8 typeId)
{
    if (move == WAZA_DATA_ID_NONE && move == WAZA_DATA_ID_NORMAL_END && move == WAZA_SHADOW_SPECIAL) return 0;
    if (typeId == WAZA_TYPE_ID_NONE) return 0;
    for (u8 i = 0; i < WAZA_DATA_TYPE_ID_COUNT; i++) {
        if (wazaDB_GetTypeId(move, i) == typeId) return 1;
    }
    return 0;
}

extern "C" u8 wazaGetMaxPP(u16 move, u8 ppUpCount)
{
    u8 pp = wazaDB_GetPp(move);
    return pp + (pp * 20 * ppUpCount) / 100;
}

extern "C" void wazaSetStatus(void *data, u16 wazaID, u16 fieldID, s32 index, s32 value)
{
    if (fieldID == WAZA_FIELD_NONE || fieldID >= WAZA_FIELD_COUNT) return;
    void *record;
    if (fieldID < WAZA_FIELD_STATIC_END) {
        record = wazaDataBiosGetPtr(wazaID);
        if (record == 0) return;
    } else {
        record = data;
        if (record == 0) return;
    }
    WazaData *wazaData = (WazaData *)record;
    switch (fieldID) {
    case WAZA_FIELD_NAME: wazaDataBiosSetName(wazaData, (u32)value); break;
    case WAZA_FIELD_PP: wazaDataBiosSetPp(wazaData, (u8)value); break;
    case WAZA_FIELD_TYPE: wazaDataBiosSetZokuseiDataId(wazaData, (u8)value); break;
    case WAZA_FIELD_PRIORITY: wazaDataBiosSetPri(wazaData, (s8)value); break;
    case WAZA_FIELD_RANGE_ID: wazaDataBiosSetRangeId(wazaData, (u8)value); break;
    case WAZA_FIELD_ACCURACY: wazaDataBiosSetAvg(wazaData, (u8)value); break;
    case WAZA_FIELD_POWER: wazaDataBiosSetIryoku(wazaData, (s16)value); break;
    case WAZA_FIELD_FIGHT_EFFECT_ID: wazaDataBiosSetFightKoukaDataId(wazaData, (u16)value); break;
    case WAZA_FIELD_SEQUENCE_ID: wazaDataBiosSetSeqId(wazaData, (u16)value); break;
    case WAZA_FIELD_ATTACK_CONNECT_MESSAGE: wazaDataBiosSetFightAttackTunagiMsgId(wazaData, (u16)value); break;
    case WAZA_FIELD_ATTACK_MESSAGE: wazaDataBiosSetFightAttackMsgId(wazaData, (u16)value); break;
    case WAZA_FIELD_ADD_EFFECT_ACCURACY: wazaDataBiosSetAddFightKoukaAvg(wazaData, (u8)value); break;
    case WAZA_FIELD_CONTACT: wazaDataBiosSetDageki(wazaData, (u8)value); break;
    case WAZA_FIELD_PROTECT: wazaDataBiosSetMamoru(wazaData, (u8)value); break;
    case WAZA_FIELD_MAGIC_COAT: wazaDataBiosSetMajikku(wazaData, (u8)value); break;
    case WAZA_FIELD_SNATCH: wazaDataBiosSetYokodori(wazaData, (u8)value); break;
    case WAZA_FIELD_MIRROR_MOVE: wazaDataBiosSetOumugaesi(wazaData, (u8)value); break;
    case WAZA_FIELD_KINGS_ROCK: wazaDataBiosSetOujanosirusi(wazaData, (u8)value); break;
    case WAZA_FIELD_METRONOME: wazaDataBiosSetYubiwohuru(wazaData, (u8)value); break;
    case WAZA_FIELD_MIMIC: wazaDataBiosSetMonomane(wazaData, (u8)value); break;
    case WAZA_FIELD_ASSIST: wazaDataBiosSetNekonote(wazaData, (u8)value); break;
    case WAZA_FIELD_SLEEP_TALK: wazaDataBiosSetNegoto(wazaData, (u8)value); break;
    case WAZA_FIELD_SOUNDPROOF: wazaDataBiosSetBouon(wazaData, (u8)value); break;
    case WAZA_FIELD_PRESSURE: wazaDataBiosSetPressure(wazaData, (u8)value); break;
    case WAZA_FIELD_HIDEN_FLAG: wazaDataBiosSetHidenFlag(wazaData, (u8)value); break;
    case WAZA_FIELD_CATEGORY: wazaDataBiosSetTypeId(wazaData, index, (u8)value); break;
    case WAZA_FIELD_RISK_FLAG: wazaDataBiosSetRiskFlag(wazaData, (u8)value); break;
    case WAZA_FIELD_TRAINER_AI_VALUE_FUNC: wazaDataBiosSetFightTrainerAiWazaValueFuncPtr(wazaData, (void *)value); break;
    case WAZA_FIELD_TRAINER_AI_HIT_FUNC: wazaDataBiosSetFightTrainerAiWazaHitFuncPtr(wazaData, (void *)value); break;
    case WAZA_FIELD_TRAINER_AI_DAMAGE_FUNC: wazaDataBiosSetFightTrainerAiWazaDamageFuncPtr(wazaData, (void *)value); break;
    case WAZA_FIELD_WAZA_WZX_TYPE_FUNC: wazaDataBiosSetFightWazaWzxTypeFuncPtr(wazaData, (void *)value); break;
    case WAZA_FIELD_WAZA_WZX_VARIATION_FUNC: wazaDataBiosSetFightWazaWzxVariationFuncPtr(wazaData, (void *)value); break;
    case WAZA_FIELD_WAZA_WZX_ID: wazaDataBiosSetWazawzxdataId(wazaData, (u32)value); break;
    case WAZA_FIELD_DESCRIPTION: wazaDataBiosSetDoc(wazaData, (u32)value); break;
    case WAZA_FIELD_MOVE_SLOT: fightWazaBiosSetWazaBanme((FIGHT_WAZA *)wazaData, (s8)value); break;
    case WAZA_FIELD_ORIGINAL_MOVE_ID: fightWazaBiosSetMotoWazaDataId((FIGHT_WAZA *)wazaData, (u16)value); break;
    case WAZA_FIELD_USED_MOVE_ID: fightWazaBiosSetUseWazaDataId((FIGHT_WAZA *)wazaData, (u16)value); break;
    case WAZA_FIELD_TARGET_ID: fightWazaBiosSetTargetDataId((FIGHT_WAZA *)wazaData, (u16)value); break;
    case WAZA_FIELD_CRITICAL: fightWazaBiosSetCritical((FIGHT_WAZA *)wazaData, (u8)value); break;
    case WAZA_FIELD_DAMAGE_VALUE: fightWazaBiosSetDamageValue((FIGHT_WAZA *)wazaData, (u8)value); break;
    case WAZA_FIELD_DAMAGE: fightWazaBiosSetDamage((FIGHT_WAZA *)wazaData, value); break;
    case WAZA_FIELD_HIT_DAMAGE: fightWazaBiosSetHitDamage((FIGHT_WAZA *)wazaData, value); break;
    case WAZA_FIELD_FIGHT_POWER: fightWazaBiosSetIryoku((FIGHT_WAZA *)wazaData, (u16)value); break;
    case WAZA_FIELD_FIGHT_TYPE: fightWazaBiosSetZokusei((FIGHT_WAZA *)wazaData, (u16)value); break;
    case WAZA_FIELD_USE_COUNT: fightWazaBiosSetKaisuu((FIGHT_WAZA *)wazaData, (u8)value); break;
    case WAZA_FIELD_AUTO_MAKE_FLAG: fightWazaBiosSetAutoMakeFlag((FIGHT_WAZA *)wazaData, (u8)value); break;
    }
}

extern "C" s32 wazaGetStatus(void *data, u16 wazaID, u16 fieldID, s32 index)
{
    if (fieldID == WAZA_FIELD_NONE || fieldID >= WAZA_FIELD_COUNT) return 0;
    void *record;
    if (fieldID < WAZA_FIELD_STATIC_END) {
        record = wazaDataBiosGetPtr(wazaID);
        if (record == 0) return 0;
    } else {
        record = data;
        if (record == 0) return 0;
    }
    WazaData *wazaData = (WazaData *)record;
    switch (fieldID) {
    case WAZA_FIELD_NAME: return wazaDataBiosGetName(wazaData);
    case WAZA_FIELD_PP: return wazaDataBiosGetPp(wazaData);
    case WAZA_FIELD_TYPE: return wazaDataBiosGetZokuseiDataId(wazaData);
    case WAZA_FIELD_PRIORITY: return wazaDataBiosGetPri(wazaData);
    case WAZA_FIELD_RANGE_ID: return wazaDataBiosGetRangeId(wazaData);
    case WAZA_FIELD_ACCURACY: return wazaDataBiosGetAvg(wazaData);
    case WAZA_FIELD_POWER: return wazaDataBiosGetIryoku(wazaData);
    case WAZA_FIELD_FIGHT_EFFECT_ID: return wazaDataBiosGetFightKoukaDataId(wazaData);
    case WAZA_FIELD_SEQUENCE_ID: return wazaDataBiosGetSeqId(wazaData);
    case WAZA_FIELD_ATTACK_CONNECT_MESSAGE: return wazaDataBiosGetFightAttackTunagiMsgId(wazaData);
    case WAZA_FIELD_ATTACK_MESSAGE: return wazaDataBiosGetFightAttackMsgId(wazaData);
    case WAZA_FIELD_ADD_EFFECT_ACCURACY: return wazaDataBiosGetAddFightKoukaAvg(wazaData);
    case WAZA_FIELD_CONTACT: return wazaDataBiosGetDageki(wazaData);
    case WAZA_FIELD_PROTECT: return wazaDataBiosGetMamoru(wazaData);
    case WAZA_FIELD_MAGIC_COAT: return wazaDataBiosGetMajikku(wazaData);
    case WAZA_FIELD_SNATCH: return wazaDataBiosGetYokodori(wazaData);
    case WAZA_FIELD_MIRROR_MOVE: return wazaDataBiosGetOumugaesi(wazaData);
    case WAZA_FIELD_KINGS_ROCK: return wazaDataBiosGetOujanosirusi(wazaData);
    case WAZA_FIELD_METRONOME: return wazaDataBiosGetYubiwohuru(wazaData);
    case WAZA_FIELD_MIMIC: return wazaDataBiosGetMonomane(wazaData);
    case WAZA_FIELD_ASSIST: return wazaDataBiosGetNekonote(wazaData);
    case WAZA_FIELD_SLEEP_TALK: return wazaDataBiosGetNegoto(wazaData);
    case WAZA_FIELD_SOUNDPROOF: return wazaDataBiosGetBouon(wazaData);
    case WAZA_FIELD_PRESSURE: return wazaDataBiosGetPressure(wazaData);
    case WAZA_FIELD_HIDEN_FLAG: return wazaDataBiosGetHidenFlag(wazaData);
    case WAZA_FIELD_CATEGORY: return wazaDataBiosGetTypeId(wazaData, index);
    case WAZA_FIELD_RISK_FLAG: return wazaDataBiosGetRiskFlag(wazaData);
    case WAZA_FIELD_TRAINER_AI_VALUE_FUNC: return (s32)wazaDataBiosGetFightTrainerAiWazaValueFuncPtr(wazaData);
    case WAZA_FIELD_TRAINER_AI_HIT_FUNC: return (s32)wazaDataBiosGetFightTrainerAiWazaHitFuncPtr(wazaData);
    case WAZA_FIELD_TRAINER_AI_DAMAGE_FUNC: return (s32)wazaDataBiosGetFightTrainerAiWazaDamageFuncPtr(wazaData);
    case WAZA_FIELD_WAZA_WZX_TYPE_FUNC: return (s32)wazaDataBiosGetFightWazaWzxTypeFuncPtr(wazaData);
    case WAZA_FIELD_WAZA_WZX_VARIATION_FUNC: return (s32)wazaDataBiosGetFightWazaWzxVariationFuncPtr(wazaData);
    case WAZA_FIELD_WAZA_WZX_ID: return wazaDataBiosGetWazawzxdataId(wazaData);
    case WAZA_FIELD_DESCRIPTION: return wazaDataBiosGetDoc(wazaData);
    case WAZA_FIELD_MOVE_SLOT: return (s8)fightWazaBiosGetWazaBanme((FIGHT_WAZA *)record);
    case WAZA_FIELD_ORIGINAL_MOVE_ID: return fightWazaBiosGetMotoWazaDataId((FIGHT_WAZA *)record);
    case WAZA_FIELD_USED_MOVE_ID: return fightWazaBiosGetUseWazaDataId((FIGHT_WAZA *)record);
    case WAZA_FIELD_TARGET_ID: return fightWazaBiosGetTargetDataId((FIGHT_WAZA *)record);
    case WAZA_FIELD_JOUTAI: return (s32)fightWazaBiosGetJoutaiPtr((FIGHT_WAZA *)record, index);
    case WAZA_FIELD_CRITICAL: return fightWazaBiosGetCritical((FIGHT_WAZA *)record);
    case WAZA_FIELD_DAMAGE_VALUE: return fightWazaBiosGetDamageValue((FIGHT_WAZA *)record);
    case WAZA_FIELD_DAMAGE: return fightWazaBiosGetDamage((FIGHT_WAZA *)record);
    case WAZA_FIELD_HIT_DAMAGE: return fightWazaBiosGetHitDamage((FIGHT_WAZA *)record);
    case WAZA_FIELD_FIGHT_POWER: return fightWazaBiosGetIryoku((FIGHT_WAZA *)record);
    case WAZA_FIELD_FIGHT_TYPE: return fightWazaBiosGetZokusei((FIGHT_WAZA *)record);
    case WAZA_FIELD_USE_COUNT: return fightWazaBiosGetKaisuu((FIGHT_WAZA *)record);
    case WAZA_FIELD_AUTO_MAKE_FLAG: return fightWazaBiosGetAutoMakeFlag((FIGHT_WAZA *)record);
    case WAZA_FIELD_ORIGINAL_MOVE_TYPE: return wazaDB_GetZokuseiDataId(fightWaza_GetMotoWazaDataId((FIGHT_WAZA *)record));
    case WAZA_FIELD_USED_MOVE_TYPE: return wazaDB_GetZokuseiDataId(fightWaza_GetUseWazaDataId((FIGHT_WAZA *)record));
    default: return 0;
    }
}
