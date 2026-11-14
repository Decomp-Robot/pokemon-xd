#ifndef WAZA_H
#define WAZA_H

#include <types.h>
#include <bool.h>

typedef struct FIGHT_WAZA FIGHT_WAZA;

enum WAZA_FIELD {
    WAZA_FIELD_NONE = 0,
    WAZA_FIELD_NAME = 1,
    WAZA_FIELD_PP = 2,
    WAZA_FIELD_TYPE = 3,
    WAZA_FIELD_PRIORITY = 4,
    WAZA_FIELD_RANGE_ID = 5,
    WAZA_FIELD_ACCURACY = 6,
    WAZA_FIELD_POWER = 7,
    WAZA_FIELD_FIGHT_EFFECT_ID = 8,
    WAZA_FIELD_SEQUENCE_ID = 9,
    WAZA_FIELD_ATTACK_CONNECT_MESSAGE = 10,
    WAZA_FIELD_ATTACK_MESSAGE = 11,
    WAZA_FIELD_ADD_EFFECT_ACCURACY = 12,
    WAZA_FIELD_CONTACT = 13,
    WAZA_FIELD_PROTECT = 14,
    WAZA_FIELD_MAGIC_COAT = 15,
    WAZA_FIELD_SNATCH = 16,
    WAZA_FIELD_MIRROR_MOVE = 17,
    WAZA_FIELD_KINGS_ROCK = 18,
    WAZA_FIELD_METRONOME = 19,
    WAZA_FIELD_MIMIC = 20,
    WAZA_FIELD_ASSIST = 21,
    WAZA_FIELD_SLEEP_TALK = 22,
    WAZA_FIELD_SOUNDPROOF = 23,
    WAZA_FIELD_PRESSURE = 24,
    WAZA_FIELD_HIDEN_FLAG = 25,
    WAZA_FIELD_CATEGORY = 26,
    WAZA_FIELD_RISK_FLAG = 27,
    WAZA_FIELD_TRAINER_AI_VALUE_FUNC = 28,
    WAZA_FIELD_TRAINER_AI_HIT_FUNC = 29,
    WAZA_FIELD_TRAINER_AI_DAMAGE_FUNC = 30,
    WAZA_FIELD_WAZA_WZX_ID = 31,
    WAZA_FIELD_WAZA_WZX_TYPE_FUNC = 32,
    WAZA_FIELD_WAZA_WZX_VARIATION_FUNC = 33,
    WAZA_FIELD_DESCRIPTION = 34,
    WAZA_FIELD_STATIC_END = 37,
    WAZA_FIELD_MOVE_SLOT = 38,
    WAZA_FIELD_ORIGINAL_MOVE_ID = 39,
    WAZA_FIELD_USED_MOVE_ID = 40,
    WAZA_FIELD_TARGET_ID = 41,
    WAZA_FIELD_JOUTAI = 42,
    WAZA_FIELD_CRITICAL = 43,
    WAZA_FIELD_DAMAGE_VALUE = 44,
    WAZA_FIELD_DAMAGE = 45,
    WAZA_FIELD_HIT_DAMAGE = 46,
    WAZA_FIELD_FIGHT_POWER = 47,
    WAZA_FIELD_FIGHT_TYPE = 48,
    WAZA_FIELD_USE_COUNT = 49,
    WAZA_FIELD_AUTO_MAKE_FLAG = 50,
    WAZA_FIELD_ORIGINAL_MOVE_TYPE = 51,
    WAZA_FIELD_USED_MOVE_TYPE = 52,
    WAZA_FIELD_COUNT = 53,
};

enum {
    WAZA_DATA_ID_SURF = 57,
    WAZA_DATA_ID_STRUGGLE = 165,
    WAZA_DATA_ID_SAFEGUARD = 219,
    WAZA_DATA_ID_BATON_PASS = 226,
    WAZA_DATA_ID_FOCUS_PUNCH = 264,
    WAZA_DATA_ID_MUDDY_WATER = 330,
    WAZA_DATA_ID_NORMAL_END = 355,
    WAZA_DATA_ID_DARK_BLITZ = 356,
    WAZA_DATA_ID_DARK_RUSH = 357,
    WAZA_DATA_ID_DARK_BREAK = 358,
    WAZA_DATA_ID_DARK_END = 359,
    WAZA_DATA_ID_DARK_WAVE = 360,
    WAZA_DATA_ID_DARK_RAVE = 361,
    WAZA_DATA_ID_DARK_STORM = 362,
    WAZA_DATA_ID_DARK_FIRE = 363,
    WAZA_DATA_ID_DARK_BOLT = 364,
    WAZA_DATA_ID_DARK_CHILL = 365,
    WAZA_DATA_ID_DARK_BLAST = 366,
    WAZA_DATA_ID_DARK_SKY = 367,
    WAZA_DATA_ID_DARK_HOLD = 368,
    WAZA_DATA_ID_DARK_MIST = 369,
    WAZA_DATA_ID_DARK_PANIC = 370,
    WAZA_DATA_ID_DARK_DOWN = 371,
    WAZA_DATA_ID_DARK_SHED = 372,
    WAZA_DATA_ID_DARK_HALF = 373
};
enum WAZA_SHADOW {
    WAZA_SHADOW_FIRST = WAZA_DATA_ID_DARK_BLITZ,
    WAZA_SHADOW_LAST = WAZA_DATA_ID_DARK_HALF,
    WAZA_SHADOW_SPECIAL = 374,
    WAZA_SHADOW_SPECIAL_ICON = 123,
};

