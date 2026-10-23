#include "s_copysign.h"

double copysign(double x, double y) {
    *(unsigned long *)&x = (*(unsigned long *)&x & 0x7FFFFFFF) |
                           (*(unsigned long *)&y & 0x80000000);
    return x;
}
