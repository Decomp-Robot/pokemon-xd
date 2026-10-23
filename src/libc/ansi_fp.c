/* Decimal <-> binary floating point conversions. */

extern double copysign(double x, double y);
extern double pow(double x, double y);
extern double ldexp(double x, int e);
extern double frexp(double x, int *e);
extern double modf(double x, double *iptr);

extern char __ctype_map[256];
extern signed char __lower_map[];

/*
 * A decimal significand held as one digit per byte.
 * Zero, "not a number" and infinity are distinguished by the value
 * stored in the first digit: 0, 'N' and 'I' respectively.
 */
typedef struct Dec {
    unsigned char sign;        /* +0x00 sign of the value */
    short exp;                 /* +0x02 power of ten */
    unsigned char len;         /* +0x04 significant digits */
    unsigned char digits[37];  /* +0x05 */
} Dec;

typedef struct DecFormat {
    char style;
    short digits;
} DecFormat;

double __dec2num(const Dec *d);
void __num2dec(const DecFormat *format, double x, Dec *d);
void __minus_dec(Dec *result, const Dec *a, Dec *b);
int __equals_dec(Dec *a, Dec *b);
void __two_exp(Dec *d, long e);

extern void __num2dec_internal(Dec *d, double x);
extern void __timesdec(Dec *result, const Dec *a, const Dec *b);

/* Classification codes, in the order the runtime numbers them. */
#define CLASS_UNKNOWN 0
#define CLASS_NAN     1
#define CLASS_INF     2
#define CLASS_ZERO    3
#define CLASS_NORMAL  4
#define CLASS_SUBNORM 5

extern float floatInfinity[];
/* Largest finite double, the clamp taken when a decimal rounds to infinity. */
extern double doubleMaxFinite[];

#define FP_CLASSIFY(value, result) {                                       \
    union {                                                                \
        double d;                                                          \
        long w[2];                                                         \
    } fp;                                                                  \
    long *hi;                                                              \
    fp.d = (value);                                                        \
    hi = &fp.w[1];                                                         \
    switch (fp.w[0] & 0x7ff00000) {                                        \
    case 0x7ff00000:                                                       \
        if ((fp.w[0] & 0xfffff) || (*hi)) result = CLASS_NAN;               \
        else result = CLASS_INF;                                           \
        break;                                                             \
    case 0:                                                                \
        if ((fp.w[0] & 0xfffff) || (*hi)) result = CLASS_SUBNORM;           \
        else result = CLASS_ZERO;                                          \
        break;                                                             \
    default:                                                               \
        result = CLASS_NORMAL;                                             \
        break;                                                             \
    }                                                                      \
}
/*
 * Number of trailing zero bits of a word.  The bits below the lowest set
 * one are removed a group at a time: mask covers the low step bits while
 * shift halves the group size on every pass, so six passes at most are
 * needed.
 */
inline int __count_trailing_zeros(unsigned long value) {
    int count = 0;
    int left = 32;
    int shift = 16;
    int step = shift;
    unsigned long mask = (1 << 16) - 1;
    while (left != 0) {
        if ((value & mask) == 0) {
            count += step;
            value >>= step;
            left -= step;
        } else if (mask == 1) {
            break;
        }
        if (shift > 1) {
            shift /= 2;
        }
        if (mask > 1) {
            mask >>= shift;
            step -= shift;
        }
    }
    return count;
}
/* Bit of the character classification table that marks a decimal digit. */
#define CTYPE_DIGIT 0x10

/* Lower-case a character through the lower-case table. */
inline int __tolower(int c) {
    if (c == -1) {
        return -1;
    }
    return (unsigned char)__lower_map[c];
}

/*
 * True when decimal a is the smaller of the two, comparing magnitudes.
 * A zero significand is smaller than any other, equal exponents compare
 * digit by digit, and the spare digits of the longer one decide a tie.
 */
inline int __dec_lt(Dec *a, Dec *b) {
    int i;
    int n;
    int nlim;

    if (a->digits[0] == 0) {
        if (b->digits[0] != 0) {
            return 1;
        }
        return 0;
    }
    if (b->digits[0] == 0) {
        return 0;
    }
    if (a->exp == b->exp) {
        n = a->len;
        if (n > b->len) {
            n = b->len;
        }
        for (i = 0; i < n; i++) {
            if (a->digits[i] < b->digits[i]) {
                return 1;
            }
            if (b->digits[i] < a->digits[i]) {
                return 0;
            }
        }
        if (n == a->len) {
            for (; i < b->len; i++) {
                if (b->digits[i] != 0) {
                    return 1;
                }
            }
        }
        return 0;
    }
    return a->exp < b->exp;
}

