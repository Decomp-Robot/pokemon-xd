#ifndef DECK_H
#define DECK_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 deckGetTrainerCount(void);
u32 getDeckResourceID(u8 deckId);
char *getDeckMsgID(u16 trainerId, u8 deckId);

extern u32 deck_group_data_number;
extern u32 deck_group_data[];

#ifdef __cplusplus
}
#endif

#endif
