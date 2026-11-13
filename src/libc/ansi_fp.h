#ifndef MSL_ANSI_FP_H
#define MSL_ANSI_FP_H

/*
 * A decimal significand held as one digit per byte.
 * Zero, "not a number" and infinity are distinguished by the value
 * stored in the first digit: 0, 'N' and 'I' respectively.
 */
typedef struct Dec {
    char sign;
    short exp;
    unsigned char len;
    unsigned char digits[37];
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

#endif /* MSL_ANSI_FP_H */
