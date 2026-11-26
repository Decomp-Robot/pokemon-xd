#include <GSmodel.h>
#include <GSmodelInternal.h>

#include <HSD.h>
#include <hsd/assert.h>
#include <hsd/dobj.h>
#include <hsd/jobj.h>
#include <hsd/list.h>
#include <hsd/mtx.h>
#include <hsd/pobj.h>

#include <libc/memory.h>
#include <libc/string.h>

static inline void modelParseLoadRigidMatrix(GSmtx *positionMtx,
                                             GSmtx *mtxStack)
{
    _HSD_JObj *current;
    void *obj;
    HSD_PObjMtxMarkState mark;

    current = HSD_JObjGetCurrent();

    HSD_PObjGetMtxMark(HSD_POBJ_MTX_MARK_POSITION, &obj, &mark);

    if (obj != current || (u32)mark != HSD_POBJ_MTX_MARK_VALID) {
        HSD_PObjSetMtxMark(HSD_POBJ_MTX_MARK_POSITION, current,
                           HSD_POBJ_MTX_MARK_VALID);

        PSMTXCopy(positionMtx->m, mtxStack[0].m);
    }
}

static inline void modelParseLoadSharedVtxMatrix(_HSD_PObj *pobj,
                                                 GSmtx *referenceMtx,
                                                 GSmtx *positionMtx,
                                                 GSmtx *mtxStack)
{
    _HSD_JObj *current;
    void *obj;
    HSD_PObjMtxMarkState mark;

    bool positionChanged = false;
    bool jobjChanged = false;

    current = HSD_JObjGetCurrent();

    HSD_PObjGetMtxMark(HSD_POBJ_MTX_MARK_POSITION, &obj, &mark);
    if (obj != current && (u32)mark != HSD_POBJ_MTX_MARK_VALID) {
        positionChanged = true;
    }

    HSD_PObjSetMtxMark(HSD_POBJ_MTX_MARK_POSITION, current,
                       HSD_POBJ_MTX_MARK_VALID);

    HSD_PObjGetMtxMark(HSD_POBJ_MTX_MARK_JOINT, &obj, &mark);
    if (obj != pobj->u.jobj && (u32)mark != HSD_POBJ_MTX_MARK_VALID) {
        jobjChanged = true;
    }

    HSD_PObjSetMtxMark(HSD_POBJ_MTX_MARK_JOINT, pobj->u.jobj,
                       HSD_POBJ_MTX_MARK_VALID);

    if (positionChanged || jobjChanged) {
        if (positionChanged) {
            PSMTXCopy(positionMtx->m, mtxStack[0].m);
        }

        if (jobjChanged) {
            GSmtx workMtx;

            HSD_JObjSetupMatrix(pobj->u.jobj);

            PSMTXConcat(referenceMtx->m, pobj->u.jobj->mtx, workMtx.m);

            PSMTXCopy(workMtx.m, mtxStack[1].m);
        }
    }
}

