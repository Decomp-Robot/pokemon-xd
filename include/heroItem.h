#ifndef HERO_ITEM_H
#define HERO_ITEM_H

#include <types.h>
#include <item.h>

#ifdef __cplusplus
class Hero;
extern "C" {
#else
typedef struct Hero Hero;
#endif

// Inventory operations return -1 if the item has no inventory array.
// An item position of -1 selects the slot by item ID.
s32 heroItemCheckAddItemDataId(Hero *hero, u16 itemID);
s32 heroItemAddItemDataId(Hero *hero, u16 itemID, u16 count, s16 itemPosition);
s32 heroItemDecItemDataId(Hero *hero, u16 itemID, u16 count, s16 itemPosition);

// A null hero selects the current hero. All metadata outputs are optional.
// Slot count is the array length; stock maximum is the limit per stack.
ItemSlot *heroItemGetItemKindToItemAryPtr(Hero *hero, u8 itemKind,
                                     u16 *outSlotCount, u16 *outStockMaximum,
                                     u8 *outSortMode, u8 *outStackMode);
ItemSlot *heroHizukiItemGetItemAryPtr(Hero *hero, u16 *outSlotCount,
                                 u16 *outStockMaximum, u8 *outSortMode,
                                 u8 *outStackMode);

#ifdef __cplusplus
}
#endif

#endif
