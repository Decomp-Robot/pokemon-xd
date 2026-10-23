#ifndef MENU_ITEM_BIOS_H
#define MENU_ITEM_BIOS_H

#include <types.h>

// One entry of an item table, 0x1C bytes. Partial: the y offset at 0x08 is
// added to the cursor position by the item cursor hooks, and the two values
// at 0x0A/0x0C are handed to menuModelInit. The rest stays padding.
typedef struct tagMENU_ITEM_DATA tagMENU_ITEM_DATA;
struct tagMENU_ITEM_DATA {
    u8 _00[0x08];  // 0x00-0x07: unknown.
    s16 mY;        // 0x08: y offset added to the cursor position.
    s16 mUnk0A;    // 0x0A: first value passed to menuModelInit.
    s16 mUnk0C;    // 0x0C: second value passed to menuModelInit.
    u8 _0E[0x0E];  // 0x0E-0x1B: remainder; unknown here.
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