void _modelParseLoadEnvelopeMatrix(_HSD_PObj *pobj,
                                   GSmtx *referenceMtx,
                                   GSmtx *positionMtx,
                                   GSmtx *mtxStack)
{
    _HSD_JObj *current;
    HSD_SList *node;
    int i;
    MtxPtr envelopeMtx;
    GSmtx nodeMtx;
    GSmtx mtx;
    GSmtx workMtx;

    current = HSD_JObjGetCurrent();

    HSD_PObjClearMtxMark(NULL, HSD_POBJ_MTX_MARK_INVALID);

    envelopeMtx = _HSD_mkEnvelopeModelNodeMtx(current, nodeMtx.m);

    for (i = 0, node = pobj->u.envelope_list;
         i < 10 && node != NULL;
         node = node->next, i++) {
        HSD_Envelope *envelope;
        MtxPtr mtxPtr;

        envelope = (HSD_Envelope *)node->data;

        HSD_ASSERT(envelope);
        if (envelope->weight >= 1.0f) {
            HSD_JObjSetupMatrix(envelope->jobj);
            if (envelopeMtx != NULL) {
                PSMTXConcat(envelope->jobj->mtx, envelope->jobj->envelopemtx, mtx.m);
                mtxPtr = mtx.m;
            } else {
                mtxPtr = envelope->jobj->mtx;
            }
        } else {
            mtx.m[0][0] =
            mtx.m[0][1] =
            mtx.m[0][2] =
            mtx.m[0][3] =
            mtx.m[1][0] =
            mtx.m[1][1] =
            mtx.m[1][2] =
            mtx.m[1][3] =
            mtx.m[2][0] =
            mtx.m[2][1] =
            mtx.m[2][2] =
            mtx.m[2][3] = 0.0f;

            while (envelope != NULL) {
                _HSD_JObj *jp;
                HSD_ASSERT(envelope->jobj);
                jp = envelope->jobj;
                HSD_JObjSetupMatrix(jp);
                HSD_ASSERT(jp->mtx);
                HSD_ASSERT(jp->envelopemtx);
                PSMTXConcat(jp->mtx, jp->envelopemtx, workMtx.m);
                HSD_MtxScaledAdd(workMtx.m, mtx.m, mtx.m, envelope->weight);
                envelope = envelope->next;
            }
            mtxPtr = mtx.m;
        }
        if (envelopeMtx != NULL) {
            PSMTXConcat(mtxPtr, envelopeMtx, mtx.m);
        }
        PSMTXConcat(referenceMtx->m, mtxPtr, workMtx.m);
        PSMTXCopy(workMtx.m, mtxStack[i].m);
    }
}

static inline void modelParsePObjDisp(_HSD_PObj *pobj, GSmtx *referenceMtx,
                                      GSmtx *positionMtx, bool instance,
                                      GSmodelParseCb cb, void *arg)
{
    static GSmtx mtxStack[10];

    if (pobj->mFlags & HSD_POBJ_HIDDEN) {
        return;
    }
    if (instance) {
        switch (pobj->mFlags & HSD_POBJ_TYPE_MASK) {
        case HSD_POBJ_SKIN:
            if (pobj->u.jobj == NULL) {
                modelParseLoadRigidMatrix(positionMtx, mtxStack);
            } else {
                modelParseLoadSharedVtxMatrix(pobj, referenceMtx, positionMtx, mtxStack);
            }
            break;
        case HSD_POBJ_SHAPE_ANIM:
            modelParseLoadRigidMatrix(positionMtx, mtxStack);
            break;
        case HSD_POBJ_ENVELOPE:
            _modelParseLoadEnvelopeMatrix(pobj, referenceMtx, positionMtx, mtxStack);
            break;
        }
    }
    if ((pobj->mFlags & HSD_POBJ_TYPE_MASK) == HSD_POBJ_SHAPE_ANIM) {
        cb(pobj, referenceMtx, positionMtx, NULL, arg);
    } else if (cb != NULL) {
        if (instance) {
            cb(pobj, referenceMtx, positionMtx, mtxStack, arg);
        } else {
            cb(pobj, referenceMtx, positionMtx, NULL, arg);
        }
    }
}

void _modelParseDObjDisp(_HSD_DObj *dobj, GSmtx *referenceMtx, GSmtx *positionMtx,
                         bool instance, GSmodelParseCb cb, void *arg)
{
    _HSD_PObj *pobj;

    for (pobj = dobj->mPrimitive; pobj != NULL; pobj = pobj->mNext) {
        modelParsePObjDisp(pobj, referenceMtx, positionMtx, instance, cb, arg);
    }
}

void _modelParseJObjDispSub(_HSD_JObj *jobj, GSmtx *referenceMtx, GSmtx *positionMtx,
                            HSD_TrspMask trspmask, bool instance, GSmodelParseCb cb,
                            void *arg)
{
    _HSD_DObj *dobj;
    u32 dobjMask;

    HSD_JObjSetCurrent(jobj);
    dobjMask = trspmask << HSD_DOBJ_TRSP_SHIFT;
    HSD_PObjClearMtxMark(NULL, HSD_POBJ_MTX_MARK_NONE);

    for (dobj = jobj->u.mDrawable; dobj != NULL; dobj = dobj->mNext) {
        if (dobj->mFlags & HSD_DOBJ_HIDDEN) {
            continue;
        }
        if (!(dobj->mFlags & dobjMask)) {
            continue;
        }
        HSD_DObjSetCurrent(dobj);
        _modelParseDObjDisp(dobj, referenceMtx, positionMtx, instance, cb, arg);
    }
    HSD_DObjSetCurrent(NULL);
    HSD_JObjSetCurrent(NULL);
}

