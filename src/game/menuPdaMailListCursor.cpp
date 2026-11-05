#include <types.h>
#include <winSprite.h>
#include <menuPdaSub.h>

CURSOR _getCursor(void);
extern "C" s32 mailGetNbMailInMailbox(void);

// Scroll arrow hooks of the PDA mail list: each shows or hides the arrow
// while the cursor stands at the down or up end of the list.
void _pdaMailListDownCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = _getCursor();
    s32 count = mailGetNbMailInMailbox();
    menuPdaSubCheckUpDownCursor(window, sprite, &cursor, count, 5, 0);
}

void _pdaMailListUpCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite)
{
    CURSOR cursor = _getCursor();
    s32 count = mailGetNbMailInMailbox();
    menuPdaSubCheckUpDownCursor(window, sprite, &cursor, count, 5, 1);
}
