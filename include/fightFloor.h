#ifndef FIGHT_FLOOR_H
#define FIGHT_FLOOR_H

#include <fightEncountData.h>

#include <fightPokemon.h>
#include <fightTrainer.h>
#include <joutai.h>
#include <types.h>

typedef struct FIGHT_WAZA FIGHT_WAZA;

/* Statuses fightFloorGetStatus reports for a floor record. */
enum FightFloorStatus {
    FIGHT_FLOOR_STATUS_FIGHT_TYPE = 20, /* fight type of the running encounter. */
};

enum FightFloorSwitchResult {
    FIGHT_FLOOR_SWITCH_ALLOWED = 0,
    FIGHT_FLOOR_SWITCH_BLOCKED_BY_STATUS = 1,
    FIGHT_FLOOR_SWITCH_BLOCKED_BY_ABILITY = 2
};

enum {
    FIGHT_FLOOR_SIDE_COUNT = 2,
    FIGHT_SIDE_TRAINER_COUNT = 2,
    FIGHT_SIDE_JOUTAI_COUNT = 6,
    FIGHT_OUT_POKEMON_COUNT = 8,
    FIGHT_FLOOR_JOUTAI_COUNT = 1
};

typedef struct FIGHT_SIDE FIGHT_SIDE;

struct FIGHT_SIDE {
#ifdef __cplusplus
    FIGHT_SIDE() {}
#endif

    u16 mFightSideDataId;
    u16 mReserved;
    _JOUTAI mJoutaiAry[FIGHT_SIDE_JOUTAI_COUNT];
    FIGHT_TRAINER mTrainerAry[FIGHT_SIDE_TRAINER_COUNT];
    u8 mMakibisiCheckFlag;
    u8 mPadding[3];
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
    u8 mUnknownTail[1202];
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

#ifdef __cplusplus
extern "C" {
#endif
struct Pokemon *fightFloorGetDefPokemonPtr(FIGHT_FLOOR *floor);
u32 fightFloorGetStatus(FIGHT_FLOOR *floor, u16 dataId, u16 selector, u32 index);
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
u32 fightFloorDB_GetEnvSndId(u16 floorDataId);
u32 fightFloorDB_GetBgmSndId(u16 floorDataId);
u32 fightFloorDB_GetSyoukaiWzxDataId(u16 floorDataId);
u8 fightFloorDB_GetTikeiDataId(u16 floorDataId);
u16 fightFloorDB_GetFightSideDataId(u16 floorDataId, u8 side);
u16 fightFloorDB_GetFloorDataId(u16 floorDataId);
FIGHT_TRAINER *fightFloorGetGcHeroFightTrainerPtr(FIGHT_FLOOR *floor);
FIGHT_TRAINER *fightFloorGetFightPokemonPtrToFightTrainerPtr(FIGHT_FLOOR *floor, FightPokemon *pokemon);
void *fightFloorBiosGetFightFloorPtr(void);
void *fightFloorGetNowPtr(void);
void fightFloorSetTimeOutAllFightResult(void *floor);
s32 fightFloor_GetStockPokemonNum(FIGHT_FLOOR *floor);
u32 fightFloor_GetFightKind(void *floor);
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
void fightFloor_SetAppointPokemonPtr(FIGHT_FLOOR *floor, FightOutPokemon *pokemon);
void fightFloor_SetAppointItemDataId(FIGHT_FLOOR *floor, u16 item);
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
u16 fightFloor_GetTurnCount(FIGHT_FLOOR *floor);
void fightFloor_SetTurnCount(FIGHT_FLOOR *floor, u16 count);
u8 fightFloor_GetFightKindDoCriticalAttackFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindGetOkaneFlag(FIGHT_FLOOR *floor);
u8 fightFloor_GetFightKindOkanePoolFlag(FIGHT_FLOOR *floor);
u16 fightFloor_GetFightTypeFightoutPokemonNum(FIGHT_FLOOR *floor);
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
void fightFloorReplaceFightOutPokemonAttackToDefense(FIGHT_FLOOR *floor);
u8 fightFloorIsNowEncountSnatchOnly(FIGHT_FLOOR *floor);
u8 fightFloorLoopValidFightTrainer(FIGHT_FLOOR *floor, u8 (*callback)(void *, u16, void *), void *argument, u32 mode);
void fightFloorSetShadow(void);

#ifdef __cplusplus
}
#endif

#endif
