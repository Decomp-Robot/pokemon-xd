#ifndef GS_MATH_H
#define GS_MATH_H

#include <dolphin/mtx.h>

#include <GSmathTypes.h>
#include <GSAPI/GSsysdolphin_baselib/baselib/quatlib.h>

extern GSvec unitScaleVector;

extern "C" {
void GSvecSet(GSvec *out, f32 x, f32 y, f32 z);
void GSvecAdd(GSvec *out, const GSvec *first, const GSvec *second);
void GSvecTransform(GSvec *out, const GSmtx *matrix, const GSvec *value);
f32 GSvecSquareDistance(const GSvec *first, const GSvec *second);
void GSvec2Set(GSvec2 *out, f32 x, f32 y);
void GSvec2Sub(GSvec2 *out, const GSvec2 *first, const GSvec2 *second);
void GSvec2Rot(GSvec2 *out, const GSvec2 *value, f32 angle);
f32 GSvec2Length(const GSvec2 *value);
void GSmtxMakeYRotation(GSmtx *out, f32 angle);

void GSlerpGetLinearInterpolationVector(GSvec *out, const GSvec *start, const GSvec *end, f32 blend);
f32 GSlerpGetLinearInterpolationFloat(f32 start, f32 end, f32 blend);
void GSvecMakeFromQuat(GSvec *euler, const f32 *quat);
void GSvecTransformQuat(GSvec *out, const f32 *quat, const GSvec *v);
}

#endif
