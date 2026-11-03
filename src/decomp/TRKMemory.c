#include <types.h>

// Memory primitives for the debug nub.

// The nub carries its own byte-fill engine, separate from the C runtime's.
extern void TRK_fill_mem(void *dst, int c, unsigned long n);

__declspec(section ".init") void *TRK_memset(void *dst, int c, unsigned long n)
{
    TRK_fill_mem(dst, c, n);
    return dst;
}

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