static const u16 WAZA_DATA_ID_NONE = 0;
static const u16 WAZA_DATA_ID_INVALID = 0xFFFF;

enum {
    WAZA_STATUS_NAME_MESSAGE_ID = WAZA_FIELD_NAME,
    WAZA_STATUS_TYPE_ID = WAZA_FIELD_TYPE
};

enum WAZA_TYPE_ID {
    WAZA_TYPE_ID_NONE = 0,
    WAZA_TYPE_ID_IRYOKU = 1,
    WAZA_TYPE_ID_KAIFUKU = 2,
    WAZA_TYPE_ID_NOURYOKU_HENKOU = 3,
    WAZA_TYPE_ID_KYOUKA = 4,
    WAZA_TYPE_ID_POKEMON_JOUTAI_HENKOU = 5,
    WAZA_TYPE_ID_SENJOU_JOUTAI_HENKOU = 6,
    WAZA_TYPE_ID_MAMORI = 7,
    WAZA_TYPE_ID_ICHIGEKI_HISSATSU = 8,
    WAZA_TYPE_ID_TAME = 9,
    WAZA_TYPE_ID_TOKUSHU = 10,
    WAZA_TYPE_ID_HISENTOU = 13,
};

enum WAZA_COMPATIBILITY {
    WAZA_COMPATIBILITY_UNDISPLAYED = 0,
    WAZA_COMPATIBILITY_TYPE_CHART = 1,
    WAZA_COMPATIBILITY_UNDISPLAYED_ALTERNATE = 2,
    WAZA_COMPATIBILITY_UNDISPLAYED_OTHER = 3,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GHOST = 4,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GRASS = 5,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_POISON_OR_STEEL = 6,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_GROUND = 7,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_FLYING = 8,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_NORMAL = 9,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY = 10,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY_ALTERNATE = 11,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_DARK = 12,
    WAZA_COMPATIBILITY_NO_EFFECT_ON_FIRE = 13,
    WAZA_COMPATIBILITY_SPECIAL_DISPLAY_OTHER = 14,
    WAZA_COMPATIBILITY_ALL_TYPES_NORMAL = 15,
};

enum { WAZA_ID_MASK = 0xFFFF };
enum { WAZA_DATA_TYPE_ID_COUNT = 3 };
enum {
    ZOKUSEI_DATA_ID_MYSTERY = 9,
    ZOKUSEI_DATA_ID_FLYING = 2,
    ZOKUSEI_DATA_ID_STEEL = 8,
    ZOKUSEI_DATA_ID_FIRE = 10,
    ZOKUSEI_DATA_ID_WATER = 11,
    ZOKUSEI_DATA_ID_ICE = 15,
    ZOKUSEI_DATA_ID_ELECTRIC = 13
};

enum {
    WAZA_STATUS_TYPE = 26,
    WAZA_STATUS_RISK_FLAG = 27
};

// An entry of the move database: everything the fight reads about one move.
struct WazaData {
    s8 mPri; // 0x00
    u8 mPp; // 0x01
    u8 mZokuseiDataId; // 0x02
    u8 mRangeId; // 0x03
    u8 mAvg; // 0x04
    u8 mAddFightKoukaAvg; // 0x05
    u8 mDageki; // 0x06
    u8 mMamoru; // 0x07
    u8 mMajikku; // 0x08
    u8 mYokodori; // 0x09
    u8 mOumugaesi; // 0x0A
    u8 mOujanosirusi; // 0x0B
    u8 _0C[4];
    u8 mBouon; // 0x10
    u8 _11[1];
    bool mHidenFlag; // 0x12
    u8 mZokuseiKindData; // 0x13
    u8 mRiskFlag; // 0x14
    u8 mCompatible; // 0x15
    u8 _16[2];
    s16 mIryoku; // 0x18
    u8 _1A[2];
    u16 mSeqId; // 0x1C
    u16 mWazaFuncDataId; // 0x1E
    u32 mNameID; // 0x20
    u32 _24[3];
    u32 mWazawzxdataId; // 0x30
    u8 mTypeId[WAZA_DATA_TYPE_ID_COUNT]; // 0x34
    u8 _37[1];
};

