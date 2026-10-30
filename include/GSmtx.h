#ifndef GSMTX_H
#define GSMTX_H

#include <types.h>

#ifdef __cplusplus
extern "C" void PSMTXConcat(const f32 first[3][4], const f32 second[3][4], f32 result[3][4]);
extern "C" void PSMTXCopy(const f32 source[3][4], f32 destination[3][4]);
extern "C" void PSMTXIdentity(f32 m[3][4]);

#else
void PSMTXConcat(const f32 first[3][4], const f32 second[3][4], f32 result[3][4]);
#endif

#ifdef __cplusplus
/* The aliasing view below needs the cpp_extensions option. */
#pragma push
#pragma cpp_extensions on
/* Affine 3x4 transform. */
struct GSmtx {
    union {
        struct {
            f32 m00, m01, m02, m03;
            f32 m10, m11, m12, m13;
            f32 m20, m21, m22, m23;
        };
        f32 m[3][4];
    };

    GSmtx() {}

    GSmtx& operator=(const GSmtx &other) {
        m[0][0] = other.m[0][0];
        m[0][1] = other.m[0][1];
        m[0][2] = other.m[0][2];
        m[0][3] = other.m[0][3];
        m[1][0] = other.m[1][0];
        m[1][1] = other.m[1][1];
        m[1][2] = other.m[1][2];
        m[1][3] = other.m[1][3];
        m[2][0] = other.m[2][0];
        m[2][1] = other.m[2][1];
        m[2][2] = other.m[2][2];
        m[2][3] = other.m[2][3];
        return *this;
    }

    /* Returns a new matrix set to the affine identity. */
    GSmtx makeIdentity() {
        GSmtx m;
        PSMTXIdentity(m.sdk());
        return m;
    }

    GSmtx operator*(const GSmtx &other) const {
        GSmtx result;
        PSMTXConcat(sdk(), other.sdk(), result.sdk());
        return result;
    }

    f32 (*sdk())[4] { return reinterpret_cast<f32 (*)[4]>(m); }
    const f32 (*sdk() const)[4] { return reinterpret_cast<const f32 (*)[4]>(m); }
};
#pragma pop

/* The affine identity matrix, filled in by this module's static initializer
 * before main() runs. */
extern "C" GSmtx mtxIdent[1];

extern "C" {
void GSmtx44Perspective(f32 matrix[4][4], f32 fov, f32 aspect, f32 nearPlane, f32 farPlane);
void GSmtx44Ortho(f32 matrix[4][4], f32 top, f32 bottom, f32 left, f32 right, f32 nearPlane, f32 farPlane);
}
#endif

#endif
