#ifndef LIBC_FPCLASSIFY_H
#define LIBC_FPCLASSIFY_H

/* Floating-point categories used by the math-library classification macro. */
#define FP_NAN       1
#define FP_INFINITE  2
#define FP_ZERO      3
#define FP_NORMAL    4
#define FP_SUBNORMAL 5
static inline long __fpclassifyf(float value)
{
    switch (*(unsigned long *)&value & 0x7f800000u) {
    case 0x7f800000u:
        if (*(unsigned long *)&value & 0x007fffffu) return FP_NAN;
        return FP_INFINITE;
    case 0:
        if (*(unsigned long *)&value & 0x007fffffu) return FP_SUBNORMAL;
        return FP_ZERO;
    }
    return FP_NORMAL;
}


#endif
