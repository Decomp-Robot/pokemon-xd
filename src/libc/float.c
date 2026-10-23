#define LIBC_FLOAT_DATA_PROVIDER
#include "float_data.h"

/*
 * MSL single-precision special values and the double-storage limits used
 * by the decimal conversion routines.
 *
 * math.h publishes NAN as __float_nan[0]; the formatter clamps overflowed
 * results to __float_huge[0].  The library stores the NaN as its raw
 * IEEE-754 word in small data.
 */
typedef union {
    unsigned long bits;
    float value;
} FloatBits;

FloatBits __float_nan = { 0x7FFFFFFFUL };
float __float_huge[] = { 1.0f / 0.0f };
#if defined(__MWERKS__)
#pragma weak floatInfinity = __float_huge
#else
float floatInfinity[] = { 1.0f / 0.0f };
#endif

/* Limits used by the MSL decimal conversion routines. */
double doubleMinNormal[] = { 2.2250738585072014e-308 };
double doubleMaxFinite[] = { 1.7976931348623157e+308 };
double conversionInfinity[] = { 1.0 / 0.0 };
double conversionMinNormal[] = { 2.2250738585072014e-308 };
double conversionMaxFinite[] = { 1.7976931348623157e+308 };

float __float_min[] = { 1.1754943508222875e-38f };
float __float_epsilon[] = { 1.1920928955078125e-7f };
