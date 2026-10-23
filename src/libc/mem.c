#include "mem.h"
#include "mem_funcs.h"

int memcmp(const void *first, const void *second, size_t length) {
    const unsigned char *left = (const unsigned char *)first - 1;
    const unsigned char *right = (const unsigned char *)second - 1;

    ++length;
    while (--length) {
        if (*++left != *++right) {
            return *left < *right ? -1 : 1;
        }
    }
    return 0;
}

void *__memrchr(const void *buffer, int character, unsigned long length) {
    unsigned char byte = character;
    const unsigned char *cursor = (const unsigned char *)buffer + length;

    ++length;
    while (--length) {
        if (*--cursor == byte) {
            return (void *)cursor;
        }
    }
    return NULL;
}

void *memchr(const void *buffer, int character, size_t length) {
    unsigned char byte = character;
    const unsigned char *cursor = (const unsigned char *)buffer - 1;

    ++length;
    while (--length) {
        if (*++cursor == byte) {
            return (void *)cursor;
        }
    }
    return NULL;
}

void *memmove(void *destination, const void *source, size_t length) {
    const unsigned char *src;
    unsigned char *dst;
    int reverse = (unsigned long)source < (unsigned long)destination;

    if (length >= 32) {
        if (((unsigned long)destination ^ (unsigned long)source) & 3) {
            if (!reverse) {
                __copy_longs_unaligned(destination, source, length);
            } else {
                __copy_longs_rev_unaligned(destination, source, length);
            }
        } else {
            if (!reverse) {
                __copy_longs_aligned(destination, source, length);
            } else {
                __copy_longs_rev_aligned(destination, source, length);
            }
        }
        return destination;
    }
    if (!reverse) {
        src = (const unsigned char *)source - 1;
        dst = (unsigned char *)destination - 1;
        ++length;
        while (--length) {
            *++dst = *++src;
        }
    } else {
        src = (const unsigned char *)source + length;
        dst = (unsigned char *)destination + length;
        ++length;
        while (--length) {
            *--dst = *--src;
        }
    }
    return destination;
}
