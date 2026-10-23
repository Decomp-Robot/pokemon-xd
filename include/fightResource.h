#ifndef FIGHT_RESOURCE_H
#define FIGHT_RESOURCE_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Localized battle resources; the reader returns one on success, zero on failure.
u8 fightReadResource(s32 id, u32 param);
void fightFreeResource(s32 id, u32 param);

#ifdef __cplusplus
}
#endif

#endif
