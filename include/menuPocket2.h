#ifndef MENU_POCKET2_H
#define MENU_POCKET2_H

#include <types.h>

enum PocketItemID {
    POCKET_ITEM_COLOGNE = 0x200,
    POCKET_ITEM_BATTLE_DISK = 0x215,
};

enum PocketMessageID {
    POCKET_MSG_CANNOT_DEPOSIT_ITEM = 0xC37B,
    POCKET_MSG_DEPOSIT_QUANTITY = 0x3AAC,
};

struct FIGHT_TRAINER;
struct Pokemon;
struct FIGHT_POKEMON;
class Hero;

// Item-menu (pocket2) work (bss 0x804282E4, 0x54 bytes). Only the slots the
// recovered pocket menu addresses are named; the rest of the block layout is
// unrecovered and stays sized padding here.
struct MenuPocket2Work {
    u8 mOwnsFloor; // 0x00: this invocation owns a pushed pocket floor.
    u8 _01[0x03]; // 0x01-0x03: padding.
    s32 mMode; // 0x04: menu the pocket was opened from.
    FIGHT_TRAINER *mTrainer; // 0x08: battle trainer the menu acts on.
    Hero *mHero; // 0x0C: hero the menu acts on.
    u32 mCurrentSlotID; // 0x10: current temochi pokemon slot ID.
    u32 mSelectedSlotID; // 0x14: selected temochi pokemon slot ID.
    f32 mTitleScroll; // 0x18: horizontal offset during tab transitions.
    f32 mListScroll; // 0x1C: how far the item list is scrolled, in pixels.
    u8 mListScrollFlag; // 0x20: non-zero while the list is scrolled.
    u8 _21[0x03]; // 0x21-0x23: alignment.
    f32 mRollFade; // 0x24: pulse phase of the scroll arrows.
    u32 mExplanationMsgID; // 0x28: message shown under the item list.
    s32 mResult; // 0x2C: result of the pocket loop, stored by menuPocketMain.
    s32 mMsgWindowID; // 0x30: window the shop messages are printed into.
    s32 mNumInputValue; // 0x34: value the number menu returned.
    s32 mSwapItemIndex; // 0x38: marked item index for swapping; -1 when none.
    u8 mSelectingItem; // 0x3C: non-zero while the item-list selection is active.
    u8 _3D[0x03]; // 0x3D-0x3F: padding.
    s32 mTab; // 0x40: tab the item list shows.
    u32 mLastSlotID; // 0x44: last selected slot ID.
    f32 mSelectedItemPulseTime; // 0x48: elapsed time of the swap-marker pulse.
    u8 mSkipActMenuResize; // 0x4C: non-zero skips the action-menu measurement.
    u8 _4D[0x07]; // 0x4D-0x53: remainder of the block; unknown here.
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

// Party/battle contexts selected by the item menu.
void menuPocket2GetTemochiPokemon(u32 slot, Pokemon **pokemon, FIGHT_POKEMON **fightPokemon);
void menuPocket2ClosePokemonMenu(u32 slot);
s32 menuPocket2OpenPokemonMenu(u32 itemID, int wait);
void menuPocketMain(void);
u16 menuPocket2Call(int mode, int messageWindowID, Hero *hero);
u16 menuPocket2Open(int mode, s32 *slotID, void *actor);

void menuPocket2GetHero(Hero **hero, FIGHT_TRAINER **trainer);
u32 menuPocket2GetCurrentTemochiPokemonSlotID(void);
u8 menuPocket2GetLastSlotID(void);
void menuPocketClearItemScript(void);
void menuPocket2AddItemScript(u32 item, u32 script);
s32 menuPocket2GetItemScript(u16 id);

#ifdef __cplusplus
}
#endif

#endif
