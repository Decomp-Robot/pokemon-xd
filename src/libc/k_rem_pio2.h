#ifndef MSL_K_REM_PIO2_H
#define MSL_K_REM_PIO2_H

#ifdef __cplusplus
extern "C" {
#endif

/* Multiprecision pi/2 argument reduction. x holds nx base-2^24 chunks of the
 * input with exponent e0, ipio2 holds the caller's reciprocal-of-pi digits,
 * and the prec+1 components of the reduced remainder are written to y.
 * Returns the quotient modulo 8 for sine/cosine branch selection. */
int __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec,
                      const int *ipio2);

#ifdef __cplusplus
}
#endif

#endif
