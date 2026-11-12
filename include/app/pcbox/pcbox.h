#ifndef INCLUDE_APP_PCBOX_PCBOX_H
#define INCLUDE_APP_PCBOX_PCBOX_H

#include <Pokemon.h>
#include <item.h>
#include <types.h>

struct PCStorageBox {
    u16 name[10];
    Pokemon pokemon[30];
};

struct PCStorageRecord {
    PCStorageBox boxes[8];
    ItemSlot items[235];
    u32 reserved;
};

class PCBOX {
public:
    enum {
        PokemonBoxCount = 8,
        PokemonPerBox = 30,
        BoxNameLength = 10,
        BoxNameMaxLength = 8,
        ItemSlotCount = 235
    };

    static PCBOX* getPointer();
    void init();
    void debugPokemonSetup();
    void debugPokemonSetupFull();
    bool isPokemonFull();
    signed char getPokemonBoxNbEmptySlot(signed char box);
    signed char getPokemonBoxNbUsedSlot(signed char box);
    u16* getPokemonBoxName(signed char box);
    bool addPokemon(Pokemon* pokemon);
    bool addPokemon(Pokemon* pokemon, signed char box);
    bool delPokemon(signed char box, signed char slot);
    bool setPokemon(Pokemon* pokemon, signed char box, signed char slot);
    Pokemon* getPokemon(signed char box, signed char slot);

private:
    void initBoxes();
    PCStorageRecord mStorage;
};

extern "C" {
void pcboxDebugPokemonSetupFull(void);
void pcboxDebugPokemonSetup(void);
bool pcboxSwapItemSlot(PCStorageRecord* storage, s16 first, s16 second);
u16 pcboxGetItemCapacity(PCStorageRecord* storage, u16 itemID);
u16 pcboxDelItem(PCStorageRecord* storage, u16 itemID, u16 quantity);
u16 pcboxAddItem(PCStorageRecord* storage, u16 itemID, u16 quantity);
ItemSlot* pcboxGetItem(PCStorageRecord* storage, s16 slot);
s32 pcboxGetNbItemSlot(PCStorageRecord* storage);
s32 pcboxGetNbPokemonBox(void);
s8 pcboxGetPokemonBoxNbEmptySlot(PCStorageRecord* storage, s8 box);
s32 pcboxSetPokemonBoxName(PCStorageRecord* storage, s8 box, const u16* name);
u16* pcboxGetPokemonBoxName(PCStorageRecord* storage, s8 box);
s32 pcboxAddPokemon(PCStorageRecord* storage, Pokemon* pokemon, s8 box);
void pcboxInit(PCStorageRecord* storage);
void pcboxSetStatus(void);
s32 pcboxGetStatus(void);
}

#endif
