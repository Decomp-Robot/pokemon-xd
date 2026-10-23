#ifndef GS_VEC_COPY_INLINE_H
#define GS_VEC_COPY_INLINE_H

#include <GSvecDecl.h>

extern "C" inline void GSvecCopy(GSvec *destination, const GSvec *source)
{
    destination->x = source->x;
    destination->y = source->y;
    destination->z = source->z;
}

#endif
