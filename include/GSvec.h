#ifndef GSVEC_H
#define GSVEC_H

#include <types.h>

/* 3D point / direction / plane vector. The GS geometry library takes and
 * returns these by pointer everywhere. */
struct GSvec {
    f32 x;
    f32 y;
    f32 z;
    GSvec() {}
};

/* 3x4 matrix (48 bytes) as the GS matrix helpers take it. */
struct GSmtx {
    f32 m00, m01, m02, m03;
    f32 m10, m11, m12, m13;
    f32 m20, m21, m22, m23;
};

#endif
