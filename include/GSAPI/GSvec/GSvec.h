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

inline f32 PSVECMag(const GSvec *v) { return ::PSVECMag((const Vec *)v); }
inline f32 PSVECSquareMag(const GSvec *v) { return ::PSVECSquareMag((const Vec *)v); }
inline void PSVECNormalize(const GSvec *src, GSvec *dst) { ::PSVECNormalize((const Vec *)src, (Vec *)dst); }
inline f32 PSVECDotProduct(const GSvec *a, const GSvec *b) { return ::PSVECDotProduct((const Vec *)a, (const Vec *)b); }
inline void PSVECCrossProduct(const GSvec *a, const GSvec *b, GSvec *axb) { ::PSVECCrossProduct((const Vec *)a, (const Vec *)b, (Vec *)axb); }

static inline GSvec &operator+=(GSvec &value, const GSvec &other) {
    PSVECAdd(&value, &other, &value);
    return value;
}
static inline GSvec &operator-=(GSvec &value, const GSvec &other) {
    PSVECSubtract(&value, &other, &value);
    return value;
}
static inline GSvec operator+(const GSvec &a, const GSvec &b) {
    GSvec result;
    PSVECAdd(&a, &b, &result);
    return result;
}
static inline GSvec operator-(const GSvec &a, const GSvec &b) {
    GSvec result;
    PSVECSubtract(&a, &b, &result);
    return result;
}
static inline GSvec operator*(const GSvec &v, f32 scale) {
    GSvec result;
    PSVECScale(&v, &result, scale);
    return result;
}
static inline f32 GSvecAbsDotProduct(const GSvec &a, const GSvec &b) {
    f32 result;
    if (PSVECDotProduct(&a, &b) > 0.0f) {
        result = PSVECDotProduct(&a, &b);
    } else {
        result = -PSVECDotProduct(&a, &b);
    }
    return result;
}

static inline GSvec GSvecNormalized(const GSvec &v) {
    GSvec result;
    PSVECNormalize(&v, &result);
    return result;
}
static inline GSvec GSvecCross(const GSvec &a, const GSvec &b) {
    GSvec result;
    PSVECCrossProduct(&a, &b, &result);
    return result;
}
static inline GSvec GSvecTransformSR(const Mtx matrix, const GSvec &v) {
    GSvec result;
    PSMTXMultVecSR(matrix, &v, &result);
    return result;
}

#endif
