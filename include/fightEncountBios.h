#ifndef FIGHT_ENCOUNT_BIOS_H
#define FIGHT_ENCOUNT_BIOS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 *fightEncountDataBiosGetPtr(u16 encountNo);
u32 fightEncountDataBiosGetFightKind(void* data);
void fightEncountDataBiosSetFightKind(void* data, u8 value);
u32 fightEncountDataBiosGetTrainer(void* data);
void fightEncountDataBiosSetTrainer(void* data, u8 value);
u32 fightEncountDataBiosGetPokemon(void* data);
void fightEncountDataBiosSetPokemon(void* data, u8 value);
u32 fightEncountDataBiosGetEntry(void* data);
void fightEncountDataBiosSetEntry(void* data, u8 value);
u32 fightEncountDataBiosGetFightFloorDataId(void* data);
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
