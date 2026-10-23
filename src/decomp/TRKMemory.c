#include <types.h>

// Straight byte copy used by the TRK nub for target memory transfers; a separate
// entry point from the overlap-aware libc memcpy in Runtime.PPCEABI.H/MemFuncs.c.
__declspec(section ".init") void *TRK_memcpy(void *dst, const void *src, unsigned long n)
{
    const u8 *s = (const u8 *)src - 1;
    u8 *d = (u8 *)dst - 1;

    n++;
    while (--n != 0) {
        *++d = *++s;
    }
    return dst;
}
