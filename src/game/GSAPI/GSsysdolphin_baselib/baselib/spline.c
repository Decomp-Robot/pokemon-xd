#include <math.h>
#include <hsd/spline.h>

/* Length rate of one segment at parameter t: the magnitude of the
 * segment's velocity vector, clamped to zero at the segment roots. */
static inline f32 splGetCoefSpeed(f32* coef, f32 t)
{
    f32 tSquared = t * t;
    f32 tCubed = tSquared * t;
    f32 tFourth = tCubed * t;
    f32 value = coef[0] * tFourth + coef[1] * tCubed + coef[2] * tSquared
              + coef[3] * t + coef[4];

    if (value < 0.0f && value > -0.001f) {
        value = 0.0f;
    }
    return sqrtf(value);
}

static inline f32 splAbs(f32 v)
{
    if (v < 0.0f) {
        v = -v;
    }
    return v;
}

void splArcLengthPoint(Vec* v, HSD_Spline* spl, f32 t)
{
    f32 u = splArcLengthGetParameter(spl, t);

    splGetSplinePoint(v, spl, u);
}

f32 splArcLengthGetParameter(HSD_Spline* spl, f32 t)
{
    f32 lower = 0.0f;
    f32 upper = 1.0f;
    f32 u;
    f32 length;
    f32 segment;
    s32 i = 0;

    if (t <= 0.0f) {
        return 0.0f;
    }
    if (t >= 1.0f) {
        return 1.0f;
    }

    while (spl->lengths[i + 1] < t) {
        i++;
    }

    switch (spl->type) {
    case SPLINE_LINEAR:
        u = (t - spl->lengths[i]) / (spl->lengths[i + 1] - spl->lengths[i]);
        break;
    case SPLINE_BEZIER:
    case SPLINE_BSPLINE:
    case SPLINE_CARDINAL:
        length = spl->length * (t - spl->lengths[i]);
        while (splAbs(lower - upper) >= 0.00001f) {
            u = (lower + upper) / 2.0f;
            segment = splArcLength(&spl->coefs[i * 5], lower, u);
            if (length < 0.00001f + segment) {
                upper = u;
            } else {
                lower = u;
                length -= segment;
            }
        }
        break;
    }
    return (u + i) / (spl->count - 1.0f);
}

f32 splArcLength(f32* coef, f32 t0, f32 t1)
{
    f32 t = t0;
    f32 step = t1 - t0;
    f32 sum = 0.0f;
    s32 i;

    step *= 0.125f;
    t += step;

    /* Nine point composite Simpson rule over the span. */
    for (i = 2; i < 9; i++) {
        if ((i & 1) == 0) {
            sum += 4.0f * splGetCoefSpeed(coef, t);
        } else {
            sum += 2.0f * splGetCoefSpeed(coef, t);
        }
        t += step;
    }
    return step * (sum + splGetCoefSpeed(coef, t0)
                 + splGetCoefSpeed(coef, t1)) / 3.0f;
}

