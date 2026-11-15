#include <hsd/wobj.h>
#include <hsd/jobj.h>
#include <hsd/assert.h>

void WObjInfoInit(void);
void WObjAmnesia(_HSD_ClassInfo *info);
void WObjRelease(void *object);
int WObjLoad(_HSD_WObj *wobj, _HSD_WObjDesc *desc);
void WObjUpdateFunc(void *object, u32 type, HSD_ObjData *value);

_HSD_WObjInfo hsdWObj = { { { WObjInfoInit } } };
static _HSD_WObjInfo *default_class;

void WObjInfoInit(void)
{
    hsdInitClassInfo(&hsdWObj.parent, &hsdObj,
                     "sysdolphin_base_library", "had_wobj",
                     sizeof(_HSD_WObjInfo), sizeof(_HSD_WObj));
    hsdWObj.parent.release = WObjRelease;
    hsdWObj.parent.amnesia = WObjAmnesia;
    hsdWObj.load = WObjLoad;
    hsdWObj.update = WObjUpdateFunc;
}

void WObjAmnesia(_HSD_ClassInfo *info)
{
    if (info == (_HSD_ClassInfo *)default_class) default_class = NULL;
    hsdWObj.parent.head.parent->amnesia(info);
}

void WObjRelease(void *object)
{
    _HSD_WObj *wobj = object;
    HSD_RObjRemoveAll(wobj->robj);
    HSD_AObjRemove(wobj->aobj);
    hsdWObj.parent.head.parent->release(object);
}

_HSD_WObj *HSD_WObjAlloc(void)
{
    _HSD_WObj *wobj = hsdNew((_HSD_ClassInfo *)(default_class ? default_class : &hsdWObj));
    HSD_ASSERTLINE(599, wobj);
    return wobj;
}

static inline void WObjPositionToWorld(_HSD_WObj *wobj)
{
    _HSD_JObj *jobj;
    if (wobj->flags & WOBJ_POSITION_JOINT) {
        if (wobj->aobj && (jobj = wobj->aobj->mObject)) {
            HSD_JObjSetupMatrix(jobj);
            PSMTXMultVec(jobj->mMatrix, (Vec *)&wobj->position, (Vec *)&wobj->position);
        }
        wobj->flags &= ~WOBJ_POSITION_JOINT;
    }
}

void HSD_WObjGetPosition(_HSD_WObj *wobj, _HSD_Vec3 *position)
{
    if (!wobj || !position) return;
    WObjPositionToWorld(wobj);
    *position = wobj->position;
}

void HSD_WObjSetPosition(_HSD_WObj *wobj, _HSD_Vec3 *position)
{
    if (!wobj || !position) return;
    wobj->position = *position;
    wobj->flags |= WOBJ_POSITION_DIRTY;
    wobj->flags &= ~WOBJ_POSITION_JOINT;
}

_HSD_WObj *HSD_WObjLoadDesc(_HSD_WObjDesc *desc)
{
    _HSD_WObj *wobj;
    _HSD_ClassInfo *info;
    if (desc) {
        if (!desc->className || !(info = hsdSearchClassInfo(desc->className))) {
            wobj = HSD_WObjAlloc();
        } else {
            wobj = hsdNew(info);
            HSD_ASSERTLINE(260, wobj);
        }
        wobj->class_info->load(wobj, desc);
        return wobj;
    }
    return NULL;
}

void HSD_WObjInit(_HSD_WObj *wobj, _HSD_WObjDesc *desc)
{
    if (!wobj || !desc) return;
    HSD_WObjSetPosition(wobj, &desc->position);
    if (wobj->robj) HSD_RObjRemoveAll(wobj->robj);
    wobj->robj = HSD_RObjLoadDesc(desc->constraints);
    HSD_RObjResolveRefsAll(wobj->robj, desc->constraints);
}

int WObjLoad(_HSD_WObj *wobj, _HSD_WObjDesc *desc)
{
    HSD_WObjSetPosition(wobj, &desc->position);
    if (wobj->robj) HSD_RObjRemoveAll(wobj->robj);
    wobj->robj = HSD_RObjLoadDesc(desc->constraints);
    HSD_RObjResolveRefsAll(wobj->robj, desc->constraints);
    return 0;
}

void HSD_WObjInterpretAnim(_HSD_WObj *wobj)
{
    if (wobj) {
        HSD_AObjInterpretAnim(wobj->aobj, wobj, wobj->class_info->update);
        HSD_RObjAnimAll(wobj->robj);
    }
}

static inline void WObjSetPositionX(_HSD_WObj *wobj, f32 x)
{
    if (!wobj) return;
    WObjPositionToWorld(wobj);
    wobj->position.x = x;
    wobj->flags |= WOBJ_POSITION_DIRTY;
}

static inline void WObjSetPositionY(_HSD_WObj *wobj, f32 y)
{
    if (!wobj) return;
    WObjPositionToWorld(wobj);
    wobj->position.y = y;
    wobj->flags |= WOBJ_POSITION_DIRTY;
}

static inline void WObjSetPositionZ(_HSD_WObj *wobj, f32 z)
{
    if (!wobj) return;
    WObjPositionToWorld(wobj);
    wobj->position.z = z;
    wobj->flags |= WOBJ_POSITION_DIRTY;
}

void WObjUpdateFunc(void *object, u32 type, HSD_ObjData *value)
{
    _HSD_WObj *wobj = object;
    _HSD_JObj *jp;
    Vec position;
    if (!wobj) return;
    switch (type) {
    case HSD_A_W_PATH:
        if (value->scalar < 0.0f) value->scalar = 0.0f;
        if (1.0f < value->scalar) value->scalar = 1.0f;
        HSD_ASSERTLINE(152, wobj->aobj);
        jp = wobj->aobj->mObject;
        HSD_ASSERTLINE(154, jp);
        HSD_ASSERTLINE(155, jp->u.spline);
        splArcLengthPoint(&position, jp->u.spline, value->scalar);
        HSD_WObjSetPosition(wobj, (_HSD_Vec3 *)&position);
        wobj->flags |= WOBJ_POSITION_JOINT;
        break;
    case HSD_A_W_X:
        WObjSetPositionX(wobj, value->scalar);
        break;
    case HSD_A_W_Y:
        WObjSetPositionY(wobj, value->scalar);
        break;
    case HSD_A_W_Z:
        WObjSetPositionZ(wobj, value->scalar);
        break;
    }
}

void HSD_WObjAddAnim(_HSD_WObj *wobj, _HSD_WObjAnim *anim)
{
    if (wobj && anim) {
        if (wobj->aobj) HSD_AObjRemove(wobj->aobj);
        wobj->aobj = HSD_AObjLoadDesc(anim->aobjdesc);
        HSD_RObjAddAnimAll(wobj->robj, anim->robj_anim);
    }
}

void HSD_WObjReqAnim(_HSD_WObj *wobj, f32 frame)
{
    if (wobj) {
        HSD_AObjReqAnim(wobj->aobj, frame);
        HSD_RObjReqAnimAll(wobj->robj, frame);
    }
}

void HSD_WObjRemoveAnim(_HSD_WObj *wobj)
{
    if (wobj) {
        HSD_AObjRemove(wobj->aobj);
        wobj->aobj = NULL;
        HSD_RObjRemoveAnimAll(wobj->robj);
    }
}
