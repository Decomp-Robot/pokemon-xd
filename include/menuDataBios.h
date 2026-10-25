#ifndef MENUDATABIOS_H
#define MENUDATABIOS_H
#include <types.h>
struct tagWINDOW_WORK;

// mCursorMode / mButtonMode value that makes the window call the handler
// stored in the menu's MenuData entry instead of the built-in behaviour.
enum { MENU_DATA_MODE_CALLBACK = 4 };

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
MenuData *menuDataBiosGetPtr(s32 menuID);
#ifdef __cplusplus
}
#endif
#endif
