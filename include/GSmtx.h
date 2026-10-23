#ifndef GSMTX_H
#define GSMTX_H

#include <types.h>

extern "C" void PSMTXConcat(const f32 first[3][4], const f32 second[3][4], f32 result[3][4]);

/* Affine 3x4 transform. */
struct GSmtx {
    f32 m[3][4];

    GSmtx operator*(const GSmtx &other) const {
        GSmtx result;
        PSMTXConcat(m, other.m, result.m);
        return result;
    }

    f32 (*sdk())[4] { return m; }
    const f32 (*sdk() const)[4] { return m; }
};

/* GS projection-matrix helpers. Both forward their arguments unchanged to the
 * matching C_MTX builder; they exist so GS code does not have to include the
 * SDK matrix header directly. */

extern "C" {
void GSmtxMakePerspective(f32 m[4][4], f32 fovY, f32 aspect, f32 n, f32 f);
void GSmtxMakeOrtho(f32 m[4][4], f32 top, f32 bottom, f32 left, f32 right, f32 n, f32 f);
}

#endif
