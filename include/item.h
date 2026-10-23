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
#ifdef __cplusplus
}
#endif
#endif
