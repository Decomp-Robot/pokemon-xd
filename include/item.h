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
enum ItemStatusKind {
    ITEM_STATUS_ITEM_DATA_ID = 27,
    ITEM_STATUS_NUM = 28
};
s32 itemGetStatus(ItemSlot* item, u16 itemID, u16 kind, s32 option);
void itemReplace(ItemSlot* first, ItemSlot* second);
s32 itemDecItemDataIdAry(ItemSlot* items, u16 count, u16 itemID, u16 quantity,
                         s16 firstSlot, u16 maxQuantity, u8 sort);
s32 itemAddItemDataIdAry(ItemSlot* items, u16 count, u16 itemID, u16 quantity,
                         s16 firstSlot, u16 maxQuantity, u8 sort, u8 option);
void itemInitAry(ItemSlot* items, u16 count);
u8 itemCheckValid(ItemSlot *slot);
void itemSortAryByPriority(void *ary, u16 num, long kind);
u16 itemGetQuantityInAry(const void *ary, u16 num, u16 itemID, u16 max);
#ifdef __cplusplus
}
#endif
#endif
