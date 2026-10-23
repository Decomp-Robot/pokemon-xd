#ifndef GSMTX_H
#define GSMTX_H

#include <types.h>

#ifdef __cplusplus
extern "C" void PSMTXConcat(const f32 first[3][4], const f32 second[3][4], f32 result[3][4]);
extern "C" void PSMTXCopy(const f32 source[3][4], f32 destination[3][4]);

#else
void PSMTXConcat(const f32 first[3][4], const f32 second[3][4], f32 result[3][4]);
#endif

#ifdef __cplusplus
/* Affine 3x4 transform. */
struct GSmtx {
    f32 m00, m01, m02, m03;
    f32 m10, m11, m12, m13;
    f32 m20, m21, m22, m23;

    GSmtx operator*(const GSmtx &other) const {
        GSmtx result;
        PSMTXConcat(sdk(), other.sdk(), result.sdk());
        return result;
    }

    f32 (*sdk())[4] { return reinterpret_cast<f32 (*)[4]>(&m00); }
    const f32 (*sdk() const)[4] { return reinterpret_cast<const f32 (*)[4]>(&m00); }
};

extern const f32 mtxIdent[12];

extern "C" {
void GSmtx44Perspective(f32 matrix[4][4], f32 fov, f32 aspect, f32 nearPlane, f32 farPlane);
void GSmtx44Ortho(f32 matrix[4][4], f32 top, f32 bottom, f32 left, f32 right, f32 nearPlane, f32 farPlane);
}
#endif

#endif