void _modelParseJObjDispDObj(_HSD_JObj *jobj, GSmtx *mtx, HSD_TrspMask trspmask,
                             bool instance, GSmodelParseCb cb, void *arg)
{
    GSmtx positionMtx;
    u32 trsp;

    if (jobj->flags & HSD_JOBJ_FLAG_HIDDEN) {
        return;
    }
    trsp = jobj->flags & (trspmask << HSD_JOBJ_TRSP_SHIFT);
    if (trsp == 0) {
        return;
    }
    HSD_JObjSetupMatrix(jobj);
    if (mtx == NULL) {
        static GSmtx foo;

        memcpy(&foo, mtxIdent, sizeof(foo));
        mtx = &foo;
    }
    HSD_JObjMakePositionMtx(jobj, mtx->m, positionMtx.m);

    if (trsp & (HSD_TRSP_OPA << HSD_JOBJ_TRSP_SHIFT)) {
        _modelParseJObjDispSub(jobj, mtx, &positionMtx, HSD_TRSP_OPA, instance, cb, arg);
    }
    if (trsp & (HSD_TRSP_TEXEDGE << HSD_JOBJ_TRSP_SHIFT)) {
        _modelParseJObjDispSub(jobj, mtx, &positionMtx, HSD_TRSP_TEXEDGE, instance, cb,
                               arg);
    }
    if (trsp & (HSD_TRSP_XLU << HSD_JOBJ_TRSP_SHIFT)) {
        _modelParseJObjDispSub(jobj, mtx, &positionMtx, HSD_TRSP_XLU, instance, cb, arg);
    }
}

void _modelParseJObjDisp(_HSD_JObj *jobj, GSmtx *mtx, HSD_TrspMask trspmask,
                         bool instance, GSmodelParseCb cb, void *arg)
{
    if (jobj != NULL && JOBJ_DRAWABLE(jobj)) {
        _modelParseJObjDispDObj(jobj, mtx, trspmask, instance, cb, arg);
    }
}

void _modelParseSetupInstanceMtx(GSmtx *mtx, _HSD_JObj *jobj, GSmtx *instanceMtx)
{
    HSD_JObjSetupMatrix(jobj);
    HSD_JObjSetupMatrix(jobj->child);

    PSMTXInverse(jobj->child->mtx, instanceMtx->m);
    PSMTXConcat(jobj->mtx, instanceMtx->m, instanceMtx->m);
    if (mtx) {
        PSMTXConcat(mtx->m, instanceMtx->m, instanceMtx->m);
    }
}

void _modelParseJObjDispAll(_HSD_JObj *jobj, GSmtx *mtx, HSD_TrspMask trspmask,
                            bool instance, GSmodelParseCb cb, void *arg)
{
    if (jobj == NULL) {
        return;
    }
    if (jobj->flags & HSD_JOBJ_TRAVERSAL_BARRIER) {
        if (jobj->flags & HSD_JOBJ_FLAG_HIDDEN) {
            return;
        }
        GSmtx instanceMtx;
        _modelParseSetupInstanceMtx(mtx, jobj, &instanceMtx);
        _modelParseJObjDispAll(jobj->child, &instanceMtx, trspmask, instance, cb, arg);
        return;
    }
    if (jobj->flags & (trspmask << HSD_JOBJ_TRSP_SHIFT)) {
        _modelParseJObjDisp(jobj, mtx, trspmask, instance, cb, arg);
    }
    if (jobj->flags & (trspmask << HSD_JOBJ_CHILD_TRSP_SHIFT)) {
        for (jobj = jobj->child; jobj != NULL; jobj = jobj->next) {
            _modelParseJObjDispAll(jobj, mtx, trspmask, instance, cb, arg);
        }
    }
}

void GSmodelParse(_GSmodel *model, bool instance, GSmodelParseCb cb, void *userContext)
{
    _modelParseJObjDispAll(modelGetRenderJObj(model), NULL,
                          (HSD_TrspMask)(HSD_TRSP_OPA | HSD_TRSP_XLU | HSD_TRSP_TEXEDGE),
                          instance, cb, userContext);
}
