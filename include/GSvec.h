#ifndef GSVEC_H
#define GSVEC_H

#include <types.h>
#include <GSmtx.h>

struct GSivec2 {
    s16 x;
    s16 y;
};

#ifdef __cplusplus
struct GSvec;
#else
typedef struct GSvec GSvec;
#endif

/* A three-component position or direction. */
struct GSvec {
    f32 x;
    f32 y;
    f32 z;
#ifdef __cplusplus
    GSvec() {}
    void clear();
    GSvec(const GSvec &other) : x(other.x), y(other.y), z(other.z) {}
    GSvec &operator=(const GSvec &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
    GSvec(f32 positionX, f32 positionY, f32 positionZ)
        : x(positionX), y(positionY), z(positionZ) {}

#endif

};

#ifdef __cplusplus
extern "C" {
#endif

void GSvecCopy(GSvec *out, const GSvec *value);

/* Euler angles of a rotation quaternion (x, y, z, w), in radians. */
void GSvecMakeFromQuat(GSvec *euler, const f32 *quat);
/* Rotate `value` by the quaternion (x, y, z, w). */
void GSvecTransformQuat(GSvec *out, const f32 *quat, const GSvec *value);

#ifdef __cplusplus
}
#endif

extern GSvec xAxisVector;
extern GSvec yAxisVector;
extern GSvec zAxisVector;
extern GSvec zeroVector;
extern GSvec unitScaleVector;

#endif