// Function table the fight dispatches a move through, looked up by the
// function data id of the move.
struct WazaFuncData {
    void *mFightTrainerAiWazaValueFunc; // 0x00
    void *mFightTrainerAiWazaHitFunc; // 0x04
    void *mFightTrainerAiWazaDamageFunc; // 0x08
    void *mFightTrainerAiWazaDamageFightPokemonFunc; // 0x0C
    void *mFightWazaWzxTypeFunc; // 0x10
    void *mFightWazaWzxVariationFunc; // 0x14
};

#ifdef __cplusplus
extern "C" {
#endif

struct WazaData *wazaDataBiosGetPtr(u16 wazaID);
u32 wazaDataBiosGetName(struct WazaData *data);
u16 wazaDataBiosGetWazaFuncDataId(struct WazaData *data);
struct WazaFuncData *wazaFuncDataBiosGetPtr(u16 funcDataId);
u32 wazaGetWazaTypeIdName(u8 typeId);
u8 wazaIsWazaTypeId(u16 move, u8 typeId);
u8 wazaGetMaxPP(u16 move, u8 ppUpCount);

s32 wazaGetStatus(void *data, u16 wazaID, u16 fieldID, s32 index);
void wazaSetStatus(void *data, u16 wazaID, u16 fieldID, s32 index, s32 value);

u8 wazaCheckDarkWaza(u16 move);
u8 waza_GetZokuseiKindData(u16 move);

u8 wazaDB_GetCompatible(u16 wazaID);
s16 wazaDB_GetIryoku(u16 move);
u16 wazaDB_GetSeqId(u16 move);
u8 wazaDB_GetOujanosirusi(u16 move);
s32 wazaDB_GetName(u16 move);
u8 wazaDB_GetRangeId(u16 move);
u8 wazaDB_GetOumugaesi(u16 wazaDataId);
u8 wazaDB_GetZokuseiDataId(u16 move);
u8 wazaDB_GetRiskFlag(u16 move);
u32 wazaDB_GetWazawzxdataId(u16 move);
u8 wazaDB_GetTypeId(u16 move, u16 index);
u8 wazaDB_GetBouon(u16 move);
u8 wazaDB_GetYokodori(u16 move);
u8 wazaDB_GetMajikku(u16 move);
u8 wazaDB_GetMamoru(u16 move);
u8 wazaDB_GetDageki(u16 move);
u8 wazaDB_GetAddFightKoukaAvg(u16 move);
u8 wazaDB_GetAvg(u16 move);
s32 wazaDB_GetPri(u16 move);
u8 wazaDB_GetPp(u16 move);

void *wazaDB_GetFightWazaWzxVariationFuncPtr(u16 move);
void *wazaDB_GetFightWazaWzxTypeFuncPtr(u16 move);
void *wazaDB_GetFightTrainerAiWazaDamageFightPokemonFuncPtr(u16 move);
void *wazaDB_GetFightTrainerAiWazaDamageFuncPtr(u16 move);
void *wazaDB_GetFightTrainerAiWazaHitFuncPtr(u16 move);
void *wazaDB_GetFightTrainerAiWazaValueFuncPtr(u16 move);

void wazaDataBiosSetName(struct WazaData *data, u32 value);
void wazaDataBiosSetPp(struct WazaData *data, u8 value);
void wazaDataBiosSetZokuseiDataId(struct WazaData *data, u8 value);
void wazaDataBiosSetPri(struct WazaData *data, s8 value);
void wazaDataBiosSetRangeId(struct WazaData *data, u8 value);
void wazaDataBiosSetAvg(struct WazaData *data, u8 value);
void wazaDataBiosSetIryoku(struct WazaData *data, s16 value);
void wazaDataBiosSetFightKoukaDataId(struct WazaData *data, u16 value);
void wazaDataBiosSetSeqId(struct WazaData *data, u16 value);
void wazaDataBiosSetFightAttackTunagiMsgId(struct WazaData *data, u16 value);
void wazaDataBiosSetFightAttackMsgId(struct WazaData *data, u16 value);
void wazaDataBiosSetAddFightKoukaAvg(struct WazaData *data, u8 value);
void wazaDataBiosSetDageki(struct WazaData *data, u8 value);
void wazaDataBiosSetMamoru(struct WazaData *data, u8 value);
void wazaDataBiosSetMajikku(struct WazaData *data, u8 value);
void wazaDataBiosSetYokodori(struct WazaData *data, u8 value);
void wazaDataBiosSetOumugaesi(struct WazaData *data, u8 value);
void wazaDataBiosSetOujanosirusi(struct WazaData *data, u8 value);
void wazaDataBiosSetYubiwohuru(struct WazaData *data, u8 value);
void wazaDataBiosSetMonomane(struct WazaData *data, u8 value);
void wazaDataBiosSetNekonote(struct WazaData *data, u8 value);
void wazaDataBiosSetNegoto(struct WazaData *data, u8 value);
void wazaDataBiosSetBouon(struct WazaData *data, u8 value);
void wazaDataBiosSetPressure(struct WazaData *data, u8 value);
void wazaDataBiosSetHidenFlag(struct WazaData *data, u8 value);
void wazaDataBiosSetTypeId(struct WazaData *data, s32 index, u8 typeId);
void wazaDataBiosSetRiskFlag(struct WazaData *data, u8 value);
void wazaDataBiosSetFightTrainerAiWazaValueFuncPtr(struct WazaData *data, void * func);
void wazaDataBiosSetFightTrainerAiWazaHitFuncPtr(struct WazaData *data, void * func);
void wazaDataBiosSetFightTrainerAiWazaDamageFuncPtr(struct WazaData *data, void * func);
void wazaDataBiosSetWazawzxdataId(struct WazaData *data, u32 value);
void wazaDataBiosSetFightWazaWzxTypeFuncPtr(struct WazaData *data, void * func);
void wazaDataBiosSetFightWazaWzxVariationFuncPtr(struct WazaData *data, void * func);
void wazaDataBiosSetDoc(struct WazaData *data, u32 value);
u8 wazaDataBiosGetPp(struct WazaData *data);
u8 wazaDataBiosGetZokuseiDataId(struct WazaData *data);
s8 wazaDataBiosGetPri(struct WazaData *data);
u8 wazaDataBiosGetRangeId(struct WazaData *data);
u8 wazaDataBiosGetAvg(struct WazaData *data);
s16 wazaDataBiosGetIryoku(struct WazaData *data);
u16 wazaDataBiosGetFightKoukaDataId(struct WazaData *data);
u16 wazaDataBiosGetSeqId(struct WazaData *data);
u32 wazaDataBiosGetFightAttackTunagiMsgId(struct WazaData *data);
u32 wazaDataBiosGetFightAttackMsgId(struct WazaData *data);
u8 wazaDataBiosGetAddFightKoukaAvg(struct WazaData *data);
u8 wazaDataBiosGetDageki(struct WazaData *data);
u8 wazaDataBiosGetMamoru(struct WazaData *data);
u8 wazaDataBiosGetMajikku(struct WazaData *data);
u8 wazaDataBiosGetYokodori(struct WazaData *data);
u8 wazaDataBiosGetOumugaesi(struct WazaData *data);
u8 wazaDataBiosGetOujanosirusi(struct WazaData *data);
u8 wazaDataBiosGetYubiwohuru(struct WazaData *data);
u8 wazaDataBiosGetMonomane(struct WazaData *data);
u8 wazaDataBiosGetNekonote(struct WazaData *data);
u8 wazaDataBiosGetNegoto(struct WazaData *data);
u8 wazaDataBiosGetBouon(struct WazaData *data);
u8 wazaDataBiosGetPressure(struct WazaData *data);
u8 wazaDataBiosGetHidenFlag(struct WazaData *data);
u8 wazaDataBiosGetTypeId(struct WazaData *data, s32 index);
u8 wazaDataBiosGetRiskFlag(struct WazaData *data);
void *wazaDataBiosGetFightTrainerAiWazaValueFuncPtr(struct WazaData *data);
void *wazaDataBiosGetFightTrainerAiWazaHitFuncPtr(struct WazaData *data);
void *wazaDataBiosGetFightTrainerAiWazaDamageFuncPtr(struct WazaData *data);
u32 wazaDataBiosGetWazawzxdataId(struct WazaData *data);
void *wazaDataBiosGetFightWazaWzxTypeFuncPtr(struct WazaData *data);
void *wazaDataBiosGetFightWazaWzxVariationFuncPtr(struct WazaData *data);
u32 wazaDataBiosGetDoc(struct WazaData *data);
#ifdef __cplusplus
}
#endif

#endif
