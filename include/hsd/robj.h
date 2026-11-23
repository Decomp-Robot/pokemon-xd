#ifndef HSD_ROBJ_H
#define HSD_ROBJ_H

#include <types.h>
#include <hsd/forward.h>
#include <hsd/objalloc.h>

/* Flags of a constraint object: the evaluated state in the top bit, the
 * object type it constrains next to it and the constraint sub-type below. */
#define HSD_ROBJ_UPDATE_FLAG 0x80000000
#define HSD_ROBJ_TYPE_MASK 0x70000000
#define HSD_ROBJ_SUBTYPE_MASK 0x0FFFFFFF

/* Flag the "ByFlags" entry points expect in the mask they are given before
 * they request or remove the animation of a constraint. */
#define HSD_ROBJ_ANIM_MASK 0x80

struct _HSD_RObjAnim {
    HSD_RObjAnim *mNext;
    HSD_AObjDesc *mAObjDesc;
};

struct _HSD_RObj {
    HSD_RObj *mNext;
    u32 mFlags;
    u8 _08[16];
    HSD_AObj *mAObj;
};

struct _HSD_Rvalue {
    HSD_Rvalue *mNext;
    u32 mFlags;
    u8 _08[4];
};

#ifdef __cplusplus
extern "C" {
#endif
void HSD_RObjRemoveAll(HSD_RObj *robj);
HSD_RObj *HSD_RObjLoadDesc(HSD_RObjDesc *desc);
void HSD_RObjResolveRefsAll(HSD_RObj *robj, HSD_RObjDesc *desc);
void HSD_RObjAddAnimAll(HSD_RObj *robj, HSD_RObjAnim *anim);
void HSD_RObjReqAnimAll(HSD_RObj *robj, f32 frame);
void HSD_RObjReqAnimAllByFlags(HSD_RObj *robj, u32 flags, f32 frame);
void HSD_RObjAnimAll(HSD_RObj *robj);
void HSD_RObjRemoveAnimAll(HSD_RObj *robj);
void HSD_RObjRemoveAnimAllByFlags(HSD_RObj *robj, u32 flags);
HSD_RObj *HSD_RObjGetByType(HSD_RObj *robj, u32 type, u32 sub_type);
HSD_ObjAllocData *HSD_RvalueObjGetAllocData(void);
HSD_ObjAllocData *HSD_RObjGetAllocData(void);
void HSD_RObjInitAllocData(void);
#ifdef __cplusplus
}
#endif

#endif
