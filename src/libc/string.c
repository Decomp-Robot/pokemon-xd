#include "string.h"

char *strchr(const char *string, int character) {
    const unsigned char *end = (const unsigned char *)string - 1;
    const unsigned char target = character;
    unsigned int c;

    while ((c = *++end) != '\0') {
        if (c == target) {
            return (char *)end;
        }
    }
    if (target == '\0') {
        return (char *)end;
    }
    return NULL;
}

int strncmp(const char *first, const char *second, size_t length) {
    const unsigned char *left = (const unsigned char *)first - 1;
    const unsigned char *right = (const unsigned char *)second - 1;
    unsigned int l;
    unsigned int r;

    ++length;
    while (--length) {
        l = *++left;
        r = *++right;
        if (l != r) {
            return l - r;
        }
        if (l == '\0') {
            break;
        }
    }
    return 0;
}

int strcmp(const char *first, const char *second) {
    const unsigned char *left = (const unsigned char *)first;
    const unsigned char *right = (const unsigned char *)second;
    unsigned int c1 = *left;
    unsigned int c2 = *right;
    unsigned long alignLeft;
    unsigned long alignRight;
    int diff;
    unsigned long count;
    unsigned long w1;
    unsigned long w2;

    diff = c1 - c2;
    if (diff != 0) {
        return diff;
    }
    alignRight = (unsigned long)right & 3;
    if (alignRight == (alignLeft = (unsigned long)left & 3)) {
        if (alignLeft != 0) {
            if (c1 == '\0') {
                return 0;
            }
            for (count = 3 - alignLeft; count != 0; --count) {
                c1 = *++left;
                c2 = *++right;
                diff = c1 - c2;
                if (diff != 0) {
                    return diff;
                }
                if (c1 == '\0') {
                    return 0;
                }
            }
            ++left;
            ++right;
        }
        w1 = *(unsigned long *)left;
        w2 = *(unsigned long *)right;
        if (((w1 - 0x01010101) & 0x80808080) != 0) {
            goto bytetail;
        }
        while (w1 == w2) {
            w1 = *(unsigned long *)(left += 4);
            w2 = *(unsigned long *)(right += 4);
            if (((w1 - 0x01010101) & 0x80808080) != 0) {
                goto bytetail;
            }
        }
        return w1 > w2 ? 1 : -1;
    }
    goto zerotest;
bytetail:
    c1 = *left;
    c2 = *right;
    diff = c1 - c2;
    if (diff != 0) {
        return diff;
    }
zerotest:
    if (c1 == '\0') {
        return 0;
    }
    do {
        c1 = *++left;
        diff = c1 - *++right;
        if (diff != 0) {
            return diff;
        }
    } while (c1 != '\0');
    return 0;
}

char *strcat(char *destination, const char *source) {
    const unsigned char *src = (const unsigned char *)source - 1;
    unsigned char *dst = (unsigned char *)destination - 1;

    while (*++dst != '\0')
        ;
    dst--;
    while ((*++dst = *++src) != '\0')
        ;
    return destination;
}

char *strncpy(char *destination, const char *source, size_t length) {
    const unsigned char *src = (const unsigned char *)source - 1;
    unsigned char *dst = (unsigned char *)destination - 1;

    ++length;
    while (--length) {
        if ((*++dst = *++src) == '\0') {
            while (--length) {
                *++dst = '\0';
            }
            return destination;
        }
    }
    return destination;
}

char *strcpy(char *destination, const char *source) {
    unsigned char *dst = (unsigned char *)destination;
    unsigned char c;
    unsigned long word;
    unsigned long count;

    if (((unsigned long)destination & 3) == ((unsigned long)source & 3)) {
        if (((unsigned long)source & 3) != 0) {
            c = *source;
            *dst = c;
            if (c == '\0') {
                return destination;
            }
            for (count = 3 - ((unsigned long)source & 3); count != 0; --count) {
                c = *++source;
                *++dst = c;
                if (c == '\0') {
                    return destination;
                }
            }
            ++dst;
            ++source;
        }
        word = *(unsigned long *)source;
        if (((word - 0x01010101) & 0x80808080) == 0) {
            dst -= 4;
            do {
                *(unsigned long *)(dst += 4) = word;
                word = *(unsigned long *)(source += 4);
            } while (((word - 0x01010101) & 0x80808080) == 0);
            dst += 4;
        }
    }
    c = *source;
    *dst = c;
    if (c == '\0') {
        return destination;
    }
    do {
        c = *++source;
        *++dst = c;
    } while (c != '\0');
    return destination;
}

size_t strlen(const char *string) {
    const unsigned char *end = (const unsigned char *)string - 1;
    size_t length = -1;

    do {
        length++;
    } while (*++end != '\0');
    return length;
}
