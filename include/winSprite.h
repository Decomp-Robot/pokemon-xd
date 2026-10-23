#ifndef WIN_SPRITE_H
#define WIN_SPRITE_H

#include <types.h>

struct tagSPRITE_WORK;

// Window work, partial. The sprite-work list head at 0x24 is walked by
// menuPtWindowWorkDrawPrime; GetNowCursor reads the cursor at 0x9E. The
// remaining layout is unrecovered and stays sized padding here.
struct tagWINDOW_WORK {
    u8 _00[0x04];                // 0x00-0x03: unknown.
    long mMenuID;                // 0x04: menu data id used for item lookups.
    u8 _08[0x1C];                // 0x08-0x23: remainder; unknown here.
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
    u8 _4C[0x2C];          // 0x4C-0x77: remainder; unknown here.
};

#endif
