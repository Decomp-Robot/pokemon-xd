#ifndef MENU_PDA_SUB_H
#define MENU_PDA_SUB_H

#include <types.h>
#include <GSvec.h>
#include <winSprite.h>

#ifdef __cplusplus
extern "C" {
#endif

// Row alphas of a list: the selected row fades in, the other rows fade out.
void menuPdaSubCalcAlpha(f32 *alpha, long count, long selected);

// Puts the row alphas at their resting value: only the selected row is drawn.
void menuPdaSubInitAlpha(f32 *alpha, long count, long selected);

// Moves a scroll position toward its target by one frame step.
void menuPdaSubCalcPositon(f32 *pos, f32 target, f32 speed);

void menuPdaSubScrollBar(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                         long menuID, long itemID, u32 color, long count,
                         long visibleCount, GSivec2 *scroll);

// Shows or hides the up/down scroll arrow of a list: the arrow appears
// when the cursor can move in the given direction (0 for down, 1 for up).
void menuPdaSubCheckUpDownCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite,
                                 CURSOR *cursor, long count,
                                 long visibleCount, bool direction);

#ifdef __cplusplus
}
#endif

#endif
