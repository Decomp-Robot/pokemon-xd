#include <hsd/mtx.h>
#include <hsd/objalloc.h>
#include <hsd/assert.h>
#include <math.h>
#include <float.h>

#define HSD_MTX_EPSILON 1e-10f
#define HSD_HALF_PI (3.14159265358979323846f / 2.0f)

static HSD_ObjAllocData mtx_alloc_data;
static HSD_ObjAllocData vec_alloc_data;

void HSD_MtxInitAllocData(void)
{
    HSD_ObjAllocInit(&mtx_alloc_data, sizeof(Mtx), 4);
}

HSD_ObjAllocData *HSD_MtxGetAllocData(void)
{
    return &mtx_alloc_data;
}

void HSD_VecInitAllocData(void)
{
    HSD_ObjAllocInit(&vec_alloc_data, sizeof(Vec), 4);
}

HSD_ObjAllocData *HSD_VecGetAllocData(void)
{
    return &vec_alloc_data;
}

void HSD_MtxFree(Mtx mtx)
{
    if (mtx != NULL) {
        HSD_ObjFree(&mtx_alloc_data, mtx);
    }
}

Mtx *HSD_MtxAlloc(void)
{
    Mtx *mtx = HSD_ObjAlloc(&mtx_alloc_data);

    HSD_ASSERTLINE(939, mtx);
    return mtx;
}

void HSD_VecFree(Vec *vec)
{
    if (vec != NULL) {
        HSD_ObjFree(&vec_alloc_data, vec);
    }
}

Vec *HSD_VecAlloc(void)
{
    Vec *vec = HSD_ObjAlloc(&vec_alloc_data);

    HSD_ASSERTLINE(908, vec);
    return vec;
}

static inline f32 HSD_SafeAsin(f32 x)
{
    if (x >= 1.0f) {
        return HSD_HALF_PI;
    } else if (x <= -1.0f) {
        return -HSD_HALF_PI;
    } else {
        return asin(x);
    }
}

static inline f32 HSD_SafeAtan2(f32 y, f32 x)
{
    if (x == 0.0f) {
        if (y >= 0.0f) {
            return HSD_HALF_PI;
        } else {
            return -HSD_HALF_PI;
        }
    }
    return atan2(y, x);
}

static inline f32 reciprocalScale(f32 scale)
{
    if (scale >= 0.0f) {
        return 1.0f / (scale + __float_epsilon[0]);
    } else {
        return 1.0f / (scale - __float_epsilon[0]);
    }
}

void HSD_VecGetOrthogonal(Vec* v, Vec* orthogonal)
{
    Vec axis;
    if (v->x <= v->y) {
        if (v->x <= v->z) {
            axis.x = 1.0f; axis.y = 0.0f; axis.z = 0.0f;
            PSVECCrossProduct(&axis, v, orthogonal);
        } else {
            axis.x = 0.0f; axis.y = 0.0f; axis.z = 1.0f;
            PSVECCrossProduct(&axis, v, orthogonal);
        }
    } else {
        if (v->y <= v->z) {
            axis.x = 0.0f; axis.y = 1.0f; axis.z = 0.0f;
            PSVECCrossProduct(&axis, v, orthogonal);
        } else {
            axis.x = 0.0f; axis.y = 0.0f; axis.z = 1.0f;
            PSVECCrossProduct(&axis, v, orthogonal);
        }
    }
}

void HSD_MtxScaledAdd(Mtx a, Mtx b, Mtx result, f32 scale)
{
    result[0][0] = scale * a[0][0] + b[0][0];
    result[0][1] = scale * a[0][1] + b[0][1];
    result[0][2] = scale * a[0][2] + b[0][2];
    result[0][3] = scale * a[0][3] + b[0][3];
    result[1][0] = scale * a[1][0] + b[1][0];
    result[1][1] = scale * a[1][1] + b[1][1];
    result[1][2] = scale * a[1][2] + b[1][2];
    result[1][3] = scale * a[1][3] + b[1][3];
    result[2][0] = scale * a[2][0] + b[2][0];
    result[2][1] = scale * a[2][1] + b[2][1];
    result[2][2] = scale * a[2][2] + b[2][2];
    result[2][3] = scale * a[2][3] + b[2][3];
}

