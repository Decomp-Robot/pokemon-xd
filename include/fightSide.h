#ifndef FIGHTSIDE_H
#define FIGHTSIDE_H

#include <fightTrainer.h>
#include <joutai.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FIGHT_SIDE FIGHT_SIDE;

typedef struct FIGHT_SIDE_DATA {
    u16 mYrot;
    u16 mReserved;
    u32 mFightTrainerStatusMenuDataId[2];
    u32 mFightoutPokemonStatusMenuDataId[2];
} FIGHT_SIDE_DATA;

enum FightSideStatusSelector {
    FIGHT_SIDE_STATUS_NONE = 0,
    FIGHT_SIDE_STATUS_YROT = 1,
    FIGHT_SIDE_STATUS_FIGHT_TRAINER_STATUS_MENU_DATA_ID = 2,
    FIGHT_SIDE_STATUS_FIGHTOUT_POKEMON_STATUS_MENU_DATA_ID = 3,
    FIGHT_SIDE_STATUS_RESERVED = 4,
    FIGHT_SIDE_STATUS_SIDE_DATA_ID = 5,
    FIGHT_SIDE_STATUS_JOUTAI_PTR = 6,
    FIGHT_SIDE_STATUS_FIGHT_TRAINER_PTR = 7,
    FIGHT_SIDE_STATUS_MAKIBISI_CHECK_FLAG = 8,
    FIGHT_SIDE_STATUS_SELECTOR_LIMIT = 10
};

u8 fightSideCheckWriteJoutaiDataId(void *side, u16 dataId);
void fightSideWriteJoutaiDataId(void *side, u16 dataId, u16 targetId);
FIGHT_TRAINER *fightSideGetValidFightTrainerPtr(FIGHT_SIDE *side, u16 index);
u16 fightSideGetJoutaiUserFightTargetId(FIGHT_SIDE *side, u16 dataId);
void fightSideSetNowKaisuuJoutaiDataId(FIGHT_SIDE *side, u16 dataId, u8 nowKaisuu);
s32 fightSideGetNowKaisuuJoutaiDataId(FIGHT_SIDE *side, u16 dataId);
s32 fightSideGetKaisuuJoutaiDataId(FIGHT_SIDE *side, u16 dataId);
s32 fightSideGetCountAsJoutaiDataId(FIGHT_SIDE *side, u16 dataId);
u8 fightSideIsJoutaiDataId(FIGHT_SIDE *side, u16 dataId);
void fightSideInitJoutaiDataId(FIGHT_SIDE *side, u16 dataId);
u32 fightSideGetHikaeFightPokemonNum(
    FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum, u16 fightoutPokemonNum);
u32 fightSideGetNokoriFightPokemonNum(
    FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum);
u32 fightSideGetFightPokemonNum(
    FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum, u8 fightOutOnly);
u32 fightSideGetFightPokemonMaxHp(FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum);
u32 fightSideGetFightPokemonNokoriHp(FIGHT_SIDE *side, u16 trainerNum, u16 pokemonNum);
void fightSideGetFightTrainerGridParam(
    u16 sideDataId, u16 trainerNum, u16 trainerIndex, u8 *reverse, s8 *offset);
u32 fightSideGetDoFightTrainerCount(FIGHT_SIDE *side);
u8 fightSideCheckValid(FIGHT_SIDE *side);
void fightSideCreate(FIGHT_SIDE *side, u16 dataId);
void fightSideInitAry(FIGHT_SIDE *sideAry, u16 sideNum);
void fightSideInit(FIGHT_SIDE *side);
void fightSideSetStatus(FIGHT_SIDE *side, u16 dataId, u16 selector, u16 index, u32 value);
u32 fightSideGetStatus(FIGHT_SIDE *side, u16 dataId, u16 selector, u16 index);

void fightSide_SetMakibisiCheckFlag(FIGHT_SIDE *side, u8 flag);
void fightSide_SetFightSideDataId(FIGHT_SIDE *side, u16 dataId);
u8 fightSide_GetMakibisiCheckFlag(FIGHT_SIDE *side);
FIGHT_TRAINER *fightSide_GetFightTrainerPtr(FIGHT_SIDE *side, u16 index);
_JOUTAI *fightSide_GetJoutaiPtr(FIGHT_SIDE *side, u16 index);
u16 fightSide_GetFightSideDataId(FIGHT_SIDE *side);

void fightSideBiosSetMakibisiCheckFlag(FIGHT_SIDE *side, u8 flag);
void fightSideBiosSetFightSideDataId(FIGHT_SIDE *side, u16 dataId);
u8 fightSideBiosGetMakibisiCheckFlag(FIGHT_SIDE *side);
FIGHT_TRAINER *fightSideBiosGetFightTrainerPtr(FIGHT_SIDE *side, u16 index);
_JOUTAI *fightSideBiosGetJoutaiPtr(FIGHT_SIDE *side, u16 index);
u16 fightSideBiosGetFightSideDataId(FIGHT_SIDE *side);
u16 fightSideDB_GetYrot(u16 dataId);

FIGHT_SIDE_DATA *fightSideDataBiosGetPtr(u16 dataId);
void fightSideDataBiosSetYrot(FIGHT_SIDE_DATA *data, u16 yrot);
void fightSideDataBiosSetFightTrainerStatusMenuDataId(
    FIGHT_SIDE_DATA *data, u8 index, u32 menuDataId);
void fightSideDataBiosSetFightoutPokemonStatusMenuDataId(
    FIGHT_SIDE_DATA *data, u8 index, u32 menuDataId);
u16 fightSideDataBiosGetYrot(FIGHT_SIDE_DATA *data);
u32 fightSideDataBiosGetFightTrainerStatusMenuDataId(
    FIGHT_SIDE_DATA *data, u8 index);
u32 fightSideDataBiosGetFightoutPokemonStatusMenuDataId(
    FIGHT_SIDE_DATA *data, u8 index);

#ifdef __cplusplus
}
#endif

#endif
