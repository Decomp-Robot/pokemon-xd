#ifndef GS_MODEL_H
#define GS_MODEL_H

#include <types.h>
#include <GSvec.h>
#include <GSattach.h>
#include <GSbound.h>
#include <GSgfx.h>
#include <GSpart.h>
#include <hsd/scene.h>

// Animation mode used by model and part animation controls.
enum GSmodelAnimType {
    GS_MODEL_ANIM_ONCE = 0,
    GS_MODEL_ANIM_LOOP = 1
};


struct _HSD_AObj;
struct _HSD_AnimJoint;
struct _HSD_JObj;
struct _HSD_PObj;
struct _HSD_MatAnimJoint;
struct _HSD_ShapeAnimJoint;
struct GSgfxLayer;
struct _GScamera;
struct _GSlight;
struct GSgfxVF;
struct _GSparticleBank;

enum GSmodelDistortionWarpFlags {
    GS_MODEL_WARP_S = 1,
    GS_MODEL_WARP_T = 2,
    GS_MODEL_WARP_REVERSE_S = 4,
    GS_MODEL_WARP_REVERSE_T = 8
};

struct GSmodelAnimState {
    s32 mType;
    u32 mIndex;
    f32 mRate;
    f32 mFrame;
    f32 mRequestedFrame;
    f32 mEndFrame;
    _HSD_AObj *mAnimation;
};
typedef GSmodelAnimState GSmodelPartAnim;

struct GSmodelPartMix {
    GSpartMixType mType;
    s32 mPartIndex;
    f32 (*mCallback)(GSpart *, GSpartMixType, void *, GSvec *);
    void *mCallbackData;
    unsigned long mFlags;
    GSvec mBasePosition;
    GSvec mBaseRotation;
    GSvec mBaseScale;
};

enum GSmodelPartMixOperation {
    GS_MODEL_PART_MIX_KEEP = 0,
    GS_MODEL_PART_MIX_REPLACE = 1,
    GS_MODEL_PART_MIX_ADD = 2,
    GS_MODEL_PART_MIX_X_MASK = 0xFF,
    GS_MODEL_PART_MIX_Y_MASK = 0xFF00,
    GS_MODEL_PART_MIX_Z_MASK = 0xFF0000
};

struct GSmodelDustEntry {
    f32 mPrevY;
    u8 mPart;
    u8 mEmitter;
    u8 mPending;
    u8 mActive;
};

struct _GSmodelAnimCBData {
    u32 mFlags;
    u32 mAnimIndex;
    void *mUserContext;
};
typedef _GSmodelAnimCBData GSmodelAnimEndedEvent;
struct _GSmodel;
typedef struct _GSmodel GSmodel;
typedef void (*GSmodelAnimEndedCallback)(_GSmodel *, GSmodelAnimEndedEvent *);
typedef GSattachMode GSmodelAttachmentType;

struct _GSmodel {
    u32 mFlags;
    _GSmodel *mNext;
    _HSD_SceneModelSet *mRoot;
    _HSD_JObj *mJObj;
    _HSD_JObj *mBlendJObj;
    _HSD_JObj *mBlendJObjTo;
    GSvec mPosition;
    GSvec mRotation;
    GSvec mScale;
    GSbound mBound;
    _GScamera *mRenderCamera;
    u16 mAnimFlags;
    u8 _062[0x0A];
    s32 mMaxAnimIndex;
    s32 mMaxTexAnimIndex;
    GSmodelAnimState mAnim;
    GSmodelAnimState mBlendAnim;
    GSmodelAnimState mTexAnim;
    GSmodelPartAnim mPartAnim[8];
    GSmodelPartAnim mPartTexAnim[4];
    f32 mBlendFactor;
    u8 mHasPartAnim;
    u8 mHasPartTexAnim;
    u8 _21E[2];
    GSmodelAnimEndedCallback mAnimEndedCB;
    void *mAnimEndedCBArg;
    GSmodelPartMix mPartMix[16];
    GSmodelAttachmentType mAttachmentType;
    _GSmodel *mAttachedModel;
    s32 mAttachedPartIndex;
    GSvec mAttachedPosition;
    GSvec mAttachedRotation;
    GSvec mAttachedScale;
    _GSparticleBank *mLinkedParticleBank;
    u8 _5DC[0x1C];
    _GSparticleBank *mDustParticleBank;
    GSmodelDustEntry mDust[2];
    f32 mDustScale;
    u16 mDistortionPart;
    u8 mDistortionMaterial;
    u8 mDistortionFlags;
    _HSD_PObj *mDistortionPrimitive;
    GStexture *mDistortionTexture;
    GSgfxVF *mDistortionVF;
};

typedef void (*GSmodelParseCb)(_HSD_PObj *, GSmtx *, GSmtx *, GSmtx *, void *);

