#ifndef MENUDATABIOS_H
#define MENUDATABIOS_H
#include <types.h>
#include <GSgfxTypes.h>
struct tagWINDOW_WORK;

#ifdef __cplusplus
extern "C" {
#endif

s32 menuBiosGetCursorSize(struct tagWINDOW_WORK *window);

#ifdef __cplusplus
}
#endif

// mCursorMode / mButtonMode value that makes the window call the handler
// stored in the menu's MenuData entry instead of the built-in behaviour.
enum { MENU_DATA_MODE_CALLBACK = 4 };

enum WindowValueMode {
    WINDOW_VALUE_NONE = 0,
    WINDOW_VALUE_CURSOR = 1,
    WINDOW_VALUE_INDEX = 2,
    WINDOW_VALUE_ITEM = 3
};

typedef struct MenuData {
    u8 mItemTable : 3;
    u8 mWindowValueMode : 2;
    u8 reserved00Middle : 1;
    u8 mOpenPriorityFlag : 1;
    u8 mIgnoreCloseRequest : 1;
    u8 mSoundEffectTable : 4;
    u8 mCursorMode : 3;
    u8 reserved01Low : 1;
    u8 mButtonMode : 3;
    u8 reserved02 : 5;
    u8 mDisplayMaximum;  // Number of entries the menu shows at once.
    u8 mCursorID;
    s16 mFirstItemID;
    s16 mPositionX, mPositionY;
    void (*mCursorFunction)(struct tagWINDOW_WORK *);   // Cursor movement handler.
    void (*mButtonFunction)(struct tagWINDOW_WORK *);   // Button handler.
    long (*mControlFunction)(struct tagWINDOW_WORK *);  // Update handler, also notified when the window opens or closes.
    long (*mDrawFunction)(struct tagWINDOW_WORK *);     // Draw handler.
} MenuData;

#ifdef __cplusplus
extern "C" {
#endif
extern long _menu_data_number;
MenuData *menuDataBiosGetPtr(s32 menuID);
u8 menuDataBiosGetDispMax(s32 menuID);
GSgfxLayerID menuItemBiosGetLayerID(s32 dataID);
#ifdef __cplusplus
}
#endif
#endif
