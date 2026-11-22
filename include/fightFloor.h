#ifndef FIGHT_FLOOR_H
#define FIGHT_FLOOR_H

#include <fightEncountData.h>

#include <fightPokemon.h>
#include <fightSide.h>
#include <fightTrainer.h>
#include <joutai.h>
#include <types.h>

typedef struct FIGHT_WAZA FIGHT_WAZA;

enum FightWeather {
    FIGHT_WEATHER_NONE = 0,
    FIGHT_WEATHER_SUNLIGHT = 1,
    FIGHT_WEATHER_RAIN = 2,
    FIGHT_WEATHER_SANDSTORM = 3,
    FIGHT_WEATHER_HAIL = 4,
    FIGHT_WEATHER_SHADOW_SKY = 5
};

/* Status selectors for battle floor records. Selectors below the data limit
 * address the floor data table record passed as floorDataId; the rest address
 * the running floor, which is resolved through the current fight when the
 * caller passes none. */
enum FightFloorStatus {
    FIGHT_FLOOR_STATUS_FIGHT_FLOOR_PTR = 0,
    /* Floor data table record fields. */
    FIGHT_FLOOR_STATUS_NAME = 1,
    FIGHT_FLOOR_STATUS_FLOOR_DATA_ID = 2,
    FIGHT_FLOOR_STATUS_FIGHT_SIDE_DATA_ID = 3,
    FIGHT_FLOOR_STATUS_TIKEI_DATA_ID = 4,
    FIGHT_FLOOR_STATUS_SYOUKAI_WZX_DATA_ID = 5,
    FIGHT_FLOOR_STATUS_BGM_SND_ID = 6,
    FIGHT_FLOOR_STATUS_ENV_SND_ID = 7,
    FIGHT_FLOOR_STATUS_FIGHT_FLOOR_DATA_LIMIT = 8,
    /* Running floor state. */
    FIGHT_FLOOR_STATUS_JOUTAI_PTR = 9,
    FIGHT_FLOOR_STATUS_JOUTAI_DATA_ID = 10,
    FIGHT_FLOOR_STATUS_CHECK_WRITE_JOUTAI_DATA_ID = 11,
    FIGHT_FLOOR_STATUS_TURN_COUNT = 12,
    FIGHT_FLOOR_STATUS_ENCOUNT_DATA_ID = 13,
    FIGHT_FLOOR_STATUS_ENCOUNT_FLOOR_DATA_ID = 14,
    FIGHT_FLOOR_STATUS_NOW_TIKEI_DATA_ID = 15,
    FIGHT_FLOOR_STATUS_NOW_SYOUKAI_WZX_DATA_ID = 16,
    FIGHT_FLOOR_STATUS_NOW_BGM_SND_DATA_ID = 17,
    FIGHT_FLOOR_STATUS_NOW_ENV_SND_DATA_ID = 18,
    FIGHT_FLOOR_STATUS_NOW_TENKOU_DATA_ID = 19, /* weather of the running encounter. */
    FIGHT_FLOOR_STATUS_FIGHT_TYPE = 20, /* fight type of the running encounter. */
    FIGHT_FLOOR_STATUS_FIGHT_TYPE_NAME = 21,
    FIGHT_FLOOR_STATUS_FIGHT_TYPE_TRAINER_NUM = 22,
    FIGHT_FLOOR_STATUS_FIGHT_TYPE_ENTRY_POKEMON_NUM = 23,
    FIGHT_FLOOR_STATUS_FIGHT_TYPE_FIGHTOUT_POKEMON_NUM = 24,
    FIGHT_FLOOR_STATUS_FIGHT_SIDE_FIGHTOUT_POKEMON_MAX = 25,
    FIGHT_FLOOR_STATUS_FIGHT_KIND = 26, /* fight kind of the running encounter. */
    FIGHT_FLOOR_STATUS_FIGHT_KIND_NAME = 27,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_BACK_SAVE_DATA_FLAG = 28,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_BADGE_CHECK_FLAG = 29,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_ZUKAN_MITA_FLAG = 30,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_ZUKAN_TUKAMAETA_FLAG = 31,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_USE_ITEM_FLAG = 32,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_CALL_FLAG = 33,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_NIGERU_FLAG = 34,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DRAW_FLAG = 35,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_GET_EXP_FLAG = 36,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_GET_OKANE_FLAG = 37,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_OKANE_POOL_FLAG = 38,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_GET_FRIEND_FLAG = 39,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_GET_INFECT_POKERUS_FLAG = 40,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_CRITICAL_ATTACK_FLAG = 41,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_HIZUKI_AI_FLAG = 42,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_HIZUKI_MIYABURI_FLAG = 43,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DO_ITEM_SOUBI_TOKUKOUTOKUBOUP_FLAG = 44,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_KEIKENTIHUERU_FLAG = 45,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_BOSS_FLAG = 46,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DOROBOU_FLAG = 47,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_MONOHIROI_FLAG = 48,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_DARKPOKEMON_HYPERMODE_FLAG = 49,
    FIGHT_FLOOR_STATUS_POKEMON_STATUS_MENU_SUBBAR_FLAG = 50, /* whether the
        running battle shows the party status subbar. */
    FIGHT_FLOOR_STATUS_FIGHT_KIND_HOST_ENEMY_MSG_FLAG = 51,
    FIGHT_FLOOR_STATUS_FIGHT_KIND_REGULATION_FLAG = 52,
    /* Combatants, appointed targets and battle messages. */
    FIGHT_FLOOR_STATUS_FIGHT_SIDE_PTR = 53,
    FIGHT_FLOOR_STATUS_ATTACK_POKEMON_PTR = 54,
    FIGHT_FLOOR_STATUS_USE_WAZA_DATA_ID = 55,
    FIGHT_FLOOR_STATUS_MOTO_WAZA_DATA_ID = 56,
    FIGHT_FLOOR_STATUS_WAZA_DAMAGE_VALUE = 57,
    FIGHT_FLOOR_STATUS_WAZA_CRITICAL = 58,
    FIGHT_FLOOR_STATUS_WAZA_JOUTAI_DATA_ID = 59,
    FIGHT_FLOOR_STATUS_WAZA_INIT_JOUTAI_DATA_ID = 60,
    FIGHT_FLOOR_STATUS_IS_HIT = 61,
    FIGHT_FLOOR_STATUS_WAZA_DAMAGE = 62,
    FIGHT_FLOOR_STATUS_WAZA_IRYOKU = 63,
    FIGHT_FLOOR_STATUS_WAZA_TARGET_ID = 64,
    FIGHT_FLOOR_STATUS_WAZA_HIT_DAMAGE = 65,
    FIGHT_FLOOR_STATUS_DEFENSE_POKEMON_PTR = 66,
    FIGHT_FLOOR_STATUS_DEFENSE_POKEMON_PTR_AND_WAZA_TARGET = 67, /* like the
        plain pointer, but also recomputes the attack waza target. */
    FIGHT_FLOOR_STATUS_ESCAPE_POKEMON_PTR = 68,
    FIGHT_FLOOR_STATUS_IREKAE_POKEMON_PTR = 69,
    FIGHT_FLOOR_STATUS_KIZETU_POKEMON_PTR = 70,
    FIGHT_FLOOR_STATUS_TUIKAKOUKA_POKEMON_PTR = 71,
    FIGHT_FLOOR_STATUS_TOKUSEI_POKEMON_PTR = 72,
    FIGHT_FLOOR_STATUS_ITEM_POKEMON_PTR = 73,
    FIGHT_FLOOR_STATUS_ENCOUNT_FLOOR_ID = 74, /* floor ID of the running encounter. */
    FIGHT_FLOOR_STATUS_APPOINT_POKEMON_PTR = 75,
    FIGHT_FLOOR_STATUS_APPOINT_TRAINER_PTR = 76,
    FIGHT_FLOOR_STATUS_APPOINT_WAZA_PTR = 77,
    FIGHT_FLOOR_STATUS_APPOINT_SIDE_PTR = 78,
    FIGHT_FLOOR_STATUS_APPOINT_ITEM_PTR = 79,
    FIGHT_FLOOR_STATUS_APPOINT_MSG_ID = 80,
    FIGHT_FLOOR_STATUS_ATTACK_MSG_ID = 81,
    FIGHT_FLOOR_STATUS_CRITICAL_MSG_ID = 82,
    FIGHT_FLOOR_STATUS_WAZAKOUKA_MSG_ID = 83,
    FIGHT_FLOOR_STATUS_APPOINT_POKEMON_DATA_ID = 84,
    FIGHT_FLOOR_STATUS_APPOINT_WAZA_DATA_ID = 85,
    FIGHT_FLOOR_STATUS_APPOINT_ITEM_DATA_ID = 86,
    FIGHT_FLOOR_STATUS_APPOINT_TOKUSEI_DATA_ID = 87,
    FIGHT_FLOOR_STATUS_FIGHT_POKEMON_ENTRY_CNT = 88,
    FIGHT_FLOOR_STATUS_FIGHT_OUT_POKEMON_PTR = 89,
    FIGHT_FLOOR_STATUS_FIGHT_OUT_POKEMON_PTR_ARY_PTR = 90,
    FIGHT_FLOOR_STATUS_FIRST_ATTACK_RND = 91,
    FIGHT_FLOOR_STATUS_FIGHT_RESULT_ID = 92,
    FIGHT_FLOOR_STATUS_FIGHT_OUT_BANME_POKEMON_PTR = 93,
    FIGHT_FLOOR_STATUS_ITEM_QUANTITY_MSG_VALUE = 94, /* fills the item
        quantity message value without touching floor state. */
    FIGHT_FLOOR_STATUS_RESERVED = 95, /* needs the caller's floor; not resolved
        through the current fight. */
    FIGHT_FLOOR_STATUS_SELECTOR_LIMIT = 96
};

