#include <math.h>

#include "k_tan.h"

/*
 * Tangent on the reduced range [-pi/4, pi/4].  x is the reduced
 * argument and tail is the low part dropped by the reduction.  Mode 1
 * returns the tangent; mode -1 returns the compensated negative
 * reciprocal used for the odd quadrants.
 */

static const double
    one = 1.00000000000000000000e+00,
    pio4 = 7.85398163397448278999e-01,
    pio4lo = 3.06161699786838301793e-17;

static const double T[] = {
     3.33333333333334091986e-01,
     1.33333333333201242699e-01,
     5.39682539762260521377e-02,
     2.18694882948595424599e-02,
     8.86323982359930005737e-03,
     3.59207910759131235356e-03,
     1.45620945432529025516e-03,
     5.88041240820264096874e-04,
     2.46463134818469906812e-04,
     7.81794442939557092300e-05,
     7.14072491382608190305e-05,
    -1.85586374855275456652e-05,
     2.59073051863633712884e-05,
};

double __kernel_tan(double x, double tail, int mode)
{
    double z, r, v, w, s;
    int ix, hx;

    hx = *(int *)&x;
    ix = hx & 0x7fffffff;
    if (ix < 0x3e300000) { /* x < 2**-28 */
        if ((int)x == 0) { /* generate inexact */
            if ((ix | *((int *)&x + 1) | (mode + 1)) == 0) {
                r = fabs(x);
                return one / r;
            }
            if (mode == 1)
                return x;
            return -one / x;
        }
    }
    if (ix >= 0x3FE59428) { /* |x| >= 0.6744 */
        if (hx < 0) {
            x = -x;
            tail = -tail;
        }
        z = pio4 - x;
        w = pio4lo - tail;
        x = z + w;
        tail = 0.0;
    }
    z = x * x;
    w = z * z;
    /* Break x^5*(T[1]+x^2*T[2]+...) into
     *   x^5*(T[1]+x^4*T[3]+...+x^20*T[11]) +
     *   x^5*(x^2*(T[2]+x^4*T[4]+...+x^22*T[12]))
     */
    r = T[1] + w * (T[3] + w * (T[5] + w * (T[7] + w * (T[9] + w * T[11]))));
    v = z * (T[2] + w * (T[4] + w * (T[6] + w * (T[8] + w * (T[10] + w * T[12])))));
    s = z * x;
    r = tail + z * (s * (r + v) + tail);
    r += T[0] * s;
    w = x + r;
    if (ix >= 0x3FE59428) {
        v = (double)mode;
        return (double)(1 - ((hx >> 30) & 2)) * (v - 2.0 * (x - (w * w / (w + v) - r)));
    }
    if (mode == 1)
        return w;
    else { /* compute -1.0/(x+r) accurately */
        double a, t;

        z = w;
        *((int *)&z + 1) = 0;
        v = r - (z - x); /* z+v = r+x */
        t = a = -1.0 / w; /* a = -1.0/w */
        *((int *)&t + 1) = 0;
        s = 1.0 + t * z;
        return t + a * (s + t * v);
    }
}
