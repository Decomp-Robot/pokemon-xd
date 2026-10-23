#ifndef MENU_POCKET2_H
#define MENU_POCKET2_H

#include <types.h>

// Item-menu (pocket2) work (bss 0x804282E4, 0x54 bytes).
// Partial declaration: only the slots addressed by the recovered helpers
// are named (current/selected temochi slot IDs at +0x10/+0x14 and the last
// slot ID at +0x44); the rest of the block layout is unrecovered, so it
// stays sized padding here.
struct MenuPocket2Work {
    u8 _00[0x10];
    u32 mCurrentSlotID; // 0x10: current temochi pokemon slot ID.
    u32 mSelectedSlotID; // 0x14: selected temochi pokemon slot ID.
    u8 _18[0x2C]; // 0x18-0x43: remainder of the block; unknown here.
    u32 mLastSlotID; // 0x44: last selected slot ID.
    u8 _48[0x0C]; // 0x48-0x53: remainder of the block; unknown here.
};

extern MenuPocket2Work menuPocket2Work;

#ifdef __cplusplus
extern "C" {
#endif

u32 menuPocket2GetCurrentTemochiPokemonSlotID(void);
u8 menuPocket2GetLastSlotID(void);

#ifdef __cplusplus
}
#endif

#endif
