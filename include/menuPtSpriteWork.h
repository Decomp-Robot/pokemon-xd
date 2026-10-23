#ifndef MENU_PT_SPRITE_WORK_H
#define MENU_PT_SPRITE_WORK_H

#include <winSprite.h>

// Look up a sprite work node on the window's list by its hook routine.
// The first `index` nodes registered with `hook` are skipped and the
// result is then advanced `offset` nodes further down the list; a null
// pointer is returned when there is no such node.
tagSPRITE_WORK *menuPtGetSpriteWorkByHookFunction(tagWINDOW_WORK *window, void *hook, unsigned long index, long offset);

#endif
