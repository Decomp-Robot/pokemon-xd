#include <GSmodelInternal.h>
#include <GSquat.h>
#include <libc/math.h>


/* Drops the root-joint animation: the removed flag is raised, and the
 * root joint stops driving the pose. */
extern "C" void GSmodelRemoveRootJointAnimation(_GSmodel *model)
{
    model->mFlags |= MODEL_FLAG_ROOT_ANIM_REMOVED;
    model->mFlags = model->mFlags & ~MODEL_FLAG_ROOT_JOINT_POSE;
}

/* Restores the root-joint animation the two calls above toggle: with
 * 'use' set, the root joint drives the pose again and the removed flag
 * is cleared. */
extern "C" void GSmodelUseRootJointAnimation(_GSmodel *model, u8 use)
{
    if (use) {
        model->mFlags |= MODEL_FLAG_ROOT_JOINT_POSE;
        model->mFlags &= ~MODEL_FLAG_ROOT_ANIM_REMOVED;
    } else {
        model->mFlags &= ~MODEL_FLAG_ROOT_JOINT_POSE;
    }
}

extern "C" s32 GSmodelIsNullAnimation(_GSmodel *model, s32 index)
{
    _HSD_AnimJoint **table;
    _HSD_AnimJoint *anim;
    _HSD_SceneModelSet *root;

    if (model == NULL || (root = model->mRoot) == NULL) {
        return 1;
    }
    if ((u32)index >= (u32)model->mMaxAnimIndex) {
        return 1;
    }
    table = root->mAnimations;
    anim = table[index];
    if (anim->mChild == NULL && anim->mNext == NULL && anim->mAnimation == NULL &&
        anim->mRObjAnimation == NULL && anim->mFlags == 0) {
        return 1;
    }
    return 0;
}

extern "C" f32 GSmodelGetAnimFrameCount(_GSmodel *model, s32 index)
{
    f32 last;

    if (!(model->mFlags & MODEL_FLAG_ANIM_PRESENT)) {
        return 0.0f;
    }
    last = 0.0f;
    {
        _HSD_AnimJoint **table = model->mRoot->mAnimations;

        GSmodelTraverseAnimJoint(table[index], _modelGetAnimFrameCountCB, &last);
    }
    return 1.0f + last;
}

void _modelGetAnimFrameCountCB(_HSD_AnimJoint *joint, void **argp, int idx)
{
    _HSD_AObjDesc *anim;
    f32 *last = (f32 *)argp;

    if (joint == NULL || (anim = joint->mAnimation) == NULL) {
        return;
    }
    if (anim->mEndFrame > *last) {
        *last = anim->mEndFrame;
    }
}

extern "C" void GSmodelForceAnimTransformUpdate(_GSmodel *model)
{
    u32 token;

    model->mFlags &= MODEL_FLAG_FORCE_UPDATE_MASK;
    token = GSgfxPerfbarCPUEntry(GSGFX_PERFBAR_MODEL_UPDATE);
    GSmodelUpdateAnimation(model);
    modelUpdateAttachments(model);
    modelUpdateTransform(model);
    modelApplyPartAnimMixes(model);
    GSgfxPerfbarCPUEntry(token);
}

extern "C" void GSmodelSetAnimEndedCallback(_GSmodel *model, GSmodelAnimEndedCallback cb, void *arg)
{
    model->mAnimEndedCB = cb;
    model->mAnimEndedCBArg = arg;
}

extern "C" void GSmodelSetForceUseFractionalFramesFlag(_GSmodel *model, u8 on)
{
    if (on == 1) {
        model->mAnimFlags |= MODEL_ANIM_FLAG_USE_FRACTIONAL;
    } else {
        model->mAnimFlags &= ~MODEL_ANIM_FLAG_USE_FRACTIONAL;
    }
}

extern "C" void GSmodelSetSkipFractionalFramesFlag(_GSmodel *model, u8 on)
{
    if (on == 1) {
        model->mAnimFlags |= MODEL_ANIM_FLAG_SKIP_FRACTIONAL;
    } else {
        model->mAnimFlags &= ~MODEL_ANIM_FLAG_SKIP_FRACTIONAL;
    }
}

extern "C" void GSmodelSet60fpsAnimFlag(_GSmodel *model, u8 on)
{
    if (on == 1) {
        model->mFlags |= MODEL_FLAG_60FPS_ANIM;
    } else {
        model->mFlags &= ~MODEL_FLAG_60FPS_ACTIVE;
    }
}

extern "C" void GSmodelLinkTexAnimToAnim(_GSmodel *model, u8 link)
{
    if (link) {
        model->mFlags |= MODEL_FLAG_TEXANIM_LINKED;
    } else {
        model->mFlags &= ~MODEL_FLAG_TEXANIM_ACTIVE;
    }
}

extern "C" s32 GSmodelGetMaxTexAnimIndex(_GSmodel *model)
{
    return model->mMaxTexAnimIndex;
}

extern "C" s32 GSmodelGetMaxAnimIndex(_GSmodel *model)
{
    return model->mMaxAnimIndex;
}



extern "C" u8 GSmodelCanShapeAnimate(_GSmodel *model)
{
    return (model->mFlags & MODEL_FLAG_SHAPE_ANIM_PRESENT) != 0;
}

extern "C" u8 GSmodelCanTexAnimate(_GSmodel *model)
{
    return (model->mFlags & MODEL_FLAG_TEXANIM_PRESENT) != 0;
}

extern "C" u8 GSmodelCanAnimate(_GSmodel *model)
{
    return (model->mFlags & MODEL_FLAG_ANIM_PRESENT) != 0;
}

extern "C" u8 GSmodelHasTexAnimationEnded(_GSmodel *model)
{
    return (model->mFlags & MODEL_FLAG_TEXANIM_ENDED) != 0;
}

extern "C" void GSmodelStopTexAnimation(_GSmodel *model)
{
    model->mFlags &= ~MODEL_FLAG_TEXANIM_RUNNING;
}

extern "C" void GSmodelStartTexAnimation(_GSmodel *model)
{
    if (model->mFlags & MODEL_FLAG_TEXANIM_PRESENT) {
        model->mFlags |= MODEL_FLAG_TEXANIM_RUNNING;
        model->mFlags &= ~MODEL_FLAG_TEXANIM_ENDED;
    }
}

extern "C" void GSmodelSetTexAnimType(_GSmodel *model, s32 type)
{
    modelSetTextureAnimType(model, type);
}

