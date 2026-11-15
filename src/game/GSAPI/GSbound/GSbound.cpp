#include <GSbound.h>
#include <GSmodel.h>
#include <GScamera.h>
#include <GSAPI/GSvec/GSvec.h>
#include <GSAPI/GSsysdolphin_baselib/baselib/quatlib.h>
#include <string.h>
#include <GSmem.h>
#include <GSprim.h>
#include <dolphin/gx/GXEnum.h>

struct GSboundBox {
    GSvec mMin;
    GSvec mMax;
};

struct GSboundAnimFrame {
    u16 mUnknown;
    u16 mNumFrames;
    GSboundBox mBounds[1];
};

struct GSboundAnimHeader {
    u16 mNumAnims;
    u16 mUnknown;
};

struct GSboundSRT {
    GSvec mPosition;
    Quaternion mRotation;
    GSvec mScale;
};

static inline void GSboundIncludePoint(GSbound *bound, f32 x, f32 y, f32 z)
{
    if (bound->mFlags & GS_BOUND_ANIMATED) return;
    if (bound->mFlags & GS_BOUND_EMPTY) {
        bound->u.box.mMin.x = x; bound->u.box.mMin.y = y; bound->u.box.mMin.z = z;
        bound->u.box.mMax.x = x; bound->u.box.mMax.y = y; bound->u.box.mMax.z = z;
        bound->mFlags &= ~GS_BOUND_EMPTY;
        return;
    }
    if (x < bound->u.box.mMin.x) bound->u.box.mMin.x = x;
    if (y < bound->u.box.mMin.y) bound->u.box.mMin.y = y;
    if (z < bound->u.box.mMin.z) bound->u.box.mMin.z = z;
    if (x > bound->u.box.mMax.x) bound->u.box.mMax.x = x;
    if (y > bound->u.box.mMax.y) bound->u.box.mMax.y = y;
    if (z > bound->u.box.mMax.z) bound->u.box.mMax.z = z;
}

extern "C" void GSboundGetShadowSize(GSvec *size, GSbound *bound, GSvec *from, GSvec *to, GSvec *direction)
{
    GSvec corners[4];
    Mtx m, scaleMtx;
    GSvec min, max;
    GSvec forward, side, v;
    Quaternion q;
    f32 maxSide, maxForward, sideDist, forwardDist;
    MtxPtr data;
    f32 scaleX, scaleY, scaleZ;
    int i;

    forward = GSvecNormalized(*direction);
    v = *to - *from;
    PSVECNormalize(&v, &v);
    side = GSvecCross(forward, v);
    PSVECNormalize(&side, &side);
    forward = GSvecCross(v, side);

    GSboundGetMinMax(bound, &min, &max, true, false, false, false);

    switch (bound->mDataType) {
    case GS_BOUND_DATA_MODEL:
        GSmodelGetRotation((_GSmodel *)bound->mData, &v);
        EulerToQuat(&v, (f32 *)&q);
        PSMTXQuat(m, &q);
        break;
    case GS_BOUND_DATA_MATRIX: {
        PSMTXCopy((MtxPtr)bound->mData, m);
        m[0][3] = 0.0f;
        m[1][3] = 0.0f;
        m[2][3] = 0.0f;
        data = (MtxPtr)bound->mData;
        scaleX = PSVECMag((GSvec *)data[0]);
        scaleY = PSVECMag((GSvec *)data[1]);
        scaleZ = PSVECMag((GSvec *)data[2]);
        PSMTXScale(scaleMtx, scaleX, scaleY, scaleZ);
        PSMTXInverse(scaleMtx, scaleMtx);
        PSMTXConcat(scaleMtx, m, m);
        break;
    }
    case GS_BOUND_DATA_SRT:
        PSMTXQuat(m, &((GSboundSRT *)bound->mData)->mRotation);
        break;
    default:
        PSMTXIdentity(m);
        break;
    }

    v = max - min;
    if (v.x < 0.0f) v.x = -v.x;
    if (v.y < 0.0f) v.y = -v.y;
    if (v.z < 0.0f) v.z = -v.z;

    corners[0] = v;
    corners[1] = v;
    corners[1].y *= -1.0f;
    corners[2] = v;
    corners[2].x *= -1.0f;
    corners[3] = v;
    corners[3].x *= -1.0f;
    corners[3].y *= -1.0f;

    maxSide = 0.0f;
    maxForward = 0.0f;
    for (i = 0; i < 4; i++) {
        PSMTXMultVec(m, &corners[i], &corners[i]);
        sideDist = GSvecAbsDotProduct(corners[i], side);
        forwardDist = GSvecAbsDotProduct(corners[i], forward);
        if (sideDist > maxSide) maxSide = sideDist;
        if (forwardDist > maxForward) maxForward = forwardDist;
    }
    size->x = maxSide;
    size->y = 0.0f;
    size->z = maxForward;
}

