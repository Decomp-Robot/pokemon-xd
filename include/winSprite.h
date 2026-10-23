#ifndef WIN_SPRITE_H
#define WIN_SPRITE_H

#include <types.h>

// Usable from C as well as C++.
typedef struct tagSPRITE_WORK tagSPRITE_WORK;
typedef struct tagWINDOW_WORK tagWINDOW_WORK;
typedef struct NAME_ENTRY_ARG NAME_ENTRY_ARG;

// Cursor record of a window, handed to the sub-menu switcher by value. The
// cursor itself is the window cursor at 0x02; 0x00 is the sub-menu state the
// window keeps next to it.
struct CURSOR {
    s16 mSubMenu; // 0x00: sub-menu the window is in.
    s16 mCursor;  // 0x02: cursor position of the window.
};

// Window work, partial. The sprite-work list head at 0x24 is walked by
// menuPtWindowWorkDrawPrime; GetNowCursor reads the cursor at 0x9E. The
// custom-menu argument at 0x68 is what the name-entry menus read back. The
// remaining layout is unrecovered and stays sized padding here.
struct tagWINDOW_WORK {
    u8 _00;                      // 0x00: unknown.
    s8 mState;                   // 0x01: menu sequence step the window is in.
    s8 mFlag;                    // 0x02: set by the step's hook, cleared when the step advances.
    u8 _03;                      // 0x03: unknown.
    long mMenuID;                // 0x04: menu data id used for item lookups.
    u8 _08[0x08];                // 0x08-0x0F: unknown.
    tagWINDOW_WORK *mNext;       // 0x10: next window in the window list.
    u8 _14[0x08];                // 0x14-0x1B: unknown.
    long mLayerID;               // 0x1C: GSgfx layer the window draws on.
    u8 _20[0x04];                // 0x20-0x23: unknown.
    tagSPRITE_WORK *mSpriteWork; // 0x24: head of the window sprite-work list.
    u8 _28[0x04];                // 0x28-0x2B: head of a second work list.
    u8 mSeq[0x3C];               // 0x2C: sequence record of the menu the window runs.
    NAME_ENTRY_ARG *mArg;        // 0x68: argument the custom menu was opened with.
    u8 _6C[0x24];                // 0x6C-0x8F: unknown.
    union {
        u32 mColor;              // 0x90: RGBA colour passed to the message printer.
        struct {
            u8 mColorR;          // 0x90
            u8 mColorG;          // 0x91
            u8 mColorB;          // 0x92
            u8 mColorA;          // 0x93: alpha channel; also read on its own.
        } mRgba;
    };
    u8 _94[0x08];                // 0x94-0x9B: unknown.
    s16 mSubMenu;                // 0x9C: sub-menu the window is in.
    s16 cursor;                  // 0x9E: selected cursor.
    s16 mUnkA0;                  // 0xA0: unknown.
    union {
        s16 mSubMenuID;          // 0xA2: last opened sub-menu.
        s16 cursorTo;            // 0xA2: scroll destination.
    };
    u8 mAccept;                  // 0xA4: set when the menu is confirmed.
    u8 mCancel;                  // 0xA5: set when the menu is cancelled.
};

// Sprite work node (0x78 bytes). winSpriteAdd hands these out of a fixed pool
// and chains them through +0x00.
struct tagSPRITE_WORK {
    tagSPRITE_WORK *mNext; // 0x00: next node in the list.
    u8 _04[0x02];          // 0x04-0x05: unknown.
    s16 mItemID;           // 0x06: item id the node displays for the menu.
    u8 _08[0x40];          // 0x08-0x47: unknown.
    void *mHookFunction;   // 0x48: hook the node was registered with.
    u32 mMsgID;            // 0x4C: message id the node prints.
    s16 mX;                // 0x50: x position the row hooks place the sprite at.
    s16 mY;                // 0x52: y position the row hooks place the sprite at.
    s16 mWidth;            // 0x54: width of the sprite.
    s16 mHeight;           // 0x56: height of the sprite.
    u8 _58[0x0C];          // 0x58-0x63: unknown.
    /* 0x64-0x67: the sprite colour word. The title cursor hook clears the
     * whole word to -1 to drop whatever blend override is in place; the entry
     * menus set the channels apart on it, and everything else writes only the
     * alpha byte at the top of it. */
    union {
        u32 mColor;            // 0x64: whole colour word.
        struct {
            u8 mRed;           // 0x64
            u8 mGreen;         // 0x65
            u8 mBlue;          // 0x66
            u8 mAlpha;         // 0x67: alpha the sprite is drawn with.
        } mRgba;
    };
    u8 _68[0x0C];          // 0x68-0x73: unknown.
    u8 mUnk74;             // 0x74: flag read by winSpriteGetLayerID.
};

// Key state read by windowGetKeyOn/windowGetKeyTrg/windowGetKeyRepeat
// (menu.a window.o) and directly by the debug menus.
struct tagWINDOW_KEY {
    u16 mKeyOn;     // 0x00: keys held this frame.
    u16 _02;        // 0x02: unknown.
    u16 mKeyTrg;    // 0x04: keys pressed this frame.
    u16 mKeyRepeat; // 0x06: repeating keys.
};

#ifdef __cplusplus
extern "C" {
#endif

// Key state of the window the debug menus read directly (menu.a window.o).
struct tagWINDOW_KEY *windowGetKeyState();

#ifdef __cplusplus
}
#endif

#endif