#ifdef __cplusplus
extern "C" {
_GSmodel* GSmodelLoad(_HSD_SceneModelSet* modelSet, u32 flags);
void GSmodelFree(_GSmodel* model);
void GSmodelAttachToGSpart(_GSmodel* model, GSpart* part, GSattachMode mode, u8 inheritScale, u8 keepTransform);
#endif
GSbound *GSmodelGetBound(_GSmodel *model);
void GSmodelAddPosition(GSmodel *model, GSvec *position);
void GSmodelRecalculateBound(GSmodel *model);
void GSmodelIterateList(void (*callback)(GSmodel *, void *), void *userContext);
void GSmodelForceAnimTransformUpdate(_GSmodel *model);
void GSmodelSetAnimRate(_GSmodel *model, f32 rate);
void GSmodelSetAnimIndex(_GSmodel *model, u32 index);
void GSmodelSetAnimFrame(_GSmodel *model, f32 frame);
void GSmodelStartAnimation(_GSmodel *model);
void GSmodelSetPosition(_GSmodel *model, GSvec *position);
void GSmodelSetVisibility(_GSmodel *model, bool visible);
void GSmodelSetRenderLayer(_GSmodel *model, s32 count, GSgfxLayerID *layers);
void GSmodelSetRenderCamera(_GSmodel *model, _GScamera *camera);
void GSmodelSetBoundCheck(_GSmodel *model, s32 enabled);
void GSmodelSetShadowFlags(_GSmodel *model, u32 flags);
void GSmodelSetShadowLight(_GSmodel *model, _GSlight *light);
void GSmodelSetShadowSurface(_GSmodel *model, s32 count, _GSmodel **surfaces);
f32 GSmodelGetAnimRate(_GSmodel *model);
void GSmodelUpdateAnimation(_GSmodel *model);
f32 GSmodelGetAnimFrameCount(_GSmodel *model, s32 index);
s32 GSmodelIsNullAnimation(_GSmodel *model, s32 index);
void GSmodelTraverseAnimJoint(void *jobj, void (*cb)(_HSD_AnimJoint *, void **, int), f32 *arg);
GSvec *GSmodelGetPositionPtr(_GSmodel *model);
void GSmodelGetPosition(_GSmodel *model, GSvec *position);
GSvec *GSmodelGetRotationPtr(_GSmodel *model);
void GSmodelGetRotation(_GSmodel *model, GSvec *rotation);
void GSmodelSetRotation(_GSmodel *model, GSvec *rotation);
void GSmodelParse(_GSmodel *model, s32 mode, GSmodelParseCb cb, void *userContext);
u8 GSmodelCanShapeAnimate(_GSmodel *model);
u8 GSmodelCanTexAnimate(_GSmodel *model);
u8 GSmodelCanAnimate(_GSmodel *model);
u8 GSmodelHasTexAnimationEnded(_GSmodel *model);
void GSmodelStopTexAnimation(_GSmodel *model);
void GSmodelStartTexAnimation(_GSmodel *model);
void GSmodelSetTexAnimType(_GSmodel *model, s32 type);
void GSmodelSetTexAnimFrame(_GSmodel *model, f32 frame);
f32 GSmodelGetTexAnimRate(_GSmodel *model);
void GSmodelSetTexAnimRate(_GSmodel *model, f32 rate);
void GSmodelSetTexAnimIndex(_GSmodel *model, u32 index);
void GSmodelSetPartTexAnimIndex(_GSmodel *model, s32 partIndex, s32 materialIndex, s32 animIndex, s32 animType, f32 frame);
void GSmodelGetFrameCount(_GSmodel *model, f32 *frameCount, f32 *blendFrameCount);
f32 GSmodelGetAnimFrame(_GSmodel *model);
s32 GSmodelGetAnimType(_GSmodel *model);
void GSmodelGetAnimIndex(_GSmodel *model, u32 *index, u32 *blendIndex);
u8 GSmodelIsBlending(_GSmodel *model);
void GSmodelSetBlendFactor(_GSmodel *model, f32 factor);
void GSmodelSetBlendAnimIndex(_GSmodel *model, u32 fromIndex, u32 toIndex);
void GSmodelSetBlendAnimFrameForce(_GSmodel *model, f32 fromFrame, f32 toFrame);
void GSmodelAllUnpauseAnimation(void);
void GSmodelAllPauseAnimation(void);
u8 GSmodelHasAnimationEnded(_GSmodel *model);
u8 GSmodelIsAnimating(_GSmodel *model);
void GSmodelStopAnimation(_GSmodel *model);
void GSmodelSetAnimType(_GSmodel *model, s32 type);
void GSmodelSetDontResetPartAnimFlag(_GSmodel *model, u8 enabled);
void GSmodelSetKeepPartAnimFlag(_GSmodel *model, u8 enabled);
void GSmodelSetPartAnimIndex(_GSmodel *model, s32 partIndex, s32 animIndex, s32 animType, f32 frame);
void GSmodelAdvanceAnimation(_GSmodel *model, f32 frameStep);
void GSmodelRemoveRootJointAnimation(_GSmodel *model);
void GSmodelUseRootJointAnimation(_GSmodel *model, u8 use);
void GSmodelSetAnimEndedCallback(_GSmodel *model, GSmodelAnimEndedCallback cb, void *arg);
void GSmodelSetForceUseFractionalFramesFlag(_GSmodel *model, u8 on);
void GSmodelSetSkipFractionalFramesFlag(_GSmodel *model, u8 on);
void GSmodelSet60fpsAnimFlag(_GSmodel *model, u8 on);
void GSmodelLinkTexAnimToAnim(_GSmodel *model, u8 link);
s32 GSmodelGetMaxTexAnimIndex(_GSmodel *model);
s32 GSmodelGetMaxAnimIndex(_GSmodel *model);
void GSmodelRemoveNull(_GSmodel *model);
void GSmodelDetachFromGSpart(_GSmodel *model, u8 keepTransform);
#ifdef __cplusplus
}
#endif

#endif
