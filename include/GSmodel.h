#ifndef GSMODEL_H
#define GSMODEL_H

#include <types.h>
#include <GSvec.h>
#include <GSattach.h>
#include <GSbound.h>
#include <GSgfx.h>
#include <GSpart.h>
#include <hsd/scene.h>

struct _HSD_AObj;
struct _HSD_AnimJoint;
struct _HSD_JObj;
struct _HSD_PObj;
struct _HSD_MatAnimJoint;
struct _HSD_ShapeAnimJoint;

struct GSgfxLayer;
struct _GScamera;
struct GSgfxVF;
struct _GSparticleBank;

enum GSmodelDistortionWarpFlags {
    GS_MODEL_WARP_S = 1,
    GS_MODEL_WARP_T = 2,
    GS_MODEL_WARP_REVERSE_S = 4,
    GS_MODEL_WARP_REVERSE_T = 8
};

/* Joint and material animation share this frame tracker. */
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
/* One dust emitter of a model. The monitored part triggers a burst at the
 * origin part after it stops falling. */
struct GSmodelDustEntry {
    f32 mPrevY;  // 0x00: emitter origin height at the last update.
    u8 mPart;    // 0x04: part supplying the spawned generator position.
    u8 mEmitter; // 0x05: part monitored for downward motion.
    u8 mPending; // 0x06: set once the emitter has moved far enough.
    u8 mActive;  // 0x07: nonzero while the entry is in use.
};

/* A joint hierarchy plus its animation, part and render state. */
struct _GSmodelAnimCBData {
    u32 mFlags;
    u32 mAnimIndex;
    void *mUserContext;
};
typedef _GSmodelAnimCBData GSmodelAnimEndedEvent;
struct _GSmodel;
typedef void (*GSmodelAnimEndedCallback)(_GSmodel *, GSmodelAnimEndedEvent *);

typedef GSattachMode GSmodelAttachmentType;

struct _GSmodel {
    u32 mFlags;               // 0x000: model flags.
    _GSmodel *mNext;          // 0x004: model-list link.
    _HSD_SceneModelSet *mRoot;              // 0x008: Scene model resource.
    _HSD_JObj *mJObj;         // 0x00C: runtime joint hierarchy of the model.
    _HSD_JObj *mBlendJObj;    // 0x010: joint the blend interpolates from.
    _HSD_JObj *mBlendJObjTo;  // 0x014: joint the blend interpolates to.
    f32 mPosition[3];         // 0x018: model translation.
    f32 mRotation[3];         // 0x024: model Euler rotation.
    f32 mScale[3];            // 0x030: model scale.
    GSbound mBound;           // 0x03C: bound accumulator, handed to the
                              //        GSbound entry points by address.
    _GScamera *mRenderCamera; // 0x05C: projection override; NULL uses active camera.
    u16 mAnimFlags;           // 0x060: fractional-frame handling flags.
    u8 _062[0x0A];
    s32 mMaxAnimIndex;        // 0x06C: joint-animation table entry count.
    s32 mMaxTexAnimIndex;     // 0x070: texture-animation table entry count.
    GSmodelAnimState mAnim;      // 0x074: primary joint animation.
    GSmodelAnimState mBlendAnim; // 0x090: blend destination animation.
    GSmodelAnimState mTexAnim;   // 0x0AC: material animation.
    GSmodelPartAnim mPartAnim[8]; // 0x0C8: part-joint animation slots.
    GSmodelPartAnim mPartTexAnim[4]; // 0x1A8: part-material animation slots.
    f32 mBlendFactor;         // 0x218: current blend interpolation factor.
    u8 mHasPartAnim;          // 0x21C
    u8 mHasPartTexAnim;       // 0x21D
    u8 _21E[2];
    GSmodelAnimEndedCallback mAnimEndedCB;       // 0x220: callback fired when an animation ends.
    void *mAnimEndedCBArg;    // 0x224: argument passed to mAnimEndedCB.
    GSmodelPartMix mPartMix[16]; // 0x228: per-part transform mixing.
    GSmodelAttachmentType mAttachmentType; // 0x5A8
    _GSmodel *mAttachedModel; // 0x5AC
    s32 mAttachedPartIndex; // 0x5B0
    GSvec mAttachedPosition; // 0x5B4
    GSvec mAttachedRotation; // 0x5C0
    GSvec mAttachedScale; // 0x5CC
    _GSparticleBank *mLinkedParticleBank; // 0x5D8: linked animation particles.
    u8 _5DC[0x1C];
    _GSparticleBank *mDustParticleBank; // 0x5F8: bank used by the dust emitters.
    GSmodelDustEntry mDust[2]; // 0x5FC: the two dust emitter entries.
    f32 mDustScale;           // 0x60C: scale the dust emitters inherit.
    u16 mDistortionPart;      // 0x610: part the distortion pass reads from.
    u8 mDistortionMaterial;   // 0x612: material index of that part.
    u8 mDistortionFlags;      // 0x613: distortion warp axis and sign flags.
    _HSD_PObj *mDistortionPrimitive; // 0x614: distortion primitive display-list descriptor.
    void *mDistortionTexture;     // 0x618: copied distortion texture.
    void *mDistortionVF;      // 0x61C: vertex formatter of the distortion pass.
};

