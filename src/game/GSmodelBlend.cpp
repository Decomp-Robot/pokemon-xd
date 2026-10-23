#include <GSmodelInternal.h>

/* Clear blending and release both joint copies if loaded. */
extern "C" void GSmodelDisableAnimBlend(_GSmodel *model)
{
    f32 rate = GSmodelGetAnimRate(model);
    model->mFlags &= ~MODEL_FLAG_BLEND_ACTIVE;
    if (model->mBlendJObjTo != NULL) {
        GSmodelSetAnimRate(model, rate);
        GSgfxRenderWaitForDrawComplete();
        HSD_JObjUnref(model->mBlendJObj);
        HSD_JObjUnref(model->mBlendJObjTo);
        model->mBlendJObj = NULL;
        model->mBlendJObjTo = NULL;
    }
}

extern "C" void GSmodelEnableAnimBlend(_GSmodel *model)
{
    if (model->mBlendJObjTo == NULL) {
        model->mBlendJObj = HSD_JObjLoadJoint(model->mRoot->mJoint);
        model->mBlendJObjTo = HSD_JObjLoadJoint(model->mRoot->mJoint);
        _modelSetRotateEulerToQuatAll(model->mBlendJObjTo);
    }
}

void modelCalculateBlendModel(_GSmodel *model, f32 factor)
{
    if (model->mBlendJObjTo == NULL) {
        return;
    }
    _modelIntpJObjAll(model, model->mJObj,
                      model->mBlendJObj,
                      model->mBlendJObjTo,
                      model->mBlendFactor, factor);
    model->mFlags &= ~MODEL_FLAG_TRANSFORM_UPDATED;
}

/* Blends three whole joint poses, driven by the per-part mix table of
 * the model. A joint whose animation object is named in the table is
 * marked so the blend is skipped for it, and the rest are interpolated
 * between the first and the second source with the two factors. */
void _modelIntpJObjAll(_GSmodel *model, _HSD_JObj *firstPose, _HSD_JObj *secondPose,
                     _HSD_JObj *resultPose, f32 blendFactor, f32 animationRate)
{
    void *animationObject;
    u8 partAnimationMatch;
    _GSmodel *blendModel = model;
    _HSD_JObj *firstJoint = (_HSD_JObj *)firstPose;
    _HSD_JObj *secondJoint = (_HSD_JObj *)secondPose;
    _HSD_JObj *resultJoint = (_HSD_JObj *)resultPose;

    if (firstJoint == NULL || secondJoint == NULL || resultJoint == NULL) {
        return;
    }
    animationObject = resultJoint->mAObj;
    partAnimationMatch = 0;
    if (animationObject != NULL) {
        s32 partAnimationIndex;

        for (partAnimationIndex = 0; partAnimationIndex < 8; partAnimationIndex++) {
            if (blendModel->mPartAnim[partAnimationIndex].mAnimation == animationObject) {
                partAnimationMatch = GS_PART_ANIM_MIXED;
                break;
            }
        }
    }
    if (partAnimationMatch != 0) {
        HSD_JObjClearFlags(resultJoint, HSD_JOBJ_STATE_EULER_QUAT);
        HSD_AObjSetRate(resultJoint->mAObj, animationRate);
        HSD_JObjAnim(resultJoint);
    } else {
        _modelIntpJObj(firstJoint, secondJoint, resultJoint, blendFactor);
    }
    if (firstJoint->flags & HSD_JOBJ_TRAVERSAL_BARRIER) {
        return;
    }
    {
        _HSD_JObj *firstChild = firstJoint->child;
        _HSD_JObj *secondChild = secondJoint->child;
        _HSD_JObj *resultChild = resultJoint->child;

        while (firstChild != NULL) {
            _modelIntpJObjAll(model, firstChild, secondChild, resultChild, blendFactor, animationRate);
            firstChild = firstChild->next;
            secondChild = secondChild->next;
            resultChild = resultChild->next;
        }
    }
}

static inline void modelIntpScale(_HSD_JObj *firstJoint, _HSD_JObj *secondJoint, _HSD_JObj *resultJoint, f32 blendFactor, f32 firstWeight,
                      GSvec *part, unsigned long flags)
{
    Vec firstScale;
    Vec secondScale;
    Vec scale;

    HSD_JObjGetScale(firstJoint, &firstScale);
    HSD_JObjGetScale(secondJoint, &secondScale);
    scale.x = firstScale.x * firstWeight + secondScale.x * blendFactor;
    scale.y = firstScale.y * firstWeight + secondScale.y * blendFactor;
    scale.z = firstScale.z * firstWeight + secondScale.z * blendFactor;
    if (part != NULL) {
        modelCalculatePartMix((GSvec *)&scale, part, flags);
    }
    HSD_JObjSetScale(resultJoint, &scale);
}

static inline void modelIntpTrans(_HSD_JObj *firstJoint, _HSD_JObj *secondJoint, _HSD_JObj *resultJoint, f32 blendFactor, f32 firstWeight,
                      GSvec *part, unsigned long flags)
{
    Vec firstTranslation;
    Vec secondTranslation;
    Vec translation;

    HSD_JObjGetTranslation(firstJoint, &firstTranslation);
    HSD_JObjGetTranslation(secondJoint, &secondTranslation);
    translation.x = firstTranslation.x * firstWeight + secondTranslation.x * blendFactor;
    translation.y = firstTranslation.y * firstWeight + secondTranslation.y * blendFactor;
    translation.z = firstTranslation.z * firstWeight + secondTranslation.z * blendFactor;
    if (part != NULL) {
        modelCalculatePartMix((GSvec *)&translation, part, flags);
    }
    HSD_JObjSetTranslation(resultJoint, &translation);
}