void HSD_MtxSRTQuat(Mtx m, Vec* scale, Quaternion* rotation,
                    Vec* translation, Vec* parent_scale)
{
    Mtx rotate;
    PSMTXScale(m, scale->x, scale->y, scale->z);
    PSMTXQuat(rotate, rotation);
    if (parent_scale) {
        f32 ix = reciprocalScale(parent_scale->x);
        f32 iy = reciprocalScale(parent_scale->y);
        f32 iz = reciprocalScale(parent_scale->z);
        rotate[0][1] *= parent_scale->y * ix;
        rotate[0][2] *= parent_scale->y * ix;
        rotate[1][0] *= parent_scale->x * iy;
        rotate[1][2] *= parent_scale->z * iy;
        rotate[2][0] *= parent_scale->x * iz;
        rotate[2][1] *= parent_scale->y * iz;
    }
    PSMTXConcat(rotate, m, m);
    m[0][3] = translation->x;
    m[1][3] = translation->y;
    m[2][3] = translation->z;
}

void HSD_MtxSRT(Mtx m, Vec* scale, Vec* rotation,
                Vec* translation, Vec* parent_scale)
{
    f32 sx, cx, sy, cy, sz, cz;
    f32 py, px, pz;
    f32 ix, iy, iz;
    f32 xx, xy, xz, yx, yy, yz, zx, zy, zz;
    sx = sin(rotation->x);
    cx = cos(rotation->x);
    sy = sin(rotation->y);
    cy = cos(rotation->y);
    sz = sin(rotation->z);
    cz = cos(rotation->z);
    xx = yx = zx = scale->x;
    xy = yy = zy = scale->y;
    xz = yz = zz = scale->z;
    if (parent_scale) {
        px = parent_scale->x;
        ix = reciprocalScale(px);
        py = parent_scale->y;
        iy = reciprocalScale(py);
        pz = parent_scale->z;
        iz = reciprocalScale(pz);
        xy *= py * ix;
        xz *= pz * ix;
        yx *= px * iy;
        yz *= pz * iy;
        zx *= px * iz;
        zy *= py * iz;
    }
    m[0][0] = cz * (xx * cy);
    m[1][0] = sz * (yx * cy);
    m[2][0] = -zx * sy;
    m[0][1] = xy * (cz * (sx * sy) - cx * sz);
    m[1][1] = yy * (sz * (sx * sy) + cx * cz);
    m[2][1] = cy * (zy * sx);
    m[0][2] = xz * (cz * (cx * sy) + sx * sz);
    m[1][2] = yz * (sz * (cx * sy) - sx * cz);
    m[2][2] = cy * (zz * cx);
    m[0][3] = translation->x;
    m[1][3] = translation->y;
    m[2][3] = translation->z;
}

void HSD_MkRotationMtx(Mtx m, Vec* rotation)
{
    f32 sx, cx, sy, cy, sz, cz;
    f32 sxsy, cxsy;
    sx = sin(rotation->x);
    cx = cos(rotation->x);
    sy = sin(rotation->y);
    cy = cos(rotation->y);
    sz = sin(rotation->z);
    cz = cos(rotation->z);
    sxsy = sx * sy;
    cxsy = cx * sy;
    m[0][0] = cy * cz;
    m[1][0] = cy * sz;
    m[2][0] = -sy;
    m[0][1] = cz * sxsy - cx * sz;
    m[1][1] = sz * sxsy + cx * cz;
    m[2][1] = sx * cy;
    m[0][2] = cz * cxsy + sx * sz;
    m[1][2] = sz * cxsy - sx * cz;
    m[2][2] = cx * cy;
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
}

static void correctScaleSign(Vec* scale, f32 orientation)
{
    if (orientation < 0.0) {
        scale->x *= -1.0;
        scale->y *= -1.0;
        scale->z *= -1.0;
    }
}

