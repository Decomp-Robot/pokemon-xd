#include <HSD.h>
#include <hsd/aobj.h>

u32 normal_buffer_size;
u32 vertex_buffer_size;
Vec *normal_buffer;
Vec *vertex_buffer;
_HSD_PObjInfo *default_class;

static inline void PObjAnim(HSD_PObj *pobj)
{
    if (pobj) {
        HSD_AObjInterpretAnim(pobj->mAObj, pobj, pobj->class_info->update);
    }
}

void HSD_PObjAnimAll(HSD_PObj *pobj)
{
    HSD_PObj *cur;

    if (pobj) {
        for (cur = pobj; cur; cur = cur->mNext) {
            PObjAnim(cur);
        }
    }
}

void PObjUpdateFunc(void *object, u32 type, HSD_ObjData *value)
{
    HSD_PObj *pobj = object;

    if (pobj && (pobj->mFlags & HSD_POBJ_TYPE_MASK) == HSD_POBJ_SHAPE_ANIM) {
        HSD_ShapeSet *shape = pobj->u.shape_set;
        if (shape->flags & HSD_SHAPE_SET_WEIGHTS) {
            shape->u.shape_weights[type - HSD_SHAPE_WEIGHT_CHANNEL_BASE] = value->scalar;
        } else {
            shape->u.shape_position = value->scalar;
        }
    }
}

static inline void PObjReqAnimByFlags(HSD_PObj *pobj, u32 flags, f32 frame)
{
    if (pobj && (flags & HSD_POBJ_ANIM_MASK)) {
        HSD_AObjReqAnim(pobj->mAObj, frame);
    }
}

void HSD_PObjReqAnimAllByFlags(HSD_PObj *pobj, u32 flags, f32 frame)
{
    HSD_PObj *cur;

    if (pobj) {
        for (cur = pobj; cur; cur = cur->mNext) {
            PObjReqAnimByFlags(cur, flags, frame);
        }
    }
}

static inline void PObjAddAnim(HSD_PObj *pobj, HSD_ShapeAnim *shape_anim)
{
    if (pobj) {
        if (pobj->mAObj) {
            HSD_AObjRemove(pobj->mAObj);
        }
        pobj->mAObj = HSD_AObjLoadDesc(shape_anim->aobjdesc);
    }
}

void HSD_PObjAddAnimAll(HSD_PObj *pobj, HSD_ShapeAnim *shape_anim)
{
    HSD_PObj *cur;
    HSD_ShapeAnim *anim;

    if (!pobj || !shape_anim) {
        return;
    }

    cur = pobj;
    anim = shape_anim;
    while (cur) {
        PObjAddAnim(cur, anim);
        cur = cur->mNext;
        anim = anim ? anim->next : NULL;
    }
}

static inline void PObjRemoveAnimByFlags(HSD_PObj *pobj, u32 flags)
{
    if (pobj && (flags & HSD_POBJ_ANIM_MASK)) {
        HSD_AObjRemove(pobj->mAObj);
        pobj->mAObj = NULL;
    }
}

void HSD_PObjRemoveAnimAllByFlags(HSD_PObj *pobj, u32 flags)
{
    HSD_PObj *cur;

    if (pobj) {
        for (cur = pobj; cur; cur = cur->mNext) {
            PObjRemoveAnimByFlags(cur, flags);
        }
    }
}

void *HSD_SetNormalBuffer(void *buffer, u32 count)
{
    Vec *old = normal_buffer;

    normal_buffer_size = count;
    normal_buffer = buffer;
    return old;
}

void *HSD_SetVertexBuffer(void *buffer, u32 count)
{
    Vec *old = vertex_buffer;

    vertex_buffer_size = count;
    vertex_buffer = buffer;
    return old;
}
