/* Single-precision math primitives. */

#define MSL_MATH_IMPLEMENTATION
#include <math.h>

#pragma defer_codegen on

long __fpclassifyf(float value)
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

float acosf(float x) { return (float)acos(x); }

float cosf(float x) { return (float)cos(x); }

float sinf(float x) { return (float)sin(x); }

float tanf(float x) { return (float)tan(x); }

float sqrtf(float value)
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

double fabs(double x)
{
    return __fabs(x);
}

#pragma defer_codegen off
