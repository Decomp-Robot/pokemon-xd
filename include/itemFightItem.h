#ifndef ITEM_FIGHT_ITEM_H
#define ITEM_FIGHT_ITEM_H

#include <types.h>

typedef struct FIGHT_ITEM {
    u16 itemDataId;
    u16 targetDataId;
    s32 count;
    s32 buff;
} FIGHT_ITEM;

#ifdef __cplusplus
extern "C" {
#endif
void fightItem_SetBuff(FIGHT_ITEM *item, s32 buff);
void fightItem_SetCount(FIGHT_ITEM *item, s32 count);
void fightItem_SetTargetDataId(FIGHT_ITEM *item, u16 targetDataId);
void fightItem_SetItemDataId(FIGHT_ITEM *item, u16 itemDataId);
s32 fightItem_GetBuff(FIGHT_ITEM *item);
s32 fightItem_GetCount(FIGHT_ITEM *item);
u16 fightItem_GetTargetDataId(FIGHT_ITEM *item);
u16 fightItem_GetItemDataId(FIGHT_ITEM *item);
#ifdef __cplusplus
}
#endif

#endif
