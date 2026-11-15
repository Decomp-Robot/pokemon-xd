#ifndef HSD_H
#define HSD_H

#include <types.h>
#include <dolphin/gx/GXEnum.h>
#include <dolphin/mtx.h>
#ifdef __cplusplus
#include <GSmath.h>
#endif

#ifndef NULL
#define NULL 0
#endif

#include <hsd/jobj.h>
#include <hsd/aobj.h>
#include <hsd/dobj.h>
#include <hsd/mobj.h>
#include <hsd/tobj.h>
#include <hsd/pobj.h>
#ifdef __cplusplus
#include <HSDScene.h>
#endif

#ifdef __cplusplus
extern "C"
#endif
int QuatSlerp(const Quaternion *first, const Quaternion *second, Quaternion *out, f32 blend);

struct _HSD_MatAnimJoint {
    _HSD_MatAnimJoint *mChild;
    _HSD_MatAnimJoint *mNext;
    _HSD_MatAnim *mMatAnim;
};
struct _HSD_ShapeAnimJoint {
    _HSD_ShapeAnimJoint *mChild;
    _HSD_ShapeAnimJoint *mNext;
};

struct _HSD_RObjAnimJoint;

struct _HSD_AnimJoint {
    _HSD_AnimJoint *mChild;
    _HSD_AnimJoint *mNext;
    _HSD_AObjDesc *mAnimation;
    _HSD_RObjAnimJoint *mRObjAnimation;
    u32 mFlags;
};

#ifdef __cplusplus
extern "C" {
#endif
void *HSD_SetVertexBuffer(void *buffer, u32 count);
void *HSD_SetExpArgBuffer(void *buffer, u32 count);
void *HSD_SetNormalBuffer(void *buffer, u32 count);
void HSD_JObjAddAnim(_HSD_JObj *joint, _HSD_AnimJoint *jointAnimation,
                    _HSD_MatAnimJoint *materialAnimation,
                    _HSD_ShapeAnimJoint *shapeAnimation);
void HSD_JObjAddAnimAll(_HSD_JObj *joint, _HSD_AnimJoint *jointAnimation,
                       _HSD_MatAnimJoint *materialAnimation,
                       _HSD_ShapeAnimJoint *shapeAnimation);
void HSD_JObjReqAnimAllByFlags(_HSD_JObj *joint, u32 flags, f32 frame);
void HSD_JObjAnimAll(_HSD_JObj *joint);
void HSD_JObjRemoveAnim(_HSD_JObj *joint);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/* Adapts the game's matrix class to the SDK matrix array. */
static inline void HSD_JObjSetMatrix(_HSD_JObj *jobj, GSmtx *mtx)
{
    HSD_JObjSetMatrix(jobj, reinterpret_cast<MtxPtr>(mtx));
}
#endif

#endif