/* Maximum precision used by the decimal arithmetic helpers. */
#define DEC_DIGITS 36

static inline void __dorounddecup(Dec* d, int digits) {
    unsigned char* b = d->digits;
    unsigned char* i = b + digits - 1;

    while (1) {
        if (*i < 9) {
            *i += 1;
            break;
        }
        if (i == b) {
            *i = 1;
            d->exp++;
            break;
        }
        *i-- = 0;
    }
}

inline void __str2dec(Dec* d, const char* s, short exp) {
    int i;

    d->exp = exp;
    d->sign = 0;

    for (i = 0; i < DEC_DIGITS && *s;) {
        d->digits[i++] = *s++ - '0';
    }
    d->len = i;

    if (*s != 0) {
        if (*s < 5)
            return;
        if (*s > 5)
            goto round;

        {
            const char* p = s + 1;

            for (; *p != 0; p++) {
                if (*p != '0')
                    goto round;
            }

            if ((d->digits[i - 1] & 1) == 0)
                return;
        }
    round:
        __dorounddecup(d, d->len);
    }
}

/**
 * Convert a decimal value back to a double.
 *
 * The first digit selects the special cases: 'I' for infinity, 'N' for
 * not-a-number, and '0' marks a value whose significand is all zeros.
 */
double __dec2num(const Dec *d) {
    /* Powers of ten for folding groups of decimal digits. */
    static double pow_10[8] = {
        1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5, 1.0e6, 1.0e7, 1.0e8
    };
    int k;
    int c;
    int digit;
    unsigned char *q;
    int nonzero;
    int odd;
    Dec t;             /* working copy of the decimal */
    Dec limit;         /* the largest finite double, as a decimal */
    Dec here;          /* the value built so far, as a decimal */
    Dec above;         /* the value one unit above the candidate */
    Dec shortfall;     /* how far the candidate falls short */
    Dec excess;        /* how far the value above overshoots */
    Dec below;         /* the value one unit below the candidate */
    Dec under;         /* how far the value below falls short */
    Dec over;          /* how far the candidate overshoots */
    union {
        double d;
        unsigned long long u;
        unsigned long w[2];
    } bits;            /* scratch for bit patterns and conversions */
    union {
        double d;
        unsigned long long u;
    } value;           /* the result */
    double next;       /* the candidate one unit above the result */
    double prev;       /* the candidate one unit below the result */
    double sum;        /* the value the current group folds to */
    double prod;       /* the group scaled, before the digits fold in */
    unsigned char *p;
    unsigned char *end;
    unsigned long acc;
    int cls;
    int exp;
    int n;
    int nlim;
    int i;

    if (d->len == 0) {
        return copysign(0.0,
                        (signed char)d->sign == 0 ? 1.0 : -1.0);
    }
    switch (d->digits[0]) {
    case '0':
        return copysign(0.0,
                        (signed char)d->sign == 0 ? 1.0 : -1.0);
    case 'I':
        return copysign(floatInfinity[0],
                        (signed char)d->sign == 0 ? 1.0 : -1.0);
    case 'N': {
        bits.u = 0x7FF0000000000000ULL;
        if ((signed char)d->sign != 0) {
            bits.u |= 0x8000000000000000ULL;
        }
        if (d->len == 1) {
            bits.u |= 0x0008000000000000ULL;
        } else {
            q = (unsigned char *)&bits + 1;
            nonzero = 0;
            odd = 1;
            nlim = d->len;
            if (nlim > 14) {
                nlim = 14;
            }
            for (i = 1; i < nlim; i++) {
                c = d->digits[i];
                if (__ctype_map[c] & CTYPE_DIGIT) {
                    digit = c - '0';
                } else {
                    /* the payload is a byte's worth of nibble either way */
                    digit = (__tolower(c) - 'a' + 10) & 0xFF;
                }
                /* the digit is a byte's worth of payload either way */
                if ((unsigned char) digit != 0) {
                    nonzero = 1;
                }
                if (odd) {
                    *q++ |= digit;
                } else {
                    *q = digit << 4;
                }
                odd = !odd;
            }
            if (!nonzero) {
                bits.u |= 0x0008000000000000ULL;
            }
        }
        return bits.d;
    }
    }

    t = *d;
    p = t.digits;
    end = p + t.len;
    for (; p < end; p++) {
        *p -= '0';
    }
    t.exp += t.len - 1;
    exp = t.exp;
    value.d = (double) t.digits[0];
    p = t.digits;
    p++;
    while (p < end) {
        acc = 0;
        n = (end - p) % 8;
        if (n == 0) {
            n = 8;
        }
        i = n;
        while (i-- > 0) {
            acc = acc * 10 + *p++;
        }
        prod = value.d * pow_10[n - 1];
        sum = prod + (double) acc;
        if (acc != 0 && prod == sum) {
            break;
        }
        value.d = sum;
        exp -= n;
    }
    if (exp < 0) {
        value.d = value.d / pow(5.0, (double)(-exp));
    } else {
        value.d = value.d * pow(5.0, (double) exp);
    }
    value.d = ldexp(value.d, exp);

    FP_CLASSIFY(*(const double *) &value, cls);
    if (cls == CLASS_INF) {
        __str2dec(&limit, "179769313486231580793729011405303420", 308);
        if (__dec_lt(&limit, &t)) {
            goto done;
        }
        value.d = doubleMaxFinite[0];
    }
    __num2dec_internal(&here, value.d);
    if (__equals_dec(&here, &t)) {
        goto done;
    }
    if (__dec_lt(&here, &t)) {
        /*
         * The value is below the decimal, so walk up one unit at a time
         * until the value's own conversion no longer falls short.
         */
        next = value.d;
        *(unsigned long long *) &next += 1;
        FP_CLASSIFY(next, cls);
        if (cls == CLASS_INF) {
            value.d = next;
            goto done;
        }
        __num2dec_internal(&above, next);
        while (__dec_lt(&above, &t)) {
            here = above;
            value.d = next;
            *(unsigned long long *) &next += 1;
            FP_CLASSIFY(next, cls);
            if (cls == CLASS_INF) {
                value.d = next;
                goto done;
            }
            __num2dec_internal(&above, next);
        }
        __minus_dec(&shortfall, &t, &here);
        __minus_dec(&excess, &above, &t);
        if (__equals_dec(&shortfall, &excess)) {
            if (value.u & 1) {
                value.d = next;
            }
        } else if (!__dec_lt(&shortfall, &excess)) {
            value.d = next;
        }
    } else {
        /*
         * The value is above the decimal, so walk down one unit at a time
         * until the value's own conversion no longer exceeds it.
         */
        prev = value.d;
        *(unsigned long long *) &prev -= 1;
        __num2dec_internal(&below, prev);
        while (__dec_lt(&t, &below)) {
            here = below;
            value.d = prev;
            *(unsigned long long *) &prev -= 1;
            __num2dec_internal(&below, prev);
        }
        __minus_dec(&under, &t, &below);
        __minus_dec(&over, &here, &t);
        if (__equals_dec(&under, &over)) {
            if (value.u & 1) {
                value.d = prev;
            }
        } else if (__dec_lt(&under, &over)) {
            value.d = prev;
        }
    }
done:
    if ((signed char)t.sign != 0) {
        value.d = -value.d;
    }
    return value.d;
}

