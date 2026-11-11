#include "e_exp.h"

static const double halF[2] = {
    5.00000000000000000000e-01,   /* 3fe00000 00000000 */
    -5.00000000000000000000e-01,  /* bfe00000 00000000 */
};
static const double ln2HI[2] = {
    6.93147180369123816490e-01,   /* 3fe62e42 fee00000 */
    -6.93147180369123816490e-01,  /* bfe62e42 fee00000 */
};
static const double ln2LO[2] = {
    1.90821492927058770002e-10,   /* 3dea39ef 35793c76 */
    -1.90821492927058770002e-10,  /* bdea39ef 35793c76 */
};

static const double
    one = 1.0,                                 /* 3ff00000 00000000 */
    huge = 1.0e+300,                           /* 7e37e43c 8800759c */
    twom1000 = 9.33263618503218878990e-302,    /* 01700000 00000000 */
    o_threshold = 7.09782712893383973096e+02,  /* 40862e42 fefa39ef */
    u_threshold = -7.45133219101941108420e+02, /* c0874910 d52d3051 */
    invln2 = 1.44269504088896338700e+00,       /* 3ff71547 652b82fe */
    P1 = 1.66666666666666019037e-01,           /* 3fc55555 5555553e */
    P2 = -2.77777777770155933842e-03,          /* bf66c16c 16bebd93 */
    P3 = 6.61375632143793436117e-05,           /* 3f11566a af25de2c */
    P4 = -1.65339022054652515390e-06,          /* bebbbd41 c5d26bf1 */
    P5 = 4.13813679705723846039e-08;           /* 3e663769 72bea4d0 */

double __ieee754_exp(double x)
{
    double y, hi, lo, c, t;
    int k, xsb;
    unsigned hx;

    hx = *(unsigned *)&x;  /* high word of x */
    xsb = (hx >> 31) & 1;  /* sign bit of x */
    hx &= 0x7fffffff;      /* high word of |x| */

    /* filter out non-finite argument */
    if (hx >= 0x40862e42) { /* |x| >= 709.78... */
        if (hx >= 0x7ff00000) {
            if (((hx & 0x000fffff) | *((unsigned *)&x + 1)) != 0)
                return x + x; /* NaN */
            else
                return (xsb == 0) ? x : 0.0; /* exp(+-inf) = {inf, 0} */
        }
        if (x > o_threshold)
            return huge * huge; /* overflow */
        if (x < u_threshold)
            return twom1000 * twom1000; /* underflow */
    }

    /* argument reduction */
    if (hx > 0x3fd62e42) {      /* if |x| > 0.5 ln2 */
        if (hx < 0x3ff0a2b2) {  /* and |x| < 1.5 ln2 */
            hi = x - ln2HI[xsb];
            lo = ln2LO[xsb];
            k = 1 - xsb - xsb;
        } else {
            k = invln2 * x + halF[xsb];
            t = k;
            hi = x - t * ln2HI[0]; /* t * ln2HI is exact here */
            lo = t * ln2LO[0];
        }
        x = hi - lo;
    } else if (hx < 0x3e300000) { /* when |x| < 2**-28 */
        if (huge + x > one)
            return one + x; /* trigger inexact */
    } else
        k = 0;

    /* x is now in primary range */
    t = x * x;
    c = x - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
    if (k == 0)
        return one - ((x * c) / (c - 2.0) - x);
    y = one - ((lo - (x * c) / (2.0 - c)) - hi);
    if (k >= -1021) {
        *(int *)&y += k << 20; /* add k to y's exponent */
        return y;
    } else {
        *(int *)&y += (k + 1000) << 20; /* add k to y's exponent */
        return y * twom1000;
    }
}
