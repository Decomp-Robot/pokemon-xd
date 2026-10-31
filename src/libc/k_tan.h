#ifndef MSL_K_TAN_H
#define MSL_K_TAN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Tangent on the reduced range [-pi/4, pi/4], with the argument's tail
 * correction applied.  Mode 1 returns the tangent; mode -1 returns the
 * compensated negative reciprocal used for odd quadrants. */
double __kernel_tan(double x, double tail, int mode);

#ifdef __cplusplus
}
#endif

#endif
