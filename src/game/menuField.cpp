#include <types.h>
#include <winSprite.h>

// Field menu window handling: the row hook the field menu registers for its
// cursor sprites, and the timer that keeps its cursor animation going.

// Whether the field menu is open; the open check reports this flag.
u8 menuFieldOpen;
// Time the field menu cursor animation has run; the animation update wraps it.
f32 _fieldMenuTimer;

// Row hook of the field menu cursor sprites: the sprite shows the row the
// window's cursor is on, and the cursor animation restarts whenever the cursor
// has moved off the row the window is scrolled to.
void _menuFieldMenuCursor(tagWINDOW_WORK *window, tagSPRITE_WORK *sprite, long row)
{
    s16 cursorY = window->mCursor.y;
    s16 cursorTo = window->cursorTo;

    if (cursorY == row) {
        if (cursorTo != cursorY) {
            winAnimeCursor(window, sprite);
            _fieldMenuTimer = 0.0f;
        }
        winSpriteSetDisp(sprite, true);
    } else {
        winSpriteSetDisp(sprite, false);
    }

    if (window->mState == WINDOW_STATE_CLOSING) {
        winSetSequence(&sprite->mSeq, 0);
    }
}
