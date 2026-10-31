#include <math.h>

#include "s_atan.h"

/*
 * Arc tangent.  The argument is reduced with one of four rational
 * transformations to [-0.66, 0.66], where a shared odd polynomial in
 * x*x applies, and the result is rebuilt from split high/low values of
 * atan at the interval anchors 0.5, 1, 1.5 and infinity.
 */

static const double
    huge = 1.0e+300,
    one = 1.00000000000000000000e+00;

static const double atanhi[] = {
     4.63647609000806093515e-01, /* atan(0.5) */
     7.85398163397448278999e-01, /* atan(1.0) */
     9.82793723247329054082e-01, /* atan(1.5) */
     1.57079632679489655800e+00, /* atan(inf) */
};

static const double atanlo[] = {
     2.26987774529616870924e-17, /* atan(0.5) low bits */
     3.06161699786838301793e-17, /* atan(1.0) low bits */
     1.39033110312309984516e-17, /* atan(1.5) low bits */
     6.12323399573676603587e-17, /* atan(inf) low bits */
};

static const double aT[] = {
     3.33333333333329318055e-01,
    -1.99999999998764832476e-01,
     1.42857142725034663711e-01,
    -1.1111110405462355788e-01,
     9.0908871334365065620e-02,
    -7.69187620504482999495e-02,
     6.66107313738753120669e-02,
    -5.83357013379057348645e-02,
     4.97687799461593204617e-02,
    -3.65315727442169154794e-02,
     1.62858201153657823623e-02,
};

double atan(double x)
{
    double w, s1, s2, z;
    int ix, hx, id;

    hx = *(int *)&x;
    ix = hx & 0x7fffffff;
    if (ix >= 0x44100000) { /* |x| >= 2^66 */
        if (ix > 0x7ff00000 || (ix == 0x7ff00000 && *((int *)&x + 1) != 0)) {
            return x + x; /* NaN */
        }
        if (hx > 0) {
            return atanhi[3] + atanlo[3];
        } else {
            return -atanhi[3] - atanlo[3];
        }
    } else if (ix < 0x3fdc0000) { /* |x| < 0.4375 */
        if (ix < 0x3e200000) { /* |x| < 2^-29 */
            if (huge + x > one) {
                return x; /* trigger inexact */
            }
        }
        id = -1;
    } else {
        x = fabs(x);
        if (ix < 0x3ff30000) { /* |x| < 1.1875 */
            if (ix < 0x3fe60000) { /* 7/16 <= |x| < 11/16 */
                id = 0;
                x = (2.0 * x - one) / (2.0 + x);
            } else { /* 11/16 <= |x| < 19/16 */
                id = 1;
                x = (x - one) / (x + one);
            }
        } else {
            if (ix < 0x40038000) { /* |x| < 2.4375 */
                id = 2;
                x = (x - 1.5) / (one + x * 1.5);
            } else { /* 2.4375 <= |x| < 2^66 */
                id = 3;
                x = -one / x;
            }
        }
    }
    /* End of argument reduction. */
    z = x * x;
    w = z * z;
    /* Break the odd polynomial into even and odd power chains. */
    s1 = z * (aT[0] + w * (aT[2] + w * (aT[4] + w * (aT[6] + w * (aT[8] + w * aT[10])))));
    s2 = w * (aT[1] + w * (aT[3] + w * (aT[5] + w * (aT[7] + w * aT[9]))));
    if (id < 0) {
        return x - x * (s1 + s2);
    } else {
        z = atanhi[id] - ((x * (s1 + s2) - atanlo[id]) - x);
        return (hx < 0) ? -z : z;
    }
}
