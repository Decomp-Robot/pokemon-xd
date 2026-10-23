#ifndef HSD_JOBJ_H
#define HSD_JOBJ_H

#include <types.h>
#include <dolphin/mtx.h>
#include <hsd/assert.h>

/* A scene joint whose rotation may hold Euler angles or a quaternion. */
typedef struct _HSD_AObj _HSD_AObj;
typedef struct _HSD_DObj _HSD_DObj;
typedef struct _HSD_MatAnimJoint _HSD_MatAnimJoint;
typedef struct _HSD_ShapeAnimJoint _HSD_ShapeAnimJoint;

typedef struct _HSD_JObj _HSD_JObj;
typedef struct _HSD_Joint _HSD_Joint;

typedef void (*HSD_JObjParticleCallback)(int type, int flags, int group,
                                         _HSD_JObj *jobj);
typedef void (*HSD_JObjParticleTargetCallback)(_HSD_JObj *jobj, int id);

struct _HSD_JObj {
    u8 _00[0x08];
    _HSD_JObj *next;     // 0x08: next sibling in the joint list.
    _HSD_JObj *mParent;  // 0x0C: parent joint.
    _HSD_JObj *child;    // 0x10: first child joint.
    u32 flags;     // 0x14: transform flags (see the HSD_JOBJ_FLAG_* below).
    _HSD_DObj *mDrawable; // 0x18: drawable chain for ordinary joints.
    f32 mRotate[4]; // 0x1C: Euler angles, later a quaternion.
    f32 mScale[3];  // 0x2C: joint scale.
    f32 mTrans[3];  // 0x38: joint translation.
    Mtx mMatrix; // 0x44
    u8 _74[8];
    _HSD_AObj *mAObj;    // 0x7C: animation object driving the joint.
};

#define HSD_JOBJ_FLAG_USER_DEFINED_MTX 0x00800000
#define HSD_JOBJ_FLAG_MTX_DIRTY 0x00000040
#define JOBJ_USE_QUATERNION 0x00020000
#define HSD_JOBJ_STATE_EULER_QUAT JOBJ_USE_QUATERNION
#define HSD_JOBJ_FLAG_EULER_QUAT JOBJ_USE_QUATERNION
#define HSD_JOBJ_TRAVERSAL_BARRIER 0x00001000
#define HSD_JOBJ_FLAG_PARTICLE 0x00000020
#define HSD_JOBJ_FLAG_SPLINE 0x00004000
#define HSD_JOBJ_FLAG_MTX_INDEP_PARENT 0x01000000
#define HSD_JOBJ_FLAG_MTX_INDEP_SRT 0x02000000

#ifdef __cplusplus
extern "C" {
#endif
void HSD_JObjUnref(_HSD_JObj *jobj);
_HSD_JObj *HSD_JObjLoadJoint(_HSD_Joint *joint);
void HSD_JObjAnim(_HSD_JObj *jobj);
void HSD_JObjClearFlags(_HSD_JObj *jobj, u32 flags);
void HSD_JObjSetFlags(_HSD_JObj *jobj, u32 flags);
void HSD_JObjSetMtxDirtySub(_HSD_JObj *jobj);
void HSD_JObjSetupMatrixSub(_HSD_JObj *jobj);
void HSD_JObjSetSPtclCallback(HSD_JObjParticleCallback callback);
void HSD_JObjSetDPtclCallback(HSD_JObjParticleCallback callback);
void HSD_JObjSetPtclTargetCallback(HSD_JObjParticleTargetCallback callback);

/* Tests the dirty flag for an automatically computed transform. */
#line 603 "jobj.h"
inline int HSD_JObjMtxIsDirty(_HSD_JObj *jobj)
{
    HSD_ASSERT(jobj);
    return !(jobj->flags & HSD_JOBJ_FLAG_USER_DEFINED_MTX) &&
           (jobj->flags & HSD_JOBJ_FLAG_MTX_DIRTY);
}

/* Marks an automatically updated joint transform for recomputation. */
static inline void HSD_JObjSetMtxDirty(_HSD_JObj *jobj)
{
    if (jobj == NULL) {
        return;
    }
    if (HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    HSD_JObjSetMtxDirtySub(jobj);
}

#line 746 "jobj.h"
static inline f32 HSD_JObjGetRotationX(_HSD_JObj *jobj)
{
    HSD_ASSERT(jobj);
    return jobj->mRotate[0];
}

#line 674 "jobj.h"
static inline void HSD_JObjSetRotationX(_HSD_JObj *jobj, f32 value)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(!(jobj->flags & JOBJ_USE_QUATERNION));
    jobj->mRotate[0] = value;
    if (!(jobj->flags & HSD_JOBJ_FLAG_MTX_INDEP_SRT)) HSD_JObjSetMtxDirty(jobj);
}

#line 694 "jobj.h"
static inline void HSD_JObjSetRotationY(_HSD_JObj *jobj, f32 value)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(!(jobj->flags & JOBJ_USE_QUATERNION));
    jobj->mRotate[1] = value;
    if (!(jobj->flags & HSD_JOBJ_FLAG_MTX_INDEP_SRT)) HSD_JObjSetMtxDirty(jobj);
}

