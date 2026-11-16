#include <deckDataVirtualBios.h>
#include <GSflag.h>
#include <relglobal.h>

// The record of the Battle CD with the given disk index, or NULL when the
// index is past the end of the loaded deckdata_virtual table.
static inline DeckDataVirtual* deckDataVirtualGetRecord(u16 disk)
{
    if (disk >= *deckdata_virtual_number) {
        return NULL;
    }
    return &deckdata_virtual[disk];
}

// Writes the completion flag of the Battle CD.
void deckDataVirtualDB_SetFlagData(u16 disk, bool value)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return;
    }
    GSflagSet(data->flagId, value);
}

// Whether the player has already completed the Battle CD.
u8 deckDataVirtualDB_GetFlagData(u16 disk)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    return GSflagTest(data->flagId);
}

// The ID of the flag the Battle CD records its completion in.
u32 deckDataVirtualDB_GetFlag(u16 disk)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    return data->flagId;
}

// The count of the item held in the given item slot of the Battle CD.
u8 deckDataVirtualDB_GetItemNum(u16 disk, u8 index)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    if (index >= DECK_DATA_VIRTUAL_ITEM_COUNT) {
        return 0;
    }
    return data->items[index].count;
}

// The item ID held in the given item slot of the Battle CD.
u16 deckDataVirtualDB_GetItemID(u16 disk, u8 index)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    if (index >= DECK_DATA_VIRTUAL_ITEM_COUNT) {
        return 0;
    }
    return data->items[index].id;
}

// The status condition data ID the enemy side starts the Battle CD in.
u8 deckDataVirtualDB_GetEnemyJoutai(u16 disk, u8 index)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    if (index >= DECK_DATA_VIRTUAL_JOUTAI_COUNT) {
        return 0;
    }
    return data->enemyJoutai[index];
}

// The status condition data ID the player side starts the Battle CD in.
u8 deckDataVirtualDB_GetAllyJoutai(u16 disk, u8 index)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    if (index >= DECK_DATA_VIRTUAL_JOUTAI_COUNT) {
        return 0;
    }
    return data->allyJoutai[index];
}

// The details message of the Battle CD.
u32 deckDataVirtualDB_GetExplanetion2(u16 disk)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    return data->explanetion2;
}

// The description message of the Battle CD.
u32 deckDataVirtualDB_GetExplanetion1(u16 disk)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    return data->explanetion1;
}

// The title message of the Battle CD.
u32 deckDataVirtualDB_GetTitle(u16 disk)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    return data->title;
}

// The deck the enemy side of the Battle CD is built from.
u32 deckDataVirtualDB_GetEnemyDeck(u16 disk)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    return data->enemyDeck;
}

// The deck the player side of the Battle CD is built from.
u32 deckDataVirtualDB_GetAllyDeck(u16 disk)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    return data->allyDeck;
}

// How many Pokemon the player enters the Battle CD with.
u8 deckDataVirtualDB_GetPokemonNum(u16 disk)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    return data->pokemonNum;
}

// The turn limit of the Battle CD battle.
u8 deckDataVirtualDB_GetTurnLimit(u16 disk)
{
    DeckDataVirtual* data = deckDataVirtualGetRecord(disk);
    if (data == NULL) {
        return 0;
    }
    return data->turnLimit;
}

// The record of the Battle CD, or NULL for an unknown disk index.
DeckDataVirtual* deckDataVirtual_GetPtr(u16 disk)
{
    return deckDataVirtualGetRecord(disk);
}
