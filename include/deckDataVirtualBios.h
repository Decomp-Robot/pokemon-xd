#ifndef DECK_DATA_VIRTUAL_BIOS_H
#define DECK_DATA_VIRTUAL_BIOS_H

#include <types.h>

#define DECK_DATA_VIRTUAL_ITEM_COUNT 4
#define DECK_DATA_VIRTUAL_JOUTAI_COUNT 6

/* One item slot supplied to a Battle CD trainer, with its count and item ID. */
struct DeckDataVirtualItem {
    u8 count;
    u8 unknown01;
    u16 id;
};

/* One Battle CD (virtual deck) record: the battle setup (turn limit, Pokemon
 * count, both decks), the title and explanation messages, the completion flag,
 * and the per-side start conditions and trainer inventory items. The deck,
 * message, flag and battle floor entries are IDs into the global deck, message,
 * flag and fight floor registries. */
struct DeckDataVirtual {
    u8 unknown00;
    u8 turnLimit;
    u8 pokemonNum;
    u8 unknown03;
    /* Battle floor the CD battle is fought on, 0 to keep the floor the
     * encounter data already selects. */
    u16 fightFloorDataId;
    u8 unknown06[2];
    u32 allyDeck;
    u32 enemyDeck;
    u32 title;
    u32 explanetion1;
    u32 explanetion2;
    u32 flagId;
    u8 allyJoutai[DECK_DATA_VIRTUAL_JOUTAI_COUNT];
    u8 enemyJoutai[DECK_DATA_VIRTUAL_JOUTAI_COUNT];
    DeckDataVirtualItem items[DECK_DATA_VIRTUAL_ITEM_COUNT];
};

void deckDataVirtualDB_SetFlagData(u16 disk, bool value);
u8 deckDataVirtualDB_GetFlagData(u16 disk);
u32 deckDataVirtualDB_GetFlag(u16 disk);
u8 deckDataVirtualDB_GetItemNum(u16 disk, u8 index);
u16 deckDataVirtualDB_GetItemID(u16 disk, u8 index);
u8 deckDataVirtualDB_GetEnemyJoutai(u16 disk, u8 index);
u8 deckDataVirtualDB_GetAllyJoutai(u16 disk, u8 index);
u32 deckDataVirtualDB_GetExplanetion1(u16 disk);
u32 deckDataVirtualDB_GetExplanetion2(u16 disk);
u32 deckDataVirtualDB_GetTitle(u16 disk);
u32 deckDataVirtualDB_GetAllyDeck(u16 disk);
u32 deckDataVirtualDB_GetEnemyDeck(u16 disk);
u8 deckDataVirtualDB_GetPokemonNum(u16 disk);
u8 deckDataVirtualDB_GetTurnLimit(u16 disk);
DeckDataVirtual* deckDataVirtual_GetPtr(u16 disk);

#endif
