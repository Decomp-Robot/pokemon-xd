#include <item.h>
#include <itemDataBios.h>
#include <itemBios.h>

extern "C" {

void item_SetNum(ItemSlot* item, u16 quantity) {
    if (item == 0) {
        return;
    }
    item->mNum = quantity;
}

void item_SetItemDataId(ItemSlot* item, u16 itemID) {
    if (item == 0) {
        return;
    }
    item->mItemID = itemID;
}

u16 item_GetNum(ItemSlot* item) {
    if (item == 0) {
        return 0;
    }
    return item->mNum;
}

u16 item_GetItemDataId(ItemSlot* item) {
    if (item == 0) {
        return 0;
    }
    return item->mItemID;
}

u32 itemBallDB_GetYaseiPokeoutWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mYaseiPokeoutWzxDataId;
}

u32 itemBallDB_GetYaseiGetWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mYaseiGetWzxDataId;
}

u32 itemBallDB_GetYaseiShakeWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mYaseiShakeWzxDataId;
}

u32 itemBallDB_GetYaseiBallLandWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mYaseiBallLandWzxDataId;
}

u32 itemBallDB_GetSnatchSnatchWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mSnatchSnatchWzxDataId;
}

u32 itemBallDB_GetSnatchShakeWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mSnatchShakeWzxDataId;
}

u32 itemBallDB_GetSnatchPokeoutWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mSnatchPokeoutWzxDataId;
}

u32 itemBallDB_GetSnatchMissWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mSnatchMissWzxDataId;
}

u32 itemBallDB_GetSnatchBalllandWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mSnatchBalllandWzxDataId;
}

u32 itemBallDB_GetSnatchAttackWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mSnatchAttackWzxDataId;
}

u32 itemBallDB_GetThrowWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mThrowWzxDataId;
}

u32 itemBallDB_GetDowninWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mDowninWzxDataId;
}

u32 itemBallDB_GetOutWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mOutWzxDataId;
}

u32 itemBallDB_GetOpenWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mOpenWzxDataId;
}

u32 itemBallDB_GetInWzxDataId(u16 itemID) {
    ItemBallData* data = itemBallDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mInWzxDataId;
}

u32 itemDB_GetBuff(u16 itemID) {
    ItemData* data = itemDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mBuff;
}

u16 itemDB_GetItemSoubiDataId(u16 itemID) {
    ItemData* data = itemDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mItemSoubiDataId;
}

u8 itemDB_GetKind(u16 itemID) {
    ItemData* data = itemDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mBagPocket;
}

u32 itemDB_GetName(u16 itemID) {
    ItemData* data = itemDataBiosGetPtr(itemID);
    if (data == 0) {
        return 0;
    }
    return data->mNameMsgID;
}

}
