#ifndef MSL_E_LOG_H
#define MSL_E_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Natural logarithm of x over the IEEE double range.  The kernel itself
 * signals the domain: negative arguments store EDOM in errno and produce
 * an invalid operation, +-0 returns -inf, and inf/NaN propagate.  The
 * public log entry point calls this kernel with its argument unchanged.
 */
double __ieee754_log(double x);

#ifdef __cplusplus
}
#endif

#endif
