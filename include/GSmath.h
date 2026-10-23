#ifndef GS_MATH_H
#define GS_MATH_H

#include <types.h>

#include <dolphin/mtx.h>
#include <GSvec.h>
#include <GSmtx.h>
#include <GSquat.h>

extern "C" {
void GSlerpGetLinearInterpolationVector(GSvec *out, const GSvec *start, const GSvec *end, f32 blend);
f32 GSlerpGetLinearInterpolationFloat(f32 start, f32 end, f32 blend);
void GSvecTransformQuat(GSvec *out, const f32 *rotation, const GSvec *value);
double fmod(double numerator, double denominator);
s32 EulerToQuat(const void *euler, void *quat);
s32 QuatSlerp(const void *a, const void *b, void *out, f32 t);
}

#endif
