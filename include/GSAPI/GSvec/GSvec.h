#ifndef INCLUDE_GSAPI_GSVEC_GSVEC_H
#define INCLUDE_GSAPI_GSVEC_GSVEC_H

#include <dolphin/mtx.h>
#include <GSgfx.h>
#include <GSmtx.h>

extern "C" {
void GSvecMakeFromQuat(GSvec* rotation, const f32* quaternion);
void GSvecTransformQuat(GSvec* result, const f32* quaternion, const GSvec* vector);
}

inline void PSVECAdd(const GSvec* a, const GSvec* b, GSvec* out) { ::PSVECAdd((const Vec*)a, (const Vec*)b, (Vec*)out); }
inline void PSVECSubtract(const GSvec* a, const GSvec* b, GSvec* out) { ::PSVECSubtract((const Vec*)a, (const Vec*)b, (Vec*)out); }
inline void PSVECScale(const GSvec* from, GSvec* to, f32 scale) { ::PSVECScale((const Vec*)from, (Vec*)to, scale); }
inline void PSMTXMultVec(const Mtx matrix, const GSvec* source, GSvec* result) { ::PSMTXMultVec(matrix, (const Vec*)source, (Vec*)result); }
inline void PSMTXMultVecSR(const Mtx matrix, const GSvec* source, GSvec* result) { ::PSMTXMultVecSR(matrix, (const Vec*)source, (Vec*)result); }


inline f32 PSVECDistance(const GSvec* a, const GSvec* b) { return ::PSVECDistance((const Vec*)a, (const Vec*)b); }

#endif
