#ifndef GS_VEC_INLINE_H
#define GS_VEC_INLINE_H

#include <GSvecDecl.h>
#include <GSvecCopyInline.h>

inline GSvec::GSvec() {}

inline GSvec::GSvec(f32 positionX, f32 positionY, f32 positionZ)
    : x(positionX), y(positionY), z(positionZ) {}

inline void GSvec::clear()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

#endif