extern "C" void GSmodelSetTexAnimFrame(_GSmodel *model, f32 frame)
{
    modelSetTextureAnimFrame(model, frame);
}

extern "C" f32 GSmodelGetTexAnimRate(_GSmodel *model)
{
    f32 rate;
    if (!(model->mFlags & MODEL_FLAG_TEXANIM_PRESENT)) {
        return 0.0f;
    }
    rate = model->mTexAnim.mRate;
    if (GSgfxVideoGetVsyncRate() == 50) {
        rate /= 1.2f;
    }
    return rate;
}

extern "C" void GSmodelSetTexAnimRate(_GSmodel *model, f32 rate)
{
    if (model->mFlags & MODEL_FLAG_TEXANIM_PRESENT) {
        model->mTexAnim.mRate = rate;
        if (GSgfxVideoGetVsyncRate() == 50) {
            model->mTexAnim.mRate *= 1.2f;
        }
    }
}

extern "C" void GSmodelSetTexAnimIndex(_GSmodel *model, unsigned long index)
{
    modelSetTextureAnimationIndex(model, index);
}

extern "C" void GSmodelSetPartTexAnimIndex(_GSmodel *model, s32 partIndex,
                                           s32 materialIndex, s32 animIndex,
                                           s32 animType, f32 frame)
{
    GSpart *part = GSmodelGetPart(model, partIndex);
    if (part != 0) {
        GSpartSetTexAnimIndex(part, materialIndex, animIndex, animType, frame);
        GSpartFree(part);
    }
}

extern "C" void GSmodelGetFrameCount(_GSmodel *model, f32 *frameCount,
                                     f32 *blendFrameCount)
{
    if (!(model->mFlags & MODEL_FLAG_BLEND_ACTIVE)) {
        if (frameCount != 0) {
            *frameCount = 1.0f + model->mAnim.mEndFrame;
        }
        if (blendFrameCount != 0) {
            *blendFrameCount = -1.0f;
        }
    } else {
        if (frameCount != 0) {
            *frameCount = 1.0f + model->mAnim.mEndFrame;
        }
        if (blendFrameCount != 0) {
            *blendFrameCount = 1.0f + model->mBlendAnim.mEndFrame;
        }
    }
}

extern "C" f32 GSmodelGetAnimFrame(_GSmodel *model)
{
    if (!(model->mFlags & MODEL_FLAG_ANIM_PRESENT)) {
        return 0.0f;
    }
    if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE) {
        return model->mBlendAnim.mFrame;
    }
    return model->mAnim.mFrame;
}

extern "C" s32 GSmodelGetAnimType(_GSmodel *model)
{
    return model->mAnim.mType;
}

extern "C" f32 GSmodelGetAnimRate(_GSmodel *model)
{
    return modelGetEffectiveAnimationRate(model);
}

extern "C" void GSmodelGetAnimIndex(_GSmodel *model, unsigned long *index,
                                    unsigned long *blendIndex)
{
    if (!(model->mFlags & MODEL_FLAG_BLEND_ACTIVE)) {
        if (index != 0) {
            *index = model->mAnim.mIndex;
        }
        if (blendIndex != 0) {
            *blendIndex = -1;
        }
    } else {
        if (index != 0) {
            *index = model->mAnim.mIndex;
        }
        if (blendIndex != 0) {
            *blendIndex = model->mBlendAnim.mIndex;
        }
    }
}

extern "C" u8 GSmodelIsBlending(_GSmodel *model)
{
    return (model->mFlags & MODEL_FLAG_BLEND_ACTIVE) != 0;
}

extern "C" void GSmodelSetBlendFactor(_GSmodel *model, f32 factor)
{
    if (!(model->mFlags & MODEL_FLAG_ANIM_PRESENT) ||
        !(model->mFlags & MODEL_FLAG_BLEND_ACTIVE)) return;

    if (factor < 0.0f) {
        factor = 0.0f;
    } else if (factor > 1.0f) {
        factor = 1.0f;
    }
    model->mBlendFactor = factor;
}


extern "C" void GSmodelSetBlendAnimIndex(_GSmodel *model, unsigned long fromIndex,
    unsigned long toIndex)
{
    if (model->mFlags & MODEL_FLAG_ANIM_PRESENT) {
        if (fromIndex > (u32)model->mMaxAnimIndex || toIndex > (u32)model->mMaxAnimIndex)
        return;
        if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT)
        GSmodelRemoveNull(model);
        u8 fromEmpty = modelAnimationIsEmpty(model, fromIndex);
        u8 toEmpty = modelAnimationIsEmpty(model, toIndex);
        if (fromEmpty == 1 && toEmpty == 1) return;
        f32 rate = modelGetEffectiveAnimationRate(model);
        model->mFlags |= MODEL_FLAG_BLEND_ACTIVE;
        model->mAnim.mIndex = fromIndex;
        model->mBlendAnim.mIndex = toIndex;
        model->mBlendFactor = 0.0f;
        HSD_JObjAddAnimAll(model->mJObj, modelResource(model)->mAnimations[model->mAnim.mIndex], NULL, NULL);
        HSD_JObjAddAnimAll(model->mBlendJObj, modelResource(model)->mAnimations[model->mBlendAnim.mIndex], NULL, NULL);
        HSD_JObjReqAnimAllByFlags(model->mJObj, MODEL_ANIM_ALL_JOINTS, 0.0f);
        HSD_JObjReqAnimAllByFlags(model->mBlendJObj, MODEL_ANIM_ALL_JOINTS, 0.0f);
        HSD_JObjAnimAll(model->mJObj);
        HSD_JObjAnimAll(model->mBlendJObj);
        model->mAnim.mEndFrame = 0.0f;
        HSD_ForeachAnim(model->mJObj, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK, (void (*)())_modelGetEndFrame,
            HSD_ANIM_ARG_POINTER, &model->mAnim.mEndFrame);
        model->mBlendAnim.mEndFrame = 0.0f;
        HSD_ForeachAnim(model->mBlendJObj, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK, (void (*)())_modelGetEndFrame,
            HSD_ANIM_ARG_POINTER, &model->mBlendAnim.mEndFrame);
        model->mAnim.mAnimation = modelGetAnimationObject(model->mJObj, MODEL_ANIM_JOINT_AOBJ);
        model->mBlendAnim.mAnimation = modelGetAnimationObject(model->mBlendJObj, MODEL_ANIM_JOINT_AOBJ);
        modelSetAnimationRate(model, rate);
        modelSetAnimationType(model, model->mAnim.mType);
        GSmodelSetAnimFrame(model, 0.0f);
        GSvec transform;
        transform.x = 0.0f;
        transform.y = 0.0f;
        transform.z = 0.0f;
        HSD_JObjSetTranslation(model->mJObj, (Vec *)&transform);
        HSD_JObjSetRotationX(model->mJObj, transform.x);
        HSD_JObjSetRotationY(model->mJObj, transform.y);
        HSD_JObjSetRotationZ(model->mJObj, transform.z);
        transform.x = 1.0f;
        transform.y = 1.0f;
        transform.z = 1.0f;
        HSD_JObjSetScale(model->mJObj, (Vec *)&transform);
    }
    if (model->mFlags & MODEL_FLAG_TEXANIM_LINKED) {
        modelSetTextureAnimationIndex(model, toIndex);
    }
    GSmodelForceAnimTransformUpdate(model);
}

