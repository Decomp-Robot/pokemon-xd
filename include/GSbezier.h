#ifndef GS_BEZIER_H
#define GS_BEZIER_H

#include <types.h>
#include <GSvec.h>

// Input order: start endpoint, end endpoint, start control, end control.
extern "C" void GSbezierCalculateVector(GSvec *out, const GSvec *points, f32 t);

#endif
