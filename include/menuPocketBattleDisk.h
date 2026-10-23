#ifndef MENU_POCKET_BATTLE_DISK_H
#define MENU_POCKET_BATTLE_DISK_H

#include <types.h>

// Battle-disk menu work (bss 0x80424058, 0x28 bytes).
// Partial declaration: only the fields addressed by getCursorPositionY
// are named (row index at +0x0E, scroll offset at +0x10, hold flag at
// +0x14); the rest of the block layout is unrecovered, so it stays
// sized padding here.
struct MenuPocketBattleDiskWork {
    u8 _00[0x0E];
    s16 mRow;    // 0x0E: row index.
    f32 mScroll; // 0x10: scroll offset.
    u8 mHold;    // 0x14: hold flag; when nonzero the scroll is ignored.
    u8 _15[0x13]; // 0x15-0x27: remainder of the block; unknown here.
};

extern MenuPocketBattleDiskWork menuPocketBattleDiskWork;

s32 getCursorPositionY(void);

#endif