extern "C" void GSmodelSetBlendAnimFrameForce(_GSmodel *model, f32 fromFrame, f32 toFrame)
{
    model->mAnim.mRate = 0.0f;
    model->mBlendAnim.mRate = 0.0f;
    model->mAnim.mFrame = fromFrame;
    model->mBlendAnim.mFrame = toFrame;
    model->mAnim.mRequestedFrame = model->mAnim.mFrame;
    model->mBlendAnim.mRequestedFrame = model->mBlendAnim.mFrame;
}

extern "C" void GSmodelAllUnpauseAnimation(void)
{
    for (_GSmodel *model = gsModelList; model; model = model->mNext)
    model->mFlags &= ~MODEL_FLAG_ANIM_PAUSED;
}

extern "C" void GSmodelAllPauseAnimation(void)
{
    for (_GSmodel *model = gsModelList; model; model = model->mNext)
    model->mFlags |= MODEL_FLAG_ANIM_PAUSED;
}

extern "C" u8 GSmodelHasAnimationEnded(_GSmodel *model)
{
    return (model->mFlags & MODEL_FLAG_ANIM_ENDED) != 0;
}

extern "C" bool GSmodelIsAnimating(_GSmodel *model)
{
    return (model->mFlags & MODEL_FLAG_ANIM_RUNNING) != 0;
}

extern "C" void GSmodelStopAnimation(_GSmodel *model)
{
    if (model->mFlags & MODEL_FLAG_ANIM_PRESENT) model->mFlags &= ~MODEL_FLAG_ANIM_RUNNING;
    if (model->mFlags & MODEL_FLAG_TEXANIM_LINKED) model->mFlags &= ~MODEL_FLAG_TEXANIM_RUNNING;
}

extern "C" void GSmodelStartAnimation(_GSmodel *model)
{
    if (model->mFlags & MODEL_FLAG_ANIM_PRESENT) {
        model->mFlags |= MODEL_FLAG_ANIM_RUNNING;
        model->mFlags &= ~MODEL_FLAG_ANIM_ENDED;
    }
    if ((model->mFlags & MODEL_FLAG_TEXANIM_LINKED) && (model->mFlags & MODEL_FLAG_TEXANIM_PRESENT)) {
        model->mFlags |= MODEL_FLAG_TEXANIM_RUNNING;
        model->mFlags &= ~MODEL_FLAG_TEXANIM_ENDED;
    }
}

extern "C" void GSmodelSetAnimRate(_GSmodel *model, f32 rate)
{
    modelSetAnimationRate(model, rate);
}

extern "C" void GSmodelSetAnimFrame(_GSmodel *model, f32 frame)
{
    if (frame < 0.0f) return;
    if (__fpclassifyf(frame) == FP_NAN) return;
    if (model->mFlags & MODEL_FLAG_ANIM_PRESENT) {
        if (!(model->mFlags & MODEL_FLAG_BLEND_ACTIVE)) {
            _HSD_JObj *joint = model->mJObj;
            if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT)
            joint = joint->child;
            model->mAnim.mFrame = frame;
            model->mAnim.mRequestedFrame = model->mAnim.mFrame;
            if (model->mFlags & MODEL_FLAG_PART_ANIM_MIX) {
                modelPausePartAnimations(model, 1);
            }
            HSD_JObjReqAnimAllByFlags(joint, MODEL_ANIM_ALL_JOINTS, model->mAnim.mRequestedFrame);
            HSD_JObjAnimAll(joint);
            if (model->mFlags & MODEL_FLAG_PART_ANIM_MIX) {
                modelPausePartAnimations(model, 0);
            }
        } else {
            model->mAnim.mFrame = frame * (model->mBlendAnim.mRate != 0.0f ? model->mAnim.mRate / model->mBlendAnim.mRate : 1.0f);
            model->mAnim.mRequestedFrame = model->mAnim.mFrame;
            model->mBlendAnim.mFrame = frame;
            model->mBlendAnim.mRequestedFrame = model->mBlendAnim.mFrame;
            HSD_JObjReqAnimAllByFlags(model->mJObj, MODEL_ANIM_ALL_JOINTS, model->mAnim.mRequestedFrame);
            HSD_JObjReqAnimAllByFlags(model->mBlendJObj, MODEL_ANIM_ALL_JOINTS, model->mBlendAnim.mRequestedFrame);
            HSD_JObjAnimAll(model->mJObj);
            HSD_JObjAnimAll(model->mBlendJObj);
        }
    }
    if (model->mFlags & MODEL_FLAG_TEXANIM_LINKED) {
        _HSD_JObj *joint = model->mJObj;
        if (model->mFlags & MODEL_FLAG_TEXANIM_PRESENT) {
            if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT)
            joint = joint->child;
            model->mTexAnim.mFrame = frame;
            model->mTexAnim.mRequestedFrame = frame;
            HSD_JObjReqAnimAllByFlags(joint, MODEL_ANIM_ALL_MATERIALS, model->mTexAnim.mRequestedFrame);
            model->mFlags &= ~MODEL_FLAG_TEXANIM_ENDED;
        }
    }
    model->mFlags &= ~MODEL_FLAG_ANIM_ENDED;
}

extern "C" void GSmodelSetAnimType(_GSmodel *model, s32 type)
{
    if (model->mFlags & MODEL_FLAG_ANIM_PRESENT) {
        _HSD_JObj *joint = model->mJObj;
        _HSD_JObj *blendJoint = model->mBlendJObj;
        if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT)
            joint = joint->child;
        modelSetJointAnimationPolicy(model, joint, blendJoint, type);
    }
    if (model->mFlags & MODEL_FLAG_TEXANIM_LINKED) {
        modelSetMaterialAnimationType(model, type);
    }
}

