#ifndef MENU_PDA_SUB_H
#define MENU_PDA_SUB_H

#include <types.h>
#include <GSvec.h>
#include <winSprite.h>

#ifdef __cplusplus
extern "C" {
#endif

void menuPdaSubScrollBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                         long menuID, long itemID, u32 color, long count,
                         long visibleCount, GSivec2 *scroll);

#ifdef __cplusplus
}
#endif

#endif
