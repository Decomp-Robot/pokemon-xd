#ifndef ITEM_FIGHT_ITEM_H
#define ITEM_FIGHT_ITEM_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
u16 fightItem_GetTargetDataId(void *item);
s32 fightItem_GetCount(void *item);
s32 fightItem_GetBuff(void *item);
#ifdef __cplusplus
}
#endif

#endif
