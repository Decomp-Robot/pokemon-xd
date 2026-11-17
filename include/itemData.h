#ifndef ITEM_DATA_H
#define ITEM_DATA_H

#include <types.h>
#include <bool.h>

enum ItemKind {
    ITEM_KIND_POKEMON_BALL = 1,
    ITEM_KIND_NORMAL = 2,
    ITEM_KIND_SEED = 3,
    ITEM_KIND_WAZA_MACHINE = 4,
    ITEM_KIND_EXTRA = 5,
    ITEM_KIND_COLOGNE = 6,
    ITEM_KIND_BATTLE_DISK = 7
};

// Item IDs used by the inventory routines.
enum ItemID {
    ITEM_MASTER_BALL = 1,
    ITEM_ULTRA_BALL = 2,
    ITEM_GREAT_BALL = 3,
    ITEM_POKE_BALL = 4,
    ITEM_SAFARI_BALL = 5,
    ITEM_NET_BALL = 6,
    ITEM_DIVE_BALL = 7,
    ITEM_NEST_BALL = 8,
    ITEM_REPEAT_BALL = 9,
    ITEM_TIMER_BALL = 10,
    ITEM_LUXURY_BALL = 11,
    ITEM_PREMIER_BALL = 12,
    ITEM_POTION = 13,
    ITEM_HYPER_POTION = 21,

    // Berries.
    ITEM_CHERI_BERRY = 133,
    ITEM_CHESTO_BERRY = 134,
    ITEM_PECHA_BERRY = 135,
    ITEM_RAWST_BERRY = 136,
    ITEM_ENIGMA_BERRY = 175,

    // Technical machines.
    ITEM_TM01 = 289,
    ITEM_TM02 = 290,

    // The cologne case opens the scent menu; the scents follow it.
    ITEM_COLOGNE_CASE = 512,
    ITEM_JOY_SCENT = 513,
    ITEM_EXCITE_SCENT = 514,
    ITEM_VIVID_SCENT = 515,

    // The battle disk case opens the disk list; the disks follow it,
    // one item ID per disk, in disk order.
    ITEM_BATTLE_DISK_CASE = 533,
    ITEM_BATTLE_DISK_01 = 534,
    ITEM_BATTLE_DISK_02 = 535,
    ITEM_BATTLE_DISK_03 = 536,
    ITEM_BATTLE_DISK_04 = 537,
    ITEM_BATTLE_DISK_05 = 538,
    ITEM_BATTLE_DISK_06 = 539,
    ITEM_BATTLE_DISK_07 = 540,
    ITEM_BATTLE_DISK_08 = 541,
    ITEM_BATTLE_DISK_09 = 542,
    ITEM_BATTLE_DISK_10 = 543,
    ITEM_BATTLE_DISK_11 = 544,
    ITEM_BATTLE_DISK_12 = 545,
    ITEM_BATTLE_DISK_13 = 546,
    ITEM_BATTLE_DISK_14 = 547,
    ITEM_BATTLE_DISK_15 = 548,
    ITEM_BATTLE_DISK_16 = 549,
    ITEM_BATTLE_DISK_17 = 550,
    ITEM_BATTLE_DISK_18 = 551,
    ITEM_BATTLE_DISK_19 = 552,
    ITEM_BATTLE_DISK_20 = 553,
    ITEM_BATTLE_DISK_21 = 554,
    ITEM_BATTLE_DISK_22 = 555,
    ITEM_BATTLE_DISK_23 = 556,
    ITEM_BATTLE_DISK_24 = 557,
    ITEM_BATTLE_DISK_25 = 558,
    ITEM_BATTLE_DISK_26 = 559,
    ITEM_BATTLE_DISK_27 = 560,
    ITEM_BATTLE_DISK_28 = 561,
    ITEM_BATTLE_DISK_29 = 562,
    ITEM_BATTLE_DISK_30 = 563,
    ITEM_BATTLE_DISK_31 = 564,
    ITEM_BATTLE_DISK_32 = 565,
    ITEM_BATTLE_DISK_33 = 566,
    ITEM_BATTLE_DISK_34 = 567,
    ITEM_BATTLE_DISK_35 = 568,
    ITEM_BATTLE_DISK_36 = 569,
    ITEM_BATTLE_DISK_37 = 570,
    ITEM_BATTLE_DISK_38 = 571,
    ITEM_BATTLE_DISK_39 = 572,
    ITEM_BATTLE_DISK_40 = 573,
    ITEM_BATTLE_DISK_41 = 574,
    ITEM_BATTLE_DISK_42 = 575,
    ITEM_BATTLE_DISK_43 = 576,
    ITEM_BATTLE_DISK_44 = 577,
    ITEM_BATTLE_DISK_45 = 578,
    ITEM_BATTLE_DISK_46 = 579,
    ITEM_BATTLE_DISK_47 = 580,
    ITEM_BATTLE_DISK_48 = 581,
    ITEM_BATTLE_DISK_49 = 582,
    ITEM_BATTLE_DISK_50 = 583,
    ITEM_BATTLE_DISK_51 = 584,
    ITEM_BATTLE_DISK_52 = 585,
    ITEM_BATTLE_DISK_53 = 586,
    ITEM_BATTLE_DISK_54 = 587,
    ITEM_BATTLE_DISK_55 = 588,
    ITEM_BATTLE_DISK_56 = 589,
    ITEM_BATTLE_DISK_57 = 590,
    ITEM_BATTLE_DISK_58 = 591,
    ITEM_BATTLE_DISK_59 = 592,
    ITEM_BATTLE_DISK_60 = 593
};

