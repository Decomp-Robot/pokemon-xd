#include <types.h>

// TRK_memcpy -- byte copy in .init next to TRK_memset (0x80003458). It is a
// distinct symbol from the overlap-aware libc memcpy elsewhere in .init
// (0x800054F4) and from the nub's local .text memcpy (0x800D64F8); observed
// callers are TRK nub .text routines copying event/buffer data. Whether the
// original lived in its own TU or grouped with TRK_memset is unknown; this
// module will host TRK_memset as it is recovered.
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
