#ifndef DBGMENU_H
#define DBGMENU_H

#include <GSgfxTypes.h>

struct tagWINDOW_WORK;

enum DbgMenuAction {
    DBG_MENU_ACTION_REDISPLAY = -1,
    DBG_MENU_ACTION_CLOSE = 0,
    DBG_MENU_ACTION_SUBMENU = 1
};

enum DbgMenuLink {
    DBG_MENU_LINK_NONE = 0,
    DBG_MENU_LINK_COMMAND = 1
};

enum DbgMenuLoop {
    DBG_MENU_ONCE = 0,
    DBG_MENU_REPEAT = 1
};

typedef long (*DbgMenuFunc)(long itemNo, long cursor);

typedef struct DbgMenuItem {
    u8 mTerminate : 1;
    u8 mFlags : 7;
    s16 mLink;
    u32 mMsgID;
    DbgMenuFunc mFunc;
} DbgMenuItem;

#ifdef __cplusplus
extern "C" {
#endif

GSgfxLayerID dbgMenuGetLayerID(void);
void dbgMenuSetLayerID(GSgfxLayerID layer);
void dbgMenuCursor(struct tagWINDOW_WORK *window);
u8 dbgMenuGetOpenFlag(void);
u8 dbgMenuIsOpen(void);
void dbgMenuMain(u8 loop);
void dbgMenuClose(void);
long dbgMenuGetRootMenu(void);
void dbgMenuSetOpenFlag(s32 flag);
s32 dbgMenuGetGsinputMsgId(s32 deviceId);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
DbgMenuItem *dbgMenuGetPtr(long itemNo);
u32 dbgMenuGetNum(void);
s16 dbgMenuGetLink(long itemNo);
#endif

#endif
