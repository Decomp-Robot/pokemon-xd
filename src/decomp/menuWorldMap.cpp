#include <WorldMap.h>

extern u8 lbl_804EA6FE;

/* WorldMap::GetNowCursor -- report the cursor currently selected on the
 * world map. While lbl_804EA6FE is set the cursor is taken from the window
 * work; otherwise it is taken from the map work. */
s32 WorldMap::GetNowCursor(tagWINDOW_WORK* window) {
    if (lbl_804EA6FE != 0) {
        return window->cursor;
    }
    return mModelWork->cursor;
}
