#include <WorldMap.h>

extern const u8 worldMapMenuOpen;

/* WorldMap::GetNowCursor -- report the cursor currently selected on the
 * world map. While worldMapMenuOpen is set the map is driven through a menu
 * script and the cursor lives in the window work; otherwise it is the one the
 * free-roam map work keeps. */
s32 WorldMap::GetNowCursor(tagWINDOW_WORK* window) {
    if (worldMapMenuOpen != 0) {
        return window->cursor;
    }
    return mModelWork->cursor;
}
