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
    ITEM_STATUS_INVALID = 0,
    ITEM_STATUS_NAME = 1,
    ITEM_STATUS_KIND = 2,
    ITEM_STATUS_PRICE = 3,
    ITEM_STATUS_IMPORTANT = 4,
    ITEM_STATUS_USEFUL = 5,
    ITEM_STATUS_DOC = 6,
    ITEM_STATUS_ITEM_SOUBI_DATA_ID = 7,
    ITEM_STATUS_FIGHT_USE_KOUKA_DATA_ID = 8,
    ITEM_STATUS_USE_FRIEND = 9,
    ITEM_STATUS_BUFF = 10,
    ITEM_STATUS_ITEM_DATA_END = 11,
    ITEM_STATUS_BALL_FIGHT_KOUKA_DATA_ID = 12,
    ITEM_STATUS_BALL_IN_WZX_DATA_ID = 13,
    ITEM_STATUS_BALL_OPEN_WZX_DATA_ID = 14,
    ITEM_STATUS_BALL_OUT_WZX_DATA_ID = 15,
    ITEM_STATUS_BALL_DOWNIN_WZX_DATA_ID = 16,
    ITEM_STATUS_BALL_THROW_WZX_DATA_ID = 17,
    ITEM_STATUS_BALL_SNATCH_ATTACK_WZX_DATA_ID = 18,
    ITEM_STATUS_BALL_SNATCH_BALLLAND_WZX_DATA_ID = 19,
    ITEM_STATUS_BALL_SNATCH_MISS_WZX_DATA_ID = 20,
    ITEM_STATUS_BALL_SNATCH_POKEOUT_WZX_DATA_ID = 21,
    ITEM_STATUS_BALL_SNATCH_SHAKE_WZX_DATA_ID = 22,
    ITEM_STATUS_BALL_SNATCH_SNATCH_WZX_DATA_ID = 23,
    ITEM_STATUS_BALL_DATA_END = 24,
    ITEM_STATUS_SOUBI_FIGHT_KOUKA_DATA_ID = 25,
    ITEM_STATUS_SOUBI_DATA_END = 26,
    ITEM_STATUS_ITEM_DATA_ID = 27,
    ITEM_STATUS_NUM = 28,
    ITEM_STATUS_FIGHT_ITEM_DATA_ID = 30,
    ITEM_STATUS_FIGHT_TARGET_DATA_ID = 31,
    ITEM_STATUS_FIGHT_COUNT = 32,
    ITEM_STATUS_FIGHT_BUFF = 33,
    ITEM_STATUS_LIMIT = 35
};
s32 itemGetStatus(ItemSlot* item, u16 itemID, u16 kind, u16 index);
void itemReplace(ItemSlot* first, ItemSlot* second);
s32 itemDecItemDataIdAry(ItemSlot* items, u16 count, u16 itemID, u16 quantity,
                         s16 firstSlot, u16 maxQuantity, u8 sort);
s32 itemAddItemDataIdAry(ItemSlot* items, u16 count, u16 itemID, u16 quantity,
                         s16 firstSlot, u16 maxQuantity, u8 sort, u8 option);
void itemInitAry(ItemSlot* items, u16 count);
u8 itemCheckValid(ItemSlot *slot);
u16 item_GetNum(ItemSlot *item);
u16 item_GetItemDataId(ItemSlot *item);
void itemSortAryByPriority(void *ary, u16 num, long kind);
u16 itemGetQuantityInAry(const void *ary, u16 num, u16 itemID, u16 max);
#ifdef __cplusplus
}
#endif
#endif
