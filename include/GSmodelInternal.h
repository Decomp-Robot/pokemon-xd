#ifndef GSMODEL_INTERNAL_H
#define GSMODEL_INTERNAL_H

#include <GSmodel.h>
#include <GSpart.h>
#include <HSD.h>
#include <GSgfx.h>
#include <GSmem.h>
#include <GSbound.h>
#include <GScamera.h>
#include <GSmaterial.h>
#include <GStexture.h>
#include <GSparticle.h>
#include <libc/memory.h>
#include <libc/fpclassify.h>
#include <new.h>

#define MODEL_AOBJ_FLAG_PART_ANIM_PROTECTED 0x10000u

/* Model update and interpolation helpers. */
void modelDustUpdate(_GSmodel *model);
void modelCalculateBlendModel(_GSmodel *model, f32 factor);
void modelUpdateAttachments(_GSmodel *model);
void modelUpdateTransform(_GSmodel *model);
void modelApplyPartAnimMixes(_GSmodel *model);
void modelCalculatePartMix(GSvec *out, GSvec *a, unsigned long flags);
void _modelSetRotateEulerToQuatAll(_HSD_JObj *jobj);
void _modelIntpJObj(_HSD_JObj *a, _HSD_JObj *b, _HSD_JObj *out, f32 t);
void _modelIntpJObjAll(_GSmodel *model, _HSD_JObj *a, _HSD_JObj *b, _HSD_JObj *c,
                       f32 t1, f32 t2);
void modelIntpJObjRotate(_HSD_JObj *a, _HSD_JObj *b, _HSD_JObj *out, f32 t,
                         GSvec *part, unsigned long flags);
void modelIntpJObjScale(_HSD_JObj *a, _HSD_JObj *b, _HSD_JObj *out, f32 t,
                        GSvec *part, unsigned long flags);
void modelIntpJObjTrans(_HSD_JObj *a, _HSD_JObj *b, _HSD_JObj *out, f32 t,
                        GSvec *part, unsigned long flags);

/* Distortion and bound display-list callbacks. */
void modelDistortionRender(void);
void _modelDrawDistortionSub(_GSmodel *model, GSgfxLayer *layer, GSmtx *mtx, GSvec *pos);
void _modelBoundVertex(unsigned long attributeFlags, void *attributeData, void *userContext);
void _modelBoundBeginSurface(GSgfxPrimType type, u16 count, unsigned long attributeMask, void *userContext);
void _modelBoundPObj(_HSD_PObj *pobj, GSmtx *referenceMatrix, GSmtx *positionMatrix, GSmtx *matrixPalette, void *userContext);
void _modelGetAnimFrameCountCB(_HSD_AnimJoint *joint, void **argp, int idx);

/* Model state flags. */
#define MODEL_FLAG_ROOT_ANIM_REMOVED 0x20000000u
#define MODEL_FLAG_ROOT_JOINT_POSE 0x400
#define MODEL_FLAG_60FPS_ANIM 0x800
#define MODEL_FLAG_TEXANIM_LINKED 0x2000
#define MODEL_FLAG_ANIM_PRESENT (1u << 2)
#define MODEL_FLAG_BLEND_ACTIVE (1u << 7)
#define MODEL_FLAG_60FPS_ACTIVE (1u << 11)
#define MODEL_FLAG_TEXANIM_ACTIVE (1u << 13)
#define MODEL_FLAG_FORCE_UPDATE_MASK 0xFFBFFDFF
#define MODEL_FLAG_FRAME_VISIBLE (1u << 26)
#define MODEL_FLAG_DISTORTION_CLASS 0x80000

/* Fractional-frame animation policy. */
#define MODEL_ANIM_FLAG_SKIP_FRACTIONAL 0x1
#define MODEL_ANIM_FLAG_USE_FRACTIONAL 0x2

#define GSGFX_PERFBAR_MODEL_UPDATE 0xFF0000
#define GSGFX_PERFBAR_MODEL_DISTORTION 0x8080FF
#define DISTORTION_RELATIVE_LAYER_ID ((GSgfxLayerID)2)

