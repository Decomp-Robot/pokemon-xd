#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <dolphin/types.h>
#include <GSmemman.h>
#include <HSD.h>
#include <hsd/robj.h>
#include <hsd/wobj.h>
#include <hsd/lobj.h>
#include <hsd/cobj.h>
#include <hsd/fog.h>
#include <hsd/id.h>
#include <hsd/debug.h>
#include <hsd/list.h>

static HSD_ObjAllocData alloc_data;
static int nb_deadanim;
static int nb_activeanim;
static HSD_SList *endcallback_list;

void HSD_AObjInitAllocData(void)
{
    HSD_ObjAllocInit(&alloc_data, sizeof(HSD_AObj), 4);
}

HSD_ObjAllocData *HSD_AObjGetAllocData(void)
{
    return &alloc_data;
}

u32 HSD_AObjGetFlags(HSD_AObj *aobj)
{
    return aobj ? aobj->mFlags : 0;
}

void HSD_AObjSetFlags(HSD_AObj *aobj, u32 flags)
{
    if (aobj) {
        aobj->mFlags |= flags & HSD_AOBJ_PUBLIC_FLAGS;
    }
}

void HSD_AObjClearFlags(HSD_AObj *aobj, u32 flags)
{
    if (aobj) {
        aobj->mFlags &= ~(flags & HSD_AOBJ_PUBLIC_FLAGS);
    }
}

void HSD_AObjInitEndCallBack(void)
{
    nb_deadanim = 0;
    nb_activeanim = 0;
}

void HSD_AObjInvokeCallBacks(void)
{
    HSD_SList *cb;

    if (nb_deadanim && !nb_activeanim) {
        for (cb = endcallback_list; cb; cb = cb->next) {
            ((void (*)(void)) cb->data)();
        }
    }
}

void HSD_AObjReqAnim(HSD_AObj *aobj, f32 frame)
{
    if (aobj) {
        aobj->mFrame = frame;
        aobj->mFlags = (aobj->mFlags & ~HSD_AOBJ_FLAG_NO_ANIM) | HSD_AOBJ_FLAG_FIRST_PLAY;
        HSD_FObjReqAnimAll(aobj->mFObj, frame);
    }
}

void HSD_AObjInterpretAnim(HSD_AObj *aobj, void *obj, HSD_AnimUpdate update)
{
    f32 rate;

    if (!aobj || (aobj->mFlags & HSD_AOBJ_FLAG_NO_ANIM)) {
        return;
    }
    if (aobj->mFlags & HSD_AOBJ_FLAG_FIRST_PLAY) {
        aobj->mFlags &= ~HSD_AOBJ_FLAG_FIRST_PLAY;
        rate = 0;
    } else {
        rate = aobj->mRate;
        aobj->mFrame += rate;
    }
    if ((aobj->mFlags & HSD_AOBJ_FLAG_LOOP) && aobj->mEndFrame <= aobj->mFrame) {
        if (aobj->mRewindFrame < aobj->mEndFrame) {
            HSD_FObjStopAnimAll(aobj->mFObj, obj, update, rate);
            aobj->mFrame = aobj->mRewindFrame + fmod(aobj->mFrame - aobj->mRewindFrame,
                                                   aobj->mEndFrame - aobj->mRewindFrame);
            HSD_FObjReqAnimAll(aobj->mFObj, aobj->mFrame);
        } else {
            aobj->mFrame = aobj->mEndFrame;
        }
        aobj->mFlags |= HSD_AOBJ_FLAG_REWINDED;
        rate = 0;
    } else {
        aobj->mFlags &= ~HSD_AOBJ_FLAG_REWINDED;
    }
    if (aobj->mFlags & HSD_AOBJ_FLAG_NO_UPDATE) {
        HSD_FObjInterpretAnimAll(aobj->mFObj, obj, NULL, rate);
    } else {
        HSD_FObjInterpretAnimAll(aobj->mFObj, obj, update, rate);
    }
    if (aobj->mFlags & HSD_AOBJ_FLAG_NO_ANIM) {
        nb_deadanim++;
    } else {
        nb_activeanim++;
    }
}

void HSD_AObjSetFObj(HSD_AObj *aobj, HSD_FObj *fobj)
{
    if (aobj) {
        if (aobj->mFObj) {
            HSD_FObjRemoveAll(aobj->mFObj);
        }
        aobj->mFObj = fobj;
    }
}

void HSD_AObjSetHSDObj(HSD_AObj *aobj, HSD_JObj *obj)
{
    if (aobj) {
        if (aobj->mObject) {
            HSD_JObjUnref(aobj->mObject);
        }
        aobj->mObject = obj;
    }
}

void HSD_AObjSetRewindFrame(HSD_AObj *aobj, f32 frame)
{
    if (aobj) {
        aobj->mRewindFrame = frame;
    }
}

void HSD_AObjSetEndFrame(HSD_AObj *aobj, f32 frame)
{
    if (aobj) {
        aobj->mEndFrame = frame;
    }
}