/* Sentinel returned when no status panel is selected; zero is not a panel. */
enum {
    FIGHT_STATUS_MENU_NONE = 0
};

/* Menu IDs accepted by the fight status-menu count accessors. */
enum FightStatusMenuId {
    FIGHT_STATUS_MENU_LEFT = 55,
    FIGHT_STATUS_MENU_UP = 56,
    FIGHT_STATUS_MENU_DOWN = 64,
    FIGHT_STATUS_MENU_RIGHT = 65
};

/* Values stored in the fight status-menu counters. */
enum FightStatusMenuCount {
    FIGHT_STATUS_MENU_INACTIVE = -1,
    FIGHT_STATUS_MENU_CLOSE_DELAY = 15
};

enum FightFloorSwitchResult {
    FIGHT_FLOOR_SWITCH_ALLOWED = 0,
    FIGHT_FLOOR_SWITCH_BLOCKED_BY_STATUS = 1,
    FIGHT_FLOOR_SWITCH_BLOCKED_BY_ABILITY = 2
};

enum {
    FIGHT_FLOOR_SIDE_COUNT = 2,
    FIGHT_OUT_POKEMON_COUNT = 8,
    FIGHT_FLOOR_JOUTAI_COUNT = 1
};

enum FightFloorStockPokemonCount {
    FIGHT_FLOOR_STOCK_POKEMON_COUNT = 6
};

