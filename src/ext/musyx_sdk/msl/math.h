#ifndef _MSL_MATH_H_
#define _MSL_MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

double fmod(double x, double y);
double floor(double x);

#ifdef __cplusplus
}
#endif

static inline float fmodf(float x, float y)
{
    return (float)fmod((double)x, (double)y);
}

static inline float floorf(float x)
{
    return (float)floor((double)x);
}

static inline float sqrtf(float x)
{
    static const double _half = .5;
    static const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

#endif
