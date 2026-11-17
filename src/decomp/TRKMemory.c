#include <types.h>

// Memory primitives for the debug nub.

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

// Fill a memory span with a byte value.  Spans of 32 bytes or more are filled
// with replicated words: the head runs up to the next word boundary, the body
// in 32-byte blocks, the remainder in whole words and trailing bytes.
void TRK_fill_mem(void *dst, int c, unsigned long n)
{
    u8 *d;
    u32 *d4;
    unsigned long quad;
    unsigned long align;
    unsigned long blocks;
    unsigned long words;

    d = (u8 *)dst - 1;
    quad = (u8)c;

    if (n >= 32) {
        align = (~(unsigned long)d) & 3;
        if (align != 0) {
            n -= align;
            do {
                *++d = quad;
            } while (--align != 0);
        }
        if (quad != 0) {
            quad = (quad << 24) | (quad << 16) | (quad << 8) | quad;
        }
        d4 = (u32 *)(d + 1) - 1;
        if ((blocks = n >> 5) != 0) {
            do {
                *++d4 = quad;
                *++d4 = quad;
                *++d4 = quad;
                *++d4 = quad;
                *++d4 = quad;
                *++d4 = quad;
                *++d4 = quad;
                *++d4 = quad;
            } while (--blocks != 0);
        }
        if ((words = (n >> 2) & 7) != 0) {
            do {
                *++d4 = quad;
            } while (--words != 0);
        }
        d = (u8 *)d4 + 3;
        n &= 3;
    }
    if (n != 0) {
        do {
            *++d = quad;
        } while (--n != 0);
    }
}
