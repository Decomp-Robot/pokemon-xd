#include <fightItemBios.h>

extern "C" {

void fightItemBiosSetBuff(FIGHT_ITEM* item, s32 buff) {
    if (item != 0) {
        item->buff = buff;
    }
}

void fightItemBiosSetCount(FIGHT_ITEM* item, s32 count) {
    if (item != 0) {
        item->count = count;
    }
}

void fightItemBiosSetTargetDataId(FIGHT_ITEM* item, u16 targetDataId) {
    if (item != 0) {
        item->targetDataId = targetDataId;
    }
}

void fightItemBiosSetItemDataId(FIGHT_ITEM* item, u16 itemDataId) {
    if (item != 0) {
        item->itemDataId = itemDataId;
    }
}

s32 fightItemBiosGetBuff(FIGHT_ITEM* item) {
    if (item == 0) {
        return 0;
    }
    return item->buff;
}

s32 fightItemBiosGetCount(FIGHT_ITEM* item) {
    if (item == 0) {
        return 0;
    }
    return item->count;
}

u16 fightItemBiosGetTargetDataId(FIGHT_ITEM* item) {
    if (item == 0) {
        return 0;
    }
    return item->targetDataId;
}

u16 fightItemBiosGetItemDataId(FIGHT_ITEM* item) {
    if (item == 0) {
        return 0;
    }
    return item->itemDataId;
}

}