extern "C" void GSmodelSetAnimIndex(_GSmodel *model, unsigned long index)
{
    if (model->mFlags & MODEL_FLAG_ANIM_PRESENT) {
        if (index >= (u32)model->mMaxAnimIndex) return;
        if (modelAnimationIsEmpty(model, index)) return;
        f32 rate = modelGetEffectiveAnimationRate(model);
        if (index != model->mAnim.mIndex || (model->mFlags & MODEL_FLAG_BLEND_ACTIVE) || model->mHasPartAnim == 1) {
            _HSD_JObj *joint = model->mJObj;
            if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT)
            joint = joint->child;
            model->mFlags &= ~MODEL_FLAG_BLEND_ACTIVE;
            model->mAnim.mIndex = index;
            _HSD_ShapeAnimJoint *shapeAnimation;
            if (model->mFlags & MODEL_FLAG_SHAPE_ANIM_PRESENT) {
                shapeAnimation = modelResource(model)->mShapeAnimations[model->mAnim.mIndex];
            } else {
                shapeAnimation = NULL;
            }
            if (model->mFlags & MODEL_FLAG_PART_ANIM_MIX) {
                modelPausePartAnimations(model, 1);
            }
            _jobjAddAnimAll(joint, modelResource(model)->mAnimations[model->mAnim.mIndex], NULL, shapeAnimation);
            if ((model->mFlags & MODEL_FLAG_ROOT_ANIM_REMOVED) && joint->mAObj)
            HSD_JObjRemoveAnim(joint);
            model->mAnim.mEndFrame = 0.0f;
            HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK, (void (*)())_modelGetEndFrame,
                HSD_ANIM_ARG_POINTER, &model->mAnim.mEndFrame);
            model->mAnim.mAnimation = modelGetAnimationObject(joint, MODEL_ANIM_JOINT_AOBJ);
            if (model->mFlags & MODEL_FLAG_PART_ANIM_MIX) {
                modelPausePartAnimations(model, 0);
            }
        }
        GSmodelSetAnimFrame(model, 0.0f);
        modelSetAnimationType(model, model->mAnim.mType);
        modelSetAnimationRate(model, rate);
        if (!(model->mFlags & MODEL_FLAG_PART_ANIM_MIX)) {
            for (s32 i = 0; i < 8; ++i) model->mPartAnim[i].mAnimation = NULL;
        }
        GSboundSetAnimIndex(&model->mBound, index);
    }
    if (model->mFlags & MODEL_FLAG_TEXANIM_LINKED) {
        modelSetTextureAnimationIndex(model, index);
    }
    GSmodelForceAnimTransformUpdate(model);
}


extern "C" void GSmodelSetDontResetPartAnimFlag(_GSmodel *model, u8 enabled)
{
    if (enabled) model->mAnimFlags |= MODEL_ANIM_FLAG_DONT_RESET_PART;
    else model->mAnimFlags &= ~MODEL_ANIM_FLAG_DONT_RESET_PART;
}

extern "C" void GSmodelSetKeepPartAnimFlag(_GSmodel *model, u8 enabled)
{
    if (enabled) model->mFlags |= MODEL_FLAG_PART_ANIM_MIX;
    else model->mFlags &= ~MODEL_FLAG_PART_ANIM_MIX;
}

extern "C" void GSmodelSetPartAnimIndex(_GSmodel *model, s32 partIndex,
                                      s32 animIndex, s32 animType, f32 frame)
{
    GSpart *part = GSmodelGetPart(model, partIndex);
    if (part) {
        GSpartSetAnimIndex(part, animIndex, animType, frame);
        GSpartFree(part);
    }
}

static inline void modelNotifyAnimationEnded(_GSmodel *model, u32 index,
                                             u32 eventType, s32 looping,
                                             u8 partAnimation)
{
    if (model->mAnimEndedCB) {
        GSmodelAnimEndedEvent event;
        event.mFlags = 0;
        event.mFlags |= eventType;
        if ((u8)looping == 1) event.mFlags |= MODEL_ANIM_EVENT_LOOP;
        event.mAnimIndex = index;
        if (partAnimation == 1) event.mFlags |= MODEL_ANIM_EVENT_PART;
        event.mUserContext = model->mAnimEndedCBArg;
        model->mAnimEndedCB(model, &event);
    }
}

static inline void modelNotifyEndedAnimations(_GSmodel *model)
{
    if (model->mFlags & MODEL_FLAG_ANIM_ENDED) {
        u32 endedFlag;
        u32 runningFlag;
        u32 index;
        s32 type;
        if (!(model->mFlags & MODEL_FLAG_BLEND_ACTIVE)) {
            index = model->mAnim.mIndex;
            endedFlag = MODEL_FLAG_ANIM_ENDED;
            type = model->mAnim.mType;
            runningFlag = MODEL_FLAG_ANIM_RUNNING;
        } else {
            index = model->mBlendAnim.mIndex;
            endedFlag = MODEL_FLAG_ANIM_ENDED;
            type = model->mBlendAnim.mType;
            runningFlag = MODEL_FLAG_ANIM_RUNNING;
        }
        switch (type) {
        case MODEL_ANIM_PLAY_ONCE: model->mFlags &= ~runningFlag; break;
        case MODEL_ANIM_PLAY_LOOP: model->mFlags &= ~endedFlag; break;
        }
        s32 looping = type == MODEL_ANIM_PLAY_LOOP ? 1 : 0;
        modelNotifyAnimationEnded(model, index, MODEL_ANIM_EVENT_JOINT, looping, 0);
    }

    if (model->mFlags & MODEL_FLAG_TEXANIM_ENDED) {
        u32 endedFlag;
        u32 runningFlag;
        s32 type;
        u32 index;
        if (!(model->mFlags & MODEL_FLAG_BLEND_ACTIVE)) {
            index = model->mTexAnim.mIndex;
            type = model->mTexAnim.mType;
            runningFlag = MODEL_FLAG_TEXANIM_RUNNING;
            endedFlag = MODEL_FLAG_TEXANIM_ENDED;
        }
        switch (type) {
        case MODEL_ANIM_PLAY_ONCE: model->mFlags &= ~runningFlag; break;
        case MODEL_ANIM_PLAY_LOOP: model->mFlags &= ~endedFlag; break;
        }
        s32 looping = type == MODEL_ANIM_PLAY_LOOP ? 1 : 0;
        modelNotifyAnimationEnded(model, index, MODEL_ANIM_EVENT_TEXTURE, looping, 0);
    }
}

