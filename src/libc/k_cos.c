#include "k_cos.h"

/*
 * Reduced-argument cosine kernel shared by sin and cos.  x must be
 * reduced to [-pi/4, pi/4]; tail holds the low part of x from the
 * caller's argument reduction.
 */
static const double
    one = 1.00000000000000000000e+00,
    C1 = 4.16666666666666019037e-02,
    C2 = -1.38888888888741095749e-03,
    C3 = 2.48015872894767294178e-05,
    C4 = -2.75573143513906633035e-07,
    C5 = 2.08757232129817482790e-09,
    C6 = -1.13596475577881948265e-11,
    half = 5.00000000000000000000e-01,
    w_big = 2.81250000000000000000e-01;

double __kernel_cos(double x, double tail)
{
    double z, r, w;
    int ix;

    ix = *(int *)&x & 0x7fffffff;
    if (ix < 0x3e400000) { /* |x| < 2^-27 */
        if ((int)x == 0) {
            return one; /* generate inexact */
        }
    }
    z = x * x;
    r = z * (C1 + z * (C2 + z * (C3 + z * (C4 + z * (C5 + z * C6)))));
    if (ix < 0x3FD33333) { /* |x| < 0.3 */
        return one - (half * z - (z * r - x * tail));
    } else {
        if (ix > 0x3FE90000) { /* |x| > 25/32 */
            w = w_big;
        } else {
            /* w is |x|/4, dropping the low 32 fraction bits */
            *(int *)&w = ix - 0x200000;
            *((int *)&w + 1) = 0;
        }
        return (one - w) - ((half * z - w) - (z * r - x * tail));
    }
}
