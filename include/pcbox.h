#ifndef PCBOX_H
#define PCBOX_H

#include <types.h>

// PC storage region of a trainer's save data.
typedef struct PCStorageRecord PCStorageRecord;

#ifdef __cplusplus
extern "C" {
#endif

// A null storage record selects the current trainer's PC storage.
u16 pcboxGetItemCapacity(PCStorageRecord *storageRecord, u16 itemID);
// Returns the quantity not stored, including the full quantity for an invalid ID.
u16 pcboxAddItem(PCStorageRecord *storageRecord, u16 itemID, u16 quantity);

s8 pcboxGetPokemonBoxNbEmptySlot(s32 boxType, s8 boxIndex);
s32 pcboxGetNbPokemonBox(void);

#ifdef __cplusplus
}
#endif

#endif