extern _GSmodel *gsModelList;
u8 modelProcessAt60fps(_GSmodel *model);
void _modelResetPartAnimMixes(_GSmodel *model);
inline void _modelRecalcMatrixAll(_HSD_JObj *joint);
void modelUpdateCenterNull(_GSmodel *model);
_HSD_JObj *modelGetRenderJObj(_GSmodel *model);
void modelSetPos(_GSmodel *model, GSvec *value);
void modelSetRot(_GSmodel *model, GSvec *value);
void modelSetScl(_GSmodel *model, GSvec *value);
void _modelSetLoopFlag(_HSD_AObj *animation, unsigned long loop);
void _modelGetAObjFunc(_HSD_AObj *animation, void *result);
inline void _jobjAddAnimAll(_HSD_JObj *joint, _HSD_AnimJoint *jointAnimation,
                    _HSD_MatAnimJoint *materialAnimation,
                    _HSD_ShapeAnimJoint *shapeAnimation);
extern "C" {
u8 GSgfxVideoGetVsyncRate(void);
f32 GSgfxVideoGetLastRenderTime(void);
_HSD_JObj *GSmodelGetJointByIndex(_HSD_JObj *joint, s32 index);
void _modelGetEndFrame(_HSD_AObj *animation, f32 *endFrame);
}

#define MODEL_FLAG_TEXANIM_PRESENT 0x8u
#define MODEL_FLAG_SHAPE_ANIM_PRESENT 0x10u
#define MODEL_FLAG_TEXANIM_RUNNING 0x40u
#define MODEL_FLAG_TEXANIM_ENDED 0x8000u
#define MODEL_FLAG_NULL_JOINT 0x20000u
#define MODEL_ANIM_MATERIAL_MASK 0x64DBu
#define MODEL_ANIM_TEXTURE_AOBJ 0x480u
#define MODEL_ANIM_ALL_MATERIALS 0x634u
#define MODEL_ANIM_PLAY_ONCE 0
#define MODEL_ANIM_PLAY_LOOP 1
#define MODEL_PART_ANIM_COUNT 8
#define MODEL_PART_TEX_ANIM_COUNT 4
#define MODEL_PART_ANIM_END_TOLERANCE 0.0001f
#define MODEL_ANIM_LOOP_END_TOLERANCE 0.001f
#define MODEL_MIN_SCALE_MAGNITUDE 0.00001f
#define MODEL_FLAG_TRANSFORM_UPDATED 0x200u
#define MODEL_ANIM_FLAG_DONT_RESET_PART 0x10u
#define MODEL_FLAG_PART_ANIM_MIX 0x80000000u
#define MODEL_FLAG_ANIM_PAUSED 0x100u
#define MODEL_FLAG_ANIM_RUNNING 0x20u
#define MODEL_FLAG_ANIM_ENDED 0x4000u
#define MODEL_FLAG_ATTACHMENT_UPDATING 0x800000u
#define MODEL_FLAG_ATTACHMENT_UPDATED 0x400000u
#define MODEL_FLAG_ATTACHMENT_DETACH 0x40000u
#define MODEL_ANIM_EVENT_JOINT 1u
#define MODEL_ANIM_EVENT_TEXTURE 2u
#define MODEL_ANIM_EVENT_LOOP 4u
#define MODEL_ANIM_EVENT_PART 8u
#define MODEL_ANIM_JOINT_MASK 0x9b2fu
#define MODEL_ANIM_JOINT_AOBJ 0x20u
#define MODEL_ANIM_ALL_JOINTS 0x1cbu

static inline void modelSetTextureAnimType(_GSmodel *model, s32 type)
{
    _HSD_JObj *joint = model->mJObj;
    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
        joint = joint->child;
    }
    model->mTexAnim.mType = type;
    switch (model->mTexAnim.mType) {
    case MODEL_ANIM_PLAY_ONCE:
        HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_MATERIAL_MASK,
                        (void (*)())_modelSetLoopFlag, HSD_ANIM_ARG_UNSIGNED, 0);
        break;
    case MODEL_ANIM_PLAY_LOOP:
        HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_MATERIAL_MASK,
                        (void (*)())_modelSetLoopFlag, HSD_ANIM_ARG_UNSIGNED, 1);
        break;
    }
}

static inline void modelSetTextureAnimFrame(_GSmodel *model, f32 frame)
{
    _HSD_JObj *joint = model->mJObj;
    if (model->mFlags & MODEL_FLAG_TEXANIM_PRESENT) {
        if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT) {
            joint = joint->child;
        }
        model->mTexAnim.mFrame = frame;
        model->mTexAnim.mRequestedFrame = frame;
        HSD_JObjReqAnimAllByFlags(joint, MODEL_ANIM_ALL_MATERIALS,
                                  model->mTexAnim.mRequestedFrame);
        model->mFlags &= ~MODEL_FLAG_TEXANIM_ENDED;
    }
}

