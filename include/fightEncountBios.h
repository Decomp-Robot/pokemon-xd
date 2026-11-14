#ifndef FIGHT_ENCOUNT_BIOS_H
#define FIGHT_ENCOUNT_BIOS_H

#include <types.h>

/* One trainer entry of an encounter record: the deck its side fights with, the
 * trainer data it plays and the controller its player holds. */
typedef struct {
    u16 mDeckGrpId;
    u16 mTrainerDataId;
    u32 mGSInputDevice;
} FightEncountTrainerInfo;

/* One record of the encounter data table: the fight it sets up and the four
 * trainers that take part in it. */
typedef struct {
    u8 mFightKind;
    u8 mTrainer;
    u8 mPokemon;
    u8 mEntry;
    u8 mZenmetuFlag;
    u8 mUnknown05;
    u16 mFightFloorDataId;
    u16 mDiskId; /* 0 uses the trainer entry's own deck group, nonzero the battle disk's */
    u16 mUnknown0A;
    u32 mFightName;
    u32 mBgmSndId;
    u32 mWipeId;
    u32 mSyoukaiWzxDataId;
    FightEncountTrainerInfo mTrainerInfo[4];
} FightEncountRecord;

#ifdef __cplusplus
extern "C" {
#endif

u8 *fightEncountDataBiosGetPtr(u16 encountNo);
u32 fightEncountGetEnvSndDataId(u16 encountDataId);
u32 fightEncountDataBiosGetFightKind(void* data);
u8 fightEncountDataBiosGetFightType(void *data);
void fightEncountDataBiosSetFightKind(void* data, u8 value);
u32 fightEncountDataBiosGetTrainer(void* data);
void fightEncountDataBiosSetTrainer(void* data, u8 value);
u32 fightEncountDataBiosGetPokemon(void* data);
void fightEncountDataBiosSetPokemon(void* data, u8 value);
u32 fightEncountDataBiosGetEntry(void* data);
void fightEncountDataBiosSetEntry(void* data, u8 value);
u16 fightEncountDataBiosGetFightFloorDataId(void* data);
void fightEncountDataBiosSetFightFloorDataId(void* data, u16 value);
u32 fightEncountDataBiosGetFightTrainerDataId(void* data, s32 player);
void fightEncountDataBiosSetFightTrainerDataId(void* data, s32 player, u16 trainerDataId);
u32 fightEncountDataBiosGetGSInputDevice(void* data, s32 player);
void fightEncountDataBiosSetGSInputDevice(void* data, s32 player, s32 devId);
void fightEncountDataBiosSetDiskId(void *data, s32 id);
u16 fightEncountDataBiosGetFightDeckGrpId(void *data, u8 index);
u32 fightEncountDataBiosGetFightName(void *data);

extern u32 *fight_encount_data_number;

#ifdef __cplusplus
}
#endif

#endif
