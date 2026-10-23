#ifndef MENU_PT_SPRITE_WORK_H
#define MENU_PT_SPRITE_WORK_H

#include <GSgfxLight.h>
#include <menuItemBios.h>
#include <menuSpriteBios.h>
#include <winSprite.h>

// Look up a sprite work node on the window list by its hook routine.
// The first `index` nodes registered with `hook` are skipped and the
// result is then advanced `offset` nodes further down the list; a null
// pointer is returned when there is no such node.
tagSPRITE_WORK *menuPtGetSpriteWorkByHookFunction(tagWINDOW_WORK *window, void *hook, unsigned long index, long offset);

// Item data of the item the sprite work `work` displays.
MENU_ITEM_dd_ *menuPtItemGetBySpriteWork(tagWINDOW_WORK *window, tagSPRITE_WORK *work);

// Position of `work` among the sprite works that display item `itemID`.
unsigned long menuPtSpriteWorkGetIndexFromId(tagWINDOW_WORK *window, tagSPRITE_WORK *work, unsigned long itemID);

// First sprite work on the window displaying item `itemID`, then advanced
// `offset` nodes further down the list; null when there is no such node.
tagSPRITE_WORK *menuPtSpriteWorkGetByItemID(tagWINDOW_WORK *window, unsigned long itemID, long offset);

tagSPRITE_WORK* menuPtSpriteWorkGetByItemID(long menuID, unsigned long itemID, long offset);
void menuPtSpriteWorkSetMsgId(long menuID, unsigned long itemID, long offset, unsigned long msgID);
MENU_ITEM_dd_* menuPtItemGetByMenuIdAndItemId(long menuID, unsigned long itemID, long offset);
void menuPtSpriteWorkSetWidthRateColor(tagWINDOW_WORK* window, unsigned long itemID,
                                      long offset, float widthRate, GSgfxColAu8 color);
void menuPtSpriteWorkSetSw(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, bool display);
void menuPtSpriteWorkSetSw(tagWINDOW_WORK* window, unsigned long itemID, long offset, bool display);
void menuPtSpriteWorkSetSpriteId(tagSPRITE_WORK* sprite, unsigned long spriteID);
void menuPtSpriteWorkSetSpriteData(tagSPRITE_WORK* sprite, MenuSpriteData* spriteData);
void menuPtSpriteWorkSetMenuItem(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite, unsigned long itemID);

#endif
