#include <menu.h>

enum {
    SEARCHER_MENU_WINDOW = 119,
    SEARCHER_MENU_LIST = 109,
};

static void _menuClose(long menuID);

extern "C" void menuPdaSearcher(void) {
    menuOpen(SEARCHER_MENU_WINDOW, 0);

    while (menuOpen(SEARCHER_MENU_LIST, 1) >= 0) {
    }

    _menuClose(SEARCHER_MENU_LIST);
    _menuClose(SEARCHER_MENU_WINDOW);
}

static void _menuClose(long menuID) {
    menuClose(menuID);
    menuCloseSync(menuID, true);
}
