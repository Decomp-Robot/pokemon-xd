#ifndef FIGHTTARGET_H
#define FIGHTTARGET_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *fightTargetDataBiosGetPtr(u32 target);
void *fightTargetDataBiosGetBuff(void *targetData);
u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(FightOutPokemon *pokemon, u8 fightType);
void *fightTargetGetPtrAsNowFightType(u16 kind, void *target);

#ifdef __cplusplus
}
#endif

#endif