static inline f32 modelGetEffectiveAnimationRate(_GSmodel *model)
{
    f32 rate;
    if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE) {
        rate = model->mBlendAnim.mRate;
    } else {
        rate = model->mAnim.mRate;
    }
    if (GSgfxVideoGetVsyncRate() == 50) {
        rate /= 1.2f;
    }
    return rate;
}

static inline _HSD_SceneModelSet *modelResource(_GSmodel *model)
{
    return model->mRoot;
}

static inline u8 modelAnimationIsEmpty(_GSmodel *model, unsigned long index)
{
    _HSD_SceneModelSet *root;
    if (!model || !(root = modelResource(model))) return 1;
    if (index >= (u32)model->mMaxAnimIndex) return 1;
    _HSD_AnimJoint *animation = root->mAnimations[index];
    if (!animation->mChild && !animation->mNext && !animation->mAnimation &&
    !animation->mRObjAnimation && !animation->mFlags) return 1;
    return 0;
}

static inline void modelResetTextureMixes(_GSmodel *model)
{
    model->mPartTexAnim[0].mAnimation = NULL;
    model->mPartTexAnim[1].mAnimation = NULL;
    model->mPartTexAnim[2].mAnimation = NULL;
    model->mPartTexAnim[3].mAnimation = NULL;
}

static inline void modelPausePartAnimations(_GSmodel *model, u8 paused)
{
    for (s32 partIndex = 0; partIndex < MODEL_PART_ANIM_COUNT; ++partIndex) {
        _HSD_AObj *animation = model->mPartAnim[partIndex].mAnimation;
        if (animation) {
            if (paused) HSD_AObjSetFlags(animation, MODEL_AOBJ_FLAG_PART_ANIM_PROTECTED);
            else HSD_AObjClearFlags(animation, MODEL_AOBJ_FLAG_PART_ANIM_PROTECTED);
        }
    }
}

static inline _HSD_AObj *modelGetAnimationObject(_HSD_JObj *joint, unsigned long animationMask)
{
    _HSD_AObj *animation = NULL;
    HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, animationMask, (void (*)())_modelGetAObjFunc,
        HSD_ANIM_ARG_POINTER, &animation);
    return animation;
}

static inline void modelSetTextureAnimationIndex(_GSmodel *model, unsigned long index)
{
    _HSD_JObj *joint = model->mJObj;
    if (model->mFlags & MODEL_FLAG_TEXANIM_PRESENT) {
        if (index < (u32)model->mMaxTexAnimIndex) {
            if (index != model->mTexAnim.mIndex || model->mHasPartTexAnim == 1) {
                if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT)
                joint = joint->child;
                model->mTexAnim.mIndex = index;
                HSD_JObjAddAnimAll(joint, NULL, modelResource(model)->mTextureAnimations[model->mTexAnim.mIndex], NULL);
                if ((model->mFlags & MODEL_FLAG_ROOT_ANIM_REMOVED) && joint->mAObj)
                HSD_JObjRemoveAnim(joint);
                model->mTexAnim.mEndFrame = 0.0f;
                HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_MATERIAL_MASK, (void (*)())_modelGetEndFrame,
                    HSD_ANIM_ARG_POINTER, &model->mTexAnim.mEndFrame);
                model->mTexAnim.mAnimation = modelGetAnimationObject(joint, MODEL_ANIM_TEXTURE_AOBJ);
            }
            modelSetTextureAnimFrame(model, 0.0f);
            modelSetTextureAnimType(model, model->mTexAnim.mType);
            modelResetTextureMixes(model);
            model->mHasPartTexAnim = 0;
        }
    }
}

static inline void modelSetAnimationRate(_GSmodel *model, f32 rate)
{
    if (rate < 0.0f) return;
    if (__fpclassifyf(rate) == FP_NAN) return;
    if (model->mFlags & MODEL_FLAG_ANIM_PRESENT) {
        f32 vsyncRate = rate;
        if (GSgfxVideoGetVsyncRate() == 50) vsyncRate *= 1.2f;
        if (!(model->mFlags & MODEL_FLAG_BLEND_ACTIVE)) model->mAnim.mRate = vsyncRate;
        else {
            f32 frameCount = 1.0f + model->mAnim.mEndFrame;
            f32 blendFrameCount = 1.0f + model->mBlendAnim.mEndFrame;
            model->mBlendAnim.mRate = vsyncRate;
            model->mAnim.mRate = vsyncRate * (frameCount / blendFrameCount);
        }
    }
    if ((model->mFlags & MODEL_FLAG_TEXANIM_LINKED) && (model->mFlags & MODEL_FLAG_TEXANIM_PRESENT)) {
        model->mTexAnim.mRate = rate;
        if (GSgfxVideoGetVsyncRate() == 50) model->mTexAnim.mRate *= 1.2f;
    }
}

