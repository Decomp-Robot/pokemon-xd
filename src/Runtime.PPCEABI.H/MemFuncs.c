#include <stddef.h>

/*
 * MSL memory routines.
 *
 * These are linked into .init ahead of the CRT copy tables so __start's
 * __init_data can clear and copy the DOL data/bss sections.  memset is the
 * public entry point; __fill_mem does the actual fill so the SDK can call it
 * directly.  memcpy is overlap-aware, which the DOL copy tables do not need
 * but the rest of the library assumes.
 */
__declspec(section ".init") void __fill_mem(void *dest, int val, size_t count);

__declspec(section ".init") void *memset(void *dest, int ch, size_t count)
{
    void *const dst = dest;

    __fill_mem(dest, ch, count);
    return dst;
}

__declspec(section ".init") void __fill_mem(void *dest, int val, size_t count)
{
    unsigned char *dst = (unsigned char *)dest - 1;
    unsigned long quad = (unsigned char)val;
    unsigned long n = count;

    if (n >= 32) {
        unsigned long words = (~(unsigned long)dst) & 3;

        if (words != 0) {
            n -= words;
            do {
                *++dst = quad;
            } while (--words != 0);
        }

        if (quad != 0) {
            quad = (quad << 24) | (quad << 16) | (quad << 8) | quad;
        }

        {
            unsigned long *dst4 = (unsigned long *)(dst + 1) - 1;

            words = n >> 5;

            if (words != 0) {
                do {
                    *++dst4 = quad;
                    *++dst4 = quad;
                    *++dst4 = quad;
                    *++dst4 = quad;
                    *++dst4 = quad;
                    *++dst4 = quad;
                    *++dst4 = quad;
                    *++dst4 = quad;
                } while (--words != 0);
            }

            words = (n >> 2) & 7;

            if (words != 0) {
                do {
                    *++dst4 = quad;
                } while (--words != 0);
            }

            dst = (unsigned char *)dst4 + 3;
            n &= 3;
        }
    }

    if (n != 0) {
        do {
            *++dst = quad;
        } while (--n != 0);
    }
}

__declspec(section ".init") void *memcpy(void *dest, const void *src, size_t count)
{
    const unsigned char *srcp = (const unsigned char *)src;

    if (srcp >= (const unsigned char *)dest) {
        const unsigned char *s = srcp - 1;
        unsigned char *dst = (unsigned char *)dest - 1;

        count++;
        while (--count != 0) {
            *++dst = *++s;
        }
    } else {
        const unsigned char *s = srcp + count;
        unsigned char *dst = (unsigned char *)dest + count;

        count++;
        while (--count != 0) {
            *--dst = *--s;
        }
    }

    return dest;
}