extern "C" void GSboundGetShadowVolume(GSbound *shadow, f32 height, GSbound *bound, GSvec *light)
{
    GSvec corners[8];
    Mtx m, rotMtx, srtRotMtx;
    GSvec bounds[2], v;
    GSvec center;
    Quaternion q;
    f32 lightHeight, distance;
    int i;

    memset(shadow, 0, sizeof(GSbound));
    shadow->mFlags |= GS_BOUND_EMPTY;
    GSboundGetMinMax(bound, &bounds[0], &bounds[1], false, false, false, false);

    corners[0].x = bounds[0].x; corners[0].y = bounds[0].y; corners[0].z = bounds[0].z;
    corners[1].x = bounds[0].x; corners[1].y = bounds[0].y; corners[1].z = bounds[1].z;
    corners[2].x = bounds[0].x; corners[2].y = bounds[1].y; corners[2].z = bounds[0].z;
    corners[3].x = bounds[0].x; corners[3].y = bounds[1].y; corners[3].z = bounds[1].z;
    corners[4].x = bounds[1].x; corners[4].y = bounds[0].y; corners[4].z = bounds[0].z;
    corners[5].x = bounds[1].x; corners[5].y = bounds[0].y; corners[5].z = bounds[1].z;
    corners[6].x = bounds[1].x; corners[6].y = bounds[1].y; corners[6].z = bounds[0].z;
    corners[7].x = bounds[1].x; corners[7].y = bounds[1].y; corners[7].z = bounds[1].z;

    center = (bounds[0] + bounds[1]) * 0.5f;

    switch (bound->mDataType) {
    case GS_BOUND_DATA_MODEL:
        GSmodelGetScale((_GSmodel *)bound->mData, &v);
        PSMTXScale(m, v.x, v.y, v.z);
        GSmodelGetRotation((_GSmodel *)bound->mData, &v);
        EulerToQuat(&v, (f32 *)&q);
        PSMTXQuat(rotMtx, &q);
        PSMTXConcat(rotMtx, m, m);
        GSmodelGetPosition((_GSmodel *)bound->mData, &v);
        PSMTXTransApply(m, m, v.x, v.y, v.z);
        break;
    case GS_BOUND_DATA_MATRIX:
        PSMTXCopy((MtxPtr)bound->mData, m);
        break;
    case GS_BOUND_DATA_SRT:
        PSMTXScale(m, ((GSboundSRT *)bound->mData)->mScale.x, ((GSboundSRT *)bound->mData)->mScale.y, ((GSboundSRT *)bound->mData)->mScale.z);
        PSMTXQuat(srtRotMtx, &((GSboundSRT *)bound->mData)->mRotation);
        PSMTXConcat(srtRotMtx, m, m);
        PSMTXTransApply(m, m, ((GSboundSRT *)bound->mData)->mPosition.x, ((GSboundSRT *)bound->mData)->mPosition.y, ((GSboundSRT *)bound->mData)->mPosition.z);
        break;
    default:
        PSMTXIdentity(m);
        break;
    }

    lightHeight = light->y - height;
    if (lightHeight < 1e-5f && lightHeight > -1e-5f) {
        return;
    }

    for (i = 0; i < 8; i++) {
        corners[i] -= center;
        PSMTXMultVec(m, &corners[i], &corners[i]);
        corners[i] += GSvecTransformSR(m, center);
        v = corners[i] - *light;
        distance = v.y - height;
        if (v.y < 1e-5f && v.y > -1e-5f) {
            continue;
        }
        corners[i] = *light - v * (lightHeight / distance);
        GSboundIncludePoint(shadow, corners[i].x, corners[i].y, corners[i].z);
    }
}

