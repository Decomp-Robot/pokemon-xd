#include <GSmtx.h>

/* The GS math module's projection-matrix entry points. They add nothing to the
 * C_MTX builders they call, but keep the SDK matrix header out of the GS code
 * that only needs to build a projection. */

extern "C" void GSmtxMakePerspective(Mtx44 m, f32 fovY, f32 aspect, f32 n, f32 f) {
    C_MTXPerspective(m, fovY, aspect, n, f);
}

extern "C" void GSmtxMakeOrtho(Mtx44 m, f32 top, f32 bottom, f32 left, f32 right, f32 n,
                               f32 f) {
    C_MTXOrtho(m, top, bottom, left, right, n, f);
}