/* Interpolates a whole joint: translation and scale in place, then the
 * rotation through the quaternion path. */
void _modelIntpJObj(_HSD_JObj *firstJoint, _HSD_JObj *secondJoint, _HSD_JObj *resultJoint, f32 blendFactor)
{
    f32 firstWeight = 1.0f - blendFactor;
    modelIntpTrans(firstJoint, secondJoint, resultJoint, blendFactor, firstWeight, NULL, 0);
    modelIntpScale(firstJoint, secondJoint, resultJoint, blendFactor, firstWeight, NULL, 0);
    modelIntpJObjRotate(firstJoint, secondJoint, resultJoint, blendFactor, NULL, 0);
}

/* Interpolates the rotation of one joint between two poses. */
void modelIntpJObjRotate(_HSD_JObj *firstJoint, _HSD_JObj *secondJoint, _HSD_JObj *resultJoint, f32 blendFactor,
                         GSvec *part, unsigned long flags)
{
    Vec firstEuler;
    Vec secondEuler;
    Quaternion rotation;
    Quaternion firstRotation;
    Quaternion secondRotation;
    f32 squaredDifference[4];
    f32 squaredSum[4];

    HSD_JObjGetRotation(firstJoint, &firstRotation);
    HSD_JObjGetRotation(secondJoint, &secondRotation);
    firstEuler.x = firstRotation.x;
    firstEuler.y = firstRotation.y;
    firstEuler.z = firstRotation.z;
    secondEuler.x = secondRotation.x;
    secondEuler.y = secondRotation.y;
    secondEuler.z = secondRotation.z;
    if (part != NULL) {
        modelCalculatePartMix((GSvec *)&firstEuler, part, flags);
        modelCalculatePartMix((GSvec *)&secondEuler, part, flags);
    }
    EulerToQuat(&firstEuler, &firstRotation);
    EulerToQuat(&secondEuler, &secondRotation);

    /* Choose the nearer of the two equivalent quaternion representations. */
    squaredDifference[0] = (firstRotation.x - secondRotation.x) * (firstRotation.x - secondRotation.x);
    squaredSum[0] = (firstRotation.x + secondRotation.x) * (firstRotation.x + secondRotation.x);
    squaredDifference[1] = (firstRotation.y - secondRotation.y) * (firstRotation.y - secondRotation.y);
    squaredSum[1] = (firstRotation.y + secondRotation.y) * (firstRotation.y + secondRotation.y);
    squaredDifference[2] = (firstRotation.z - secondRotation.z) * (firstRotation.z - secondRotation.z);
    squaredSum[2] = (firstRotation.z + secondRotation.z) * (firstRotation.z + secondRotation.z);
    squaredDifference[3] = (firstRotation.w - secondRotation.w) * (firstRotation.w - secondRotation.w);
    squaredSum[3] = (firstRotation.w + secondRotation.w) * (firstRotation.w + secondRotation.w);
    if (squaredDifference[0] + squaredDifference[1] + squaredDifference[2] + squaredDifference[3] >
        squaredSum[0] + squaredSum[1] + squaredSum[2] + squaredSum[3]) {
        secondRotation.x = -secondRotation.x;
        secondRotation.y = -secondRotation.y;
        secondRotation.z = -secondRotation.z;
        secondRotation.w = -secondRotation.w;
    }

    QuatSlerp(&firstRotation, &secondRotation, &rotation, blendFactor);
    HSD_JObjSetRotation(resultJoint, &rotation);
}

/* Interpolates the scale of one joint between two poses. */
void modelIntpJObjScale(_HSD_JObj *firstJoint, _HSD_JObj *secondJoint, _HSD_JObj *resultJoint, f32 blendFactor,
                      GSvec *part, unsigned long flags)
{
    f32 firstWeight = 1.0f - blendFactor;
    modelIntpScale(firstJoint, secondJoint, resultJoint, blendFactor, firstWeight, part, flags);
}

/* Interpolates the translation of one joint between two poses. */
void modelIntpJObjTrans(_HSD_JObj *firstJoint, _HSD_JObj *secondJoint, _HSD_JObj *resultJoint, f32 blendFactor,
                      GSvec *part, unsigned long flags)
{
    f32 firstWeight = 1.0f - blendFactor;
    modelIntpTrans(firstJoint, secondJoint, resultJoint, blendFactor, firstWeight, part, flags);
}

/* Walks the joint tree below 'jobj' and marks every joint whose rotation
 * still holds Euler angles, so the model can convert them in place. The
 * recursion follows the child chain first and then the sibling chain. */
void _modelSetRotateEulerToQuatAll(_HSD_JObj *jobj)
{
    _HSD_JObj *child;


    if (jobj == NULL) {
        return;
    }
    HSD_JObjSetFlags(jobj, HSD_JOBJ_FLAG_EULER_QUAT);
    if (jobj->flags & HSD_JOBJ_TRAVERSAL_BARRIER) {
        return;
    }
    child = jobj->child;
    while (child != NULL) {
        _modelSetRotateEulerToQuatAll(child);
        child = child->next;
    }
}