static inline int decimalRoundDirection(const Dec *d, const unsigned char *p) {
    const unsigned char *q;

    if (*p > 5) {
        return 1;
    }
    if (*p < 5) {
        return -1;
    }
    for (q = p + 1; q < d->digits + d->len; q++) {
        if (*q != 0) {
            return 1;
        }
    }
    if (p[-1] & 1) {
        return 1;
    }
    return -1;
}

void __num2dec(const DecFormat *format, double x, Dec *d) {
    short want = (short) format->digits;
    int i;
    unsigned char *q;
    int pos;
    int round;
    unsigned char *p;

    __num2dec_internal(d, x);
    if (d->digits[0] > 9) {
        return;
    }
    if (want > 36) {
        want = 36;
    }
    pos = want;
    if ((pos > 0) && (pos < (int) d->len)) {
        unsigned char *digits = d->digits;
        p = digits + pos;
        round = decimalRoundDirection(d, p);
        d->len = (unsigned char) pos;
        if (round >= 0) {
            __dorounddecup(d, pos);
        }
    }
    while (d->len < pos) {
        d->digits[d->len++] = 0;
    }

    d->exp = (short) (d->exp - (d->len - 1));
    for (i = 0; i < d->len; i++) {
        d->digits[i] += '0';
    }
}

