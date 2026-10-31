#include "s_tan.h"

#include "e_rem_pio2.h"
#include "k_tan.h"

/*
 * Tangent.  Arguments within one quadrant of the origin go straight to
 * the reduced-range kernel with a zero tail; nonfinite inputs fold
 * through x - x to produce a NaN, and everything else is reduced
 * modulo pi/2 first, with the kernel mode flipped to the negative
 * reciprocal on the odd quadrants.
 */
double tan(double x)
{
    double y[2], z = 0.0;
    int n, hx, ix;

    /* High word of x. */
    hx = *(int *)&x;
    ix = hx & 0x7fffffff;
    if (ix <= 0x3fe921fb) { /* |x| ~<= pi/4 */
        return __kernel_tan(x, z, 1);
    }
    if (ix >= 0x7ff00000) { /* tan(NaN/Inf) is NaN */
        return x - x;
    }
    /* Argument reduction needed. */
    n = __ieee754_rem_pio2(x, y);
    return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1)); /* 1 even, -1 odd */
}