static inline void modelSetJointAnimationPolicy(_GSmodel *model, _HSD_JObj *joint, _HSD_JObj *blendJoint, s32 type)
{
    model->mAnim.mType = type;
    model->mBlendAnim.mType = type;
    if (model->mFlags & MODEL_FLAG_PART_ANIM_MIX) {
        modelPausePartAnimations(model, 1);
    }
    switch (model->mAnim.mType) {
    case MODEL_ANIM_PLAY_ONCE:
        HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK, (void (*)())_modelSetLoopFlag, HSD_ANIM_ARG_UNSIGNED, 0);
        if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE)
            HSD_ForeachAnim(blendJoint, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK, (void (*)())_modelSetLoopFlag, HSD_ANIM_ARG_UNSIGNED, 0);
        break;
    case MODEL_ANIM_PLAY_LOOP:
        HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK, (void (*)())_modelSetLoopFlag, HSD_ANIM_ARG_UNSIGNED, 1);
        if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE)
            HSD_ForeachAnim(blendJoint, HSD_OBJECT_JOINT, MODEL_ANIM_JOINT_MASK, (void (*)())_modelSetLoopFlag, HSD_ANIM_ARG_UNSIGNED, 1);
        break;
    }
    if (model->mFlags & MODEL_FLAG_PART_ANIM_MIX) {
        modelPausePartAnimations(model, 0);
    }
}

static inline void modelSetMaterialAnimationType(_GSmodel *model, s32 type)
{
    _HSD_JObj *joint = model->mJObj;
    if ((model->mFlags & MODEL_FLAG_NULL_JOINT) == MODEL_FLAG_NULL_JOINT)
        joint = joint->child;
    model->mTexAnim.mType = type;
    switch (model->mTexAnim.mType) {
    case MODEL_ANIM_PLAY_ONCE:
        HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_MATERIAL_MASK, (void (*)())_modelSetLoopFlag, HSD_ANIM_ARG_UNSIGNED, 0);
        break;
    case MODEL_ANIM_PLAY_LOOP:
        HSD_ForeachAnim(joint, HSD_OBJECT_JOINT, MODEL_ANIM_MATERIAL_MASK, (void (*)())_modelSetLoopFlag, HSD_ANIM_ARG_UNSIGNED, 1);
        break;
    }
}

static inline void modelSetAnimationType(_GSmodel *model, s32 type)
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

static inline void modelCalculatePartMixInline(GSvec *out, GSvec *part, unsigned long flags)
{
    switch (flags & GS_MODEL_PART_MIX_X_MASK) {
    case GS_MODEL_PART_MIX_KEEP: break;
    case GS_MODEL_PART_MIX_REPLACE: out->x = part->x; break;
    case GS_MODEL_PART_MIX_ADD: out->x += part->x; break;
    }
    switch (flags & GS_MODEL_PART_MIX_Y_MASK) {
    case GS_MODEL_PART_MIX_KEEP: break;
    case GS_MODEL_PART_MIX_REPLACE << 8: out->y = part->y; break;
    case GS_MODEL_PART_MIX_ADD << 8: out->y += part->y; break;
    }
    switch (flags & GS_MODEL_PART_MIX_Z_MASK) {
    case GS_MODEL_PART_MIX_KEEP: break;
    case GS_MODEL_PART_MIX_REPLACE << 16: out->z = part->z; break;
    case GS_MODEL_PART_MIX_ADD << 16: out->z += part->z; break;
    }
}

static inline void modelGetPartMixVector(_GSmodel *model, GSmodelPartMix *mix,
                                         GSpart *part, GSvec *out)
{
    GSpart *resolvedPart = part;
    if (resolvedPart == NULL) {
        resolvedPart = GSmodelGetPart(model, mix->mPartIndex);
        if (resolvedPart == NULL) return;
    }
    f32 scale = mix->mCallback(resolvedPart, mix->mType, mix->mCallbackData, out);
    if (scale != 1.0f) PSVECScale((Vec *)out, (Vec *)out, scale);
    if (part == NULL) GSpartFree(resolvedPart);
}

#endif
