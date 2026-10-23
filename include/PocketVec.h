#ifndef POCKET_VEC_H
#define POCKET_VEC_H

// 2D position vector used by the pocket-menu position tables and by the
// direction helpers that measure the angle between two of them.
template <typename T>
struct PocketVec {
    T x;
    T y;
    PocketVec &operator=(PocketVec other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }
};

#endif
