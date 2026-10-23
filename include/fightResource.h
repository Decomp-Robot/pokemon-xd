#ifndef FIGHTRESOURCE_H
#define FIGHTRESOURCE_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Returns 1 if the member was loaded, 0 if it was already resident. */
u8 fightReadResource(u32 groupId, u32 resourceId);
void fightFreeResource(u32 groupId, u32 resourceId);
void fightFreeResourceDeclArchive(void);

#ifdef __cplusplus
}
#endif

#endif
