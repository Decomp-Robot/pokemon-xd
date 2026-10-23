#ifndef MENU_POCKET2_H
#define MENU_POCKET2_H

#include <types.h>

// Item-menu (pocket2) work (bss 0x804282E4, 0x54 bytes).
// Partial declaration: menuPocket2GetLastSlotID only reads the last
// slot ID at +0x44 (returned as a byte); the rest of the block layout
// is unrecovered, so it stays sized padding here.
struct MenuPocket2Work {
    u8 _00[0x44];
    u32 mLastSlotID; // 0x44: last selected slot ID.
    u8 _48[0x0C];    // 0x48-0x53: remainder of the block; unknown here.
};

extern MenuPocket2Work menuPocket2Work;

#ifdef __cplusplus
extern "C" {
#endif

u8 menuPocket2GetLastSlotID(void);

#ifdef __cplusplus
}
#endif

#endif