extern "C" void GSboundGetLocalPosition(GSbound *bound, GSvec *position)
{
    GSvec min, max;

    GSboundGetMinMax(bound, &min, &max, false, false, false, false);
    PSVECAdd(&min, &max, position);
    PSVECScale(position, position, 0.5f);
}

extern "C" void GSboundGetWorldPosition(GSbound *bound, GSvec *position)
{
    GSvec min, max;

    GSboundGetMinMax(bound, &min, &max, true, true, true, false);
    PSVECAdd(&min, &max, position);
    PSVECScale(position, position, 0.5f);
}

extern "C" void GSboundGetSize(GSbound *bound, GSvec *size, bool scale, bool rotate)
{
    Mtx m, scaleMtx;
    GSvec min, max, v;
    Quaternion q;
    GSvec orig;
    MtxPtr data;
    f32 scaleX, scaleY, scaleZ;
    int i;

    GSboundGetMinMax(bound, &min, &max, scale, false, false, false);
    *size = max - min;
    if (size->x < 0.0f) size->x = -size->x;
    if (size->y < 0.0f) size->y = -size->y;
    if (size->z < 0.0f) size->z = -size->z;
    if (!rotate) {
        return;
    }

    switch (bound->mDataType) {
    case GS_BOUND_DATA_MODEL:
        GSmodelGetRotation((_GSmodel *)bound->mData, &v);
        EulerToQuat(&v, (f32 *)&q);
        PSMTXQuat(m, &q);
        break;
    case GS_BOUND_DATA_MATRIX: {
        PSMTXCopy((MtxPtr)bound->mData, m);
        m[0][3] = 0.0f;
        m[1][3] = 0.0f;
        m[2][3] = 0.0f;
        data = (MtxPtr)bound->mData;
        scaleX = PSVECMag((GSvec *)data[0]);
        scaleY = PSVECMag((GSvec *)data[1]);
        scaleZ = PSVECMag((GSvec *)data[2]);
        PSMTXScale(scaleMtx, scaleX, scaleY, scaleZ);
        PSMTXInverse(scaleMtx, scaleMtx);
        PSMTXConcat(scaleMtx, m, m);
        break;
    }
    case GS_BOUND_DATA_SRT:
        PSMTXQuat(m, &((GSboundSRT *)bound->mData)->mRotation);
        break;
    default:
        return;
    }

    orig = *size;
    PSMTXMultVec(m, size, size);
    for (i = 0; i < 3; i++) {
        v = orig;
        ((f32 *)&v)[i] *= -1.0f;
        PSMTXMultVec(m, &v, &v);
        if (v.x < 0.0f) v.x = -v.x;
        if (v.y < 0.0f) v.y = -v.y;
        if (v.z < 0.0f) v.z = -v.z;
        if (v.x > size->x) size->x = v.x;
        if (v.y > size->y) size->y = v.y;
        if (v.z > size->z) size->z = v.z;
    }
}

