#include <types.h>
#include <itemData.h>
#include <heroItem.h>

u16 getStockMax(u16 itemID) {
    u16 stockMax;

    heroItemGetItemKindToItemAryPtr(NULL, itemDataBiosGetKind(itemDataBiosGetPtr(itemID)),
                                    NULL, &stockMax, NULL, NULL);
    return stockMax;
}

int getItemName(u32 itemID) {
    return itemDataBiosGetName(itemDataBiosGetPtr(itemID));
}
