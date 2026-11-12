#include <dbgMenu.h>
#include <relglobal.h>
#include <window.h>
#include <menu.h>
#include <menuDataBios.h>
#include <GSmsg.h>
#include <GSgfx.h>

enum {
    DBG_MENU_WINDOW_OFFSET = 1,
    DBG_MENU_INDEX_INVALID = -1,
    DBG_MENU_ITEM_INVALID = 0,
    DBG_MENU_RESULT_CANCEL = -1,
    DBG_MENU_RESULT_BACK = 0,
    DBG_MENU_RESULT_CLOSE = 1
};

GSgfxLayerID _dbgLayerID = GSGFX_LAYER_INVALID;
u8 _dbgMenuOpenFlag;
long *_pCursor;
u8 _dbgMenuLoop;

long _dbgMenuSub(long level);
long _dbgMenuGetMenuNo(long menuID);
long _dbgMenuGetMenuNum(tagWINDOW_WORK *window, long *width);
long _dbgMenuCheckTerminate(tagWINDOW_WORK *window, long index);
u32 _dbgMenuGetMsgID(tagWINDOW_WORK *window, long index);
long _dbgMenuGetItemNo(tagWINDOW_WORK *window, long cursor);
long _dbgMenuGetIndex(tagWINDOW_WORK *window);
tagWINDOW_WORK *_dbgMenuGetWin(long index);
s16 _dbgMenuGetLink(long itemNo);

DbgMenuItem *dbgMenuGetPtr(long itemNo)
{
    if (debugMenuDataGetPtr == NULL)
    {
        return NULL;
    }
    return debugMenuDataGetPtr(itemNo);
}

inline DbgMenuFunc dbgMenuGetFunc(long itemNo)
{
    DbgMenuItem *item = dbgMenuGetPtr(itemNo);
    if (item == NULL)
    {
        return NULL;
    }
    return item->mFunc;
}

inline u8 dbgMenuIsTerminate(long itemNo)
{
    DbgMenuItem *item = dbgMenuGetPtr(itemNo);
    u8 terminate;
    if (item == NULL)
    {
        terminate = 1;
    } else {
        terminate = item->mTerminate != 0;
    }
    return terminate;
}

u32 dbgMenuGetNum()
{
    if (debugMenuDataGetNum == NULL)
    {
        return 0;
    }
    return debugMenuDataGetNum();
}

s16 dbgMenuGetLink(long itemNo)
{
    DbgMenuItem *item = dbgMenuGetPtr(itemNo);
    if (item == NULL)
    {
        return 0;
    }
    return item->mLink;
}

long _dbgMenuGetIndex(tagWINDOW_WORK *window)
{
    if (window->mMenuID < _menu_data_number + DBG_MENU_WINDOW_OFFSET)
    {
        return DBG_MENU_INDEX_INVALID;
    }
    return window->mMenuID - (_menu_data_number + DBG_MENU_WINDOW_OFFSET);
}

tagWINDOW_WORK *_dbgMenuGetWin(long index)
{
    return windowSearchID(index < 0 ? 0 : _menu_data_number + DBG_MENU_WINDOW_OFFSET + index);
}

s16 _dbgMenuGetLink(long itemNo)
{
    s16 link;
    if (itemNo <= DBG_MENU_ITEM_INVALID || (long)dbgMenuGetNum() <= itemNo)
    {
        return DBG_MENU_LINK_NONE;
    }
    link = dbgMenuGetLink(itemNo);
    if (link <= DBG_MENU_LINK_NONE || (long)dbgMenuGetNum() <= link)
    {
        link = DBG_MENU_LINK_NONE;
    }
    return link;
}

long _dbgMenuGetItemNo(tagWINDOW_WORK *window, long cursor)
{
    long index = _dbgMenuGetIndex(window);
    long itemNo;
    if (index < 0)
    {
        return DBG_MENU_ITEM_INVALID;
    }
    if (index == 0)
    {
        itemNo = dbgMenuGetRootMenu();
    } else {
        tagWINDOW_WORK *parent = _dbgMenuGetWin(index - 1);
        if (parent == NULL)
        {
            itemNo = dbgMenuGetRootMenu();
        } else {
            itemNo = _dbgMenuGetItemNo(parent, parent->mCursor.x + parent->mCursor.y);
        }
        itemNo = _dbgMenuGetLink(itemNo);
    }
    return itemNo + cursor;
}

u32 _dbgMenuGetMsgID(tagWINDOW_WORK *window, long index)
{
    DbgMenuItem *item = dbgMenuGetPtr(_dbgMenuGetItemNo(window, index));
    if (item == NULL)
    {
        return 0;
    }
    return item->mMsgID;
}

long _dbgMenuGetMenuNo(long menuID)
{
    tagWINDOW_WORK *window = windowSearchID(menuID);
    long menuNo;
    long itemNo;
    s16 firstItemNo;
    long i;

    if (window)
    {
        itemNo = _dbgMenuGetItemNo(window, window->mCursor.x + window->mCursor.y);
        firstItemNo = _dbgMenuGetLink(itemNo);
        menuNo = 0;
        for (i = 0; i < firstItemNo; i++)
        {
            if (dbgMenuIsTerminate(i))
            {
                menuNo++;
            }
        }
    } else {
        menuNo = 0;
        for (i = 0; i < dbgMenuGetRootMenu(); i++)
        {
            if (dbgMenuIsTerminate(i))
            {
                menuNo++;
            }
        }
    }
    return menuNo;
}

long _dbgMenuCheckTerminate(tagWINDOW_WORK *window, long index)
{
    DbgMenuItem *item = dbgMenuGetPtr(_dbgMenuGetItemNo(window, index));
    u8 terminate;
    if (item == NULL)
    {
        terminate = 1;
    } else {
        terminate = item->mTerminate != 0;
    }
    return terminate;
}

