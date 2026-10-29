#ifndef FIGHTSIDE_H
#define FIGHTSIDE_H

#include <fightTrainer.h>
#include <joutai.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
u8 fightSideCheckWriteJoutaiDataId(void *side, u16 status);
void fightSideWriteJoutaiDataId(void *side, u16 status, u16 targetId);

typedef struct FIGHT_SIDE FIGHT_SIDE;

FIGHT_TRAINER *fightSideGetValidFightTrainerPtr(FIGHT_SIDE *side, u16 index);

void fightSideInitJoutaiDataId(FIGHT_SIDE *side, u16 id);

void fightSide_SetMakibisiCheckFlag(FIGHT_SIDE *side, u8 flag);
void fightSide_SetFightSideDataId(FIGHT_SIDE *side, u16 dataId);
u8 fightSide_GetMakibisiCheckFlag(FIGHT_SIDE *side);
FIGHT_TRAINER *fightSide_GetFightTrainerPtr(FIGHT_SIDE *side, u16 index);
_JOUTAI *fightSide_GetJoutaiPtr(FIGHT_SIDE *side, u16 index);
u16 fightSide_GetFightSideDataId(FIGHT_SIDE *side);
u16 fightSideDB_GetYrot(u16 dataId);

typedef struct FIGHT_SIDE_DATA {
    u16 mYrot;
    u16 mReserved;
    u32 mFightTrainerStatusMenuDataId[2];
    u32 mFightoutPokemonStatusMenuDataId[2];
} FIGHT_SIDE_DATA;

FIGHT_SIDE_DATA *fightSideDataBiosGetPtr(u16 dataId);

u32 fightSideGetStatus(FIGHT_SIDE *side, u16 dataId, u16 selector, u16 index);

#ifdef __cplusplus
}
#endif

#endif