/* Called once per primitive object. The fourth argument is the position
 * matrix palette, or NULL when unavailable; the fifth is caller context. */
typedef void (*GSmodelParseCb)(_HSD_PObj *pobj, GSmtx *referenceMatrix,
                               GSmtx *positionMatrix, GSmtx *matrixPalette,
                               void *userContext);

extern "C" {
void GSmodelFree(_GSmodel *model);
void GSmodelSetTextureOverride(_GSmodel *model, GStexture *texture);
void GSmodelResetTextureOverride(_GSmodel *model);
void *GSmodelGetBound(_GSmodel *model);
void GSmodelForceAnimTransformUpdate(_GSmodel *model);
void GSmodelSetAnimRate(_GSmodel *model, f32 rate);
void GSmodelSetAnimIndex(_GSmodel *model, unsigned long index);
void GSmodelSetAnimFrame(_GSmodel *model, f32 frame);
void GSmodelStartAnimation(_GSmodel *model);
void GSmodelSetPosition(_GSmodel *model, GSvec *position);
void GSmodelSetVisibility(_GSmodel *model, s32 visible);
void GSmodelSetRenderLayer(_GSmodel *model, s32 count, GSgfxLayerID *layers);
void GSmodelSetRenderCamera(_GSmodel *model, void *camera);
void GSmodelSetBoundCheck(_GSmodel *model, s32 enabled);
void GSmodelSetShadowFlags(_GSmodel *model, u32 flags);
void GSmodelSetShadowLight(_GSmodel *model, void *light);
void GSmodelSetShadowSurface(_GSmodel *model, s32 count, _GSmodel **surfaces);
f32 GSmodelGetAnimRate(_GSmodel *model);
void GSmodelUpdateAnimation(_GSmodel *model);
f32 GSmodelGetAnimFrameCount(_GSmodel *model, s32 index);
s32 GSmodelIsNullAnimation(_GSmodel *model, s32 index);
void GSmodelTraverseAnimJoint(void *jobj, void (*cb)(_HSD_AnimJoint *, void **, int), f32 *arg);
void GSmodelGetPosition(_GSmodel *model, GSvec *position);
void GSmodelGetRotation(_GSmodel *model, GSvec *rotation);
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
void GSmodelSetTexAnimIndex(_GSmodel *model, unsigned long index);
void GSmodelSetPartTexAnimIndex(_GSmodel *model, s32 partIndex,
                                           s32 materialIndex, s32 animIndex,
                                           s32 animType, f32 frame);
void GSmodelGetFrameCount(_GSmodel *model, f32 *frameCount,
                                     f32 *blendFrameCount);
f32 GSmodelGetAnimFrame(_GSmodel *model);
s32 GSmodelGetAnimType(_GSmodel *model);
void GSmodelGetAnimIndex(_GSmodel *model, unsigned long *index,
                                    unsigned long *blendIndex);
u8 GSmodelIsBlending(_GSmodel *model);
void GSmodelSetBlendFactor(_GSmodel *model, f32 factor);
void GSmodelSetBlendAnimIndex(_GSmodel *model, unsigned long fromIndex,
    unsigned long toIndex);
void GSmodelSetBlendAnimFrameForce(_GSmodel *model, f32 fromFrame, f32 toFrame);
void GSmodelAllUnpauseAnimation(void);
void GSmodelAllPauseAnimation(void);
u8 GSmodelHasAnimationEnded(_GSmodel *model);
u8 GSmodelIsAnimating(_GSmodel *model);
void GSmodelStopAnimation(_GSmodel *model);
void GSmodelSetAnimType(_GSmodel *model, s32 type);
void GSmodelSetDontResetPartAnimFlag(_GSmodel *model, u8 enabled);
void GSmodelSetKeepPartAnimFlag(_GSmodel *model, u8 enabled);
void GSmodelSetPartAnimIndex(_GSmodel *model, s32 partIndex,
                                      s32 animIndex, s32 animType, f32 frame);
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
}

#endif
