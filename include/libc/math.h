#ifndef LIBC_MATH_H
#define LIBC_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

double sin(double x);
double fmod(double numerator, double denominator);

#ifdef __cplusplus
}

namespace std {
float sinf(float angle);
float cosf(float angle);
}
#endif

#endif
