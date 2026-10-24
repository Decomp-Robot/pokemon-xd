#ifndef FIGHTTARGET_H
#define FIGHTTARGET_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *fightTargetDataBiosGetPtr(u16 target);
s32 fightTargetDataBiosGetBuff(void *targetData);
u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(void *target, u16 fightType);
void *fightTargetGetPtrAsNowFightType(u16 kind, void *target);

#ifdef __cplusplus
}
#endif

#endif