static inline void modelNotifyPartJointAnimationsEnded(_GSmodel *model)
{
    if (model->mHasPartAnim == 1) {
        for (s32 part = 0; part < MODEL_PART_ANIM_COUNT; part++) {
            if (model->mPartAnim[part].mAnimation &&
                model->mPartAnim[part].mRequestedFrame >= model->mPartAnim[part].mEndFrame - MODEL_PART_ANIM_END_TOLERANCE &&
                !(model->mAnimFlags & MODEL_ANIM_FLAG_DONT_RESET_PART)) {
                if (model->mPartAnim[part].mType == MODEL_ANIM_PLAY_ONCE) model->mPartAnim[part].mAnimation = NULL;
                s32 looping = model->mPartAnim[part].mType == MODEL_ANIM_PLAY_LOOP ? 1 : 0;
                u32 index = model->mPartAnim[part].mIndex;
                modelNotifyAnimationEnded(model, index, MODEL_ANIM_EVENT_JOINT, looping, 1);
            }
        }
    }
}

static inline void modelNotifyPartTextureAnimationsEnded(_GSmodel *model)
{
    if (model->mHasPartTexAnim == 1) {
        for (s32 part = 0; part < MODEL_PART_TEX_ANIM_COUNT; part++) {
            if (model->mPartTexAnim[part].mAnimation &&
                model->mPartTexAnim[part].mRequestedFrame >= model->mPartTexAnim[part].mEndFrame - MODEL_PART_ANIM_END_TOLERANCE &&
                !(model->mAnimFlags & MODEL_ANIM_FLAG_DONT_RESET_PART)) {
                if (model->mPartTexAnim[part].mType == MODEL_ANIM_PLAY_ONCE) model->mPartTexAnim[part].mAnimation = NULL;
                s32 looping = model->mPartTexAnim[part].mType == MODEL_ANIM_PLAY_LOOP ? 1 : 0;
                u32 index = model->mPartTexAnim[part].mIndex;
                modelNotifyAnimationEnded(model, index, MODEL_ANIM_EVENT_TEXTURE, looping, 1);
            }
        }
    }
}

extern "C" void GSmodelAdvanceAnimation(_GSmodel *model, f32 frameStep)
{
    if (model->mFlags & MODEL_FLAG_ANIM_PAUSED) return;

    modelNotifyEndedAnimations(model);
    modelNotifyPartJointAnimationsEnded(model);
    modelNotifyPartTextureAnimationsEnded(model);

    if (model->mFlags & MODEL_FLAG_ANIM_RUNNING) {
        f32 step = model->mAnim.mRate * frameStep;
        if (0.0f != step) {
            model->mAnim.mFrame += step;
            switch (model->mAnim.mType) {
            case MODEL_ANIM_PLAY_ONCE:
                if (model->mAnim.mFrame >= model->mAnim.mEndFrame) {
                    model->mAnim.mFrame = model->mAnim.mEndFrame;
                    model->mFlags |= MODEL_FLAG_ANIM_ENDED;
                }
                break;
            case MODEL_ANIM_PLAY_LOOP:
                if (model->mAnim.mFrame >= model->mAnim.mEndFrame - MODEL_ANIM_LOOP_END_TOLERANCE) {
                    model->mAnim.mFrame -= model->mAnim.mEndFrame;
                    if (model->mAnim.mFrame < 0.0f) model->mAnim.mFrame = 0.0f;
                    model->mFlags |= MODEL_FLAG_ANIM_ENDED;
                }
                break;
            }
        }
        GSboundSetAnimFrame(&model->mBound, model->mAnim.mFrame);
        if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE) {
            step = model->mBlendAnim.mRate * frameStep;
            if (0.0f != step) {
                model->mBlendAnim.mFrame += step;
                switch (model->mBlendAnim.mType) {
                case MODEL_ANIM_PLAY_ONCE:
                    if (model->mBlendAnim.mFrame >= model->mBlendAnim.mEndFrame) {
                        model->mBlendAnim.mFrame = model->mBlendAnim.mEndFrame;
                        model->mFlags |= MODEL_FLAG_ANIM_ENDED;
                    }
                    break;
                case MODEL_ANIM_PLAY_LOOP:
                    if (model->mBlendAnim.mFrame >= model->mBlendAnim.mEndFrame - MODEL_ANIM_LOOP_END_TOLERANCE) {
                        model->mBlendAnim.mFrame -= model->mBlendAnim.mEndFrame;
                        if (model->mBlendAnim.mFrame < 0.0f) model->mBlendAnim.mFrame = 0.0f;
                        model->mFlags |= MODEL_FLAG_ANIM_ENDED;
                    }
                    break;
                }
            }
        }
    }
    if (model->mFlags & MODEL_FLAG_TEXANIM_RUNNING) {
        f32 step = model->mTexAnim.mRate * frameStep;
        if (0.0f != step) {
            model->mTexAnim.mFrame += step;
            switch (model->mTexAnim.mType) {
            case MODEL_ANIM_PLAY_ONCE:
                if (model->mTexAnim.mFrame >= model->mTexAnim.mEndFrame) {
                    model->mTexAnim.mFrame = model->mTexAnim.mEndFrame;
                    model->mFlags |= MODEL_FLAG_TEXANIM_ENDED;
                }
                break;
            case MODEL_ANIM_PLAY_LOOP:
                if (model->mTexAnim.mFrame >= model->mTexAnim.mEndFrame - MODEL_ANIM_LOOP_END_TOLERANCE) {
                    model->mTexAnim.mFrame -= model->mTexAnim.mEndFrame;
                    if (model->mTexAnim.mFrame < 0.0f) model->mTexAnim.mFrame = 0.0f;
                    model->mFlags |= MODEL_FLAG_TEXANIM_ENDED;
                }
                break;
            }
        }
    }
}

