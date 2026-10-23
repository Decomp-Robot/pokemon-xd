#ifndef PESPLINE_H
#define PESPLINE_H

#include <GSgfx.h>

/* Mode a spline runs in, kept in the low bits of flags. Mode 0 leaves the
 * spline unused: the callers keep their constant multiplier instead. */
enum peSplineMode {
    PE_SPLINE_MODE_NONE = 0,
    PE_SPLINE_MODE_MASK = 3
};

struct peSpline {
    u8 flags;
    u8 reserved[3];
    f32 value;
    f32 time;
};
struct peColorSpline;


extern "C" {
f32 peSplineGetValue(peSpline *spline, f32 time);
void peColorSplineGetValue(GScolor *color, peColorSpline *spline, f32 time);
}

#endif
