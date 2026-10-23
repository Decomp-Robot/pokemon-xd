#include <GSmath.h>

void GSlerpGetLinearInterpolationVector(GSvec *out, const GSvec *start,
                                      const GSvec *end, f32 blend) {
    if (blend <= 0.0f) {
        out->x = start->x;
        out->y = start->y;
        out->z = start->z;
    } else if (blend >= 1.0f) {
        out->x = end->x;
        out->y = end->y;
        out->z = end->z;
    } else {
        PSVECSubtract((const Vec *)end, (const Vec *)start, (Vec *)out);
        PSVECScale((const Vec *)out, (Vec *)out, blend);
        PSVECAdd((const Vec *)out, (const Vec *)start, (Vec *)out);
    }
}

f32 GSlerpGetLinearInterpolationFloat(f32 start, f32 end, f32 blend) {
    if (blend <= 0.0f) {
        return start;
    } else if (blend >= 1.0f) {
        return end;
    } else {
        return start + blend * (end - start);
    }
}
