#ifndef MENU_POCKET_BATTLE_DISK_H
#define MENU_POCKET_BATTLE_DISK_H

#include <types.h>

// BattleDisk pocket-menu work (bss 0x80424058, 0x28 bytes).
// This header declares only what getCursorPositionY observes: the cursor
// row at +0x0E (lha), the fine scroll offset at +0x10 (lfs) and the hold
// flag at +0x14 (lbz). Everything else in the 0x28-byte block is unobserved
// by this translation unit, so it stays sized padding; material layout
// beyond this prefix remains unknown here.
struct MenuPocketBattleDiskWork {
    u8 _00[0x0E];
    s16 mCursorRow;    // 0x0E: cursor row; row height is 30 (see below).
    f32 mScrollOffset; // 0x10: fine scroll offset added when idle.
    u8 mScrollHold;    // 0x14: when nonzero the fine offset is ignored.
    u8 _15[0x13];      // 0x15-0x27: remainder of the block; unknown here.
};

extern MenuPocketBattleDiskWork gMenuPocketBattleDiskWork;

s32 getCursorPositionY();

#endif
