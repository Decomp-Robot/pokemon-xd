#include "s_modf.h"

double modf(double x, double *iptr)
{
    int i0, i1, j0;
    unsigned int i;

    i0 = *(int *)&x;
    i1 = *((unsigned int *)&x + 1);
    j0 = ((i0 >> 20) & 0x7FF) - 1023; /* exponent of x */
    if (j0 < 20) {
        if (j0 < 0) {
            /* |x| < 1: the integral part is a zero carrying x's sign */
            *(int *)iptr = i0 & 0x80000000;
            *((unsigned int *)iptr + 1) = 0;
            return x;
        }
        i = 0x000FFFFF >> j0;
        if (((i0 & i) | i1) == 0) {
            /* x is integral: the fraction is a zero carrying x's sign */
            *iptr = x;
            *(int *)&x = i0 & 0x80000000;
            *((unsigned int *)&x + 1) = 0;
            return x;
        } else {
            *(int *)iptr = i0 & ~i;
            *((unsigned int *)iptr + 1) = 0;
            return x - *iptr;
        }
    } else if (j0 > 51) {
        /* no fraction part */
        *iptr = x;
        *(int *)&x = i0 & 0x80000000;
        *((unsigned int *)&x + 1) = 0;
        return x;
    } else {
        i = 0xFFFFFFFF >> (j0 - 20);
        if ((i1 & i) == 0) {
            /* x is integral: the fraction is a zero carrying x's sign */
            *iptr = x;
            *(int *)&x = i0 & 0x80000000;
            *((unsigned int *)&x + 1) = 0;
            return x;
        } else {
            *(int *)iptr = i0;
            *((unsigned int *)iptr + 1) = i1 & ~i;
            return x - *iptr;
        }
    }
}
