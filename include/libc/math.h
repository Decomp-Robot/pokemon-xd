#ifndef LIBC_MATH_H
#define LIBC_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

double sin(double x);
double tan(double x);
double fmod(double numerator, double denominator);

#ifdef __cplusplus
}

namespace std {
float sinf(float angle);
float cosf(float angle);
float pow(float base, float exponent);
float atan2f(float y, float x);
}
#endif

#endif
