#ifndef MENU_PT_SPRITE_WORK_H
#define MENU_PT_SPRITE_WORK_H

#include <menuItemBios.h>
#include <winSprite.h>

// Look up a sprite work node on the window list by its hook routine.
// The first `index` nodes registered with `hook` are skipped and the
// result is then advanced `offset` nodes further down the list; a null
// pointer is returned when there is no such node.
tagSPRITE_WORK *menuPtGetSpriteWorkByHookFunction(tagWINDOW_WORK *window, void *hook, unsigned long index, long offset);

// Item data of the item the sprite work `work` displays.
tagMENU_ITEM_DATA *menuPtItemGetBySpriteWork(tagWINDOW_WORK *window, tagSPRITE_WORK *work);

// Position of `work` among the sprite works that display item `itemID`.
unsigned long menuPtSpriteWorkGetIndexFromId(tagWINDOW_WORK *window, tagSPRITE_WORK *work, unsigned long itemID);

// First sprite work on the window displaying item `itemID`, then advanced
// `offset` nodes further down the list; null when there is no such node.
tagSPRITE_WORK *menuPtSpriteWorkGetByItemID(tagWINDOW_WORK *window, unsigned long itemID, long offset);

#endif
