#include <fightItem.h>

extern "C" {

void fightItemCreate(FIGHT_ITEM* item, u16 itemDataId, u16 targetDataId, s32 count) {
    if (item != 0) {
        fightItemInit(item);
        fightItem_SetItemDataId(item, itemDataId);
        fightItem_SetTargetDataId(item, targetDataId);
        fightItem_SetCount(item, count);
    }
}

void fightItemInit(FIGHT_ITEM* item) {
    if (item != 0) {
        fightItem_SetItemDataId(item, FIGHT_ITEM_DATA_ID_NONE);
        fightItem_SetTargetDataId(item, FIGHT_ITEM_TARGET_DATA_ID_NONE);
        fightItem_SetCount(item, FIGHT_ITEM_SLOT_NONE);
        fightItem_SetBuff(item, 0);
    }
}

}