// Use routines receive an item ID and write the quantity consumed.
// Completion finishes a battle selection; continue keeps the pocket open.
// Close requests closing the pocket even in the field.
typedef int (*PocketItemUseFunc)(unsigned long itemID, int *consumedQuantity);

enum PocketItemUseResult {
    POCKET_ITEM_USE_COMPLETE = 0,
    POCKET_ITEM_USE_CONTINUE = 1,
    POCKET_ITEM_USE_CLOSE = 2
};

// Item-table record; separate from the menu-item layout.
typedef struct ItemData ItemData;
struct ItemData {
    u8 mBagPocket; // bag pocket the item belongs to.
    u8 mImportant; // item cannot be thrown away or sold.
    u8 mUsable; // item may be used.
    u8 mDepositAble; // item may be put into the PC.
    u8 mFunctionalID; // index of the item-use parameters.
    u8 mReservedFlags;
    u16 mPrice; // price the item is sold for.
    u16 mCoupon;
    u16 mItemSoubiDataId;
    u16 mFightUseKoukaDataId;
    u16 mUnknown;
    u32 mNameMsgID; // message id of the item name.
    u32 mDescriptionMsgID; // message id describing the item.
    u32 mBuff; // Item-specific data; TM/HM items store their machine number here.
    PocketItemUseFunc mFieldUseFunc; // routine the item runs in the field.
    PocketItemUseFunc mBattleUseFunc; // routine the item runs in battle.
    s8 mUseFriend[3];
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
u16 itemDataBiosGetItemSoubiDataId(ItemData *data);
u16 itemDataBiosGetWazaIDByWazaMachineNo(u8 machineNo);
u8 itemDataBiosGetKind(ItemData *data);
u32 itemDataBiosGetDoc(ItemData *data);
u16 itemDataBiosGetPrice(ItemData *data);
u32 itemDataBiosGetName(ItemData *data);
PocketItemUseFunc itemDataBiosGetFieldUseFunc(ItemData *data);
PocketItemUseFunc itemDataBiosGetBattleUseFunc(ItemData *data);
u8 itemDataBiosGetUseful(ItemData *data);
u16 itemDataBiosGetFightUseKoukaDataId(ItemData *data);
s8 itemDataBiosGetUseFriend(ItemData *data, u16 friendIndex);
u32 itemDataBiosGetBuff(ItemData *data);
u8 itemDataBiosCheckExportable(ItemData *data);
u8 itemDataBiosCheckImportable(ItemData *data);

void itemDataBiosSetBuff(ItemData *data, u32 value);
void itemDataBiosSetUseFriend(ItemData *data, u16 range, s8 value);
void itemDataBiosSetFightUseKoukaDataId(ItemData *data, u16 value);
void itemDataBiosSetItemSoubiDataId(ItemData *data, u16 value);
void itemDataBiosSetDoc(ItemData *data, u32 value);
void itemDataBiosSetUseful(ItemData *data, u8 value);
void itemDataBiosSetImportant(ItemData *data, u8 value);
void itemDataBiosSetPrice(ItemData *data, u16 value);
void itemDataBiosSetKind(ItemData *data, u8 value);
void itemDataBiosSetName(ItemData *data, u32 value);
u32 itemDataBiosGetBuff(ItemData *data);
s8 itemDataBiosGetUseFriend(ItemData *data, u16 range);
u16 itemDataBiosGetFightUseKoukaDataId(ItemData *data);
u16 itemDataBiosGetCoupon(ItemData *data);

bool itemDataCheckValid(u16 itemID);
u8 itemDataBiosGetItemEffectParam(ItemData *data);
u32 itemDB_GetBuff(u16 itemID);
u16 itemDB_GetItemSoubiDataId(u16 itemID);
u8 itemDB_GetKind(u16 itemID);
u32 itemDB_GetName(u16 itemID);

#ifdef __cplusplus
}
#endif

#endif
