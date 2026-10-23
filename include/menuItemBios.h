#ifndef MENU_ITEM_BIOS_H
#define MENU_ITEM_BIOS_H

#include <types.h>

// One entry of an item table, 0x1C bytes. Partial: only the y offset
// added to the cursor position by the item cursor hooks is named; the
// rest stays padding.
typedef struct tagMENU_ITEM_DATA tagMENU_ITEM_DATA;
struct tagMENU_ITEM_DATA {
    u8 _00[0x08];
    s16 mY;       // 0x08: y offset added to the cursor position.
    u8 _0A[0x12]; // 0x0A-0x1B: remainder; unknown here.
};

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
