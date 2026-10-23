#ifndef _MATH_H_
#define _MATH_H_

#define M_PI 3.14159265358979323846f

extern float __float_nan[];
#define NAN       (__float_nan[0])
#define HUGE_VALF (1.0f / 0.0f)
#define INFINITY  (1.0f / 0.0f)

double fabs(double x);
double sin(double x);
double cos(double x);

float sinf(float x);
float cosf(float x);
float tanf(float x);
float acosf(float x);
float powf(float base, float exponent);

double ldexp(double x, int exp);

double scalbn(double x, int n);

double copysign(double x, double y);

#ifdef __MWERKS__
#pragma cplusplus on
#endif

double floor(double x);

#define FP_NAN       1
#define FP_INFINITE  2
#define FP_ZERO      3
#define FP_NORMAL    4
#define FP_SUBNORMAL 5

extern inline long __fpclassifyf(float value)
{
    const unsigned int exponentMask = 0x7F800000;
    const unsigned int mantissaMask = 0x007FFFFF;
    switch (*(unsigned int*)&value & exponentMask) {
    case exponentMask:
        if (*(unsigned int*)&value & mantissaMask) return FP_NAN;
        return FP_INFINITE;
    case 0:
        if (*(unsigned int*)&value & mantissaMask) return FP_SUBNORMAL;
        return FP_ZERO;
    }
    return FP_NORMAL;
}

extern inline float sqrtf(float value)
{
    const double half = 0.5;
    const double three = 3.0;
    if (value > 0.0f) {
        double estimate = __frsqrte((double)value);
        estimate = half * estimate * (three - estimate * estimate * value);
        estimate = half * estimate * (three - estimate * estimate * value);
        estimate = half * estimate * (three - estimate * estimate * value);
        return (float)(value * estimate);
    } else if (value < 0.0) {
        return NAN;
    } else if (__fpclassifyf(value) == FP_NAN) {
        return NAN;
    } else {
        return value;
    }
}

double sqrt(double value);

#ifdef __MWERKS__
#define fabs(x) __fabs(x)
#define fabsf(x) __fabsf(x)
#else
double fabs(double x);
float fabsf(float x);
#endif

long __fpclassifyd(double x);

#define fpclassify(x) (sizeof(x) == sizeof(float) ? __fpclassifyf((float)(x)) : __fpclassifyd((double)(x)))
#define isfinite(x) ((fpclassify(x) > FP_INFINITE))

#ifdef __MWERKS__
#pragma cplusplus reset
#endif

#endif