void __num2dec_internal(Dec *d, double x) {
    unsigned long *bits_of_x = (unsigned long *)&x;
    int exponent;
    double ipart;
    Dec number;
    char sign;
    int class;
    unsigned long long integer;
    int zeros;
    int bits;
    unsigned long word;
    unsigned char digit;
    unsigned char *q;
    unsigned char *p;

    sign = (char)(bits_of_x[0] >> 31);
    if (0.0 == x) {
        d->sign = sign;
        d->exp = 0;
        d->len = 1;
        d->digits[0] = 0;
        return;
    }
    FP_CLASSIFY(x, class);
    if (class <= CLASS_INF) {
        d->sign = sign;
        d->exp = 0;
        d->len = 1;
        FP_CLASSIFY(x, class);
        d->digits[0] = (unsigned char)(class == CLASS_NAN ? 'N' : 'I');
        return;
    }
    {
        double m;
        union {
            double d;
            unsigned long w[2];
        } mantissa;
        Dec power;

        if (sign) {
            x = -x;
        }
        m = frexp(x, &exponent);
        mantissa.d = m;
        /*
         * The mantissa is scaled up until its lowest set bit is the value
         * itself, so the integer part is exact and at most 53 bits wide.
         */
        zeros = mantissa.w[1] != 0
                    ? __count_trailing_zeros(mantissa.w[1])
                    : __count_trailing_zeros(mantissa.w[0] | 0x100000) + 32;
        bits = 53 - zeros;
        __two_exp(&power, exponent - bits);
        modf(ldexp(m, bits), &ipart);
        integer = (unsigned long long)ipart;
        number.sign = 0;
        if (integer == 0) {
            number.exp = 0;
            number.len = 1;
            number.digits[0] = 0;
        } else {
            number.len = 0;
            while (integer != 0) {
                number.digits[number.len++] = (unsigned char)(integer % 10);
                integer /= 10;
            }
            /* The digits came out least significant first. */
            p = number.digits;
            q = number.digits + number.len;
            while (p < --q) {
                digit = *p;
                *p++ = *q;
                *q = digit;
            }
            number.exp = (short)(number.len - 1);
        }
        __timesdec(d, &number, &power);
        d->sign = sign;
    }
}

void __minus_dec(Dec *result, const Dec *a, Dec *b) {
    int width;
    int skew;
    int i;
    int off;
    int j;
    unsigned char *digits;
    unsigned char *p;
    unsigned char *bdigits;
    unsigned char *q;
    unsigned char *r;
    int up;
    unsigned char *end;
    unsigned char *bend;

    *result = *a;
    if (b->digits[0] == 0) {
        return;
    }
    width = result->len;
    if (width < b->len) {
        width = b->len;
    }
    skew = (int)result->exp - (int)b->exp;
    width += skew;
    if (width > 36) {
        width = 36;
    }
    while (result->len < width) {
        result->digits[result->len++] = 0;
    }
    digits = result->digits;
    p = digits + width;
    if (b->len + skew < width) {
        p = digits + b->len + skew;
    }
    bdigits = b->digits;
    q = bdigits + (p - digits - skew);
    r = q;
    while (p > digits && q > bdigits) {
        unsigned char *t;

        --p;
        --q;
        if (*p < *q) {
            t = p - 1;
            while (*t == 0) {
                t--;
            }
            while (t != p) {
                *t -= 1;
                t[1] += 10;
                t++;
            }
        }
        *p = (unsigned char)(*p - *q);
    }
    off = (int)(r - bdigits);
    if (off < b->len) {
        up = 0;
        if (*r < 5) {
            up = 1;
        } else if (*r == 5) {
            bend = &b->digits[b->len];
            for (q = r + 1; q < bend; q++) {
                if (*q != 0) {
                    goto dropped;
                }
            }
            p = digits + off + skew - 1;
            if (*p & 1) {
                up = 1;
            }
        }
        if (up) {
            unsigned char *t;

            if (*p < 1) {
                t = p - 1;
                while (*t == 0) {
                    t--;
                }
                while (t != p) {
                    *t -= 1;
                    t[1] += 10;
                    t++;
                }
            }
            *p = (unsigned char)(*p - 1);
        }
    }
dropped:
    p = digits;
    while (*p == 0) {
        p++;
    }
    if (p > digits) {
        unsigned char skipped = p - digits;
        result->exp -= skipped;
        end = digits + result->len;
        while (p < end) {
            *digits++ = *p++;
        }
        result->len -= skipped;
    }
    digits = result->digits;
    p = digits + result->len;
    while (p > digits) {
        if (*--p != 0) {
            break;
        }
    }
    result->len = (unsigned char)(p - digits + 1);
}

/**
 * Compare two decimal values for equality.
 *
 * Two zero values compare equal whatever their exponent says, and the
 * shorter significand is padded with implicit leading zeros.
 *
 * @return Non-zero when the values are equal.
 */