typedef struct FIGHT_FLOOR_STOCK_POKEMON FIGHT_FLOOR_STOCK_POKEMON;

struct FIGHT_FLOOR_STOCK_POKEMON {
    Pokemon mPokemon;
    u8 mBallId;
    u8 mReserved[3];
};

typedef struct FIGHT_FLOOR FIGHT_FLOOR;

struct FIGHT_FLOOR {
    _JOUTAI mJoutaiAry[FIGHT_FLOOR_JOUTAI_COUNT];
    u16 mTurnCount;
    u16 mEncountDataId;
    FIGHT_SIDE mSideAry[FIGHT_FLOOR_SIDE_COUNT];
    FightOutPokemon *mAttackPokemon;
    FightOutPokemon *mDefensePokemon;
    FightOutPokemon *mEscapePokemon;
    FightOutPokemon *mIrekaePokemon;
    FightOutPokemon *mKizetuPokemon;
    FightOutPokemon *mTuikakoukaPokemon;
    FightOutPokemon *mTokuseiPokemon;
    FightOutPokemon *mItemPokemon;
    FightOutPokemon *mAppointPokemon;
    FIGHT_TRAINER *mAppointTrainer;
    FIGHT_WAZA *mAppointWaza;
    FIGHT_SIDE *mAppointSide;
    void *mAppointItem;
    u32 mAttackMsgId;
    u32 mCriticalMsgId;
    u32 mWazakoukaMsgId;
    u32 mAppointMsgId;
    u16 mAppointPokemonDataId;
    u16 mAppointWazaDataId;
    u16 mAppointItemDataId;
    u16 mAppointTokuseiDataId;
    s16 mFightPokemonEntryCnt;
    u16 mReserved;
    FightOutPokemon *mFightOutPokemonPtrAry[FIGHT_OUT_POKEMON_COUNT];
    u16 mFirstAttackRnd;
    u8 mStockPokemonNum;
    u8 mStockPokemonPadding;
    FIGHT_FLOOR_STOCK_POKEMON mStockPokemonAry[FIGHT_FLOOR_STOCK_POKEMON_COUNT];
};

