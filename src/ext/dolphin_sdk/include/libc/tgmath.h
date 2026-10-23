#ifndef _TGMATH_H_
#define _TGMATH_H_

/* The float overload clears the sign bit without conversion to double. */

#pragma cplusplus on
extern "C" double fabs(double x);
extern inline float fabs(float x) { (*(int*)&x)&=0x7fffffff; return x; }
#pragma cplusplus reset

#endif // _TGMATH_H_
