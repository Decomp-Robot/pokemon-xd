#include <menu.h>
#include <window.h>

namespace {
enum { TITLE_OPTION_NORMAL_BUTTON_ROW = 2 };
}

extern "C" void menuTitleOptionButton(tagWINDOW_WORK *window)
{
    if (window->mCursor.y == TITLE_OPTION_NORMAL_BUTTON_ROW) {
        menuButtonNormal(window);
    } else if (windowGetKeyInfo()->mKeyTrg & WINDOW_KEY_CANCEL) {
        window->mAccept = 1;
        window->mCancel = 1;
    }
}

void _menuTitleOptionClose(long menuID)
{
    menuCloseCustom(menuID, 0, true);
}

void _menuTitleOptionExit(void)
{
}
