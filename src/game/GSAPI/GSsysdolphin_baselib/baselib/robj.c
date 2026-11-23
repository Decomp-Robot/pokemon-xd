#include <hsd/robj.h>
#include <hsd/aobj.h>
#include <hsd/fobj.h>

/* Allocators of the constraint objects and of their reference values. */
HSD_ObjAllocData rv_alloc_data;
HSD_ObjAllocData robj_alloc_data;

void RObjUpdateFunc(void *object, u32 type, HSD_ObjData *value);

static inline void RObjAddAnim(HSD_RObj *robj, HSD_RObjAnim *anim)
{
    if (robj != NULL && anim != NULL) {
        if (robj->mAObj != NULL) {
            HSD_AObjRemove(robj->mAObj);
        }
        robj->mAObj = HSD_AObjLoadDesc(anim->mAObjDesc);
    }
}

static inline void RObjReqAnim(HSD_RObj *robj, f32 frame, u32 flags)
{
    if (robj != NULL && robj->mAObj != NULL && flags != 0) {
        HSD_AObjReqAnim(robj->mAObj, frame);
    }
}

static inline void RObjRemoveAnim(HSD_RObj *robj, u32 flags)
{
    if (robj != NULL && robj->mAObj != NULL && flags != 0) {
        HSD_AObjRemove(robj->mAObj);
        robj->mAObj = NULL;
    }
}

static inline void RObjAnim(HSD_RObj *robj)
{
    if (robj != NULL) {
        HSD_AObjInterpretAnim(robj->mAObj, robj, RObjUpdateFunc);
    }
}

void HSD_RObjAddAnimAll(HSD_RObj *robj, HSD_RObjAnim *anim)
{
    HSD_RObj *rp;
    HSD_RObjAnim *ap;

    if (robj == NULL || anim == NULL) {
        return;
    }
    rp = robj;
    ap = anim;
    while (rp != NULL && ap != NULL) {
        RObjAddAnim(rp, ap);
        rp = rp->mNext;
        ap = ap->mNext;
    }
}

void HSD_RObjReqAnimAll(HSD_RObj *robj, f32 frame)
{
    HSD_RObj *rp = robj;

    if (rp == NULL) {
        return;
    }
    while (rp != NULL) {
        RObjReqAnim(rp, frame, HSD_ROBJ_ANIM_MASK);
        rp = rp->mNext;
    }
}

void HSD_RObjReqAnimAllByFlags(HSD_RObj *robj, u32 flags, f32 frame)
{
    flags &= HSD_ROBJ_ANIM_MASK;
    if (robj == NULL) {
        return;
    }
    while (robj != NULL) {
        RObjReqAnim(robj, frame, flags);
        robj = robj->mNext;
    }
}

void HSD_RObjRemoveAnimAll(HSD_RObj *robj)
{
    HSD_RObj *rp = robj;

    if (rp == NULL) {
        return;
    }
    while (rp != NULL) {
        RObjRemoveAnim(rp, HSD_ROBJ_ANIM_MASK);
        rp = rp->mNext;
    }
}

void HSD_RObjRemoveAnimAllByFlags(HSD_RObj *robj, u32 flags)
{
    flags &= HSD_ROBJ_ANIM_MASK;
    if (robj == NULL) {
        return;
    }
    while (robj != NULL) {
        RObjRemoveAnim(robj, flags);
        robj = robj->mNext;
    }
}

void HSD_RObjAnimAll(HSD_RObj *robj)
{
    HSD_RObj *rp;

    if (robj == NULL) {
        return;
    }
    rp = robj;
    while (rp != NULL) {
        RObjAnim(rp);
        rp = rp->mNext;
    }
}

void RObjUpdateFunc(void *object, u32 type, HSD_ObjData *value)
{
    HSD_RObj *robj = object;

    if (robj == NULL) {
        return;
    }
    switch (type) {
    case HSD_ROBJ_UPDATE_CHANNEL:
        if (value->scalar >= 0.5) {
            robj->mFlags |= HSD_ROBJ_UPDATE_FLAG;
        } else {
            robj->mFlags &= ~HSD_ROBJ_UPDATE_FLAG;
        }
        break;
    }
}

HSD_RObj *HSD_RObjGetByType(HSD_RObj *robj, u32 type, u32 sub_type)
{
    HSD_RObj *rp;

    if (robj == NULL) {
        return NULL;
    }
    rp = robj;
    while (rp != NULL) {
        if ((rp->mFlags >> 31) != 0 && (rp->mFlags & HSD_ROBJ_TYPE_MASK) == type &&
            (sub_type == 0 || sub_type == (rp->mFlags & HSD_ROBJ_SUBTYPE_MASK))) {
            return rp;
        }
        rp = rp->mNext;
    }
    return NULL;
}

HSD_ObjAllocData *HSD_RvalueObjGetAllocData(void)
{
    return &rv_alloc_data;
}

HSD_ObjAllocData *HSD_RObjGetAllocData(void)
{
    return &robj_alloc_data;
}

void HSD_RObjInitAllocData(void)
{
    HSD_ObjAllocInit(&robj_alloc_data, sizeof(HSD_RObj), 4);
    HSD_ObjAllocInit(&rv_alloc_data, sizeof(HSD_Rvalue), 4);
}