void modelUpdateTransform(_GSmodel *model)
{
    if (model->mFlags & MODEL_FLAG_TRANSFORM_UPDATED) return;
    modelUpdateCenterNull(model);
    _HSD_JObj *joint = modelGetRenderJObj(model);
    if (model->mScale.x < MODEL_MIN_SCALE_MAGNITUDE && model->mScale.x > -MODEL_MIN_SCALE_MAGNITUDE)
        model->mScale.x = model->mScale.x < 0.0f ? -MODEL_MIN_SCALE_MAGNITUDE : MODEL_MIN_SCALE_MAGNITUDE;
    if (model->mScale.y < MODEL_MIN_SCALE_MAGNITUDE && model->mScale.y > -MODEL_MIN_SCALE_MAGNITUDE)
        model->mScale.y = model->mScale.y < 0.0f ? -MODEL_MIN_SCALE_MAGNITUDE : MODEL_MIN_SCALE_MAGNITUDE;
    if (model->mScale.z < MODEL_MIN_SCALE_MAGNITUDE && model->mScale.z > -MODEL_MIN_SCALE_MAGNITUDE)
        model->mScale.z = model->mScale.z < 0.0f ? -MODEL_MIN_SCALE_MAGNITUDE : MODEL_MIN_SCALE_MAGNITUDE;

    if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE) {
        f32 rotation[4];
        EulerToQuat(&model->mRotation, rotation);
        HSD_JObjSetTranslation(joint, (Vec *)&model->mPosition);
        HSD_JObjSetRotation(joint, (Quaternion *)rotation);
        HSD_JObjSetScale(joint, (Vec *)&model->mScale);
        _modelRecalcMatrixAll(joint);
    } else {
        if (model->mFlags & MODEL_FLAG_ROOT_JOINT_POSE) {
            HSD_JObjGetTranslation(joint, (Vec *)&model->mPosition);
            model->mRotation.x = HSD_JObjGetRotationX(joint);
            model->mRotation.y = HSD_JObjGetRotationX(joint);
            model->mRotation.z = HSD_JObjGetRotationX(joint);
            HSD_JObjGetScale(joint, (Vec *)&model->mScale);
        } else {
            HSD_JObjSetTranslation(joint, (Vec *)&model->mPosition);
            HSD_JObjSetRotationX(joint, model->mRotation.x);
            HSD_JObjSetRotationY(joint, model->mRotation.y);
            HSD_JObjSetRotationZ(joint, model->mRotation.z);
            HSD_JObjSetScale(joint, (Vec *)&model->mScale);
        }
        if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT)
            HSD_JObjSetupMatrix(joint->child);
        HSD_JObjSetupMatrix(joint);
    }
    model->mFlags |= MODEL_FLAG_TRANSFORM_UPDATED;
}

void modelUpdateAttachments(_GSmodel *model)
{
    if (model->mAttachmentType == GS_ATTACH_NONE) return;
    switch (model->mFlags & MODEL_FLAG_ATTACHMENT_UPDATED) {
    case 0: {
        model->mFlags |= MODEL_FLAG_ATTACHMENT_UPDATING;
        modelUpdateAttachments(model->mAttachedModel);
        modelUpdateTransform(model->mAttachedModel);
        GSvec position, rotation, scale;
        GSpart *part = GSmodelGetPart(model->mAttachedModel, model->mAttachedPartIndex);
        GSpartGetTransform(part, &position, &rotation, &scale);
        GSpartFree(part);
        f32 quaternion[4];
        GSvec offset;
        EulerToQuat(&rotation, quaternion);
        GSvecTransformQuat(&offset, quaternion, &model->mAttachedPosition);
        PSVECAdd((Vec *)&position, (Vec *)&offset, (Vec *)&position);
        PSVECAdd((Vec *)&rotation, (Vec *)&model->mAttachedRotation, (Vec *)&rotation);
        scale.x *= model->mAttachedScale.x;
        scale.y *= model->mAttachedScale.y;
        scale.z *= model->mAttachedScale.z;
        modelSetPos(model, &model->mAttachedPosition);
        modelSetRot(model, &model->mAttachedRotation);
        modelSetScl(model, &model->mAttachedScale);
        switch (model->mAttachmentType) {
        case GS_ATTACH_POSITION:
            modelSetPos(model, &position);
            break;
        case GS_ATTACH_ROTATION:
            modelSetRot(model, &rotation);
            break;
        case GS_ATTACH_SCALE:
            modelSetScl(model, &scale);
            break;
        case GS_ATTACH_POSITION_ROTATION:
            modelSetPos(model, &position);
            modelSetRot(model, &rotation);
            break;
        case GS_ATTACH_ROTATION_SCALE:
            modelSetRot(model, &rotation);
            modelSetScl(model, &scale);
            break;
        case GS_ATTACH_POSITION_SCALE:
            modelSetPos(model, &position);
            modelSetScl(model, &scale);
            break;
        case GS_ATTACH_ALL:
            modelSetPos(model, &position);
            modelSetRot(model, &rotation);
            modelSetScl(model, &scale);
            break;
        case GS_ATTACH_MATRIX:
            modelSetPos(model, &position);
            modelSetRot(model, &rotation);
            modelSetScl(model, &scale);
            GSmtx matrix, component;
            PSMTXRotRad(matrix.sdk(), 'Z', rotation.z);
            PSMTXRotRad(component.sdk(), 'Y', rotation.y);
            PSMTXConcat(component.sdk(), matrix.sdk(), matrix.sdk());
            PSMTXRotRad(component.sdk(), 'X', rotation.x);
            PSMTXConcat(component.sdk(), matrix.sdk(), matrix.sdk());
            PSMTXTrans(component.sdk(), position.x, position.y, position.z);
            PSMTXConcat(component.sdk(), matrix.sdk(), matrix.sdk());
            HSD_JObjSetMatrix(model->mJObj, &matrix);
            break;
        }
        if (model->mFlags & MODEL_FLAG_ATTACHMENT_DETACH) GSmodelDetachFromGSpart(model, 0);
        model->mFlags |= MODEL_FLAG_ATTACHMENT_UPDATED;
        model->mFlags &= ~MODEL_FLAG_ATTACHMENT_UPDATING;
        break;
    }
    }
}



#define HSD_ANIM_RATE_ARGUMENT 1u
#define VIDEO_PAL_VSYNC_RATE 50u

static inline u8 modelRequires60fpsUpdate(_GSmodel *model)
{
    if (model->mAnimFlags & MODEL_ANIM_FLAG_USE_FRACTIONAL) {
        return 0;
    }
    if ((model->mFlags & MODEL_FLAG_60FPS_ANIM) == MODEL_FLAG_60FPS_ANIM ||
        (model->mAttachmentType != 0 && model->mAttachedModel != NULL)) {
        return 1;
    }
    return 0;
}