void splGetSplinePoint(Vec* v, HSD_Spline* spl, f32 t)
{
    s16 i;

    if (t < 0.0f || t > 1.0f) {
        return;
    }
    if (t < 1.0f) {
        Vec* p;
        f32 w3;
        f32 tCubed;
        f32 w1;
        f32 remainingSquared;
        f32 frac;
        f32 w2;
        f32 w0;
        f32 remaining;
        f32 oneSixth;

        frac = t * (spl->count - 1);
        i = (s16) frac;
        frac -= i;
        switch (spl->type) {
        case SPLINE_LINEAR:
            p = &spl->points[i];
            v->x = p[0].x + (p[1].x - p[0].x) * frac;
            v->y = p[0].y + (p[1].y - p[0].y) * frac;
            v->z = p[0].z + (p[1].z - p[0].z) * frac;
            break;
        case SPLINE_BEZIER: {
            f32 tSquared;
            remaining = 1.0f - frac;
            remainingSquared = remaining * remaining;
            tSquared = frac * frac;
            p = &spl->points[i * 3];
            w3 = tSquared * frac;
            w2 = 3.0f * tSquared * remaining;
            w1 = 3.0f * frac * remainingSquared;
            w0 = remainingSquared * remaining;
            v->x = p[0].x * w0 + p[1].x * w1 + p[2].x * w2 + p[3].x * w3;
            v->y = p[0].y * w0 + p[1].y * w1 + p[2].y * w2 + p[3].y * w3;
            v->z = p[0].z * w0 + p[1].z * w1 + p[2].z * w2 + p[3].z * w3;
            break;
        }
        case SPLINE_BSPLINE: {
            f32 tSquared;
            oneSixth = 1.0f / 6.0f;
            tSquared = frac * frac;
            remaining = 1.0f - frac;
            tCubed = tSquared * frac;
            p = &spl->points[i];
            w0 = oneSixth * remaining * remaining * remaining;
            w1 = oneSixth * (3.0f * tCubed - 6.0f * tSquared + 4.0f);
            w2 = oneSixth * (3.0f * (-tCubed + tSquared + frac) + 1.0f);
            w3 = oneSixth * tCubed;
            v->x = p[0].x * w0 + p[1].x * w1 + p[2].x * w2 + p[3].x * w3;
            v->y = p[0].y * w0 + p[1].y * w1 + p[2].y * w2 + p[3].y * w3;
            v->z = p[0].z * w0 + p[1].z * w1 + p[2].z * w2 + p[3].z * w3;
            break;
        }
        case SPLINE_CARDINAL: {
            f32 tSquared;
            f32 tension;
            tSquared = frac * frac;
            tCubed = tSquared * frac;
            tension = spl->tension;
            p = &spl->points[i];
            w0 = tension * (-tCubed + 2.0f * tSquared - frac);
            w1 = 1.0f + ((2.0f - tension) * tCubed + (tension - 3.0f) * tSquared);
            w2 = tension * frac + ((tension - 2.0f) * tCubed + (3.0f - 2.0f * tension) * tSquared);
            w3 = tension * (tCubed - tSquared);
            v->x = p[0].x * w0 + p[1].x * w1 + p[2].x * w2 + p[3].x * w3;
            v->y = p[0].y * w0 + p[1].y * w1 + p[2].y * w2 + p[3].y * w3;
            v->z = p[0].z * w0 + p[1].z * w1 + p[2].z * w2 + p[3].z * w3;
            break;
        }
        }
        return;
    }
    i = spl->count - 1;
    switch (spl->type) {
    case SPLINE_LINEAR:
        *v = spl->points[i];
        break;
    case SPLINE_BEZIER:
        *v = spl->points[i * 3];
        break;
    case SPLINE_BSPLINE: {
        Vec* p = &spl->points[i] - 1;
        f32 frac = 1.0f;
        f32 tSquared = frac * frac;
        f32 tCubed = tSquared * frac;
        f32 remaining = 1.0f - frac;
        f32 w0 = (1.0f / 6.0f) * remaining * remaining * remaining;
        f32 w1 = (1.0f / 6.0f) * (3.0f * tCubed - 6.0f * tSquared + 4.0f);
        f32 w2 = (1.0f / 6.0f) * (3.0f * (-tCubed + tSquared + frac) + 1.0f);
        f32 w3 = (1.0f / 6.0f) * tCubed;

        v->x = p[0].x * w0 + p[1].x * w1 + p[2].x * w2 + p[3].x * w3;
        v->y = p[0].y * w0 + p[1].y * w1 + p[2].y * w2 + p[3].y * w3;
        v->z = p[0].z * w0 + p[1].z * w1 + p[2].z * w2 + p[3].z * w3;
        break;
    }
    case SPLINE_CARDINAL: {
        Vec* p = &spl->points[i];
        *v = p[1];
        break;
    }
    }
}

f32 splGetHelmite(f32 rate, f32 t, f32 p0, f32 p1, f32 m0, f32 m1)
{
    f32 rate2 = rate * rate;
    f32 t2 = t * t;
    f32 t3 = t2 * t;
    f32 t2rate = t2 * rate;
    f32 rate2t3 = rate2 * t3;
    f32 twiceRate2t3 = 2.0f * rate2t3;
    f32 m1Weight = rate2t3 - t2rate;
    f32 cubicRate = twiceRate2t3 * rate;
    f32 threeT2Rate2 = 3.0f * t2 * rate2;
    f32 m0Weight = m1Weight - t2rate;
    f32 m0Total = t + m0Weight;
    f32 p1Weight = -cubicRate + threeT2Rate2;
    f32 p0Weight = 1.0f + (cubicRate - threeT2Rate2);

    return p0 * p0Weight
         + p1 * p1Weight
         + m0 * m0Total
         + m1 * m1Weight;
}
