#ifndef HSD_AOBJ_H
#define HSD_AOBJ_H

#include <types.h>
#include <hsd/forward.h>
#include <hsd/fobj.h>
#include <hsd/objalloc.h>

struct _HSD_AObj {
    u32 mFlags;
    f32 mFrame;
    f32 mRewindFrame;
    f32 mEndFrame;
    f32 mRate;
    HSD_FObj *mFObj;
    HSD_JObj *mObject;
};

struct _HSD_AObjDesc {
    u32 mFlags;
    f32 mEndFrame;
    HSD_FObjDesc *mFObjDesc;
    HSD_Joint *mJoint;
};

typedef enum _HSD_ObjType {
    COBJ_TYPE = 2,
    DOBJ_TYPE = 3,
    FOG_TYPE = 5,
    JOBJ_TYPE = 6,
    LOBJ_TYPE = 7,
    MOBJ_TYPE = 8,
    POBJ_TYPE = 9,
    ROBJ_TYPE = 10,
    TOBJ_TYPE = 11,
    WOBJ_TYPE = 12
} HSD_ObjType;

enum _HSD_TypeMask {
    COBJ_MASK = 1 << (COBJ_TYPE - 1),
    DOBJ_MASK = 1 << (DOBJ_TYPE - 1),
    FOG_MASK = 1 << (FOG_TYPE - 1),
    JOBJ_MASK = 1 << (JOBJ_TYPE - 1),
    LOBJ_MASK = 1 << (LOBJ_TYPE - 1),
    MOBJ_MASK = 1 << (MOBJ_TYPE - 1),
    POBJ_MASK = 1 << (POBJ_TYPE - 1),
    ROBJ_MASK = 1 << (ROBJ_TYPE - 1),
    TOBJ_MASK = 1 << (TOBJ_TYPE - 1),
    WOBJ_MASK = 1 << (WOBJ_TYPE - 1),
    HSD_ALL_MASK = 0xFFFF
};
typedef int HSD_TypeMask;

typedef enum _AObjArgType {
    AOBJ_ARG_A,
    AOBJ_ARG_AF,
    AOBJ_ARG_AV,
    AOBJ_ARG_AU,
    AOBJ_ARG_AO,
    AOBJ_ARG_AOF,
    AOBJ_ARG_AOV,
    AOBJ_ARG_AOU,
    AOBJ_ARG_AOT,
    AOBJ_ARG_AOTF,
    AOBJ_ARG_AOTV,
    AOBJ_ARG_AOTU
} AObjArgType;

typedef union _callbackArg {
    f32 f;
    void *v;
    u32 u;
} callbackArg;

#define HSD_AOBJ_FLAG_NO_ANIM 0x40000000u
#define HSD_AOBJ_FLAG_LOOP 0x20000000u
#define HSD_AOBJ_FLAG_NO_UPDATE 0x10000000u
#define HSD_AOBJ_FLAG_FIRST_PLAY 0x08000000u
#define HSD_AOBJ_FLAG_REWINDED 0x04000000u
#define HSD_AOBJ_PUBLIC_FLAGS 0x30010000u

#ifdef __cplusplus
extern "C" {
#endif
void __assert(const char *file, int line, const char *condition);
void HSD_AObjSetRate(HSD_AObj *aobj, f32 rate);
void HSD_AObjSetFlags(HSD_AObj *aobj, u32 flags);
void HSD_AObjClearFlags(HSD_AObj *aobj, u32 flags);
u32 HSD_AObjGetFlags(HSD_AObj *aobj);
void HSD_ForeachAnim(void *object, HSD_ObjType objectType, HSD_TypeMask animationMask,
                     void (*callback)(), AObjArgType argumentType, ...);
void HSD_AObjInitAllocData(void);
HSD_ObjAllocData *HSD_AObjGetAllocData(void);
void HSD_AObjInitEndCallBack(void);
void HSD_AObjInvokeCallBacks(void);
void HSD_AObjReqAnim(HSD_AObj *aobj, f32 frame);
void HSD_AObjInterpretAnim(HSD_AObj *aobj, void *object, HSD_AnimUpdate update);
HSD_AObj *HSD_AObjLoadDesc(HSD_AObjDesc *desc);
void HSD_AObjRemove(HSD_AObj *aobj);
#ifdef __cplusplus
}

#line 170 "aobj.h"
inline f32 HSD_AObjGetEndFrame(HSD_AObj *aobj)
{
    if (aobj == 0) {
        __assert(__FILE__, 172, "aobj");
    }
    return aobj->mEndFrame;
}
#endif

#endif
