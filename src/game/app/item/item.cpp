#include <item.h>
#include <itemData.h>
#include <itemBios.h>
#include <fightItemBios.h>

s32 itemGetStatus(ItemSlot* item, u16 itemID, u16 kind, u16 index) {
    void* data = item;

    if (kind == ITEM_STATUS_INVALID || kind >= ITEM_STATUS_LIMIT) {
        return 0;
    }

    if (kind < ITEM_STATUS_ITEM_DATA_END) {
        data = itemDataBiosGetPtr(itemID);
        if (data == 0) {
            return 0;
        }
    } else if (kind < ITEM_STATUS_BALL_DATA_END) {
        data = itemBallDataBiosGetPtr(itemID);
        if (data == 0) {
            return 0;
        }
    } else if (kind < ITEM_STATUS_SOUBI_DATA_END) {
        data = itemSoubiDataBiosGetPtr(itemID);
        if (data == 0) {
            return 0;
        }
    } else if (data == 0) {
        return 0;
    }

    switch (kind) {
    case ITEM_STATUS_NAME:
        return itemDataBiosGetName((ItemData*)data);
    case ITEM_STATUS_KIND:
        return itemDataBiosGetKind((ItemData*)data);
    case ITEM_STATUS_PRICE:
        return itemDataBiosGetPrice((ItemData*)data);
    case ITEM_STATUS_IMPORTANT:
        return itemDataBiosGetImportant((ItemData*)data);
    case ITEM_STATUS_USEFUL:
        return itemDataBiosGetUseful((ItemData*)data);
    case ITEM_STATUS_DOC:
        return itemDataBiosGetDoc((ItemData*)data);
    case ITEM_STATUS_ITEM_SOUBI_DATA_ID:
        return itemDataBiosGetItemSoubiDataId((ItemData*)data);
    case ITEM_STATUS_FIGHT_USE_KOUKA_DATA_ID:
        return itemDataBiosGetFightUseKoukaDataId((ItemData*)data);
    case ITEM_STATUS_USE_FRIEND:
        return itemDataBiosGetUseFriend((ItemData*)data, index);
    case ITEM_STATUS_BUFF:
        return itemDataBiosGetBuff((ItemData*)data);
    case ITEM_STATUS_BALL_FIGHT_KOUKA_DATA_ID:
        return itemBallDataBiosGetFightKoukaDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_IN_WZX_DATA_ID:
        return itemBallDataBiosGetInWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_OPEN_WZX_DATA_ID:
        return itemBallDataBiosGetOpenWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_OUT_WZX_DATA_ID:
        return itemBallDataBiosGetOutWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_DOWNIN_WZX_DATA_ID:
        return itemBallDataBiosGetDowninWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_THROW_WZX_DATA_ID:
        return itemBallDataBiosGetThrowWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_SNATCH_ATTACK_WZX_DATA_ID:
        return itemBallDataBiosGetSnatchAttackWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_SNATCH_BALLLAND_WZX_DATA_ID:
        return itemBallDataBiosGetSnatchBalllandWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_SNATCH_MISS_WZX_DATA_ID:
        return itemBallDataBiosGetSnatchMissWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_SNATCH_POKEOUT_WZX_DATA_ID:
        return itemBallDataBiosGetSnatchPokeoutWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_SNATCH_SHAKE_WZX_DATA_ID:
        return itemBallDataBiosGetSnatchShakeWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_BALL_SNATCH_SNATCH_WZX_DATA_ID:
        return itemBallDataBiosGetSnatchSnatchWzxDataId((ItemBallData*)data);
    case ITEM_STATUS_SOUBI_FIGHT_KOUKA_DATA_ID:
        return itemSoubiDataBiosGetFightKoukaDataId((ItemSoubiData*)data);
    case ITEM_STATUS_ITEM_DATA_ID:
        return itemBiosGetItemDataId((ItemSlot*)data);
    case ITEM_STATUS_NUM:
        return itemBiosGetNum((ItemSlot*)data);
    case ITEM_STATUS_FIGHT_ITEM_DATA_ID:
        return fightItemBiosGetItemDataId((FIGHT_ITEM*)data);
    case ITEM_STATUS_FIGHT_TARGET_DATA_ID:
        return fightItemBiosGetTargetDataId((FIGHT_ITEM*)data);
    case ITEM_STATUS_FIGHT_COUNT:
        return fightItemBiosGetCount((FIGHT_ITEM*)data);
    case ITEM_STATUS_FIGHT_BUFF:
        return fightItemBiosGetBuff((FIGHT_ITEM*)data);
    default:
        return 0;
    }
}
