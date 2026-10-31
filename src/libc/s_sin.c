#include "s_sin.h"

#include "e_rem_pio2.h"
#include "k_cos.h"
#include "k_sin.h"

/*
 * Sine.  Arguments within pi/4 of the origin go straight to the
 * reduced-range kernel with a zero tail; nonfinite inputs fold
 * through x - x to produce a NaN, and everything else is reduced
 * modulo pi/2 first, with the kernel choice and sign taken from the
 * quadrant of the reduction.
 */
double sin(double x)
{
    double y[2], z = 0.0;
    int n, hx, ix;

    /* High word of x. */
    hx = *(int *)&x;
    ix = hx & 0x7fffffff;
    if (ix <= 0x3fe921fb) { /* |x| ~<= pi/4 */
        return __kernel_sin(x, z, 0);
    }
    if (ix >= 0x7ff00000) { /* sin(NaN/Inf) is NaN */
        return x - x;
    }
    /* Argument reduction needed. */
    n = __ieee754_rem_pio2(x, y);
    switch (n & 3) {
    case 0:
        return __kernel_sin(y[0], y[1], 1);
    case 1:
        return __kernel_cos(y[0], y[1]);
    case 2:
        return -__kernel_sin(y[0], y[1], 1);
    default:
        return -__kernel_cos(y[0], y[1]);
    }
}
