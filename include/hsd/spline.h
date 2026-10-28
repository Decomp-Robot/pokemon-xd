#ifndef HSD_SPLINE_H
#define HSD_SPLINE_H

#include <types.h>
#include <dolphin/mtx.h>

/* Interpolation mode selected by HSD_Spline::type. */
#define SPLINE_LINEAR   0
#define SPLINE_BEZIER   1
#define SPLINE_BSPLINE  2
#define SPLINE_CARDINAL 3

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _HSD_Spline {
    u8 type;          /* interpolation mode */
    s16 count;        /* number of values in points */
    f32 tension;      /* outer control value weight of the cardinal mode */
    Vec* points;      /* control values */
    f32 length;       /* arc length of the spline */
    f32* lengths;     /* arc length up to each control span */
    f32* coefs;       /* five polynomial coefficients per span */
} HSD_Spline;

void splArcLengthPoint(Vec* v, HSD_Spline* spl, f32 t);
f32 splArcLengthGetParameter(HSD_Spline* spl, f32 t);
f32 splArcLength(f32* coef, f32 t0, f32 t1);
void splGetSplinePoint(Vec* v, HSD_Spline* spl, f32 t);
f32 splGetHelmite(f32 rate, f32 t, f32 p0, f32 p1, f32 m0, f32 m1);

#ifdef __cplusplus
}
#endif

#endif
