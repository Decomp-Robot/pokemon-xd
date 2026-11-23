#include <GSmodelInternal.h>
#include <hsd/jobj.h>

u32 _modelCurPartIdx;
u32 _modelDestPartIdx;
_HSD_JObj *_modelFoundJObj;

void _modelFindJObj(_HSD_JObj *, void **, int);

s32 GSmodelGetPartCount(_GSmodel *model)
{
    _modelCurPartIdx = 0;
    _modelDestPartIdx = -1;
    HSD_JObjWalkTree(model->mJObj, _modelFindJObj, NULL);
    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
        _modelCurPartIdx--;
    }
    return _modelCurPartIdx;
}

GSpart *GSmodelGetPart(_GSmodel *model, s32 partIndex)
{
    s32 index = partIndex;
    _HSD_JObj *joint;
    GSpart *part;

    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
        index++;
    }
    joint = GSpartGetJObjPtr(modelGetRenderJObj(model), index);
    if (joint == NULL) {
        return NULL;
    }
    part = GSpartCreate();
    if (part == NULL) {
        return NULL;
    }
    part->mModel = model;
    part->mJoint = joint;
    part->mIndex = partIndex;
    return part;
}

void _modelFindJObj(_HSD_JObj *jobj, void **arg, int index)
{
    if (_modelCurPartIdx++ == _modelDestPartIdx) {
        _modelFoundJObj = jobj;
    }
}
