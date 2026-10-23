#ifndef MENU_DATA_BIOS_H
#define MENU_DATA_BIOS_H

#include <types.h>

struct tagWINDOW_WORK;

// Window geometry and callbacks shared by the menu data tables.
struct MenuWindowData {
    u8 mWindowType;
    union {
        u8 mCursorFlags;
        struct {
            u8 mCursorFlagsHigh : 4;
            u8 mCursorMode : 3;
            u8 mCursorFlagsLow : 1;
        } mCursor;
    };
    union {
        u8 mButtonFlags;
        struct {
            u8 mButtonMode : 3;
            u8 mButtonFlagsLow : 5;
        } mButton;
    };
    u8 mDisplayMaximum;
    u8 mWindowFlags;
    short mWindowParameter;
    short mX;
    short mY;
    void (*mCursorFunction)(struct tagWINDOW_WORK*);
    void (*mButtonFunction)(struct tagWINDOW_WORK*);
    void (*mControlFunction)(struct tagWINDOW_WORK*);
    void (*mDrawFunction)(struct tagWINDOW_WORK*);
};

enum MenuWindowCallbackMode {
    MENU_WINDOW_CALLBACK_CUSTOM = 4
};

#ifdef __cplusplus
extern "C" {
#endif

struct MenuWindowData* menuDataBiosGetPtr(long menuID);

#ifdef __cplusplus
}
#endif

#endif
