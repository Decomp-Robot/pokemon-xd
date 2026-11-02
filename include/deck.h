#ifndef DECK_H
#define DECK_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* File-system archive groups the fight resources are read from. */
enum DeckResourceGroup {
    DECK_RESOURCE_GROUP = 2114,  /* the deck archive group */
    COMMON_RESOURCE_GROUP = 10   /* the common archive group */
};

/* The Dark Pokemon deck. */
enum DeckResourceId {
    DECK_RESOURCE_DARK_POKEMON = 0x184D3400
};

u32 deckGetTrainerCount(void);
u32 getDeckResourceID(u8 deckId);
char *getDeckMsgID(u16 trainerId, u8 deckId);

extern u32 deck_group_data_number;
extern u32 deck_group_data[];

#ifdef __cplusplus
}
#endif

#endif
