#ifndef DECK_DATA_VIRTUAL_BIOS_H
#define DECK_DATA_VIRTUAL_BIOS_H

#include <types.h>

u8 deckDataVirtualDB_GetFlagData(u16 disk);
u16 deckDataVirtualDB_GetAllyDeck(u16 disk);
u16 deckDataVirtualDB_GetEnemyDeck(u16 disk);
u8 deckDataVirtualDB_GetPokemonNum(u16 disk);
u32 deckDataVirtualDB_GetExplanetion1(u16 disk);
u32 deckDataVirtualDB_GetExplanetion2(u16 disk);
u32 deckDataVirtualDB_GetTitle(u16 disk);

#endif
