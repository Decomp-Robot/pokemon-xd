#ifndef WAZA_H
#define WAZA_H

#include <types.h>
#include <bool.h>

typedef struct FIGHT_WAZA FIGHT_WAZA;

enum WAZA_FIELD {
    WAZA_FIELD_NAME = 1,
    WAZA_FIELD_PP = 2,
    WAZA_FIELD_TYPE = 3,
    WAZA_FIELD_ACCURACY = 6,
    WAZA_FIELD_POWER = 7,
    WAZA_FIELD_HIDEN_FLAG = 25,
    WAZA_FIELD_DESCRIPTION = 34,
};

enum WAZA_SHADOW {
    WAZA_SHADOW_FIRST = 356,
    WAZA_SHADOW_LAST = 373,
    WAZA_SHADOW_SPECIAL = 374,
    WAZA_SHADOW_SPECIAL_ICON = 123,
};

enum {
    WAZA_DATA_ID_SURF = 57,
    WAZA_DATA_ID_STRUGGLE = 165,
    WAZA_DATA_ID_SAFEGUARD = 219,
    WAZA_DATA_ID_BATON_PASS = 226,
    WAZA_DATA_ID_FOCUS_PUNCH = 264,
    WAZA_DATA_ID_MUDDY_WATER = 330
};
static const u16 WAZA_DATA_ID_NONE = 0;
static const u16 WAZA_DATA_ID_INVALID = 0xFFFF;

enum {
    WAZA_STATUS_NAME_MESSAGE_ID = WAZA_FIELD_NAME,
    WAZA_STATUS_TYPE_ID = WAZA_FIELD_TYPE
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

#ifdef __cplusplus
}
#endif

#endif
