#include "nub.h"

asm void TRK_flush_cache(void *address, u32 size)
{
    nofralloc
    lis r5, 0xffff
    ori r5, r5, 0xfff1
    and r5, r5, r3
    subf r3, r5, r3
    add r4, r4, r3
flush:
    dcbst 0, r5
    dcbf 0, r5
    sync
    icbi 0, r5
    addic r5, r5, 8
    subic. r4, r4, 8
    bge flush
    isync
    blr
}
