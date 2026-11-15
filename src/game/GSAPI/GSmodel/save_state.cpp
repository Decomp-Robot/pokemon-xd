#include <GSmodelInternal.h>

/* Flags preserved across a GSmodelPushState/GSmodelPopState round trip. */
#define MODEL_FLAG_STATE_SAVE_MASK                                              \
    (MODEL_FLAG_VISIBLE | MODEL_FLAG_ANIM_RUNNING | MODEL_FLAG_TEXANIM_RUNNING | \
     MODEL_FLAG_ANIM_PAUSED | MODEL_FLAG_60FPS_ANIM | MODEL_FLAG_TEXANIM_LINKED | \
     MODEL_FLAG_NULL_JOINT | MODEL_FLAG_CHECK_BOUND | MODEL_FLAG_DRAW_BOUND)

extern "C" void GSmodelPopState(_GSmodel *model, GSmodelState *state)
{
    model->mFlags &= ~MODEL_FLAG_STATE_SAVE_MASK;
    model->mFlags |= state->mFlags;
    GSmodelSetPosition(model, &state->mPosition);
    GSmodelSetRotation(model, &state->mRotation);
    GSmodelSetScale(model, &state->mScale);
    if (state->mFlags & MODEL_FLAG_NULL_JOINT) {
        model->mFlags ^= MODEL_FLAG_NULL_JOINT;
        GSmodelAddNull(model, &state->mNullPosition, &state->mNullRotation,
                       &state->mNullScale);
    }
    model->mAnim.mIndex = -1;
    model->mTexAnim.mIndex = -1;
    GSmodelSetAnimIndex(model, state->mAnimIndex);
    GSmodelSetAnimFrame(model, state->mAnimFrame);
    GSmodelSetAnimRate(model, state->mAnimRate);
    GSmodelSetAnimType(model, state->mAnimType);
    GSmodelSetTexAnimIndex(model, state->mTexAnimIndex);
    GSmodelSetTexAnimFrame(model, state->mTexAnimFrame);
    GSmodelSetTexAnimRate(model, state->mTexAnimRate);
    GSmodelSetTexAnimType(model, state->mTexAnimType);
    if (state->mFlags & MODEL_FLAG_ANIM_RUNNING) {
        GSmodelStartAnimation(model);
    }
    if (state->mFlags & MODEL_FLAG_TEXANIM_RUNNING) {
        GSmodelStartTexAnimation(model);
    }
    GSmodelUpdateAnimation(model);
}

extern "C" void GSmodelPushState(_GSmodel *model, GSmodelState *state)
{
    state->mFlags = model->mFlags & MODEL_FLAG_STATE_SAVE_MASK;
    state->mPosition = *GSmodelGetPositionPtr(model);
    state->mRotation = *GSmodelGetRotationPtr(model);
    state->mScale = *GSmodelGetScalePtr(model);
    state->mAnimIndex = model->mAnim.mIndex;
    state->mTexAnimIndex = model->mTexAnim.mIndex;
    state->mAnimFrame = model->mAnim.mFrame;
    state->mAnimRate = GSmodelGetAnimRate(model);
    state->mAnimType = model->mAnim.mType;
    state->mTexAnimFrame = model->mTexAnim.mFrame;
    state->mTexAnimRate = GSmodelGetTexAnimRate(model);
    state->mTexAnimType = model->mTexAnim.mType;
    if (state->mFlags & MODEL_FLAG_NULL_JOINT) {
        _HSD_JObj *joint = model->mJObj->child;
        HSD_JObjGetTranslation(joint, (Vec *)&state->mNullPosition);
        state->mNullRotation.x = HSD_JObjGetRotationX(joint);
        state->mNullRotation.y = HSD_JObjGetRotationY(joint);
        state->mNullRotation.z = HSD_JObjGetRotationZ(joint);
        HSD_JObjGetScale(joint, (Vec *)&state->mNullScale);
    }
}
