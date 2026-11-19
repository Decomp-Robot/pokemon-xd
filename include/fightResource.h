#ifndef FIGHTRESOURCE_H
#define FIGHTRESOURCE_H

#include <types.h>
#include <bool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Reads a resource and tests GSresGetResource again after the read; the FSYS
 * read status itself is ignored. A resource already resident at the initial
 * lookup returns false, as do unsupported or unavailable IDs.
 * A zero groupID selects the deck archive here only; fightFreeResource takes
 * its group as given. */
bool fightReadResource(u32 groupID, u32 resourceID);
void fightFreeResource(u32 groupID, u32 resourceID);
void fightFreeResourceDeclArchive(void);

#ifdef __cplusplus
}
#endif

#endif
