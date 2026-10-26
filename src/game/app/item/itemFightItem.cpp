#include <itemFightItem.h>

extern "C" {

void fightItem_SetBuff(FIGHT_ITEM* item, s32 buff) {
    if (item != 0) {
        item->buff = buff;
    }
}

void fightItem_SetCount(FIGHT_ITEM* item, s32 count) {
    if (item != 0) {
        item->count = count;
    }
}

void fightItem_SetTargetDataId(FIGHT_ITEM* item, u16 targetDataId) {
    if (item != 0) {
        item->targetDataId = targetDataId;
    }
}

void fightItem_SetItemDataId(FIGHT_ITEM* item, u16 itemDataId) {
    if (item != 0) {
        item->itemDataId = itemDataId;
    }
}

s32 fightItem_GetBuff(FIGHT_ITEM* item) {
    if (item == 0) {
        return 0;
    }
    return item->buff;
}

s32 fightItem_GetCount(FIGHT_ITEM* item) {
    if (item == 0) {
        return 0;
    }
    return item->count;
}

u16 fightItem_GetTargetDataId(FIGHT_ITEM* item) {
    if (item == 0) {
        return 0;
    }
    return item->targetDataId;
}

u16 fightItem_GetItemDataId(FIGHT_ITEM* item) {
    if (item == 0) {
        return 0;
    }
    return item->itemDataId;
}

}
