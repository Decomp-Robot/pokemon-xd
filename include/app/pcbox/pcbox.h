#ifndef INCLUDE_APP_PCBOX_PCBOX_H
#define INCLUDE_APP_PCBOX_PCBOX_H

#include <Pokemon.h>
#include <types.h>

class PCBOX {
public:
    enum { PokemonBoxCount = 8, PokemonPerBox = 30, BoxNameLength = 10 };

    static PCBOX* getPointer();
    void init();
    bool setPokemon(Pokemon* pokemon, signed char box, signed char slot);
    Pokemon* getPokemon(signed char box, signed char slot);
    u16* getPokemonBoxName(signed char box);

private:
    struct PokemonBox {
        u16 mName[BoxNameLength];
        Pokemon mPokemon[PokemonPerBox];
    };

    PokemonBox mPokemonBoxes[PokemonBoxCount];
};

// PC inventory stored alongside the trainer's Pokemon boxes.
typedef struct PCStorageRecord PCStorageRecord;
extern "C" {
u16 pcboxGetItemCapacity(PCStorageRecord* storageRecord, u16 itemID);
u16 pcboxAddItem(PCStorageRecord* storageRecord, u16 itemID, u16 quantity);
s8 pcboxGetPokemonBoxNbEmptySlot(PCStorageRecord* storageRecord, s8 boxIndex);
s32 pcboxSetPokemonBoxName(PCStorageRecord* storageRecord, s8 boxIndex, const u16* name);
u16* pcboxGetPokemonBoxName(PCStorageRecord* storageRecord, s8 boxIndex);
s32 pcboxGetNbPokemonBox(void);
}

#endif
