#ifndef PE_NODE_MATH_H
#define PE_NODE_MATH_H

#include <dolphin/mtx.h>
#include <peNode.h>

// GS and Dolphin math objects have the same component layout.
static inline const Vec *SDKVector(const GSvec *value) {
    return reinterpret_cast<const Vec *>(value);
}
static inline Vec *SDKVector(GSvec *value) {
    return reinterpret_cast<Vec *>(value);
}
static inline const Quaternion *SDKQuaternion(const f32 *value) {
    return reinterpret_cast<const Quaternion *>(value);
}
static inline Quaternion *SDKQuaternion(f32 *value) {
    return reinterpret_cast<Quaternion *>(value);
}

static inline GSvec operator+(const GSvec &a, const GSvec &b) {
    GSvec result;
    PSVECAdd(SDKVector(&a), SDKVector(&b), SDKVector(&result));
    return result;
}
static inline GSvec operator-(const GSvec &a, const GSvec &b) {
    GSvec result;
    PSVECSubtract(SDKVector(&a), SDKVector(&b), SDKVector(&result));
    return result;
}
static inline GSvec operator*(const GSvec &a, f32 value) {
    GSvec result;
    PSVECScale(SDKVector(&a), SDKVector(&result), value);
    return result;
}
static inline GSvec &operator+=(GSvec &a, const GSvec &b) {
    PSVECAdd(SDKVector(&a), SDKVector(&b), SDKVector(&a));
    return a;
}
static inline GSvec &operator-=(GSvec &a, const GSvec &b) {
    PSVECSubtract(SDKVector(&a), SDKVector(&b), SDKVector(&a));
    return a;
}
static inline GSvec &operator*=(GSvec &a, f32 value) {
    PSVECScale(SDKVector(&a), SDKVector(&a), value);
    return a;
}
static inline GSvec operator-(const GSvec &a) {
    GSvec result(-a.x, -a.y, -a.z);
    return result;
}
static inline GSvec Cross(const GSvec &a, const GSvec &b) {
    GSvec result;
    PSVECCrossProduct(SDKVector(&a), SDKVector(&b), SDKVector(&result));
    return result;
}
struct peQuaternion {
    f32 x, y, z, w;
    peQuaternion() {}
    peQuaternion(const peQuaternion &other) {
        x = other.x; y = other.y; z = other.z; w = other.w;
    }
    peQuaternion &operator=(const peQuaternion &other) {
        x = other.x; y = other.y; z = other.z; w = other.w;
        return *this;
    }
};
static inline peQuaternion QuaternionInverse(const f32 *rotation) {
    peQuaternion result;
    PSQUATInverse(SDKQuaternion(rotation), SDKQuaternion(&result.x));
    return result;
}
static inline peQuaternion Inverse(const f32 *rotation) {
    return QuaternionInverse(rotation);
}
static inline peQuaternion Multiply(const f32 *a, const f32 *b) {
    peQuaternion result;
    PSQUATMultiply(SDKQuaternion(a), SDKQuaternion(b), SDKQuaternion(&result.x));
    return result;
}
static inline void Rotate(GSvec &value, const f32 *rotation) {
    f32 matrix[3][4];
    PSMTXQuat(matrix, SDKQuaternion(rotation));
    PSMTXMultVecSR(matrix, SDKVector(&value), SDKVector(&value));
}
static inline GSvec Rotated(const GSvec &value, const f32 *rotation) {
    f32 matrix[3][4];
    GSvec result;
    PSMTXQuat(matrix, SDKQuaternion(rotation));
    PSMTXMultVecSR(matrix, SDKVector(&value), SDKVector(&result));
    return result;
}
static inline void ApplyScale(GSvec &value, const GSvec &scale) {
    value.x *= scale.x;
    value.y *= scale.y;
    value.z *= scale.z;
}
static inline void RemoveScale(GSvec &value, const GSvec &scale) {
    if (0.0f != scale.x) value.x /= scale.x;
    if (0.0f != scale.y) value.y /= scale.y;
    if (0.0f != scale.z) value.z /= scale.z;
}
static inline GSvec Product(const GSvec &a, const GSvec &b) {
    GSvec result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}
static inline f32 Dot(const GSvec &a, const GSvec &b) { return PSVECDotProduct(SDKVector(&a), SDKVector(&b)); }
static inline f32 Magnitude(f32 value) { return value < 0.0f ? -value : value; }

#endif
