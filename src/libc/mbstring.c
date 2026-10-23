/*
 * Lead byte of a UTF-8 sequence of one to three bytes, indexed by the
 * length of the sequence.  The library holds the four bytes in small
 * data; wcstombs copies the whole word into a local.
 */
typedef union {
    unsigned long all;
    unsigned char byte[4];
} Utf8LeadBytes;
extern Utf8LeadBytes utf8LeadBytes;

extern char *strncpy(char *dst, const char *src, unsigned long n);

static inline int encodeUtf8Character(unsigned char *buf, unsigned short c) {
    unsigned char *p;
    int len;
    Utf8LeadBytes lead;

    lead.all = utf8LeadBytes.all;
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
        *--p = (unsigned char)(c | lead.byte[len]);
        break;
    }
    return len;
}

unsigned long wcstombs(char *dst, const unsigned short *src, unsigned long n) {
    int len;
    int done = 0;
    const unsigned short *cur;
    unsigned char buf[4];

    if (dst == ((void *)0) || src == ((void *)0)) {
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
        strncpy(dst + done, (const char *)buf, (unsigned long)len);
        done += (unsigned long)len;
    }
    return done;
}