void HSD_MtxGetScale(Mtx m, Vec* scale)
{
    Vec axis_x;
    Vec axis_y;
    Vec axis_z;
    Vec tmp;
    f32 squared_length;
    f32 inverse_length;

    axis_x.x = m[0][0];
    axis_x.y = m[1][0];
    axis_x.z = m[2][0];
    squared_length = PSVECSquareMag(&axis_x);
    if (squared_length > HSD_MTX_EPSILON) {
        inverse_length = sqrtf(1.0f / squared_length);
        scale->x = 1.0f / inverse_length;
        PSVECScale(&axis_x, &axis_x, inverse_length);

        axis_y.x = m[0][1];
        axis_y.y = m[1][1];
        axis_y.z = m[2][1];
        PSVECScale(&axis_x, &tmp, PSVECDotProduct(&axis_x, &axis_y));
        PSVECSubtract(&axis_y, &tmp, &axis_y);
        squared_length = PSVECSquareMag(&axis_y);
        if (squared_length > HSD_MTX_EPSILON) {
            inverse_length = sqrtf(1.0f / squared_length);
            scale->y = 1.0f / inverse_length;
            PSVECScale(&axis_y, &axis_y, inverse_length);

            axis_z.x = m[0][2];
            axis_z.y = m[1][2];
            axis_z.z = m[2][2];
            PSVECScale(&axis_y, &tmp, PSVECDotProduct(&axis_y, &axis_z));
            PSVECSubtract(&axis_z, &tmp, &axis_z);
            PSVECScale(&axis_x, &tmp, PSVECDotProduct(&axis_x, &axis_z));
            PSVECSubtract(&axis_z, &tmp, &axis_z);
            squared_length = PSVECSquareMag(&axis_z);
            if (squared_length > HSD_MTX_EPSILON) {
                scale->z = sqrtf(squared_length);
                PSVECCrossProduct(&axis_y, &axis_z, &tmp);
                correctScaleSign(scale, PSVECDotProduct(&axis_x, &tmp));
            } else {
                scale->z = 0.0f;
            }
        } else {
            scale->y = 0.0f;

            axis_z.x = m[0][2];
            axis_z.y = m[1][2];
            axis_z.z = m[2][2];
            PSVECScale(&axis_x, &tmp, PSVECDotProduct(&axis_x, &axis_z));
            PSVECSubtract(&axis_z, &tmp, &axis_z);
            squared_length = PSVECSquareMag(&axis_z);
            if (squared_length > HSD_MTX_EPSILON) {
                scale->z = sqrtf(squared_length);
            } else {
                scale->z = 0.0f;
            }
        }
    } else {
        scale->x = 0.0f;

        axis_y.x = m[0][1];
        axis_y.y = m[1][1];
        axis_y.z = m[2][1];
        squared_length = PSVECSquareMag(&axis_y);
        if (squared_length > HSD_MTX_EPSILON) {
            inverse_length = sqrtf(1.0f / squared_length);
            scale->y = 1.0f / inverse_length;
            PSVECScale(&axis_y, &axis_y, inverse_length);

            axis_z.x = m[0][2];
            axis_z.y = m[1][2];
            axis_z.z = m[2][2];
            PSVECScale(&axis_y, &tmp, PSVECDotProduct(&axis_y, &axis_z));
            PSVECSubtract(&axis_z, &tmp, &axis_z);
            scale->z = PSVECMag(&axis_z);
        } else {
            scale->y = 0.0f;

            axis_z.x = m[0][2];
            axis_z.y = m[1][2];
            axis_z.z = m[2][2];
            scale->z = PSVECMag(&axis_z);
        }
    }
}

void HSD_MtxGetTranslate(Mtx m, Vec* translation)
{
    translation->x = m[0][3];
    translation->y = m[1][3];
    translation->z = m[2][3];
}

