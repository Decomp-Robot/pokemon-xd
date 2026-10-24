#include <stdlib.h>
#include <string.h>

static inline int encodeUtf8Character(unsigned char *buf, unsigned short c) {
    unsigned char *p;
    int len;
    unsigned char lead[4] = {0, 0, 0xC0, 0xE0};

    if ((unsigned int)c < 0x80) {
        len = 1;
    } else if ((unsigned int)c < 0x800) {
        len = 2;
    } else {
        len = 3;
    }
    p = buf + len;
    switch (len) {
    case 3:
        *--p = (unsigned char)(0x80 | (c & 0x3F));
        c >>= 6;
    case 2:
        *--p = (unsigned char)(0x80 | (c & 0x3F));
        c >>= 6;
    case 1:
        *--p = (unsigned char)(c | lead[len]);
        break;
    }
    return len;
}

size_t wcstombs(char *dst, const wchar_t *src, size_t n) {
    int len;
    int done = 0;
    const wchar_t *cur;
    unsigned char buf[4];

    if (dst == NULL || src == NULL) {
        return 0;
    }
    cur = src;
    while (done <= n) {
        if (*cur == 0) {
            dst[done] = 0;
            break;
        }
        len = encodeUtf8Character(buf, *cur++);
        if (done + len > n) {
            break;
        }
        strncpy(dst + done, (const char *)buf, (size_t)len);
        done += (size_t)len;
    }
    return done;
}