HSD_AObj *HSD_AObjAlloc(void)
{
    HSD_AObj *new;
    GSmemPoolAllocator *pool = GSmemGetSysdolAllocator(sizeof(HSD_AObj));
    if (pool && GSmemGetFreePool(pool)) {
        new = GSmemAllocPool(pool);
        HSD_ASSERTLINE(502, new);
        return new;
    }
    new = HSD_ObjAlloc(&alloc_data);
    HSD_ASSERTLINE(509, new);
    memset(new, 0, sizeof(HSD_AObj));
    new->mFlags = HSD_AOBJ_FLAG_NO_ANIM;
    new->mRate = 1;
    return new;
}

HSD_AObj *HSD_AObjLoadDesc(HSD_AObjDesc *desc)
{
    HSD_JObj *obj;
    HSD_AObj *aobj;
    if (desc) {
        aobj = HSD_AObjAlloc();
        HSD_AObjSetFlags(aobj, desc->mFlags);
        HSD_AObjSetRewindFrame(aobj, 0);
        HSD_AObjSetEndFrame(aobj, desc->mEndFrame);
        HSD_AObjSetFObj(aobj, HSD_FObjLoadDesc(desc->mFObjDesc));
        if (desc->mJoint) {
            obj = HSD_IDGetDataFromTable(NULL, (u32)desc->mJoint, NULL);
            if (obj) {
                HSD_JObjRef(obj);
            } else {
                obj = HSD_JObjLoadJoint(desc->mJoint);
            }
            HSD_AObjSetHSDObj(aobj, obj);
        }
        return aobj;
    }
    return NULL;
}

void HSD_AObjFree(HSD_AObj *aobj)
{
    GSmemPoolAllocator *pool;
    if (aobj) {
        pool = GSmemGetSysdolAllocator(sizeof(HSD_AObj));
        if (pool && GSmemIsRegistrationPool(pool, aobj)) {
            GSmemFreePool(pool, aobj);
        } else {
            HSD_ObjFree(&alloc_data, aobj);
        }
    }
}

void HSD_AObjRemove(HSD_AObj *aobj)
{
    if (aobj) {
        HSD_AObjSetFObj(aobj, NULL);
        HSD_AObjSetHSDObj(aobj, NULL);
        HSD_AObjFree(aobj);
    }
}

static inline BOOL union_type_dobj(HSD_JObj *jobj)
{
    return (jobj->flags & (HSD_JOBJ_FLAG_PARTICLE | HSD_JOBJ_FLAG_SPLINE)) ? FALSE : TRUE;
}

static inline void callbackForeachFunc(HSD_AObj *aobj, void *obj, HSD_ObjType type, void (*func)(),
                                AObjArgType arg_type, callbackArg *arg)
{
    if (aobj == NULL) {
        return;
    }
    switch (arg_type) {
    case AOBJ_ARG_A:
        (*func)(aobj);
        break;
    case AOBJ_ARG_AF:
        (*func)(aobj, arg->f);
        break;
    case AOBJ_ARG_AV:
        (*func)(aobj, arg->v);
        break;
    case AOBJ_ARG_AU:
        (*func)(aobj, arg->u);
        break;
    case AOBJ_ARG_AO:
        (*func)(aobj, obj);
        break;
    case AOBJ_ARG_AOT:
        (*func)(aobj, obj, type);
        break;
    case AOBJ_ARG_AOF:
        (*func)(aobj, obj, arg->f);
        break;
    case AOBJ_ARG_AOV:
        (*func)(aobj, obj, arg->v);
        break;
    case AOBJ_ARG_AOU:
        (*func)(aobj, obj, arg->u);
        break;
    case AOBJ_ARG_AOTF:
        (*func)(aobj, obj, type, arg->f);
        break;
    case AOBJ_ARG_AOTV:
        (*func)(aobj, obj, type, arg->v);
        break;
    case AOBJ_ARG_AOTU:
        (*func)(aobj, obj, type, arg->u);
        break;
    }
}

static inline void RObjForeachAnim(HSD_RObj *robj, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                            callbackArg *arg)
{
    for (; robj; robj = robj->mNext) {
        if (mask & ROBJ_MASK) {
            callbackForeachFunc(robj->mAObj, robj, ROBJ_TYPE, func, arg_type, arg);
        }
    }
}

static inline void TObjForeachAnim(HSD_TObj *tobj, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                            callbackArg *arg)
{
    for (; tobj; tobj = tobj->mNext) {
        if (mask & TOBJ_MASK) {
            callbackForeachFunc(tobj->mAObj, tobj, TOBJ_TYPE, func, arg_type, arg);
        }
    }
}

static inline void PObjForeachAnim(HSD_PObj *pobj, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                            callbackArg *arg)
{
    for (; pobj; pobj = pobj->mNext) {
        if (mask & POBJ_MASK) {
            callbackForeachFunc(pobj->mAObj, pobj, POBJ_TYPE, func, arg_type, arg);
        }
    }
}

static inline void MObjForeachAnim(HSD_MObj *mobj, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                            callbackArg *arg)
{
    if (mobj == NULL) {
        return;
    }
    if (mask & MOBJ_MASK) {
        callbackForeachFunc(mobj->mAObj, mobj, MOBJ_TYPE, func, arg_type, arg);
    }
    TObjForeachAnim(mobj->mTObj, mask, func, arg_type, arg);
}

