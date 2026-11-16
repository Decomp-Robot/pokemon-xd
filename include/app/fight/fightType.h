#ifndef APP_FIGHT_FIGHTTYPE_H
#define APP_FIGHT_FIGHTTYPE_H

#include <types.h>

#define FIGHT_TYPE_ID_MASK 0xFFFF

// One entry of the fight type table: the number of trainers a side fields, the
// number of Pokemon it enters the fight with, the number of Pokemon it keeps
// out at once and the name the fight type is displayed with.
typedef struct FIGHT_TYPE_DATA {
    u8 mTrainerNum;
    u8 mEntryPokemonNum;
    u8 mFightoutPokemonNum;
    u32 mName;
} FIGHT_TYPE_DATA;

#ifdef __cplusplus
extern "C" {
#endif

u8 fightTypeDataBiosGetEntryPokemonNum(void *fightTypeData);
u8 fightTypeDataBiosGetFightoutPokemonNum(void *fightTypeData);
FIGHT_TYPE_DATA *fightTypeDataBiosGetPtr(u16 type);
u8 fightTypeDataBiosGetTrainerNum(void *fightTypeData);
u16 fightTypeGetFightSideFightOutPokemonMax(u16 fightType);

#ifdef __cplusplus
}
#endif

#endif