#line 714 "jobj.h"
static inline void HSD_JObjSetRotationZ(_HSD_JObj *jobj, f32 value)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(!(jobj->flags & JOBJ_USE_QUATERNION));
    jobj->mRotate[2] = value;
    if (!(jobj->flags & HSD_JOBJ_FLAG_MTX_INDEP_SRT)) HSD_JObjSetMtxDirty(jobj);
}

inline void HSD_JObjSetupMatrix(_HSD_JObj *jobj)
{
    if (!jobj || HSD_JObjMtxIsDirty(jobj) == 0) return;
    HSD_JObjSetupMatrixSub(jobj);
}

#line 1171 "jobj.h"
static inline void HSD_JObjSetMatrix(_HSD_JObj *jobj, Mtx mtx)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(mtx);
    PSMTXCopy(mtx, jobj->mMatrix);
    jobj->flags |= HSD_JOBJ_FLAG_USER_DEFINED_MTX |
                     HSD_JOBJ_FLAG_MTX_INDEP_PARENT |
                     HSD_JOBJ_FLAG_MTX_INDEP_SRT;
    HSD_JObjSetMtxDirty(jobj);
}

#line 847 "jobj.h"
static inline void HSD_JObjGetScale(_HSD_JObj *jobj, Vec *scale)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(scale);
    scale->x = jobj->mScale[0];
    scale->y = jobj->mScale[1];
    scale->z = jobj->mScale[2];
}
#line 788 "jobj.h"
static inline void HSD_JObjSetScale(_HSD_JObj *jobj, const Vec *scale)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(scale);
    jobj->mScale[0] = scale->x;
    jobj->mScale[1] = scale->y;
    jobj->mScale[2] = scale->z;
    if (jobj->flags & HSD_JOBJ_FLAG_MTX_INDEP_SRT) {
        return;
    }
    HSD_JObjSetMtxDirty(jobj);
}
#line 994 "jobj.h"
static inline void HSD_JObjGetTranslation(_HSD_JObj *jobj, Vec *translate)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(translate);
    translate->x = jobj->mTrans[0];
    translate->y = jobj->mTrans[1];
    translate->z = jobj->mTrans[2];
}
#line 935 "jobj.h"
static inline void HSD_JObjSetTranslation(_HSD_JObj *jobj, const Vec *translate)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(translate);
    jobj->mTrans[0] = translate->x;
    jobj->mTrans[1] = translate->y;
    jobj->mTrans[2] = translate->z;
    if (jobj->flags & HSD_JOBJ_FLAG_MTX_INDEP_SRT) {
        return;
    }
    HSD_JObjSetMtxDirty(jobj);
}
#line 731 "jobj.h"
static inline void HSD_JObjGetRotation(_HSD_JObj *jobj, Quaternion *rotate)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(rotate);
    rotate->x = jobj->mRotate[0];
    rotate->y = jobj->mRotate[1];
    rotate->z = jobj->mRotate[2];
    rotate->w = jobj->mRotate[3];
}
#line 654 "jobj.h"
static inline void HSD_JObjSetRotation(_HSD_JObj *jobj, const Quaternion *rotate)
{
    HSD_ASSERT(jobj);
    HSD_ASSERT(rotate);
    jobj->mRotate[0] = rotate->x;
    jobj->mRotate[1] = rotate->y;
    jobj->mRotate[2] = rotate->z;
    jobj->mRotate[3] = rotate->w;
    if (jobj->flags & HSD_JOBJ_FLAG_MTX_INDEP_SRT) {
        return;
    }
    HSD_JObjSetMtxDirty(jobj);
}


#ifdef __cplusplus
}
#endif

#endif
