#ifndef GSQUAT_H
#define GSQUAT_H

#include <types.h>

/* Unit quaternion of the GS math module: the rotation a transform falls back to
 * while nothing has rotated it. */
class GSquat {
public:
    f32 x; // 0x00
    f32 y; // 0x04
    f32 z; // 0x08
    f32 w; // 0x0C

    GSquat() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
};

extern "C" GSquat zeroQuaternion;

#endif