extern "C" void GSboundGetMinMax(GSbound *bound, GSvec *min, GSvec *max, bool scale, bool rotate, bool translate, bool ignoreRoot)
{
    Mtx m, rootRotMtx, rotMtx, rotMinMtx, rotMaxMtx;
    GSvec v;
    f32 modelQuat[4];
    Quaternion q;

    if (bound->mFlags & GS_BOUND_ANIMATED) {
        GSboundAnimFrame *anim = bound->u.anim.mAnimData[bound->u.anim.mAnimIndex];
        s32 frame = (s32)bound->u.anim.mAnimFrame;
        min->x = anim->mBounds[frame].mMin.x;
        min->y = anim->mBounds[frame].mMin.y;
        min->z = anim->mBounds[frame].mMin.z;
        max->x = anim->mBounds[frame].mMax.x;
        max->y = anim->mBounds[frame].mMax.y;
        max->z = anim->mBounds[frame].mMax.z;
    } else {
        min->x = bound->u.box.mMin.x;
        min->y = bound->u.box.mMin.y;
        min->z = bound->u.box.mMin.z;
        max->x = bound->u.box.mMax.x;
        max->y = bound->u.box.mMax.y;
        max->z = bound->u.box.mMax.z;
    }

    switch (bound->mDataType) {
    case GS_BOUND_DATA_MODEL:
        if (ignoreRoot || !GSmodelIsRootNullAdded((_GSmodel *)bound->mData)) {
            PSMTXIdentity(m);
        } else {
            GSmodelGetRootScale((_GSmodel *)bound->mData, &v);
            PSMTXScale(m, v.x, v.y, v.z);
            GSmodelGetRootRotation((_GSmodel *)bound->mData, &v);
            EulerToQuat(&v, modelQuat);
            PSMTXQuat(rootRotMtx, (Quaternion *)modelQuat);
            PSMTXConcat(rootRotMtx, m, m);
            GSmodelGetRootPosition((_GSmodel *)bound->mData, &v);
            PSMTXTransApply(m, m, v.x, v.y, v.z);
        }
        if (scale) {
            GSmodelGetScale((_GSmodel *)bound->mData, &v);
            PSMTXScaleApply(m, m, v.x, v.y, v.z);
        }
        if (rotate) {
            GSmodelGetRotation((_GSmodel *)bound->mData, &v);
            EulerToQuat(&v, modelQuat);
            PSMTXQuat(rotMtx, (Quaternion *)modelQuat);
            PSMTXConcat(rotMtx, m, m);
        }
        if (translate) {
            GSmodelGetPosition((_GSmodel *)bound->mData, &v);
            PSMTXTransApply(m, m, v.x, v.y, v.z);
        }
        PSMTXMultVec(m, min, min);
        PSMTXMultVec(m, max, max);
        break;
    case GS_BOUND_DATA_MATRIX:
        if (scale && rotate && translate) {
            PSMTXMultVec((MtxPtr)bound->mData, min, min);
            PSMTXMultVec((MtxPtr)bound->mData, max, max);
        } else if (scale && rotate) {
            PSMTXMultVecSR((MtxPtr)bound->mData, min, min);
            PSMTXMultVecSR((MtxPtr)bound->mData, max, max);
        } else {
            if (scale) {
                MtxPtr data = (MtxPtr)bound->mData;
                f32 scaleX = PSVECMag((GSvec *)data[0]);
                f32 scaleY = PSVECMag((GSvec *)data[1]);
                f32 scaleZ = PSVECMag((GSvec *)data[2]);
                PSMTXScale(m, scaleX, scaleY, scaleZ);
            } else if (rotate) {
                C_QUATMtx(&q, (MtxPtr)bound->mData);
                PSMTXQuat(m, &q);
            }
            if (translate) {
                MtxPtr data = (MtxPtr)bound->mData;
                PSMTXTransApply(m, m, data[0][3], data[1][3], data[2][3]);
            }
            PSMTXMultVec(m, min, min);
            PSMTXMultVec(m, max, max);
        }
        break;
    case GS_BOUND_DATA_SRT:
        if (scale) {
            GSboundSRT *srt = (GSboundSRT *)bound->mData;
            min->x *= srt->mScale.x;
            min->y *= srt->mScale.y;
            min->z *= srt->mScale.z;
            srt = (GSboundSRT *)bound->mData;
            max->x *= srt->mScale.x;
            max->y *= srt->mScale.y;
            max->z *= srt->mScale.z;
        }
        if (rotate) {
            PSMTXQuat(rotMinMtx, &((GSboundSRT *)bound->mData)->mRotation);
            PSMTXMultVecSR(rotMinMtx, min, min);
            PSMTXQuat(rotMaxMtx, &((GSboundSRT *)bound->mData)->mRotation);
            PSMTXMultVecSR(rotMaxMtx, max, max);
        }
        if (translate) {
            PSVECAdd(min, &((GSboundSRT *)bound->mData)->mPosition, min);
            PSVECAdd(max, &((GSboundSRT *)bound->mData)->mPosition, max);
        }
        break;
    }
}