static inline f32 modelAnimationFrameStep(_GSmodel *model, GSmodelAnimState *animation)
{
    u8 processAt60fps = modelRequires60fpsUpdate(model);
    f32 step = 0.0f;
    f32 frame;

    if ((model->mAnimFlags & MODEL_ANIM_FLAG_SKIP_FRACTIONAL) ==
            MODEL_ANIM_FLAG_SKIP_FRACTIONAL && !processAt60fps) {
        frame = (s32)animation->mFrame;
    } else {
        frame = animation->mFrame;
    }
    if (animation->mEndFrame > 0.0f) {
        switch (animation->mType) {
        case MODEL_ANIM_PLAY_ONCE:
            if (frame >= animation->mEndFrame) {
                frame = animation->mEndFrame;
            }
            break;
        case MODEL_ANIM_PLAY_LOOP:
            if (frame >= animation->mEndFrame - 0.001f) {
                frame = fmod(frame, animation->mEndFrame);
            }
            break;
        }
    }
    if (frame != animation->mRequestedFrame) {
        if (frame < animation->mRequestedFrame) {
            if (animation->mType == MODEL_ANIM_PLAY_LOOP) {
                step = frame + (animation->mEndFrame - animation->mRequestedFrame);
            } else {
                step = 0.0f;
            }
        } else {
            step = frame - animation->mRequestedFrame;
        }
    }
    if (step < 0.0f) {
        step = 0.0f;
    }
    return step;
}

static inline void modelStorePartAnimationFrames(_GSmodel *model)
{
    for (s32 part = 0; part < MODEL_PART_ANIM_COUNT; part++) {
        if (model->mPartAnim[part].mAnimation != NULL) {
            model->mPartAnim[part].mRequestedFrame = model->mPartAnim[part].mAnimation->mFrame;
        }
    }
}

static inline void modelStorePartTextureAnimationFrames(_GSmodel *model)
{
    for (s32 part = 0; part < MODEL_PART_TEX_ANIM_COUNT; part++) {
        if (model->mPartTexAnim[part].mAnimation != NULL) {
            model->mPartTexAnim[part].mRequestedFrame = model->mPartTexAnim[part].mAnimation->mFrame;
        }
    }
}

extern "C" void GSmodelUpdateAnimation(_GSmodel *model)
{
    if (!(model->mFlags & (MODEL_FLAG_ANIM_PRESENT | MODEL_FLAG_TEXANIM_PRESENT))) {
        return;
    }
    if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE) {
        if ((model->mFlags & MODEL_FLAG_ANIM_PRESENT) &&
            model->mAnim.mAnimation != NULL && model->mBlendAnim.mAnimation != NULL) {
            HSD_ForeachAnim(model->mJObj, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK,
                            (void (*)())HSD_AObjSetRate, HSD_ANIM_RATE_ARGUMENT,
                            modelAnimationFrameStep(model, &model->mAnim));
            HSD_ForeachAnim(model->mBlendJObj, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK,
                            (void (*)())HSD_AObjSetRate, HSD_ANIM_RATE_ARGUMENT,
                            modelAnimationFrameStep(model, &model->mBlendAnim));
        }
        HSD_JObjAnimAll(model->mJObj);
        HSD_JObjAnimAll(model->mBlendJObj);
        if ((model->mFlags & MODEL_FLAG_ANIM_PRESENT) &&
            model->mAnim.mAnimation != NULL && model->mBlendAnim.mAnimation != NULL) {
            model->mAnim.mRequestedFrame = model->mAnim.mAnimation->mFrame;
            model->mBlendAnim.mRequestedFrame = model->mBlendAnim.mAnimation->mFrame;
            if (model->mHasPartAnim == 1) {
                modelStorePartAnimationFrames(model);
            }
        }
        f32 rate = modelGetEffectiveAnimationRate(model);
        f32 step = rate * GSgfxVideoGetLastRenderTime();
        step *= 0.5f;
        modelCalculateBlendModel(model, step);
    } else {
        _modelResetPartAnimMixes(model);
        _HSD_JObj *joint = model->mJObj;
        if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
            joint = joint->child;
        }
        if ((model->mFlags & MODEL_FLAG_ANIM_PRESENT) && model->mAnim.mAnimation != NULL) {
            HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK,
                            (void (*)())HSD_AObjSetRate, HSD_ANIM_RATE_ARGUMENT,
                            modelAnimationFrameStep(model, &model->mAnim));
        }
        if ((model->mFlags & MODEL_FLAG_TEXANIM_PRESENT) && model->mTexAnim.mAnimation != NULL) {
            HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_MATERIAL_MASK,
                            (void (*)())HSD_AObjSetRate, HSD_ANIM_RATE_ARGUMENT,
                            modelAnimationFrameStep(model, &model->mTexAnim));
        }
        HSD_JObjAnimAll(joint);
        if ((model->mFlags & MODEL_FLAG_ANIM_PRESENT) && model->mAnim.mAnimation != NULL) {
            model->mAnim.mRequestedFrame = model->mAnim.mAnimation->mFrame;
            if (model->mHasPartAnim == 1) {
                modelStorePartAnimationFrames(model);
            }
        }
        if ((model->mFlags & MODEL_FLAG_TEXANIM_PRESENT) && model->mTexAnim.mAnimation != NULL) {
            model->mTexAnim.mRequestedFrame = model->mTexAnim.mAnimation->mFrame;
            if (model->mHasPartTexAnim == 1) {
                modelStorePartTextureAnimationFrames(model);
            }
        }
    }
}

u8 modelProcessAt60fps(_GSmodel *model)
{
    return modelRequires60fpsUpdate(model);
}

inline void _jobjAddAnimAll(_HSD_JObj *joint, _HSD_AnimJoint *jointAnimation,
                     _HSD_MatAnimJoint *materialAnimation, _HSD_ShapeAnimJoint *shapeAnimation)
{
    if (joint == NULL) return;
    if (joint->mAObj == NULL || !(HSD_AObjGetFlags(joint->mAObj) & MODEL_AOBJ_FLAG_PART_ANIM_PROTECTED)) {
        HSD_JObjAddAnim(joint, jointAnimation, materialAnimation, shapeAnimation);
    }
    if (joint->flags & HSD_JOBJ_TRAVERSAL_BARRIER) return;
    _HSD_JObj *child = joint->child;
    _HSD_AnimJoint *childAnimation = jointAnimation ? jointAnimation->mChild : NULL;
    _HSD_MatAnimJoint *childMaterial = materialAnimation ? materialAnimation->mChild : NULL;
    _HSD_ShapeAnimJoint *childShape = shapeAnimation ? shapeAnimation->mChild : NULL;
    while (child != NULL) {
        _jobjAddAnimAll(child, childAnimation, childMaterial, childShape);
        child = child->next;
        childAnimation = childAnimation ? childAnimation->mNext : NULL;
        childMaterial = childMaterial ? childMaterial->mNext : NULL;
        childShape = childShape ? childShape->mNext : NULL;
    }
}

