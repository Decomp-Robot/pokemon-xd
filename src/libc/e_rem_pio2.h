#ifndef MSL_E_REM_PIO2_H
#define MSL_E_REM_PIO2_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pi/2 argument reduction front end.  x is reduced modulo pi/2 into the
 * two-component remainder y[0] + y[1]; the return value is the quadrant
 * count for the sine/cosine branch selection.  Nonfinite inputs store
 * x - x in both components and return 0.
 */
int __ieee754_rem_pio2(double x, double *y);

#ifdef __cplusplus
}
#endif

#endif