void HSD_MtxGetRotationMtx(Mtx src, Mtx dst, char primary, char secondary)
{
    Vec axis_x;
    Vec axis_y;
    Vec axis_z;

    switch (primary) {
    case 'X':
    case 'x':
        axis_x.x = src[0][0];
        axis_x.y = src[1][0];
        axis_x.z = src[2][0];
        PSVECNormalize(&axis_x, &axis_x);
        switch (secondary) {
        case 'Z':
        case 'z':
            axis_z.x = src[0][2];
            axis_z.y = src[1][2];
            axis_z.z = src[2][2];
            PSVECNormalize(&axis_z, &axis_z);
            PSVECCrossProduct(&axis_z, &axis_x, &axis_y);
            PSVECCrossProduct(&axis_x, &axis_y, &axis_z);
            break;
        default:
            axis_y.x = src[0][1];
            axis_y.y = src[1][1];
            axis_y.z = src[2][1];
            PSVECNormalize(&axis_y, &axis_y);
            PSVECCrossProduct(&axis_x, &axis_y, &axis_z);
            PSVECCrossProduct(&axis_z, &axis_x, &axis_y);
            break;
        }
        break;
    case 'Y':
    case 'y':
        axis_y.x = src[0][1];
        axis_y.y = src[1][1];
        axis_y.z = src[2][1];
        PSVECNormalize(&axis_y, &axis_y);
        switch (secondary) {
        case 'X':
        case 'x':
            axis_x.x = src[0][0];
            axis_x.y = src[1][0];
            axis_x.z = src[2][0];
            PSVECNormalize(&axis_x, &axis_x);
            PSVECCrossProduct(&axis_x, &axis_y, &axis_z);
            PSVECCrossProduct(&axis_y, &axis_z, &axis_x);
            break;
        default:
            axis_z.x = src[0][2];
            axis_z.y = src[1][2];
            axis_z.z = src[2][2];
            PSVECNormalize(&axis_z, &axis_z);
            PSVECCrossProduct(&axis_y, &axis_z, &axis_x);
            PSVECCrossProduct(&axis_x, &axis_y, &axis_z);
            break;
        }
        break;
    default:
        axis_z.x = src[0][2];
        axis_z.y = src[1][2];
        axis_z.z = src[2][2];
        PSVECNormalize(&axis_z, &axis_z);
        switch (secondary) {
        case 'Y':
        case 'y':
            axis_y.x = src[0][1];
            axis_y.y = src[1][1];
            axis_y.z = src[2][1];
            PSVECNormalize(&axis_y, &axis_y);
            PSVECCrossProduct(&axis_y, &axis_z, &axis_x);
            PSVECCrossProduct(&axis_z, &axis_x, &axis_y);
            break;
        default:
            axis_x.x = src[0][0];
            axis_x.y = src[1][0];
            axis_x.z = src[2][0];
            PSVECNormalize(&axis_x, &axis_x);
            PSVECCrossProduct(&axis_z, &axis_x, &axis_y);
            PSVECCrossProduct(&axis_y, &axis_z, &axis_x);
            break;
        }
        break;
    }

    dst[0][0] = axis_x.x;
    dst[1][0] = axis_x.y;
    dst[2][0] = axis_x.z;
    dst[0][1] = axis_y.x;
    dst[1][1] = axis_y.y;
    dst[2][1] = axis_y.z;
    dst[0][2] = axis_z.x;
    dst[1][2] = axis_z.y;
    dst[2][2] = axis_z.z;
    dst[0][3] = 0.0f;
    dst[1][3] = 0.0f;
    dst[2][3] = 0.0f;
}

