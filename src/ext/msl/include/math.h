#ifndef _MATH_H_
#define _MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

#define M_PI 3.14159265358979323846f

extern float __float_nan[];
#define NAN       (__float_nan[0])
#define HUGE_VALF (1.0f / 0.0f)
#define INFINITY  (1.0f / 0.0f)

double fabs(double x);
double sin(double x);
double cos(double x);
double tan(double x);
double acos(double x);
double fmod(double x, double y);
double atan2(double y, double x);
double asin(double x);
double atan(double x);
double exp(double x);
double log(double x);
double pow(double x, double y);

float sinf(float x);
float cosf(float x);
float tanf(float x);
float acosf(float x);
float powf(float base, float exponent);

double ldexp(double x, int exp);

double scalbn(double x, int n);

double copysign(double x, double y);
double sqrt(double value);

#if defined(__MWERKS__) && !defined(MSL_MATH_IMPLEMENTATION)
#pragma cplusplus on
#endif

double floor(double x);

#define FP_NAN       1
#define FP_INFINITE  2
#define FP_ZERO      3
#define FP_NORMAL    4
#define FP_SUBNORMAL 5

#ifndef MSL_MATH_IMPLEMENTATION
extern inline long __fpclassifyf(float value)
{
    const long exponentMask = 0x7F800000;
    const long mantissaMask = 0x007FFFFF;
    switch (*(long*)&value & exponentMask) {
    case exponentMask:
        if (*(long*)&value & mantissaMask) return FP_NAN;
        return FP_INFINITE;
    case 0:
        if (*(long*)&value & mantissaMask) return FP_SUBNORMAL;
        return FP_ZERO;
    }
    return FP_NORMAL;
}

extern inline float sqrtf(float value)
{
    const double half = 0.5;
    const double three = 3.0;
    if (value > 0.0f) {
        double positiveValue = value;
        double estimate = __frsqrte(positiveValue);
        estimate = half * estimate * (three - estimate * estimate * positiveValue);
        estimate = half * estimate * (three - estimate * estimate * positiveValue);
        estimate = half * estimate * (three - estimate * estimate * positiveValue);
        return (float)(positiveValue * estimate);
    } else if (value < 0.0) {
        return NAN;
    } else if (__fpclassifyf(value) == FP_NAN) {
        return NAN;
    } else {
        return value;
    }
}

#else
long __fpclassifyf(float value);
float sqrtf(float value);
#endif

#if defined(__MWERKS__) && !defined(MSL_MATH_IMPLEMENTATION)
#define fabs(x) __fabs(x)
#endif

#if defined(__MWERKS__)
#define fabsf(x) __fabsf(x)
#else
float fabsf(float x);
#endif

long __fpclassifyd(double x);

#define fpclassify(x) (sizeof(x) == sizeof(float) ? __fpclassifyf((float)(x)) : __fpclassifyd((double)(x)))
#define isfinite(x) ((fpclassify(x) > FP_INFINITE))

#if defined(__MWERKS__) && !defined(MSL_MATH_IMPLEMENTATION)
#pragma cplusplus reset
#endif

#ifdef __cplusplus
}
#endif

#endif
