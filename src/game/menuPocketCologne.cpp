#include <types.h>
#include <item.h>
#include <itemBios.h>
#include <itemData.h>
#include <heroItem.h>

int getItemNum(u16 itemID) {
    u16 num;
    ItemSlot *ary;
    int i;
    int count = 0;

    ary = heroItemGetItemKindToItemAryPtr(NULL, itemDataBiosGetKind(itemDataBiosGetPtr(itemID)),
                                          &num, NULL, NULL, NULL);
    if (ary == NULL) {
        return 0;
    }

    for (i = 0; i < num; i++, ary++) {
        if (itemCheckValid(ary) && itemID == (u32)itemGetStatus(ary, 0, ITEM_STATUS_ITEM_DATA_ID, 0)) {
            count += itemBiosGetNum(ary);
        }
    }

    return count;
}

u16 getStockMax(u16 itemID) {
    u16 stockMax;

    heroItemGetItemKindToItemAryPtr(NULL, itemDataBiosGetKind(itemDataBiosGetPtr(itemID)),
                                    NULL, &stockMax, NULL, NULL);
    return stockMax;
}

int getItemName(u32 itemID) {
    return itemDataBiosGetName(itemDataBiosGetPtr(itemID));
}
