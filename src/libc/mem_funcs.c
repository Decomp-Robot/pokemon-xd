#include "mem_funcs.h"

void __copy_longs_rev_unaligned(void *destination, const void *source, unsigned long length) {
    unsigned long count;
    unsigned long offset;
    unsigned long left_shift;
    unsigned long right_shift;
    const unsigned char *src;
    unsigned char *dst;
    unsigned long first;
    unsigned long second;

    src = (const unsigned char *)source + length;
    dst = (unsigned char *)destination + length;

    count = (unsigned long)dst & 3;
    if (count) {
        length -= count;
        do {
            *--dst = *--src;
        } while (--count);
    }
    offset = (unsigned long)src & 3;
    left_shift = offset * 8;
    right_shift = 32 - left_shift;
    src += 4 - offset;
    count = length >> 3;
    src -= 4;
    first = *(const unsigned long *)src;
    do {
        src -= 4;
        second = *(const unsigned long *)src;
        dst -= 4;
        *(unsigned long *)dst = (second << left_shift) | (first >> right_shift);
        src -= 4;
        first = *(const unsigned long *)src;
        dst -= 4;
        *(unsigned long *)dst = (first << left_shift) | (second >> right_shift);
    } while (--count);
    if (length & 4) {
        src -= 4;
        second = *(const unsigned long *)src;
        dst -= 4;
        *(unsigned long *)dst = (second << left_shift) | (first >> right_shift);
    }
    length &= 3;
    if (length) {
        src += offset;
        do {
            *--dst = *--src;
        } while (--length);
    }
}

void __copy_longs_unaligned(void *destination, const void *source, unsigned long length) {
    register unsigned long *dst_word;
    register unsigned long count;
    register const unsigned char *src;
    register unsigned char *dst;
    register unsigned long alignment;
    register unsigned long offset;
    register unsigned long left_shift;
    register unsigned long right_shift;
    register unsigned long first;
    register unsigned long second;
    register const unsigned char *tail_src;
    src = (const unsigned char *)source - 1;
    dst = (unsigned char *)destination - 1;

    alignment = -(unsigned long)destination & 3;
    if (alignment) {
        length -= alignment;
        do {
            *++dst = *++src;
        } while (--alignment);
    }
    dst_word = (unsigned long *)(dst - 3);
    offset = (unsigned long)(src + 1) & 3;
    left_shift = offset * 8;
    right_shift = 32 - left_shift;
    src -= offset;
    src += 1;
    first = *(const unsigned long *)src;
    count = length >> 3;
    do {
        src += 4;
        second = *(const unsigned long *)src;
        *++dst_word = (first << left_shift) | (second >> right_shift);
        src += 4;
        first = *(const unsigned long *)src;
        *++dst_word = (second << left_shift) | (first >> right_shift);
    } while (--count);
    if (length & 4) {
        src += 4;
        second = *(const unsigned long *)src;
        *++dst_word = (first << left_shift) | (second >> right_shift);
    }
    length &= 3;
    tail_src = src + 3;
    dst = (unsigned char *)dst_word + 3;
    if (length) {
        tail_src -= 4 - offset;
        do {
            *++dst = *++tail_src;
        } while (--length);
    }
}

void __copy_longs_rev_aligned(void *destination, const void *source, unsigned long length) {
    register const unsigned char *src = (const unsigned char *)source + length;
    register unsigned char *dst = (unsigned char *)destination + length;
    register unsigned long count;

    count = (unsigned long)dst & 3;
    if (count) {
        length -= count;
        do {
            *--dst = *--src;
        } while (--count);
    }
    count = length >> 5;
    if (count) {
        do {
            dst -= 4;
            src -= 4;
            *(unsigned long *)dst = *(const unsigned long *)src;
            dst -= 4;
            src -= 4;
            *(unsigned long *)dst = *(const unsigned long *)src;
            dst -= 4;
            src -= 4;
            *(unsigned long *)dst = *(const unsigned long *)src;
            dst -= 4;
            src -= 4;
            *(unsigned long *)dst = *(const unsigned long *)src;
            dst -= 4;
            src -= 4;
            *(unsigned long *)dst = *(const unsigned long *)src;
            dst -= 4;
            src -= 4;
            *(unsigned long *)dst = *(const unsigned long *)src;
            dst -= 4;
            src -= 4;
            *(unsigned long *)dst = *(const unsigned long *)src;
            dst -= 4;
            src -= 4;
            *(unsigned long *)dst = *(const unsigned long *)src;
        } while (--count);
    }
    count = (length >> 2) & 7;
    if (count) {
        do {
            dst -= 4;
            src -= 4;
            *(unsigned long *)dst = *(const unsigned long *)src;
        } while (--count);
    }
    length &= 3;
    if (length) {
        do {
            *--dst = *--src;
        } while (--length);
    }
}

void __copy_longs_aligned(void *destination, const void *source, unsigned long length) {
    register unsigned long *dst_word;
    register unsigned long count;
    register const unsigned long *src_word;
    register const unsigned char *src = (const unsigned char *)source - 1;
    register unsigned char *dst = (unsigned char *)destination - 1;
    register unsigned long alignment;
    register const unsigned char *tail_src;

    alignment = -(unsigned long)destination & 3;
    if (alignment) {
        length -= alignment;
        do {
            *++dst = *++src;
        } while (--alignment);
    }
    src_word = (const unsigned long *)(src - 3);
    dst_word = (unsigned long *)(dst - 3);
    if ((count = length >> 5) != 0) {
        do {
            *++dst_word = *++src_word;
            *++dst_word = *++src_word;
            *++dst_word = *++src_word;
            *++dst_word = *++src_word;
            *++dst_word = *++src_word;
            *++dst_word = *++src_word;
            *++dst_word = *++src_word;
            *++dst_word = *++src_word;
        } while (--count);
    }
    count = (length >> 2) & 7;
    if (count) {
        do {
            *++dst_word = *++src_word;
        } while (--count);
    }
    length &= 3;
    tail_src = (const unsigned char *)src_word + 3;
    dst = (unsigned char *)dst_word + 3;
    if (length) {
        do {
            *++dst = *++tail_src;
        } while (--length);
    }
}
