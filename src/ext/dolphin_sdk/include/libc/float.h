#ifndef _FLOAT_H_
#define _FLOAT_H_

#ifdef __cplusplus
extern "C" {
#endif

extern float __float_min[];
extern float __float_epsilon[];

#ifdef __cplusplus
}
#endif

#define FLT_MIN (__float_min[0])
#define FLT_EPSILON 1.1920928955078125e-07f

#endif
