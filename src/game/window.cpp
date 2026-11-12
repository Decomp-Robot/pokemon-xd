#include <window.h>
#include <menuDataBios.h>

u8 _windowCheckParent(tagWINDOW_WORK *window, tagWINDOW_WORK *parent);
long _windowDeleteWork(tagWINDOW_WORK *window);
void _windowSetCloseFlag(tagWINDOW_WORK *window, bool respectMenuFlag);

void windowCloseMain(tagWINDOW_WORK *window)
{
    MenuData *menu = menuDataBiosGetPtr(window->mMenuID);
    if (menu->mControlFunction) {
        window->mState = WINDOW_STATE_DELETE;
        menu->mControlFunction(window);
    }
    _windowDeleteWork(window);
}

long windowClose(tagWINDOW_WORK *window, unsigned long flags)
{
    u8 foundActiveWindow = 0;
    tagWINDOW_WORK *child;
    bool respectMenuFlag = (bool)(flags & WINDOW_CLOSE_RESPECT_MENU_FLAG);
    if (!(flags & WINDOW_CLOSE_KEEP_CHILDREN)) {
        for (child = winSysWork.mWindowList; child; child = child->mNext) {
            if (_windowCheckParent(child, window)) {
                _windowSetCloseFlag(child, respectMenuFlag);
            }
        }
    }
    if (!(flags & WINDOW_CLOSE_CHILDREN_ONLY) && window) {
        _windowSetCloseFlag(window, respectMenuFlag);
        if (flags & WINDOW_CLOSE_KEEP_CHILDREN) {
            for (child = winSysWork.mWindowList; child; child = child->mNext) {
                if (child->mParent == window) {
                    child->mParent = window->mParent;
                }
            }
        }
    }
    window = windowSearchID(windowGetActiveID());
    while (window) {
        if (window->mCloseRequested || window->mInactive) {
            for (child = winSysWork.mWindowList; child; child = child->mNext) {
                if (!child->mCloseRequested && !child->mInactive &&
                    _windowCheckParent(child, window->mParent)) {
                    winSysWork.mActiveID = child->mMenuID;
                    foundActiveWindow = 1;
                }
            }
            if (foundActiveWindow) {
                break;
            }
        } else {
            winSysWork.mActiveID = window->mMenuID;
            break;
        }
        window = window->mParent;
    }
    if (!window) {
        winSysWork.mActiveID = 0;
    }
    return 0;
}

void _windowSetCloseFlag(tagWINDOW_WORK *window, bool respectMenuFlag)
{
    if (!respectMenuFlag || !menuDataBiosGetPtr(window->mMenuID)->mIgnoreCloseRequest) {
        window->mCloseRequested = 1;
    }
}
