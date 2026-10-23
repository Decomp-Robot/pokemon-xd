/*
 * targimpl.c -- the target side of the MetroTRK nub.  The host sends the nub
 * requests to read and write the target's memory; every such request is
 * checked against the memory map before the nub touches it.
 */

#include "nub.h"

/* The debugger vouches for a single memory-map entry: gTRKMemMap holds one
   16-byte record, so the walk below examines exactly that entry. */
enum { kTRKMemMapEntryCount = 1 };

/*
 * Check the range [address, address + length - 1] against the memory map,
 * vouching for it the way `access` asks about.  Entries the request does
 * not touch are skipped; a request that runs past either end of the entry
 * it does touch is checked piece by piece, validating the overhang in
 * front of the entry and the overhang past it on their own.
 */
s32 TRKValidMemory32(u32 address, u32 length, TRKAccess access)
{
    u32 last = address + (length - 1);
    s32 result = kTRKInvalidMemory;
    s32 rangeIndex;

    if (last < address)
        return kTRKInvalidMemory;

    for (rangeIndex = 0; rangeIndex < kTRKMemMapEntryCount; ++rangeIndex) {
        if (address > gTRKMemMap[rangeIndex].last ||
            last < gTRKMemMap[rangeIndex].first)
            continue;
        if ((access == TRK_MEMMAP_READ && !gTRKMemMap[rangeIndex].readable) ||
            (access == TRK_MEMMAP_WRITE && !gTRKMemMap[rangeIndex].writeable)) {
            result = kTRKInvalidMemory;
            break;
        }
        result = 0;
        if (address < gTRKMemMap[rangeIndex].first)
            result = TRKValidMemory32(address,
                                      gTRKMemMap[rangeIndex].first - address,
                                      access);
        if (result == 0 && last > gTRKMemMap[rangeIndex].last)
            result = TRKValidMemory32(gTRKMemMap[rangeIndex].last,
                                      last - gTRKMemMap[rangeIndex].last,
                                      access);
        break;
    }
    return result;
}
