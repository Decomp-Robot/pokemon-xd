#ifndef FIGHT_ITEM_BIOS_H
#define FIGHT_ITEM_BIOS_H

#include <itemFightItem.h>

#ifdef __cplusplus
extern "C" {
#endif

void fightItemBiosSetBuff(FIGHT_ITEM *item, s32 buff);
void fightItemBiosSetCount(FIGHT_ITEM *item, s32 count);
void fightItemBiosSetTargetDataId(FIGHT_ITEM *item, u16 targetDataId);
void fightItemBiosSetItemDataId(FIGHT_ITEM *item, u16 itemDataId);
s32 fightItemBiosGetBuff(FIGHT_ITEM *item);
s32 fightItemBiosGetCount(FIGHT_ITEM *item);
u16 fightItemBiosGetTargetDataId(FIGHT_ITEM *item);
u16 fightItemBiosGetItemDataId(FIGHT_ITEM *item);

#ifdef __cplusplus
}
#endif

#endif
