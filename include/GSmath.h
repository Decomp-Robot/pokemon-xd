#ifndef GS_MATH_H
#define GS_MATH_H

#include <dolphin/mtx.h>

#include <GSmathTypes.h>
#include <GSAPI/GSsysdolphin_baselib/baselib/quatlib.h>

extern GSvec unitScaleVector;

extern "C" {
void GSlerpGetLinearInterpolationVector(GSvec *out, const GSvec *start, const GSvec *end, f32 blend);
f32 GSlerpGetLinearInterpolationFloat(f32 start, f32 end, f32 blend);
void GSvecMakeFromQuat(GSvec *euler, const f32 *quat);
void GSvecTransformQuat(GSvec *out, const f32 *quat, const GSvec *v);
}

#endif
