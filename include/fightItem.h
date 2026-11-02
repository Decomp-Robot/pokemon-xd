#ifndef FIGHT_ITEM_H
#define FIGHT_ITEM_H

#include <itemFightItem.h>

// Values fightItemInit stores into a battle item that carries no selection.
enum {
    FIGHT_ITEM_DATA_ID_NONE = 0, // no item selected.
    FIGHT_ITEM_TARGET_DATA_ID_NONE = 0, // no fight target selected.
    FIGHT_ITEM_SLOT_NONE = -1 // no bag slot; the item ID selects the slot.
};

#ifdef __cplusplus
extern "C" {
#endif

void fightItemCreate(FIGHT_ITEM *item, u16 itemDataId, u16 targetDataId, s32 count);
void fightItemInit(FIGHT_ITEM *item);

#ifdef __cplusplus
}
#endif

#endif
