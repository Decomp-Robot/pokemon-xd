#ifndef HSD_WOBJ_H
#define HSD_WOBJ_H

#include <types.h>
#include <hsd/forward.h>
#include <hsd/class.h>
#include <hsd/aobj.h>
#include <hsd/robj.h>

typedef struct _HSD_WObjInfo _HSD_WObjInfo;
typedef struct _HSD_WObjAnim {
    _HSD_AObjDesc *aobjdesc;
    _HSD_RObjAnim *robj_anim;
} _HSD_WObjAnim;

struct _HSD_Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct _HSD_WObj {
    _HSD_WObjInfo *class_info;
    u16 ref_count;
    u16 ref_count_individual;
    u32 flags;
    _HSD_Vec3 position;
    _HSD_AObj *aobj;
    _HSD_RObj *robj;
};

struct _HSD_WObjDesc {
    const char *className;
    _HSD_Vec3 position;
    void *constraints;
};

struct _HSD_WObjInfo {
    _HSD_ClassInfo parent;
    int (*load)(_HSD_WObj *wobj, _HSD_WObjDesc *desc);
    HSD_AnimUpdate update;
};

enum HSD_WObjPositionFlags {
    WOBJ_POSITION_JOINT = 0x1u,
    WOBJ_POSITION_DIRTY = 0x2u
};

enum HSD_WObjAnimChannel {
    HSD_A_W_PATH = 4,
    HSD_A_W_X = 5,
    HSD_A_W_Y = 6,
    HSD_A_W_Z = 7
};

#ifdef __cplusplus
extern "C" {
#endif
extern _HSD_WObjInfo hsdWObj;
_HSD_WObj *HSD_WObjAlloc(void);
void HSD_WObjGetPosition(_HSD_WObj *wobj, _HSD_Vec3 *position);
void HSD_WObjSetPosition(_HSD_WObj *wobj, _HSD_Vec3 *position);
_HSD_WObj *HSD_WObjLoadDesc(_HSD_WObjDesc *desc);
void HSD_WObjInit(_HSD_WObj *wobj, _HSD_WObjDesc *desc);
void HSD_WObjInterpretAnim(_HSD_WObj *wobj);
void HSD_WObjAddAnim(_HSD_WObj *wobj, _HSD_WObjAnim *anim);
void HSD_WObjReqAnim(_HSD_WObj *wobj, f32 frame);
void HSD_WObjRemoveAnim(_HSD_WObj *wobj);
#ifdef __cplusplus
}
#endif

#endif