void HSD_MtxGetRotation(Mtx m, Vec* rotate)
{
    f32 scale_x;
    f32 scale_y;
    f32 scale_z;

    scale_x = sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
    if (scale_x < FLT_MIN) {
        goto degenerate;
    }
    scale_y = sqrtf(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
    if (scale_y < FLT_MIN) {
        goto degenerate;
    }
    scale_z = sqrtf(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);
    if (scale_z < FLT_MIN) {
        goto degenerate;
    }

    rotate->y = HSD_SafeAsin(-m[2][0] / scale_x);
    if ((f32)cos(rotate->y) >= FLT_MIN) {
        rotate->x = HSD_SafeAtan2(m[2][1] / scale_y, m[2][2] / scale_z);
        rotate->z = HSD_SafeAtan2(m[1][0], m[0][0]);
    } else {
        rotate->x = HSD_SafeAtan2(m[0][1], m[1][1]);
        rotate->z = 0.0f;
    }
    return;

degenerate:
    rotate->x = 0.0f;
    rotate->y = 0.0f;
    rotate->z = 0.0f;
}

int HSD_MtxInverseConcat(Mtx left, Mtx right, Mtx result)
{
    Mtx temporary;
    f32 determinant;
    f32 reciprocal;
    f32 inverse00, inverse01, inverse02;
    f32 inverse10, inverse11, inverse12;
    f32 inverse20, inverse21, inverse22;
    f32 inverse03, inverse13, inverse23;

    determinant = left[0][0] * left[1][1] * left[2][2]
                + left[0][1] * left[1][2] * left[2][0]
                + left[0][2] * left[1][0] * left[2][1]
                - left[2][0] * left[1][1] * left[0][2]
                - left[1][0] * left[0][1] * left[2][2]
                - left[0][0] * left[2][1] * left[1][2];
    if (determinant == 0.0f) {
        return 0;
    }
    reciprocal = 1.0f / determinant;
    inverse00 = (left[1][1] * left[2][2] - left[2][1] * left[1][2]) * reciprocal;
    inverse01 = -(left[0][1] * left[2][2] - left[2][1] * left[0][2]) * reciprocal;
    inverse02 = (left[0][1] * left[1][2] - left[1][1] * left[0][2]) * reciprocal;
    inverse10 = -(left[1][0] * left[2][2] - left[2][0] * left[1][2]) * reciprocal;
    inverse11 = (left[0][0] * left[2][2] - left[2][0] * left[0][2]) * reciprocal;
    inverse12 = -(left[0][0] * left[1][2] - left[1][0] * left[0][2]) * reciprocal;
    inverse20 = (left[1][0] * left[2][1] - left[2][0] * left[1][1]) * reciprocal;
    inverse21 = -(left[0][0] * left[2][1] - left[2][0] * left[0][1]) * reciprocal;
    inverse22 = (left[0][0] * left[1][1] - left[1][0] * left[0][1]) * reciprocal;
    inverse03 = -inverse00 * left[0][3] - inverse01 * left[1][3] - inverse02 * left[2][3];
    inverse13 = -inverse10 * left[0][3] - inverse11 * left[1][3] - inverse12 * left[2][3];
    inverse23 = -inverse20 * left[0][3] - inverse21 * left[1][3] - inverse22 * left[2][3];
    if (left == result || right == result) {
        temporary[0][0] = inverse00 * right[0][0] + inverse01 * right[1][0] + inverse02 * right[2][0];
        temporary[0][1] = inverse00 * right[0][1] + inverse01 * right[1][1] + inverse02 * right[2][1];
        temporary[0][2] = inverse00 * right[0][2] + inverse01 * right[1][2] + inverse02 * right[2][2];
        temporary[0][3] = inverse03 + (inverse00 * right[0][3] + inverse01 * right[1][3] + inverse02 * right[2][3]);
        temporary[1][0] = inverse10 * right[0][0] + inverse11 * right[1][0] + inverse12 * right[2][0];
        temporary[1][1] = inverse10 * right[0][1] + inverse11 * right[1][1] + inverse12 * right[2][1];
        temporary[1][2] = inverse10 * right[0][2] + inverse11 * right[1][2] + inverse12 * right[2][2];
        temporary[1][3] = inverse13 + (inverse10 * right[0][3] + inverse11 * right[1][3] + inverse12 * right[2][3]);
        temporary[2][0] = inverse20 * right[0][0] + inverse21 * right[1][0] + inverse22 * right[2][0];
        temporary[2][1] = inverse20 * right[0][1] + inverse21 * right[1][1] + inverse22 * right[2][1];
        temporary[2][2] = inverse20 * right[0][2] + inverse21 * right[1][2] + inverse22 * right[2][2];
        temporary[2][3] = inverse23 + (inverse20 * right[0][3] + inverse21 * right[1][3] + inverse22 * right[2][3]);
        PSMTXCopy(temporary, result);
    } else {
        result[0][0] = inverse00 * right[0][0] + inverse01 * right[1][0] + inverse02 * right[2][0];
        result[0][1] = inverse00 * right[0][1] + inverse01 * right[1][1] + inverse02 * right[2][1];
        result[0][2] = inverse00 * right[0][2] + inverse01 * right[1][2] + inverse02 * right[2][2];
        result[0][3] = inverse03 + (inverse00 * right[0][3] + inverse01 * right[1][3] + inverse02 * right[2][3]);
        result[1][0] = inverse10 * right[0][0] + inverse11 * right[1][0] + inverse12 * right[2][0];
        result[1][1] = inverse10 * right[0][1] + inverse11 * right[1][1] + inverse12 * right[2][1];
        result[1][2] = inverse10 * right[0][2] + inverse11 * right[1][2] + inverse12 * right[2][2];
        result[1][3] = inverse13 + (inverse10 * right[0][3] + inverse11 * right[1][3] + inverse12 * right[2][3]);
        result[2][0] = inverse20 * right[0][0] + inverse21 * right[1][0] + inverse22 * right[2][0];
        result[2][1] = inverse20 * right[0][1] + inverse21 * right[1][1] + inverse22 * right[2][1];
        result[2][2] = inverse20 * right[0][2] + inverse21 * right[1][2] + inverse22 * right[2][2];
        result[2][3] = inverse23 + (inverse20 * right[0][3] + inverse21 * right[1][3] + inverse22 * right[2][3]);
    }
    return 1;
}
