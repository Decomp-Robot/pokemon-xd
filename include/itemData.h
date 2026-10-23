#ifndef ITEM_DATA_H
#define ITEM_DATA_H

#include <types.h>

enum ItemID { ITEM_COLOGNE_CASE = 0x200, ITEM_BATTLE_DISK_CASE = 0x215 };

// Use routines receive an item ID and write the quantity consumed.
// Completion finishes a battle selection; continue keeps the pocket open.
// Close requests closing the pocket even in the field.
typedef int (*PocketItemUseFunc)(unsigned long itemID, int *consumedQuantity);

enum PocketItemUseResult {
    POCKET_ITEM_USE_COMPLETE = 0,
    POCKET_ITEM_USE_CONTINUE = 1,
    POCKET_ITEM_USE_CLOSE = 2
};

// Item-table record, 0x28 bytes; separate from the menu-item layout.
typedef struct ItemData ItemData;
struct ItemData {
    u8 mBagPocket; // 0x00: bag pocket the item belongs to.
    u8 mImportant; // 0x01: item cannot be thrown away or sold.
    u8 mUsable; // 0x02: item may be used.
    u8 mDepositAble; // 0x03: item may be put into the PC.
    u8 mFunctionalID; // 0x04: index of the item-use parameters.
    u8 mReservedFlags;
    u16 mPrice; // 0x06: price the item is sold for.
    u8 _08[0x08]; // 0x08-0x0F: unknown.
    u32 mDescriptionMsgID; // 0x10: message id describing the item.
    u32 mNameMsgID; // 0x14: message id of the item name.
    u32 mMachineNo; // 0x18: machine number in the low byte.
    PocketItemUseFunc mFieldUseFunc; // 0x1C: routine the item runs in the field.
    PocketItemUseFunc mBattleUseFunc; // 0x20: routine the item runs in battle.
    u8 _24[0x04]; // 0x24-0x27: unknown.
};

#ifdef __cplusplus
extern "C" {
#endif

ItemData *itemDataBiosGetPtr(u16 itemID);
u8 itemDataBiosGetImportant(ItemData *data);
u8 itemDataBiosGetDeposit(ItemData *data);
u8 itemDataBiosGetKinomiNo(u16 itemID);
u8 itemDataBiosGetHidenMachineNo(ItemData *data);
u8 itemDataBiosGetWazaMachineNo(ItemData *data);
u16 itemDataBiosGetWazaIDByWazaMachineNo(u8 machineNo);
u8 itemDataBiosGetKind(ItemData *data);
u32 itemDataBiosGetDoc(ItemData *data);
int itemDataBiosGetPrice(ItemData *data);
int itemDataBiosGetName(ItemData *data);
PocketItemUseFunc itemDataBiosGetFieldUseFunc(ItemData *data);
PocketItemUseFunc itemDataBiosGetBattleUseFunc(ItemData *data);
u8 itemDataBiosGetUseful(ItemData *data);
u8 itemDataBiosCheckExportable(ItemData *data);
u8 itemDataBiosCheckImportable(ItemData *data);

u8 itemDataCheckValid(u16 itemID);
u8 itemDataBiosGetItemEffectParam(ItemData *data);
#ifdef __cplusplus
}
#endif

#endif
