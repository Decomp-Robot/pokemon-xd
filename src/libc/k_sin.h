#ifndef MSL_K_SIN_H
#define MSL_K_SIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Sine on the reduced range [-pi/4, pi/4], with the argument's tail
 * correction applied when hasTail is nonzero. */
double __kernel_sin(double x, double tail, int hasTail);

#ifdef __cplusplus
}
#endif

#endif
