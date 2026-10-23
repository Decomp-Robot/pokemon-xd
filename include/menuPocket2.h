#ifndef MENU_POCKET2_H
#define MENU_POCKET2_H

#include <types.h>

struct FIGHT_TRAINER;
class Hero;

// Item-menu (pocket2) work (bss 0x804282E4, 0x54 bytes).
// Partial declaration: only the slots addressed by the recovered helpers
// are named (the open-source mode at +0x04, the hero and battle-trainer
// contexts at +0x08/+0x0C, current/selected temochi slot IDs at +0x10/+0x14
// and the last slot ID at +0x44); the rest of the block layout is
// unrecovered, so it stays sized padding here.
struct MenuPocket2Work {
    u8 _00[0x04]; // 0x00-0x03: unknown.
    s32 mMode; // 0x04: menu the pocket was opened from.
    FIGHT_TRAINER *mTrainer; // 0x08: battle trainer the menu acts on.
    Hero *mHero; // 0x0C: hero the menu acts on.
    u32 mCurrentSlotID; // 0x10: current temochi pokemon slot ID.
    u32 mSelectedSlotID; // 0x14: selected temochi pokemon slot ID.
    u8 _18[0x2C]; // 0x18-0x43: remainder of the block; unknown here.
    u32 mLastSlotID; // 0x44: last selected slot ID.
    u8 _48[0x0C]; // 0x48-0x53: remainder of the block; unknown here.
};

extern MenuPocket2Work menuPocket2Work;

// Item-script queue (bss 0x804282A0, 0x44 bytes): the item scripts the
// pocket menu queues for the script engine, eight entries plus the number
// of live entries. An entry is an item ID and the script queued for it;
// the two bytes between them are alignment padding.
struct MenuPocket2ItemScriptEntry {
    u16 mID; // 0x0: item the script belongs to.
    s32 mScript; // 0x4: script queued for the item.
};

struct MenuPocket2ItemScript {
    MenuPocket2ItemScriptEntry mEntries[8]; // 0x00: queued item scripts.
    s32 mCount; // 0x40: number of live entries.
};

extern MenuPocket2ItemScript menuPocket2ItemScript;

#ifdef __cplusplus
extern "C" {
#endif

void menuPocket2GetHero(Hero **hero, FIGHT_TRAINER **trainer);
u32 menuPocket2GetCurrentTemochiPokemonSlotID(void);
u8 menuPocket2GetLastSlotID(void);
void menuPocketClearItemScript(void);
s32 menuPocket2GetItemScript(u16 id);

#ifdef __cplusplus
}
#endif

#endif