void modelApplyPartAnimMixes(_GSmodel *model)
{
    GSvec pose;
    GSvec mixVector;
    GSquat rotation;
    for (s32 i = 0; i < GS_MODEL_PART_MIX_COUNT; i++) {
        if (model->mPartMix[i].mType == GS_MODEL_PART_MIX_NONE) continue;
        GSpart *part = GSmodelGetPart(model, model->mPartMix[i].mPartIndex);
        if (part == NULL) continue;
        modelGetPartMixVector(model, &model->mPartMix[i], part, &mixVector);
        if (!(model->mFlags & MODEL_FLAG_BLEND_ACTIVE)) {
            switch (model->mPartMix[i].mType) {
            case GS_MODEL_PART_MIX_TRANSLATION:
                HSD_JObjGetTranslation(part->mJoint, (Vec *)&pose);
                modelCalculatePartMixInline(&pose, &mixVector, model->mPartMix[i].mFlags);
                HSD_JObjSetTranslation(part->mJoint, (const Vec *)&pose);
                break;
            case GS_MODEL_PART_MIX_ROTATION:
                HSD_JObjGetRotation(part->mJoint, (Quaternion *)&rotation);
                modelCalculatePartMixInline((GSvec *)&rotation, &mixVector, model->mPartMix[i].mFlags);
                HSD_JObjSetRotation(part->mJoint, (const Quaternion *)&rotation);
                break;
            case GS_MODEL_PART_MIX_SCALE:
                HSD_JObjGetScale(part->mJoint, (Vec *)&pose);
                modelCalculatePartMixInline(&pose, &mixVector, model->mPartMix[i].mFlags);
                HSD_JObjSetScale(part->mJoint, (const Vec *)&pose);
                break;
            }
        } else {
            _HSD_JObj *first = GSmodelGetJointByIndex(model->mJObj, model->mPartMix[i].mPartIndex);
            _HSD_JObj *second = GSmodelGetJointByIndex(model->mBlendJObj, model->mPartMix[i].mPartIndex);
            if (first != NULL && second != NULL) {
                switch (model->mPartMix[i].mType) {
                case GS_MODEL_PART_MIX_TRANSLATION:
                    modelIntpJObjTrans(first, second, part->mJoint, model->mBlendFactor,
                                       &mixVector, model->mPartMix[i].mFlags);
                    break;
                case GS_MODEL_PART_MIX_ROTATION:
                    modelIntpJObjRotate(first, second, part->mJoint, model->mBlendFactor,
                                        &mixVector, model->mPartMix[i].mFlags);
                    break;
                case GS_MODEL_PART_MIX_SCALE:
                    modelIntpJObjScale(first, second, part->mJoint, model->mBlendFactor,
                                       &mixVector, model->mPartMix[i].mFlags);
                    break;
                }
            }
        }
        GSpartFree(part);
    }
}



void _modelResetPartAnimMixes(_GSmodel *model)
{
    for (s32 index = 0; index < GS_MODEL_PART_MIX_COUNT; index++) {
        GSmodelPartMix *mix = &model->mPartMix[index];
        if (mix->mType == GS_MODEL_PART_MIX_NONE) continue;
        GSpart *part = GSmodelGetPart(model, mix->mPartIndex);
        if (part == NULL) continue;
        switch (mix->mType) {
        case GS_MODEL_PART_MIX_TRANSLATION:
            HSD_JObjSetTranslation(part->mJoint, (const Vec *)&mix->mBasePosition);
            break;
        case GS_MODEL_PART_MIX_ROTATION: {
            GSquat rotation(mix->mBaseRotation.x, mix->mBaseRotation.y,
                            mix->mBaseRotation.z, 0.0f);
            HSD_JObjSetRotation(part->mJoint, (Quaternion *)&rotation);
            break;
        }
        case GS_MODEL_PART_MIX_SCALE:
            HSD_JObjSetScale(part->mJoint, (const Vec *)&mix->mBaseScale);
            break;
        }
        GSpartFree(part);
    }
}

void modelCalculatePartMix(GSvec *out, GSvec *mix, unsigned long flags)
{
    switch (flags & GS_MODEL_PART_MIX_X_MASK) {
    case GS_MODEL_PART_MIX_KEEP: break;
    case GS_MODEL_PART_MIX_REPLACE: out->x = mix->x; break;
    case GS_MODEL_PART_MIX_ADD: out->x += mix->x; break;
    }
    switch (flags & GS_MODEL_PART_MIX_Y_MASK) {
    case GS_MODEL_PART_MIX_KEEP << 8: break;
    case GS_MODEL_PART_MIX_REPLACE << 8: out->y = mix->y; break;
    case GS_MODEL_PART_MIX_ADD << 8: out->y += mix->y; break;
    }
    switch (flags & GS_MODEL_PART_MIX_Z_MASK) {
    case GS_MODEL_PART_MIX_KEEP << 16: break;
    case GS_MODEL_PART_MIX_REPLACE << 16: out->z = mix->z; break;
    case GS_MODEL_PART_MIX_ADD << 16: out->z += mix->z; break;
    }
}

inline void _modelRecalcMatrixAll(_HSD_JObj *joint)
{
    if (joint != NULL) {
        HSD_JObjSetupMatrix(joint);
        if (!(joint->flags & HSD_JOBJ_TRAVERSAL_BARRIER)) {
            _HSD_JObj *child = joint->child;
            while (child != NULL) {
                _modelRecalcMatrixAll(child);
                child = child->next;
            }
        }
    }
}

void _modelGetAObjFunc(_HSD_AObj *animation, void *result)
{
    if (animation != NULL && *(_HSD_AObj **)result == NULL) {
        *(_HSD_AObj **)result = animation;
    }
}

void _modelSetLoopFlag(_HSD_AObj *animation, unsigned long loop)
{
    if (!(HSD_AObjGetFlags(animation) & MODEL_AOBJ_FLAG_PART_ANIM_PROTECTED)) {
        if (loop == 0) HSD_AObjClearFlags(animation, HSD_AOBJ_FLAG_LOOP);
        else HSD_AObjSetFlags(animation, HSD_AOBJ_FLAG_LOOP);
    }
}

extern "C" void _modelGetEndFrame(_HSD_AObj *animation, f32 *endFrame)
{
    f32 frame = HSD_AObjGetEndFrame(animation);
    if (frame > *endFrame) *endFrame = frame;
}
