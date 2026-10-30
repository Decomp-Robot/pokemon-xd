#include "s_ldexp.h"
#include "s_copysign.h"
#include <libc/fpclassify.h>

static const double zero = 0.0;
static const double two54 = 1.80143985094819840000e+16;  /* 2^54 */
static const double tiny = 1.0e-300;
static const double huge = 1.0e+300;
static const double twom54 = 5.55111512312578270212e-17; /* 2^-54 */

double ldexp(double x, int exp)
{
    int cls;
    int k;
    int hx, lx;
    union {
        double d;
        long w[2];
    } fp;

    fp.d = x;
    switch (fp.w[0] & 0x7ff00000) {
    case 0x7ff00000:
        if ((fp.w[0] & 0xfffff) || fp.w[1]) {
            cls = FP_NAN;
        } else {
            cls = FP_INFINITE;
        }
        break;
    case 0:
        if ((fp.w[0] & 0xfffff) || fp.w[1]) {
            cls = FP_SUBNORMAL;
        } else {
            cls = FP_ZERO;
        }
        break;
    default:
        cls = FP_NORMAL;
        break;
    }
    if (cls <= FP_INFINITE || x == zero) {
        return x;
    }
    hx = *(int *)&x;
    lx = *((int *)&x + 1);
    k = (hx & 0x7ff00000) >> 20; /* exponent of x */
    if (k == 0) {                /* subnormal x */
        if ((lx | (hx & 0x7fffffff)) == 0) {
            return x;
        }
        x *= two54; /* normalize x */
        hx = *(int *)&x;
        k = ((hx & 0x7ff00000) >> 20) - 54;
        if (exp < -50000) {
            return tiny * x; /* underflow */
        }
    }
    if (k == 0x7ff) {
        return x + x; /* overflow or NaN */
    }
    k = k + exp;
    if (k > 0x7fe) {
        return huge * copysign(huge, x); /* overflow */
    }
    if (k > 0) { /* normal result */
        *(int *)&x = (hx & 0x800fffff) | (k << 20);
        return x;
    }
    if (k <= -54) {
        if (exp > 50000) { /* integer overflow in exp + k */
            return huge * copysign(huge, x); /* overflow */
        }
        return tiny * copysign(tiny, x); /* underflow */
    }
    k += 54; /* subnormal result */
    *(int *)&x = (hx & 0x800fffff) | (k << 20);
    return twom54 * x;
}
