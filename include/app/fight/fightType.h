#ifndef APP_FIGHT_FIGHTTYPE_H
#define APP_FIGHT_FIGHTTYPE_H

#include <types.h>

#define FIGHT_TYPE_ID_MASK 0xFFFF

#ifdef __cplusplus
extern "C" {
#endif

// Fight type data record of a fight: the number of Pokemon a side of the fight
// enters with and the number of Pokemon it has out at once.
u8 fightTypeDataBiosGetEntryPokemonNum(void *fightTypeData);
u8 fightTypeDataBiosGetFightoutPokemonNum(void *fightTypeData);
void *fightTypeDataBiosGetPtr(long type);
u8 fightTypeDataBiosGetTrainerNum(void *fightTypeData);

#ifdef __cplusplus
}
#endif

#endif
