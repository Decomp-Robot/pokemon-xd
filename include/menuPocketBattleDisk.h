#ifndef MENU_POCKET_BATTLE_DISK_H
#define MENU_POCKET_BATTLE_DISK_H

#include <types.h>

// BattleDisk pocket-menu work block (bss 0x80424058, 0x28 bytes).
// This header declares only what getCursorPositionY reads: the item row
// at +0x0E (lha), the smooth scroll offset at +0x10 (lfs) and the scroll
// hold flag at +0x14 (lbz). Everything else in the block is unobserved
// by this translation unit, so it stays sized padding; material layout
// beyond these fields remains unknown here.
struct MenuPocketBattleDiskWork {
    u8 _00[0x0E];      // 0x00-0x0D: unknown here.
    s16 mItemRow;      // 0x0E: item list row; each row is 30 pixels tall.
    f32 mScrollOffset; // 0x10: fine scroll offset folded in while scrolling.
    u8 mScrollHold;    // 0x14: scroll animation hold flag; nonzero skips the offset.
    u8 _15[0x13];      // 0x15-0x27: remainder of the block; unknown here.
};

extern MenuPocketBattleDiskWork gMenuPocketBattleDiskWork;

s32 getCursorPositionY();

#endif
