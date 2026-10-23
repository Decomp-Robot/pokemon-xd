#ifndef PCBOX_H
#define PCBOX_H

#include <types.h>
#include <Pokemon.h>

class PCBOX {
public:
    static PCBOX *getPointer();
    void init();
    void setPokemon(Pokemon *pokemon, signed char box, signed char slot);
    Pokemon *getPokemon(signed char box, signed char slot);
    u16 *getPokemonBoxName(signed char box);
};

// PC inventory stored alongside the trainer's Pokemon boxes.
typedef struct PCStorageRecord PCStorageRecord;
extern "C" {
u16 pcboxGetItemCapacity(PCStorageRecord *storageRecord, u16 itemID);
u16 pcboxAddItem(PCStorageRecord *storageRecord, u16 itemID, u16 quantity);
}

extern "C" {
s8 pcboxGetPokemonBoxNbEmptySlot(PCStorageRecord *storageRecord, s8 boxIndex);
s32 pcboxSetPokemonBoxName(PCStorageRecord *storageRecord, s8 boxIndex, const u16 *name);
u16 *pcboxGetPokemonBoxName(PCStorageRecord *storageRecord, s8 boxIndex);
s32 pcboxGetNbPokemonBox(void);
}
#endif
