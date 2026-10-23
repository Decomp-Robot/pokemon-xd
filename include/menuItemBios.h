#ifndef MENU_ITEM_BIOS_H
#define MENU_ITEM_BIOS_H

#include <types.h>

// One entry of an item table, 0x1C bytes.
struct tagMENU_ITEM_DATA;

#ifdef __cplusplus
extern "C" {
#endif

// Item data of `itemID` in the item table selected by the menu data
// entry `dataID`. Returns null outside the table.
tagMENU_ITEM_DATA *menuItemBiosGetPtr(long dataID, long itemID);

#ifdef __cplusplus
}
#endif

#endif
