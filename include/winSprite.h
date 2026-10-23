#ifndef WIN_SPRITE_H
#define WIN_SPRITE_H

#include <types.h>

// Usable from C as well as C++.
typedef struct tagSPRITE_WORK tagSPRITE_WORK;
typedef struct tagWINDOW_WORK tagWINDOW_WORK;

// Window work, partial. The sprite-work list head at 0x24 is walked by
// menuPtWindowWorkDrawPrime; GetNowCursor reads the cursor at 0x9E. The
// remaining layout is unrecovered and stays sized padding here.
struct tagWINDOW_WORK {
    u8 _00[0x04];                // 0x00-0x03: unknown.
    long mMenuID;                // 0x04: menu data id used for item lookups.
    u8 _08[0x14];                // 0x08-0x1B: remainder; unknown here.
    long mLayerID;               // 0x1C: GSgfx layer the window draws on.
    u8 _20[0x04];                // 0x20-0x23: unknown.
    tagSPRITE_WORK *mSpriteWork; // 0x24: head of the window sprite-work list.
    u8 _28[0x76];                // 0x28-0x9D: remainder; unknown here.
    s16 cursor;                  // 0x9E: selected cursor.
};

// Sprite work node (0x78 bytes). winSpriteAdd hands these out of a fixed
// pool and chains them through +0x00; the hook at +0x48 is the routine
// the window invokes for the sprite.
struct tagSPRITE_WORK {
    tagSPRITE_WORK *mNext; // 0x00: next node in the list.
    u8 _04[0x02];          // 0x04-0x05: unknown.
    s16 mItemID;           // 0x06: item id the node displays for the menu.
    u8 _08[0x40];          // 0x08-0x47: remainder; unknown here.
    void *mHookFunction;   // 0x48: hook the node was registered with.
    u8 _4C[0x06];          // 0x4C-0x51: unknown.
    s16 mY;                // 0x52: y position set by the item cursor hooks.
    u8 _54[0x24];          // 0x54-0x77: remainder; unknown here.
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
struct tagWINDOW_KEY *zz_0116a80_();

#ifdef __cplusplus
}
#endif

#endif
