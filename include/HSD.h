#ifndef HSD_H
#define HSD_H

#include <types.h>
#include <GSmath.h>

#ifndef NULL
#define NULL 0
#endif

#include <hsd/jobj.h>
#include <hsd/aobj.h>

extern "C" void QuatSlerp(const Quaternion *first, const Quaternion *second, Quaternion *out, f32 blend);

#define HSD_POBJ_TYPE_MASK 0x3000
#define HSD_POBJ_SHAPE_ANIM 0x1000

enum HSDVertexDescriptorConstants {
    HSD_VTX_ATTR_POSITION = 9,
    HSD_VTX_ATTR_NULL = 0xFF,
    HSD_VTX_INPUT_DIRECT = 1
};
struct _HSD_VtxDescList {
    int mAttribute;
    s32 mInputType;
    u32 mComponentCount;
    u32 mComponentFormat;
    u8 mFractionalBits;
    u8 _11;
    u16 mStride;
    void *mArrayBase;
};
struct _HSD_PObj;
struct _HSD_DObj {
    u8 _00[8];
    void *mMaterial;
    _HSD_PObj *mPrimitive;
};

/* A primitive object: one display list plus its vertex description. */
struct _HSD_PObj {
    u8 _00[0x08];
    _HSD_VtxDescList *mVtxAttr; // 0x08: vertex attribute layout array.
    u16 mFlags; // 0x0C: primitive rendering flags.
    u16 mDisplayListBlocks; // 0x0E: display-list length in 32-byte blocks.
    void *mDisplayList; // 0x10
    u8 _14[4];
};

struct _HSD_MatAnimJoint {
    _HSD_MatAnimJoint *mChild;
    _HSD_MatAnimJoint *mNext;
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

extern "C" {
void HSD_ForeachAnim(void *object, u32 objectType, u32 animationMask,
                     void *callback, u32 argumentType, ...);
void HSD_JObjAddAnim(_HSD_JObj *joint, _HSD_AnimJoint *jointAnimation,
                    _HSD_MatAnimJoint *materialAnimation,
                    _HSD_ShapeAnimJoint *shapeAnimation);
void HSD_JObjAddAnimAll(_HSD_JObj *joint, _HSD_AnimJoint *jointAnimation,
                       _HSD_MatAnimJoint *materialAnimation,
                       _HSD_ShapeAnimJoint *shapeAnimation);
void HSD_JObjReqAnimAllByFlags(_HSD_JObj *joint, u32 flags, f32 frame);
void HSD_JObjAnimAll(_HSD_JObj *joint);
void HSD_JObjRemoveAnim(_HSD_JObj *joint);
}

#define HSD_ANIM_ARG_POINTER 2u
#define HSD_ANIM_ARG_UNSIGNED 3u
#define HSD_OBJECT_JOINT 6u

/* Adapts the game's matrix class to the SDK matrix array. */
static inline void HSD_JObjSetMatrix(_HSD_JObj *jobj, GSmtx *mtx)
{
    HSD_JObjSetMatrix(jobj, reinterpret_cast<MtxPtr>(mtx));
}

#endif
