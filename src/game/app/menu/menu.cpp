#include <menu.h>
#include <menuDataBios.h>
#include <menuItemBios.h>
#include <winSprite.h>
#include <window.h>

s32 menuGetSelectItemNum(s32 menuID)
{
    s32 count = 0;
    MenuData *data = menuDataBiosGetPtr(menuID);
    if (data == 0) {
        return 0;
    }

    s16 itemID = data->mFirstItemID;
    while (true) {
        tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(menuID, itemID);
        if (item->mShownFlag) {
            count++;
        }
        if (item->mChainEndFlag) {
            break;
        }
        itemID = item->mNextID;
    }
    return count;
}
s32 menuGetCursorFromItemID(s32 menuID, s32 itemID)
{
    MenuData *data = menuDataBiosGetPtr(menuID);
    if (data == 0) {
        return MENU_RESULT_NO_DATA;
    }

    s16 currentID;
    s32 cursor = 0;
    currentID = data->mFirstItemID;
    while (true) {
        tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(menuID, currentID);
        if ((u32)currentID == (u32)itemID) {
            if (item->mShownFlag) {
                return cursor;
            }
            return MENU_RESULT_INVALID;
        }
        if (item->mShownFlag) {
            cursor++;
        }
        if (item->mChainEndFlag) {
            break;
        }
        currentID = item->mNextID;
    }
    return MENU_RESULT_NOT_FOUND;
}

bool menuGetDisp(s32 menuID)
{
    tagWINDOW_WORK *window = windowSearchID(menuID);
    if (window == 0) {
        return false;
    }
    return (window->mStateFlags & WINDOW_STATE_FLAG_DISP) != 0;
}

void menuSetDisp(s32 menuID, bool visible)
{
    tagWINDOW_WORK *window = windowSearchID(menuID);
    if (window != 0) {
        if (visible) {
            window->mStateFlags |= WINDOW_STATE_FLAG_DISP;
        } else {
            window->mStateFlags &= ~WINDOW_STATE_FLAG_DISP;
        }
    }
}

s32 menuGetCursorItemID(s32 menuID)
{
    s32 cursor = menuGetCursor(menuID);
    if (cursor == MENU_RESULT_INVALID) {
        return 0;
    }

    MenuData *data = menuDataBiosGetPtr(menuID);
    if (data == 0) {
        return 0;
    }

    s16 itemID;
    s32 index = 0;
    itemID = data->mFirstItemID;
    while (true) {
        tagMENU_ITEM_DATA *item = menuItemBiosGetPtr(menuID, itemID);
        if (item->mShownFlag) {
            if (index == cursor) {
                return (u32)itemID;
            }
            index++;
        }
        if (item->mChainEndFlag) {
            break;
        }
        itemID = item->mNextID;
    }
    return 0;
}

s32 menuSetCursor(s32 menuID, s32 cursor)
{
    tagWINDOW_WORK *window = windowSearchID(menuID);
    if (window != 0) {
        window->mCursor.y = (s8)cursor;
    } else {
        return MENU_RESULT_INVALID;
    }
    return MENU_RESULT_OK;
}

s32 menuGetCursor(s32 menuID)
{
    tagWINDOW_WORK *window = windowSearchID(menuID);
    if (window != 0) {
        return window->mCursor.y + window->mCursor.x;
    }
    return MENU_RESULT_INVALID;
}