extern "C" void GSboundSetAnimFrame(GSbound *bound, f32 frame)
{
    if (bound->mFlags & GS_BOUND_ANIMATED) {
        u16 count = bound->u.anim.mAnimData[bound->u.anim.mAnimIndex]->mNumFrames;
        if (frame >= count) {
            frame = count - 1;
        }
        bound->u.anim.mAnimFrame = frame;
    }
}

extern "C" void GSboundSetAnimIndex(GSbound *bound, u32 index)
{
    if (bound->mFlags & GS_BOUND_ANIMATED) {
        bound->u.anim.mAnimIndex = index;
        bound->u.anim.mAnimFrame = 0.0f;
    }
}

extern "C" bool GSboundHasAnimData(GSbound *bound)
{
    return (bound->mFlags & GS_BOUND_ANIMATED) ? true : false;
}

enum {
    GS_BOUND_FACE_COLOR = 0x80808040,
    GS_BOUND_OUTLINE_COLOR = 0x80808080
};

static inline void GSboundDrawVertex(f32 x, f32 y, f32 z, u32 color)
{
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
    GXWGFifo.f32 = z;
    GXWGFifo.u32 = color;
}

void GSboundDraw(GSbound *bound, GSgfxLayer *layer)
{
    GSvec size, center, rotation;
    Quaternion q;
    GSvec max, min;
    Mtx m, scaleMtx, matrix, rotMtx, srtRotMtx;
    f32 side;
    f32 lo, hi;

    if (layer->mCurrentPass != GSGFX_LAYER_PASS_XLU) {
        return;
    }
    if (bound->mFlags & GS_BOUND_EMPTY) {
        return;
    }
    GSboundGetSize(bound, &size, true, false);
    if (PSVECSquareMag(&size) < 1e-5f) {
        return;
    }

    GScameraLoadActiveCameraMatrix(layer->mLayerID);
    layer->pixelMode(true, true, false);
    layer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);
    layer->cullMode(GSGFX_CULL_NONE);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_ONE, true);
    layer->clipMode(GSGFX_CLIP_ENABLED);
    GSprimLayerLineWidth(layer, 6);
    layer->setRenderState(true, 0, false);
    layer->mVertexFormat = GSgfxVFBuildTemp(GX_POS_XYZ, GX_F32, GX_FALSE, GX_NRM_XYZ, 1, GX_RGBA8, 0, 0, 0);

    PSMTXScale(m, size.x, size.y, size.z);
    switch (bound->mDataType) {
    case GS_BOUND_DATA_MODEL:
        GSmodelGetRotation((_GSmodel *)bound->mData, &rotation);
        EulerToQuat(&rotation, (f32 *)&q);
        PSMTXQuat(rotMtx, &q);
        PSMTXConcat(rotMtx, m, m);
        break;
    case GS_BOUND_DATA_MATRIX: {
        MtxPtr data;
        f32 scaleX, scaleY, scaleZ;
        PSMTXCopy((MtxPtr)bound->mData, matrix);
        matrix[0][3] = 0.0f;
        matrix[1][3] = 0.0f;
        matrix[2][3] = 0.0f;
        data = (MtxPtr)bound->mData;
        scaleX = PSVECMag((GSvec *)data[0]);
        scaleY = PSVECMag((GSvec *)data[1]);
        scaleZ = PSVECMag((GSvec *)data[2]);
        PSMTXScale(scaleMtx, scaleX, scaleY, scaleZ);
        PSMTXInverse(scaleMtx, scaleMtx);
        PSMTXConcat(scaleMtx, matrix, matrix);
        PSMTXConcat(matrix, m, m);
        break;
    }
    case GS_BOUND_DATA_SRT:
        PSMTXQuat(srtRotMtx, &((GSboundSRT *)bound->mData)->mRotation);
        PSMTXConcat(srtRotMtx, m, m);
        break;
    }

    GSboundGetMinMax(bound, &min, &max, true, true, true, false);
    PSVECAdd(&min, &max, &center);
    PSVECScale(&center, &center, 0.5f);
    PSMTXTransApply(m, m, center.x, center.y, center.z);
    layer->mMatrixStack->pushMult((GSmtx *)m);

    /* Translucent faces of the unit cube, two at a time per axis. */
    lo = -0.5f;
    hi = 0.5f;
    layer->mPrimitiveType = GSGFX_QUADS;
    layer->beginSurface(24);
    for (side = -0.5f; side < 1.0f; side += 1.0f) {
        GSboundDrawVertex(side, lo, lo, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(side, hi, lo, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(side, hi, hi, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(side, lo, hi, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(lo, side, lo, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(hi, side, lo, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(hi, side, hi, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(lo, side, hi, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(lo, lo, side, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(hi, lo, side, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(hi, hi, side, GS_BOUND_FACE_COLOR);
        GSboundDrawVertex(lo, hi, side, GS_BOUND_FACE_COLOR);
    }
    layer->endSurface();

    /* Outline edges. */
    layer->zCompare(true, GSGFX_Z_COMPARE_LEQUAL);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->mPrimitiveType = (GSgfxPrimType)GX_LINESTRIP;
    layer->beginSurface(5);
    GSboundDrawVertex(lo, lo, lo, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(lo, hi, lo, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(lo, hi, hi, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(lo, lo, hi, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(lo, lo, lo, GS_BOUND_OUTLINE_COLOR);
    layer->endSurface();
    layer->beginSurface(5);
    GSboundDrawVertex(hi, lo, lo, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(hi, hi, lo, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(hi, hi, hi, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(hi, lo, hi, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(hi, lo, lo, GS_BOUND_OUTLINE_COLOR);
    layer->endSurface();
    layer->mPrimitiveType = (GSgfxPrimType)GX_LINES;
    layer->beginSurface(8);
    GSboundDrawVertex(lo, lo, lo, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(hi, lo, lo, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(lo, hi, lo, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(hi, hi, lo, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(lo, hi, hi, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(hi, hi, hi, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(lo, lo, hi, GS_BOUND_OUTLINE_COLOR);
    GSboundDrawVertex(hi, lo, hi, GS_BOUND_OUTLINE_COLOR);
    layer->endSurface();
    layer->mMatrixStack->pop();
}

extern "C" void GSboundAlwaysInView(GSbound *bound, bool enabled)
{
    if (enabled) bound->mFlags |= GS_BOUND_ALWAYS_IN_VIEW;
    else bound->mFlags &= ~GS_BOUND_ALWAYS_IN_VIEW;
}

enum { GS_BOUND_PROJECTION_SUCCESS = 2 };

#define GS_BOUND_BEHIND_CAMERA_Z (-1.0f)
#define GS_BOUND_CLIP_LEFT (1 << 0)
#define GS_BOUND_CLIP_RIGHT (1 << 1)
#define GS_BOUND_CLIP_TOP (1 << 2)
#define GS_BOUND_CLIP_BOTTOM (1 << 3)
#define GS_BOUND_CLIP_DEPTH (1 << 4)
#define GS_BOUND_CLIP_ALL (GS_BOUND_CLIP_LEFT | GS_BOUND_CLIP_RIGHT | GS_BOUND_CLIP_TOP | GS_BOUND_CLIP_BOTTOM | GS_BOUND_CLIP_DEPTH)

static inline s32 GSboundClassifyProjectedBox(GSvec *screen)
{
    s32 result;
    int i;
    u8 outcode, anyOutside, allOutside;
    anyOutside = 0;
    allOutside = GS_BOUND_CLIP_ALL;
    i = 0;
    for (; i < 8; i++) {
        if (screen[i].x < 0.0f) {
            outcode = GS_BOUND_CLIP_LEFT;
        } else if (screen[i].x > 640.0f) {
            outcode = GS_BOUND_CLIP_RIGHT;
        } else {
            outcode = 0;
        }
        if (screen[i].y < 0.0f) {
            outcode |= GS_BOUND_CLIP_TOP;
        } else if (screen[i].y > 480.0f) {
            outcode |= GS_BOUND_CLIP_BOTTOM;
        }
        if (screen[i].z == GS_BOUND_BEHIND_CAMERA_Z) {
            outcode |= GS_BOUND_CLIP_DEPTH;
        }
        anyOutside |= outcode;
        allOutside &= outcode;
    }
    if (anyOutside == 0) {
        result = GS_BOUND_IN_VIEW;
    } else if (allOutside != 0) {
        result = GS_BOUND_OUT_OF_VIEW;
    } else {
        result = GS_BOUND_PARTLY_IN_VIEW;
    }
    return result;
}


extern "C" s32 GSboundIsInViewFast(GSbound *bound)
{
    GSvec corners[8];
    GSvec screen[8];
    GSvec bounds[2];

    if (bound->mFlags & GS_BOUND_ALWAYS_IN_VIEW) {
        return GS_BOUND_IN_VIEW;
    }
    GSboundGetMinMax(bound, &bounds[0], &bounds[1], true, true, true, false);
    corners[0].x = bounds[0].x; corners[0].y = bounds[0].y; corners[0].z = bounds[0].z;
    corners[1].x = bounds[0].x; corners[1].y = bounds[0].y; corners[1].z = bounds[1].z;
    corners[2].x = bounds[0].x; corners[2].y = bounds[1].y; corners[2].z = bounds[0].z;
    corners[3].x = bounds[0].x; corners[3].y = bounds[1].y; corners[3].z = bounds[1].z;
    corners[4].x = bounds[1].x; corners[4].y = bounds[0].y; corners[4].z = bounds[0].z;
    corners[5].x = bounds[1].x; corners[5].y = bounds[0].y; corners[5].z = bounds[1].z;
    corners[6].x = bounds[1].x; corners[6].y = bounds[1].y; corners[6].z = bounds[0].z;
    corners[7].x = bounds[1].x; corners[7].y = bounds[1].y; corners[7].z = bounds[1].z;
    if (GSgfxProjectFastList(corners, screen, 8) != GS_BOUND_PROJECTION_SUCCESS) {
        return GS_BOUND_IN_VIEW;
    }

    return GSboundClassifyProjectedBox(screen);
}

extern "C" void GSboundAddPoint(GSbound *bound, f32 x, f32 y, f32 z)
{
    GSboundIncludePoint(bound, x, y, z);
}

extern "C" void GSboundReset(GSbound *bound)
{
    if (bound->mFlags & GS_BOUND_ANIMATED) return;
    bound->mFlags |= GS_BOUND_EMPTY;
}

extern "C" void GSboundSetDataPtr(GSbound *bound, s32 type, void *data)
{
    if (data == 0) { bound->mDataType = GS_BOUND_DATA_NONE; return; }
    bound->mDataType = type;
    switch (type) {
    case GS_BOUND_DATA_MODEL:
    case GS_BOUND_DATA_MATRIX:
    case GS_BOUND_DATA_SRT:
        bound->mData = data;
        break;
    default:
        bound->mDataType = GS_BOUND_DATA_NONE;
        bound->mData = 0;
        break;
    }
}

extern "C" void GSboundFree(GSbound *bound)
{
    bound->mDataType = GS_BOUND_DATA_NONE;
    if (bound->mFlags & GS_BOUND_ANIMATED) {
        GSmemFree(bound->u.anim.mAnimData);
        bound->u.anim.mAnimData = 0;
    }
}

extern "C" void GSboundCreate(GSbound *bound, GSboundAnimHeader *header)
{
    s32 i;
    u8 *frame;
    memset(bound, 0, sizeof(GSbound));
    if (header == 0) {
        bound->mFlags |= GS_BOUND_EMPTY;
        return;
    }
    bound->u.anim.mAnimHeader = header;
    bound->u.anim.mAnimIndex = 0;
    bound->u.anim.mAnimFrame = 0.0f;
    bound->u.anim.mAnimData = (GSboundAnimFrame **)GSmemAlloc(bound->u.anim.mAnimHeader->mNumAnims * sizeof(GSboundAnimFrame *));
    frame = (u8 *)(bound->u.anim.mAnimHeader + 1);
    for (i = 0; i < bound->u.anim.mAnimHeader->mNumAnims; i++) {
        bound->u.anim.mAnimData[i] = (GSboundAnimFrame *)frame;
        frame += ((GSboundAnimFrame *)frame)->mNumFrames * sizeof(GSboundBox);
        frame += sizeof(u16) * 2;
    }
    bound->mFlags |= GS_BOUND_ANIMATED;
}