int __equals_dec(Dec *a, Dec *b) {
    int blen;
    int alen;
    int i;
    int common;

    /* A zero first digit marks a zero value. */
    if (a->digits[0] == 0) {
        return b->digits[0] == 0;
    }
    if (b->digits[0] == 0) {
        return a->digits[0] == 0;
    }
    if (a->exp == b->exp) {
        alen = a->len;
        blen = b->len;
        common = alen;
        if (alen > blen) {
            common = blen;
        }
        /* The digits both values share have to agree. */
        for (i = 0; i < common; i++) {
            if (a->digits[i] != b->digits[i]) {
                return 0;
            }
        }
        /* Whatever one value has beyond that has to be zero. */
        if (common == alen) {
            for (; i < blen; i++) {
                if (b->digits[i] != 0) {
                    return 0;
                }
            }
        } else {
            for (; i < alen; i++) {
                if (a->digits[i] != 0) {
                    return 0;
                }
            }
        }
        return 1;
    }
    return 0;
}

void __two_exp(Dec* result, long exp) {
    switch (exp) {
    case -64:
        __str2dec(result, "542101086242752217003726400434970855712890625", -20);
        return;
    case -53:
        __str2dec(result, "11102230246251565404236316680908203125", -16);
        return;
    case -32:
        __str2dec(result, "23283064365386962890625", -10);
        return;
    case -16:
        __str2dec(result, "152587890625", -5);
        return;
    case -8:
        __str2dec(result, "390625", -3);
        return;
    case -7:
        __str2dec(result, "78125", -3);
        return;
    case -6:
        __str2dec(result, "15625", -2);
        return;
    case -5:
        __str2dec(result, "3125", -2);
        return;
    case -4:
        __str2dec(result, "625", -2);
        return;
    case -3:
        __str2dec(result, "125", -1);
        return;
    case -2:
        __str2dec(result, "25", -1);
        return;
    case -1:
        __str2dec(result, "5", -1);
        return;
    case 0:
        __str2dec(result, "1", 0);
        return;
    case 1:
        __str2dec(result, "2", 0);
        return;
    case 2:
        __str2dec(result, "4", 0);
        return;
    case 3:
        __str2dec(result, "8", 0);
        return;
    case 4:
        __str2dec(result, "16", 1);
        return;
    case 5:
        __str2dec(result, "32", 1);
        return;
    case 6:
        __str2dec(result, "64", 1);
        return;
    case 7:
        __str2dec(result, "128", 2);
        return;
    case 8:
        __str2dec(result, "256", 2);
        return;
    }

    {
        Dec x2, temp;

        __two_exp(&x2, exp / 2);
        __timesdec(result, &x2, &x2);

        if (exp & 1) {
            temp = *result;
            if (exp > 0) {
                __str2dec(&x2, "2", 0);
            } else {
                __str2dec(&x2, "5", -1);
            }
            __timesdec(result, &temp, &x2);
        }
    }
}

void __timesdec(Dec *result, const Dec *a, const Dec *b) {
    int blen = b->len;
    unsigned char buf[80];
    int alen = a->len;
    unsigned int acc = 0;
    int count = alen + blen - 1;
    int last;
    int first;
    int n;
    int nlim;
    int avail;
    int i;
    const unsigned char *pa;
    const unsigned char *pb;
    unsigned char *end;
    unsigned char *p;

    p = buf + count + 1;
    result->sign = 0;
    end = p;
    while (count > 0) {
        last = blen - 1;
        first = count - last - 1;
        if (first < 0) {
            first = 0;
            last = count - 1;
        }
        pa = &a->digits[first];
        pb = &b->digits[last];
        n = last + 1;
        avail = alen - first;
        if (n > avail) {
            n = avail;
        }
        while (n > 0) {
            acc += *pa * *pb;
            pa++;
            pb--;
            n--;
        }
        count--;
        *--p = (unsigned char)(acc % 10);
        acc /= 10;
    }
    result->exp = (short)(a->exp + b->exp);
    if (acc != 0) {
        *--p = (unsigned char)acc;
        result->exp = (short)(result->exp + 1);
    }
    for (i = 0; i < DEC_DIGITS && p < end; i++) {
        result->digits[i] = *p++;
    }
    result->len = (unsigned char)i;
    if (p < end && *p >= 5) {
        unsigned char *q;

        if (*p == 5) {
            for (q = p + 1; q < end; q++) {
                if (*q != 0) {
                    goto round_up;
                }
            }
            if ((p[-1] & 1) == 0) {
                return;
            }
        }
    round_up:
        __dorounddecup(result, result->len);
    }
}

