#include <GSbezier.h>
#include <dolphin/mtx.h>

void GSbezierCalculateVector(GSvec *out, const GSvec *points, f32 t) {
    GSvec contribution;
    f32 tSquared;
    f32 remaining = 1.0f;
    f32 remainingSquared;
    if (t < 0.0f) {
        t = 0.0f;
    }
    if (t > remaining) {
        t = remaining;
    }
    tSquared = t * t;
    remaining = 1.0f - t;
    remainingSquared = remaining * remaining;
    PSVECScale((const Vec *)&points[0], (Vec *)out,
               remainingSquared * remaining);
    PSVECScale((const Vec *)&points[2], (Vec *)&contribution,
               3.0f * t * remainingSquared);
    PSVECAdd((const Vec *)out, (const Vec *)&contribution, (Vec *)out);
    PSVECScale((const Vec *)&points[3], (Vec *)&contribution,
               3.0f * tSquared * remaining);
    PSVECAdd((const Vec *)out, (const Vec *)&contribution, (Vec *)out);
    PSVECScale((const Vec *)&points[1], (Vec *)&contribution, tSquared * t);
    PSVECAdd((const Vec *)out, (const Vec *)&contribution, (Vec *)out);
}