typedef struct FIGHT_FLOOR_DATA FIGHT_FLOOR_DATA;

struct FIGHT_FLOOR_DATA {
    u8 mTikeiDataId;
    u8 mReserved;
    u16 mFloorDataId;
    u32 mName;
    u32 mSyoukaiWzxDataId;
    u32 mBgmSndId;
    u32 mEnvSndId;
    u16 mFightSideDataId[FIGHT_FLOOR_SIDE_COUNT];
};

extern u32 encount_floor_id;
extern u32 *fight_floor_data_number;
extern FIGHT_FLOOR_DATA *fight_floor_data;
extern FIGHT_FLOOR fight_floor;

#ifdef __cplusplus
extern "C" {
#endif
struct Pokemon *fightFloorGetDefPokemonPtr(FIGHT_FLOOR *floor);
u32 fightFloorGetStatus(FIGHT_FLOOR *floor, u16 dataId, u16 selector, u16 index);
void fightFloorBiosSetEncountFloorId(u32 floorId);
u32 fightFloorBiosGetEncountFloorId(void);
void fightFloorBiosSetItemPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
FightOutPokemon *fightFloorBiosGetItemPokemonPtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetTokuseiPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
FightOutPokemon *fightFloorBiosGetTokuseiPokemonPtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetTuikakoukaPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
FightOutPokemon *fightFloorBiosGetTuikakoukaPokemonPtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetKizetuPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
FightOutPokemon *fightFloorBiosGetKizetuPokemonPtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetFirstAttackRnd(FIGHT_FLOOR *floor, u16 random);
u16 fightFloorBiosGetFirstAttackRnd(FIGHT_FLOOR *floor);
void fightFloorBiosSetFightOutPokemonPtrAryPtr(FIGHT_FLOOR *floor, FightOutPokemon **pokemonAry);
FightOutPokemon **fightFloorBiosGetFightOutPokemonPtrAryPtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetFightOutPokemonPtrAry(FIGHT_FLOOR *floor, u16 slot, FightOutPokemon *pokemon);
FightOutPokemon *fightFloorBiosGetFightOutPokemonPtrAry(FIGHT_FLOOR *floor, u16 slot);
void fightFloorBiosSetFightPokemonEntryCnt(FIGHT_FLOOR *floor, s16 count);
s16 fightFloorBiosGetFightPokemonEntryCnt(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointTokuseiDataId(FIGHT_FLOOR *floor, u16 dataId);
u16 fightFloorBiosGetAppointTokuseiDataId(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointItemDataId(FIGHT_FLOOR *floor, u16 dataId);
u16 fightFloorBiosGetAppointItemDataId(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointWazaDataId(FIGHT_FLOOR *floor, u16 dataId);
u16 fightFloorBiosGetAppointWazaDataId(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointPokemonDataId(FIGHT_FLOOR *floor, u16 dataId);
u16 fightFloorBiosGetAppointPokemonDataId(FIGHT_FLOOR *floor);
void fightFloorBiosSetWazakoukaMsgId(FIGHT_FLOOR *floor, u32 msgId);
u32 fightFloorBiosGetWazakoukaMsgId(FIGHT_FLOOR *floor);
void fightFloorBiosSetCriticalMsgId(FIGHT_FLOOR *floor, u32 msgId);
u32 fightFloorBiosGetCriticalMsgId(FIGHT_FLOOR *floor);
void fightFloorBiosSetAttackMsgId(FIGHT_FLOOR *floor, u32 msgId);
u32 fightFloorBiosGetAttackMsgId(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointMsgId(FIGHT_FLOOR *floor, u32 msgId);
u32 fightFloorBiosGetAppointMsgId(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointItemPtr(FIGHT_FLOOR *floor, void *item);
void *fightFloorBiosGetAppointItemPtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointSidePtr(FIGHT_FLOOR *floor, FIGHT_SIDE *side);
FIGHT_SIDE *fightFloorBiosGetAppointSidePtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointWazaPtr(FIGHT_FLOOR *floor, FIGHT_WAZA *waza);
FIGHT_WAZA *fightFloorBiosGetAppointWazaPtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointTrainerPtr(FIGHT_FLOOR *floor, FIGHT_TRAINER *trainer);
FIGHT_TRAINER *fightFloorBiosGetAppointTrainerPtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetAppointPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
FightOutPokemon *fightFloorBiosGetAppointPokemonPtr(FIGHT_FLOOR *floor);
void fightFloorBiosSetIrekaePokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloorBiosSetEscapePokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloorBiosSetDefensePokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloorBiosSetAttackPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
FightOutPokemon *fightFloorBiosGetAttackPokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloorBiosGetDefensePokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloorBiosGetEscapePokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloorBiosGetIrekaePokemonPtr(FIGHT_FLOOR *floor);
FIGHT_SIDE *fightFloorBiosGetFightSidePtr(FIGHT_FLOOR *floor, u16 sideIndex);
_JOUTAI *fightFloorBiosGetJoutaiPtr(FIGHT_FLOOR *floor, u16 index);
u16 fightFloorBiosGetEncountDataId(FIGHT_FLOOR *floor);
u16 fightFloorBiosGetTurnCount(FIGHT_FLOOR *floor);
void fightFloorBiosSetEncountDataId(FIGHT_FLOOR *floor, u16 dataId);
void fightFloorBiosSetTurnCount(FIGHT_FLOOR *floor, u16 count);
void fightFloorDataBiosSetName(FIGHT_FLOOR_DATA *data, u32 name);
u32 fightFloorDataBiosGetName(FIGHT_FLOOR_DATA *data);
void fightFloorDataBiosSetEnvSndId(FIGHT_FLOOR_DATA *data, u32 dataId);
void fightFloorDataBiosSetBgmSndId(FIGHT_FLOOR_DATA *data, u32 dataId);
void fightFloorDataBiosSetSyoukaiWzxDataId(FIGHT_FLOOR_DATA *data, u32 dataId);
void fightFloorDataBiosSetTikeiDataId(FIGHT_FLOOR_DATA *data, u8 dataId);
void fightFloorDataBiosSetFightSideDataId(FIGHT_FLOOR_DATA *data, u8 side, u16 dataId);
void fightFloorDataBiosSetFloorDataId(FIGHT_FLOOR_DATA *data, u16 dataId);
u32 fightFloorDataBiosGetEnvSndId(FIGHT_FLOOR_DATA *data);
u32 fightFloorDataBiosGetBgmSndId(FIGHT_FLOOR_DATA *data);
u32 fightFloorDataBiosGetSyoukaiWzxDataId(FIGHT_FLOOR_DATA *data);
u8 fightFloorDataBiosGetTikeiDataId(FIGHT_FLOOR_DATA *data);
u16 fightFloorDataBiosGetFightSideDataId(FIGHT_FLOOR_DATA *data, u8 side);
u16 fightFloorDataBiosGetFloorDataId(FIGHT_FLOOR_DATA *data);
FIGHT_FLOOR_DATA *fightFloorDataBiosGetPtr(u16 index);
u8 fightFloorIsGcHeroWin(FIGHT_FLOOR *floor, u16 result);
void fightFloor_SetEncountFloorId(FIGHT_FLOOR *floor, u32 floorID);
u32 fightFloorSetStatus(FIGHT_FLOOR *floor, u16 floorDataId, u16 status, u16 index, u32 value);
u32 fightFloorDB_GetEnvSndId(u16 floorDataId);
u32 fightFloorGetBgmSndDataId(FIGHT_FLOOR *floor);
u32 fightFloorDB_GetBgmSndId(u16 floorDataId);
u32 fightFloorDB_GetSyoukaiWzxDataId(u16 floorDataId);
u8 fightFloorDB_GetTikeiDataId(u16 floorDataId);
u16 fightFloorDB_GetFightSideDataId(u16 floorDataId, u8 side);
u16 fightFloorDB_GetFloorDataId(u16 floorDataId);
FIGHT_TRAINER *fightFloorGetGcHeroFightTrainerPtr(FIGHT_FLOOR *floor);
FIGHT_TRAINER *fightFloorGetFightPokemonPtrToFightTrainerPtr(FIGHT_FLOOR *floor, FightPokemon *pokemon);
FIGHT_FLOOR *fightFloorBiosGetFightFloorPtr(void);
void *fightFloorGetNowPtr(void);
void fightFloorSetTimeOutAllFightResult(void *floor);
void fightFloor_SetStockPokemonBallID(FIGHT_FLOOR *floor, u8 slot, u8 ballId);
u8 fightFloor_GetStockPokemonBallID(FIGHT_FLOOR *floor, u8 slot);
void fightFloor_SetStockPokemonPtr(FIGHT_FLOOR *floor, u8 slot, Pokemon *pokemon);
Pokemon *fightFloor_GetStockPokemonPtr(FIGHT_FLOOR *floor, u8 slot);
void fightFloor_SetStockPokemonNum(FIGHT_FLOOR *floor, u8 count);
s32 fightFloor_GetStockPokemonNum(FIGHT_FLOOR *floor);
void fightFloor_SetfightoutPokemonPtrAryPtr(FIGHT_FLOOR *floor, FightOutPokemon **pokemonAry);
void fightFloor_SetfightPokemonEntryCnt(FIGHT_FLOOR *floor, s16 count);
u32 fightFloor_GetFightKind(void *floor);
u8 fightFloor_GetFightKindRegulationFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindHostEnemyMsgFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindPokemonStatusMenuSubbarFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindDarkpokemonHypermodeFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindMonohiroiFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindDorobouFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindBossFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindKeikentihueruFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindDoItemSoubiTokukoutokubouupFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindDoHizukiMiyaburiFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindDoCriticalAttackFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindGetInfectPokerusFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindGetFriendFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindGetNekoniKobanFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindGetOkaneFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindOkanePoolFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindGetExpFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindDrawFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindNigeruFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindUseItemFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindDoZukanTukamaetaFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindDoZukanMitaFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindDoBadgeCheckFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindBackSaveDataFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightTypeEntryPokemonNum(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightTypeTrainerNum(FIGHT_FLOOR *floor);
u32 fightFloor_GetEnvSndId(FIGHT_FLOOR *floor);
u32 fightFloor_GetBgmSndId(FIGHT_FLOOR *floor);
u32 fightFloor_GetSyoukaiWzxDataId(FIGHT_FLOOR *floor);
u8 fightFloor_GetTikeiDataId(FIGHT_FLOOR *floor);
u16 fightFloor_GetFightFloorDataId(FIGHT_FLOOR *floor);
u32 fightFloor_GetTrainer(FIGHT_FLOOR *floor);
u16 fightFloor_GetFirstAttackRnd(FIGHT_FLOOR *floor);
FightOutPokemon **fightFloor_GetFightOutPokemonPtrAryPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloor_GetFightOutPokemonPtrAry(FIGHT_FLOOR *floor, u16 index);
s16 fightFloor_GetFightPokemonEntryCnt(FIGHT_FLOOR *floor);
u32 fightFloor_GetAppointMsgId(FIGHT_FLOOR *floor);
u8 fightFloorIsLastActionFightOutPokemon(FIGHT_FLOOR *floor);
u8 fightFloorIsUseFightTimerCommand(FIGHT_FLOOR *floor);
u8 fightFloorCheckFightActionFightOutPokemonIrekaeSelect(void *battleData, FightOutPokemon *fightOutPokemon, FightOutPokemon **out);
u8 fightFloor_GetFightType(FIGHT_FLOOR *floor);
u8 fightFloorIsNowEncountTuusinTaisen(FIGHT_FLOOR *floor);
void fightFloorInitFightOutPokemonPtrAryFightWazaJoutai(FIGHT_FLOOR *floor);
void fightFloor_SetDefPokemonAndAtkFightWazaTargetId(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetDefensePokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetAttackPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetItemPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetTokuseiPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetTuikakoukaPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetKizetuPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetIrekaePokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetEscapePokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetAppointPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetAppointItemDataId(FIGHT_FLOOR *floor, u16 item);
void fightFloor_SetAppointPokemonDataId(FIGHT_FLOOR *floor, u16 dataId);
void fightFloor_SetAppointPokemonWazaDataId(FIGHT_FLOOR *floor, u16 dataId);
void fightFloor_SetAppointTokuseiDataId(FIGHT_FLOOR *floor, u16 dataId);
void fightFloor_SetAttackPokemonFightWazaTargetId(FIGHT_FLOOR *floor, u16 targetId);
void fightFloorInitFightTarget(FIGHT_FLOOR *floor);
u8 fightFloorCheckWriteJoutaiDataId(FIGHT_FLOOR *floor, u16 joutaiDataId);
void fightFloorWriteJoutaiDataId(FIGHT_FLOOR *floor, u16 joutaiDataId, u16 value);
void fightFloorSetFightResultId(FIGHT_FLOOR *floor, u16 resultId);
FightOutPokemon *fightFloorGetFightOutBanmeToValidFightOutPokemon(FIGHT_FLOOR *floor, u16 banme);
FIGHT_TRAINER *fightFloorGetFightOutPokemonPtrToFightTrainerPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
u8 fightFloorLoopValidFightOutPokemon(FIGHT_FLOOR *floor, u8 (*callback)(void *, u16, void *), void *argument, u32 mode);
void fightFloor_SetAttackPokemonFightWazaJoutaiDataId(FIGHT_FLOOR *floor, u16 status);
u16 fightFloor_GetFightSideFightOutPokemonSideMax(FIGHT_FLOOR *floor);
u16 fightFloorGetValidFightOutPokemonCount(FIGHT_FLOOR *floor, u32 side, FightOutPokemon *pokemon, u32 validOnly);
FightOutPokemon *fightFloor_GetFightOutPokemonFromDefaultSortData(u8 index);
FightOutPokemon *fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(FIGHT_FLOOR *floor, u32 ability, u32 iterationMode, u32 relationship, FightOutPokemon *referencePokemon);

FightOutPokemon *fightFloor_GetAttackPokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloor_GetDefensePokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloor_GetIrekaePokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloor_GetEscapePokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloor_GetKizetuPokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloor_GetTuikakoukaPokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloor_GetTokuseiPokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloor_GetItemPokemonPtr(FIGHT_FLOOR *floor);
FightOutPokemon *fightFloor_GetAppointPokemonPtr(FIGHT_FLOOR *floor);
FIGHT_TRAINER *fightFloor_GetAppointTrainerPtr(FIGHT_FLOOR *floor);
FIGHT_WAZA *fightFloor_GetAppointWazaPtr(FIGHT_FLOOR *floor);
FIGHT_SIDE *fightFloor_GetAppointSidePtr(FIGHT_FLOOR *floor);
void *fightFloor_GetAppointItemPtr(FIGHT_FLOOR *floor);
u8 fightFloorGetNowTenkouDataId(FIGHT_FLOOR *floor, s32 checkWeatherSuppressingAbilities);
u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(FIGHT_FLOOR *floor, void *trainer, FightOutPokemon **pokemonArray, u8 allies, u8 deployedOnly);
u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(FIGHT_FLOOR *floor, void *trainer, u8 allies, u8 deployedOnly, u16 wazaDataId, FightOutPokemon *target);
u16 fightFloorGetFightTrainerFightPokemonPtrAry(FIGHT_FLOOR *floor, void *trainer, FightPokemon **pokemonArray, u8 allies, u8 deployedOnly);
u16 fightFloor_GetTurnCount(FIGHT_FLOOR *floor);
void fightFloor_SetTurnCount(FIGHT_FLOOR *floor, u16 count);
void fightFloor_SetEncountDataId(FIGHT_FLOOR *floor, u16 dataId);
u32 fightFloor_GetEncountFloorId(void);
u8 fightFloor_GetFightTypeFightoutPokemonNum(FIGHT_FLOOR *floor);
void fightFloor_SetFisstAttackRnd(FIGHT_FLOOR *floor, u16 value);
u16 fightFloor_GetEncountDataId(FIGHT_FLOOR *floor);
u8 fightFloor_GetIsJoutaiDataId(FIGHT_FLOOR *floor, u16 joutaiDataId);
_JOUTAI *fightFloor_GetJoutaiPtr(FIGHT_FLOOR *floor, u16 index);
u8 fightFloorIsJoutaiDataId(FIGHT_FLOOR *floor, u16 joutaiDataId);
void fightFloor_SetAttackMsgId(FIGHT_FLOOR *floor, s32 messageId);
void fightFloor_SetAppointMsgPtr(FIGHT_FLOOR *floor, s32 messageId);
void fightFloor_SetCriticakMsgId(FIGHT_FLOOR *floor, s32 messageId);
void fightFloor_SetWazakoukaMsgId(FIGHT_FLOOR *floor, s32 messageId);
FIGHT_SIDE *fightFloorGetValidFightSidePtr(FIGHT_FLOOR *floor, u16 sideIndex);
FIGHT_SIDE *fightFloor_GetFightSidePtr(FIGHT_FLOOR *floor, u16 sideIndex);
void fightFloorReplaceFightOutPokemonAttackToDefense(FIGHT_FLOOR *floor);
u8 fightFloorIsNowEncountSnatchOnly(FIGHT_FLOOR *floor);
u8 fightFloorIsNowEncountHostPokemonKime(FIGHT_FLOOR *floor);
u8 fightFloorIsNowEncountHostEnemyPokemonKime(FIGHT_FLOOR *floor);
s8 fightFloor_GetStatusMenuCnt(s32 menuID);
void fightFloor_SetStatusMenuCnt(s32 menuID, s8 count);
u8 fightFloorLoopValidFightTrainer(FIGHT_FLOOR *floor, u8 (*callback)(void *, u16, void *), void *argument, u32 mode);
void fightFloorSetShadow(void);

#ifdef __cplusplus
}
#endif

#endif
