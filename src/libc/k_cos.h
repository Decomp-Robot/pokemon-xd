#ifndef MSL_K_COS_H
#define MSL_K_COS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Cosine on the reduced range [-pi/4, pi/4]; tail holds the low part
 * of the reduced argument from the caller's reduction. */
double __kernel_cos(double x, double tail);

#ifdef __cplusplus
}
#endif

#endif
