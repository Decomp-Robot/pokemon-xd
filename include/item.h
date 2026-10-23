#ifndef ITEM_H
#define ITEM_H
#include <types.h>
typedef struct ItemSlot {
    u16 mItemID;
    u16 mNum;
} ItemSlot;

#ifdef __cplusplus
extern "C" {
#endif
u8 itemCheckValid(ItemSlot *slot);
void itemSortAryByPriority(void *ary, u16 num, long kind);
u16 itemGetQuantityInAry(const void *ary, u16 num, u16 itemID, u16 max);
#ifdef __cplusplus
}
#endif
#endif
