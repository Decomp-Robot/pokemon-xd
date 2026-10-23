#ifndef LIBC_FLOAT_DATA_H
#define LIBC_FLOAT_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MSL special values and floating conversion limits.  NAN is published by
 * math.h as __float_nan[0].
 */
#ifndef LIBC_FLOAT_DATA_PROVIDER
extern float __float_nan[];
#endif
extern float __float_huge[];
/* Historical formatter-facing alias required by the shared special-value interface. */
extern float floatInfinity[];
extern double doubleMinNormal[];
extern double doubleMaxFinite[];
extern double conversionInfinity[];
extern double conversionMinNormal[];
extern double conversionMaxFinite[];

#ifdef __cplusplus
}
#endif

#endif
