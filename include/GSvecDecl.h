#ifndef GS_VEC_DECL_H
#define GS_VEC_DECL_H

#include <types.h>

/* A point or direction in three-dimensional space. */
struct GSvec {
    f32 x;
    f32 y;
    f32 z;

    GSvec();
    GSvec(f32 positionX, f32 positionY, f32 positionZ);
    void clear();
};

extern "C" inline void GSvecCopy(GSvec *destination, const GSvec *source);

#endif