long _dbgMenuGetMenuNum(tagWINDOW_WORK *window, long *width)
{
    if (width)
    {
        *width = 0;
    }
    long i = 0;
    do {
        u32 msgID = _dbgMenuGetMsgID(window, i);
        if (width)
        {
            long w = GSmsgGetMessageSize(msgID) >> 16;
            if (*width < w)
            {
                *width = w;
            }
        }
    } while (!_dbgMenuCheckTerminate(window, i++));
    return i;
}

long _dbgMenuSub(long level)
{
    long result = DBG_MENU_RESULT_BACK;
    long menuID = _menu_data_number + DBG_MENU_WINDOW_OFFSET + level;

    for (;;)
    {
        long parentID = level < 0 ? 0 : _menu_data_number + level;
        long *cursor = &_pCursor[_dbgMenuGetMenuNo(parentID)];
        long menuResult = menuOpenCustom(menuID, parentID, cursor, 0, true, 0);
        tagWINDOW_WORK *window = windowSearchID(menuID);
        if (window)
        {
            *cursor = window->mCursor.x + window->mCursor.y;
        } else {
            *cursor = 0;
        }

        if (menuResult == DBG_MENU_RESULT_CANCEL)
        {
            if (level == 0)
            {
                result = DBG_MENU_RESULT_CANCEL;
            }
            break;
        }

        long itemNo = _dbgMenuGetItemNo(window, window->mCursor.x + window->mCursor.y);
        s16 link = _dbgMenuGetLink(itemNo);
        if (link != DBG_MENU_LINK_NONE)
        {
            DbgMenuFunc func = dbgMenuGetFunc(itemNo);
            long action = func ? func(itemNo, window->mCursor.x + window->mCursor.y) : DBG_MENU_ACTION_SUBMENU;
            if (action == DBG_MENU_ACTION_CLOSE)
            {
                menuClose(_menu_data_number + DBG_MENU_WINDOW_OFFSET);
                return DBG_MENU_RESULT_CLOSE;
            }
            if (action == DBG_MENU_ACTION_REDISPLAY || link == DBG_MENU_LINK_COMMAND)
            {
                continue;
            }
            if (_dbgMenuSub(level + 1) == DBG_MENU_RESULT_CLOSE)
            {
                return DBG_MENU_RESULT_CLOSE;
            }
        } else {
            menuClose(_menu_data_number + DBG_MENU_WINDOW_OFFSET);
            DbgMenuFunc func = dbgMenuGetFunc(itemNo);
            if (func)
            {
                func(itemNo, window->mCursor.x + window->mCursor.y);
            }
            return DBG_MENU_RESULT_CLOSE;
        }
    }
    menuClose(menuID);
    return result;
}

extern "C" void dbgMenuMain(u8 loop)
{
    if (_dbgLayerID == GSGFX_LAYER_INVALID || !GSgfxLayerFindByID(_dbgLayerID))
    {
        if (dbgMenuIsOpen() != 0)
        {
            dbgMenuClose();
        }
        return;
    }
    if (dbgMenuGetNum() && _dbgMenuOpenFlag && dbgMenuIsOpen() == 0)
    {
        _dbgMenuLoop = loop;
        do {
            if (_dbgMenuSub(0) < DBG_MENU_RESULT_BACK)
            {
                break;
            }
        } while (_dbgMenuLoop == DBG_MENU_REPEAT);
    }
}

extern "C" void dbgMenuClose()
{
    _dbgMenuLoop = DBG_MENU_ONCE;
    menuClose(_menu_data_number + DBG_MENU_WINDOW_OFFSET);
}

extern "C" u8 dbgMenuIsOpen()
{
    return menuIsCheck(_menu_data_number + DBG_MENU_WINDOW_OFFSET) != 0;
}

extern "C" void dbgMenuSetOpenFlag(s32 flag)
{
    _dbgMenuOpenFlag = flag;
}

extern "C" u8 dbgMenuGetOpenFlag()
{
    return _dbgMenuOpenFlag;
}

extern "C" void dbgMenuCursor(tagWINDOW_WORK *window)
{
    u16 key = windowGetKeyInfo()->mKeyRepeat;
    s8 menuNum = _dbgMenuGetMenuNum(window, 0);
    s8 visibleRows = menuDataBiosGetDispMax(window->mMenuID);
    CURSOR cursor;

    if (menuNum < visibleRows)
    {
        visibleRows = menuNum;
    }
    cursor.x = window->mCursor.x;
    cursor.y = window->mCursor.y;

    if (key & WINDOW_KEY_UP)
    {
        cursor.y--;
    } else if (key & WINDOW_KEY_DOWN)
    {
        cursor.y++;
    }

    if (cursor.y < 0)
    {
        cursor.x += cursor.y;
        cursor.y = 0;
        if (cursor.x < 0)
        {
            cursor.y = (s8)(visibleRows - 1);
            cursor.x = (s8)(menuNum - visibleRows);
        }
    } else if (cursor.y >= visibleRows)
    {
        cursor.x += cursor.y - (visibleRows - 1);
        cursor.y = (s8)(visibleRows - 1);
        if (cursor.x + cursor.y >= menuNum)
        {
            cursor.x = 0;
            cursor.y = 0;
        }
    }

    window->mCursor.x = cursor.x;
    window->mCursor.y = cursor.y;
}

extern "C" void dbgMenuSetLayerID(GSgfxLayerID layer)
{
    _dbgLayerID = layer;
}

extern "C" GSgfxLayerID dbgMenuGetLayerID()
{
    return _dbgLayerID;
}
