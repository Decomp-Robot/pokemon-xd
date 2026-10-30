#include "k_sin.h"

/*
 * Reduced-argument sine kernel shared by sin, cos and the argument
 * reduction paths.  x must be reduced to [-pi/4, pi/4]; tail holds the
 * low part of x from the reduction, and hasTail selects the tail-aware
 * result.
 */
static const double
    S2 = 8.33333333332248946124e-03,
    S3 = -1.98412698298579493134e-04,
    S4 = 2.75573137070700676789e-06,
    S5 = -2.50507602534068634195e-08,
    S6 = 1.58969099521155010221e-10,
    S1 = -1.66666666666666324348e-01,
    half = 5.00000000000000000000e-01;

double __kernel_sin(double x, double tail, int hasTail)
{
    double z, r, v;
    int ix;

    ix = *(int *)&x & 0x7fffffff;
    if (ix < 0x3e400000) { /* |x| < 2^-27 */
        if ((int)x == 0) {
            return x; /* generate inexact */
        }
    }
    z = x * x;
    v = z * x;
    r = S2 + z * (S3 + z * (S4 + z * (S5 + z * S6)));
    if (hasTail == 0) {
        return x + v * (S1 + z * r);
    }
    return x - ((z * (half * tail - v * r) - tail) - v * S1);
}
