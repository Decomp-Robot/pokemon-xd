#ifndef GSVEC_H
#define GSVEC_H

#include <types.h>
#include <GSmtx.h>

struct GSivec2 {
    s16 x;
    s16 y;
};

/* 3D point / direction / plane vector. The GS geometry library takes and
 * returns these by pointer everywhere. */
struct GSvec {
    f32 x;
    f32 y;
    f32 z;
#ifdef __cplusplus
    GSvec() {}
    GSvec(f32 positionX, f32 positionY, f32 positionZ)
        : x(positionX), y(positionY), z(positionZ) {}
#endif

};

#endif