static inline void WObjForeachAnim(HSD_WObj *wobj, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                            callbackArg *arg)
{
    if (wobj == NULL) {
        return;
    }
    if (mask & WOBJ_MASK) {
        callbackForeachFunc(wobj->aobj, wobj, WOBJ_TYPE, func, arg_type, arg);
    }
    RObjForeachAnim(wobj->robj, mask, func, arg_type, arg);
}

static inline void FogForeachAnim(HSD_Fog *fog, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                           callbackArg *arg)
{
    if (mask & FOG_MASK) {
        if (fog) {
            callbackForeachFunc(fog->mAObj, fog, FOG_TYPE, func, arg_type, arg);
        }
    }
}

void CObjForeachAnim(HSD_CObj *cobj, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                     callbackArg *arg)
{
    if (cobj == NULL) {
        return;
    }
    if (mask & COBJ_MASK) {
        callbackForeachFunc(cobj->mAObj, cobj, COBJ_TYPE, func, arg_type, arg);
    }
    WObjForeachAnim(cobj->eye, mask, func, arg_type, arg);
    WObjForeachAnim(cobj->interest, mask, func, arg_type, arg);
}

void LObjForeachAnim(HSD_LObj *lobj, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                     callbackArg *arg)
{
    for (; lobj; lobj = lobj->next) {
        if (mask & LOBJ_MASK) {
            callbackForeachFunc(lobj->mAObj, lobj, LOBJ_TYPE, func, arg_type, arg);
        }
        WObjForeachAnim(lobj->position, mask, func, arg_type, arg);
        WObjForeachAnim(lobj->interest, mask, func, arg_type, arg);
    }
}

void DObjForeachAnim(HSD_DObj *dobj, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                     callbackArg *arg)
{
    for (; dobj; dobj = dobj->mNext) {
        if (mask & DOBJ_MASK) {
            callbackForeachFunc(dobj->mAObj, dobj, DOBJ_TYPE, func, arg_type, arg);
        }
        MObjForeachAnim(dobj->mMaterial, mask, func, arg_type, arg);
        PObjForeachAnim(dobj->mPrimitive, mask, func, arg_type, arg);
    }
}

void JObjForeachAnim(HSD_JObj *obj, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type,
                     callbackArg *arg)
{
    HSD_ASSERTLINE(747, obj);
    if (mask & JOBJ_MASK) {
        callbackForeachFunc(obj->mAObj, obj, JOBJ_TYPE, func, arg_type, arg);
    }
    if (union_type_dobj(obj)) {
        DObjForeachAnim(obj->u.mDrawable, mask, func, arg_type, arg);
    }
    RObjForeachAnim(obj->mRObj, mask, func, arg_type, arg);
    if (!(obj->flags & HSD_JOBJ_TRAVERSAL_BARRIER)) {
        for (obj = obj->child; obj; obj = obj->next) {
            JObjForeachAnim(obj, mask, func, arg_type, arg);
        }
    }
}

void HSD_ForeachAnim(void *obj, HSD_ObjType type, HSD_TypeMask mask, void (*func)(), AObjArgType arg_type, ...)
{
    callbackArg arg;
    va_list ap;

    if (obj == NULL) {
        return;
    }

    va_start(ap, arg_type);
    switch (arg_type) {
    case AOBJ_ARG_AF:
    case AOBJ_ARG_AOF:
    case AOBJ_ARG_AOTF:
        arg.f = (f32) va_arg(ap, f64);
        break;
    case AOBJ_ARG_AV:
    case AOBJ_ARG_AOV:
    case AOBJ_ARG_AOTV:
        arg.v = va_arg(ap, void *);
        break;
    case AOBJ_ARG_AU:
    case AOBJ_ARG_AOU:
    case AOBJ_ARG_AOTU:
        arg.u = va_arg(ap, u32);
        break;
    case AOBJ_ARG_A:
    case AOBJ_ARG_AO:
    case AOBJ_ARG_AOT:
        break;
    default:
        HSD_Panic(__FILE__, 858, "unexpected argument format.\n");
        break;
    }
    va_end(ap);

    switch (type) {
    case JOBJ_TYPE:
        JObjForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    case DOBJ_TYPE:
        DObjForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    case MOBJ_TYPE:
        MObjForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    case POBJ_TYPE:
        PObjForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    case TOBJ_TYPE:
        TObjForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    case LOBJ_TYPE:
        LObjForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    case COBJ_TYPE:
        CObjForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    case ROBJ_TYPE:
        RObjForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    case WOBJ_TYPE:
        WObjForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    case FOG_TYPE:
        FogForeachAnim(obj, mask, func, arg_type, &arg);
        break;
    default:
        HSD_Panic(__FILE__, 894, "unexpected type of object.\n");
        break;
    }
}

void HSD_AObjSetRate(HSD_AObj *aobj, f32 rate)
{
    if (aobj) {
        aobj->mRate = rate;
    }
}

void _HSD_AObjForgetMemory(void)
{
    endcallback_list = NULL;
}
