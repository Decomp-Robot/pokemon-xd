#include "s_floor.h"

static const double huge = 1.0e+300;

double floor(double x)
{
    union {
        double d;
        long w[2];
    } fp;
    int hx, lx;
    int e;
    unsigned int i;
    unsigned int j, k;

    fp.d = x;
    hx = fp.w[0];
    lx = fp.w[1];
    e = ((hx >> 20) & 0x7ff) - 1023; /* exponent of x */

    if (e < 20) {
        if (e < 0) {
            /* |x| < 1.0 */
            if (huge + x > 0.0) {
                if (hx >= 0) {
                    hx = lx = 0;
                } else if ((hx & 0x7fffffff) | lx) {
                    hx = 0xbff00000;
                    lx = 0;
                }
            }
        } else {
            i = (0x100000 - 1) >> e;
            if (((hx & i) | lx) == 0) {
                return x; /* integral */
            }
            if (huge + x > 0.0) {
                if (hx < 0) {
                    hx += 0x100000 >> e;
                }
                hx &= ~i;
                lx = 0;
            }
        }
    } else if (e > 51) {
        if (e == 1024) {
            return x + x; /* inf or NaN */
        }
        return x; /* integral */
    } else {
        i = 0xffffffff >> (e - 20);
        if ((lx & i) == 0) {
            return x; /* integral */
        }
        if (huge + x > 0.0) {
            if (hx < 0) {
                if (e == 20) {
                    hx += 1;
                } else {
                    j = 1 << (52 - e);
                    k = lx + j;
                    if (k < lx) {
                        hx += 1;
                    }
                    lx = k;
                }
            }
            lx &= ~i;
        }
    }
    fp.w[0] = hx;
    fp.w[1] = lx;
    return fp.d;
}
