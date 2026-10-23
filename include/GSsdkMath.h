#ifndef GS_SDK_MATH_H
#define GS_SDK_MATH_H

#include <GSmath.h>
#include <dolphinSdk.h>

namespace Dolphin {
extern "C" {
#include <dolphin/mtx.h>
}
}

// GS math storage matches the SDK vector and matrix layouts.
inline Dolphin::Vec* GSvecToVec(GSvec* vector)
{
    return reinterpret_cast<Dolphin::Vec*>(vector);
}

inline const Dolphin::Vec* GSvecToVec(const GSvec* vector)
{
    return reinterpret_cast<const Dolphin::Vec*>(vector);
}

inline Dolphin::Mtx& GSmtxToMtx(GSmtx& matrix)
{
    return reinterpret_cast<Dolphin::Mtx&>(matrix);
}

inline const Dolphin::Mtx& GSmtxToMtx(const GSmtx& matrix)
{
    return reinterpret_cast<const Dolphin::Mtx&>(matrix);
}

#endif
