#include <WorldMap.h>

extern u8 worldMapMenuOpen;

/* WorldMap::GetNowCursor -- report the cursor currently selected on the
 * world map. While worldMapMenuOpen is set the cursor is taken from the window
 * work; otherwise it is taken from the map work. */
s32 WorldMap::GetNowCursor(tagWINDOW_WORK* window) {
    if (worldMapMenuOpen != 0) {
        return window->cursor;
    }
    return mModelWork->cursor;
}
