#include <GSmodelInternal.h>
#include <GSlight.h>
#include <dolphin/os/OSFastCast.h>
#include <dolphin/mtx.h>
#include <GSmtx.h>
#include <GSAPI/GSsysdolphin_baselib/baselib/quatlib.h>
#include <HSDScene.h>
#include <GSmaterial.h>
#include <hsd/jobj.h>
#include <dolphin/gx/GXEnum.h>

extern "C" void GSmodelDrawModel(_GSmodel *model, GSgfxLayer *layer,
                                   GSgfxLayerPass pass);
extern void _modelRenderModelSub(GSgfxLayer *layer, GSgfxLayerPass pass,
                                 _GSmodel *model);

enum { kModelRenderPriority = 32 };

void _modelFastRender(void)
{
    if (modelShadowDebugEnabled == 1) modelShadowRenderDebug();
    modelDistortionRender();
}

static inline void modelLayerViewport(GSgfxLayer *layer, f32 viewportLeft, f32 viewportTop,
                                      f32 halfWidth, f32 halfHeight,
                                      bool fullEFB)
{
    if (fullEFB) {
        u16 viewportHeight, viewportWidth, width, height;
        GSgfxVideoGetEFBSize(&width, &height);
        viewportLeft = 0.0f;
        viewportTop = 0.0f;
        viewportWidth = width >> 1;
        halfWidth = __OSu16tof32(&viewportWidth);
        viewportHeight = height >> 1;
        halfHeight = __OSu16tof32(&viewportHeight);
    }
    layer->mProjectionState.mViewportLeft = viewportLeft;
    layer->mProjectionState.mViewportTop = viewportTop;
    layer->mProjectionState.mViewportCentreX = halfWidth;
    layer->mProjectionState.mViewportCentreY = halfHeight;
    layer->mPassState[0].mGroupBFlags |= GS_GFX_RASTER_DIRTY_VIEWPORT;
    layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
    layer->mPassState[1].mGroupBFlags |= GS_GFX_RASTER_DIRTY_VIEWPORT;
    layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
}

void _modelRender(GSgfxLayer *layer, GSgfxLayerPass pass)
{
    u32 perfbarEntry;
    u8 layerID = (u8)layer->mLayerID;

    if (modelLoading == 1) {
        layer->mRenderFlags |= GSGFX_RENDER_FLAG_ZDRAW_BANK;
        layer->mRenderFlags |= GSGFX_RENDER_FLAG_ZDRAW_GENERATOR;
        modelLoading = 0;
    }

    if (layerID == modelShadowLayerID) {
        perfbarEntry = GSgfxPerfbarCPUEntry(GSGFX_PERFBAR_MODEL_SHADOW);
        modelShadowRender(layer);

        modelLayerViewport(layer, 0.0f, 0.0f, 320.0f, 240.0f,
                           pass == GSGFX_LAYER_PASS_OPAQUE);
        layer->mProjectionState.mScissorLeft = 0;
        layer->mProjectionState.mScissorTop = 0;
        GSgfxVideoGetEFBSize(&layer->mProjectionState.mScissorWidth,
                             &layer->mProjectionState.mScissorHeight);
        layer->mPassState[0].mGroupBFlags |= GS_GFX_RASTER_DIRTY_SCISSOR;
        layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
        layer->mPassState[1].mGroupBFlags |= GS_GFX_RASTER_DIRTY_SCISSOR;
        layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
        layer->FlushState();
    } else {
        bool fog;
        _GSmodel *model;
        u8 rendered = 0;
        perfbarEntry = GSgfxPerfbarCPUEntry(GSGFX_PERFBAR_MODEL_RENDER);
        if (layer->mRenderMode == GSGFX_RENDER_MODE_BLUE_CHANNEL)
            HSD_InitColorSwapTable(GX_TEV_SWAP0, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
        else
            HSD_InitColorSwapTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        HSD_InitColorSwapTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        HSD_InitColorSwapTable(GX_TEV_SWAP2, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        HSD_InitColorSwapTable(GX_TEV_SWAP3, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        model = gsModelList;
        fog = layer->mFog != 0;
        for (; model != 0; model = model->mNext) {
            GSgfxTokenIfClearedReset(&model->mRenderToken);
            if (model->mFlags & MODEL_FLAG_FRAME_VISIBLE) {
                for (s32 i = 0; i < 8; ++i) {
                    if (model->mRenderLayers[i] == layerID) {
                        _modelRenderModelSub(layer, pass, model);
                        rendered = 1;
                        break;
                    }
                }
            }
        }
        if (rendered) {
            HSD_ResetColorSwapTables();
            GScameraLoadActiveCameraMatrix(layer->mLayerID);
        }
        layer->fog(fog);
    }

    GSgfxPerfbarCPUEntry(perfbarEntry);
}

extern "C" void GSmodelDrawModel(_GSmodel *model, GSgfxLayer *layer,
                                  GSgfxLayerPass pass)
{
    bool oldFog;

    if (model != 0 && (model->mFlags & MODEL_FLAG_RENDER_ENABLED) != 0) {
        oldFog = layer->mFog != 0;
        if (layer->mRenderMode == GSGFX_RENDER_MODE_BLUE_CHANNEL)
            HSD_InitColorSwapTable(GX_TEV_SWAP0, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
        else
            HSD_InitColorSwapTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        HSD_InitColorSwapTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        HSD_InitColorSwapTable(GX_TEV_SWAP2, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        HSD_InitColorSwapTable(GX_TEV_SWAP3, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        _modelRenderModelSub(layer, pass, model);
        HSD_ResetColorSwapTables();
        layer->fog(oldFog);
    }
}

void _modelRenderModelSub(GSgfxLayer *layer, GSgfxLayerPass pass,
                          _GSmodel *model)
{
    _GScamera *camera;
    modelUpdateTransform(model);
    bool oldFog = layer->mFog != 0;
    if (layer->mRenderMode != GSGFX_RENDER_MODE_NORMAL) return;
    switch (pass) {
    case GSGFX_LAYER_PASS_OPAQUE: {
        if (oldFog != ((model->mFlags & MODEL_FLAG_OPAQUE_FOG_ENABLED) == MODEL_FLAG_OPAQUE_FOG_ENABLED))
            layer->fog((model->mFlags & MODEL_FLAG_OPAQUE_FOG_ENABLED) == MODEL_FLAG_OPAQUE_FOG_ENABLED);
        break;
    }
    case GSGFX_LAYER_PASS_XLU: {
        if (oldFog != ((model->mFlags & MODEL_FLAG_TRANSLUCENT_FOG_ENABLED) == MODEL_FLAG_TRANSLUCENT_FOG_ENABLED))
            layer->fog((model->mFlags & MODEL_FLAG_TRANSLUCENT_FOG_ENABLED) == MODEL_FLAG_TRANSLUCENT_FOG_ENABLED);
        break;
    }
    }

    camera = model->mRenderCamera;
    if (camera == 0) camera = GScameraGetActiveCamera();
    if (camera != 0 && camera->mCObj != 0) {
        GScameraLoadProjection(camera, layer);
        layer->mClipMode = GSGFX_CLIP_ENABLED;
        for (s32 i = 0; i < 2; ++i) {
            layer->mPassState[i].mGroupBFlags |= GS_GFX_RASTER_DIRTY_CLIP;
            layer->mPassState[i].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
        }
        layer->FlushState();
        HSD_StateInvalidate(0xFFFFFFFF);
        GSlightSetupLights(camera->mCObj);
        modelShadowPrepare(model, true);
        _HSD_JObj *renderJoint;
        if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE)
            renderJoint = model->mBlendJObjTo;
        else
            renderJoint = model->mJObj;
        switch (pass) {
        case GSGFX_LAYER_PASS_OPAQUE:
            HSD_JObjDispAll(renderJoint, 0, HSD_TRSP_OPA, 0);
            break;
        case GSGFX_LAYER_PASS_XLU:
            HSD_JObjDispAll(renderJoint, 0, HSD_TRSP_XLU | HSD_TRSP_TEXEDGE, 0);
            break;
        }

        GSgfxFifo.u8 = GSGFX_FIFO_LOAD_XF;
        GSgfxFifo.u16 = 0;
        GSgfxFifo.u16 = GSGFX_XF_CLIP_DISABLE;
        GSgfxFifo.u32 = 0;
        modelShadowPrepare(model, false);
        layer->InvalidateState();
        model->mRenderToken = layer->tokenMark();
    }

    if (pass == GSGFX_LAYER_PASS_XLU) {
        if (model->mFlags & MODEL_FLAG_RENDER_MARUKAGE)
            modelShadowRenderMarukage(model, layer);
        if (model->mFlags & MODEL_FLAG_DRAW_BOUND)
            GSboundDraw(&model->mBound, layer);
    }
}

extern "C" f32 GSmodelGetDistanceFromCamera(_GSmodel *model)
{
    _GScamera *camera = model->mRenderCamera;
    GSvec modelPosition;
    if (camera == 0) camera = GScameraGetActiveCamera();
    if (camera == 0) return 0.0f;
    GSboundGetWorldPosition(&model->mBound, &modelPosition);
    return PSVECDistance((Vec *)&modelPosition, (Vec *)&camera->mPosition);
}

extern "C" void GSmodelSetRenderCamera(_GSmodel *model, _GScamera *camera)
{
    model->mRenderCamera = camera;
}

extern "C" void GSmodelSetRenderLayer(_GSmodel *model, s32 count,
                                       GSgfxLayerID *layers)
{
    for (s32 i = 0; i < 8; ++i) {
        if (i < count) model->mRenderLayers[i] = *layers;
        else model->mRenderLayers[i] = GSGFX_LAYER_INVALID;
        ++layers;
    }
}

void _modelPostUpdate60fps(void)
{
    u32 perfbarEntry = GSgfxPerfbarCPUEntry(GSGFX_PERFBAR_MODEL_ANIMATION);
    _GSmodel *model;
    for (model = gsModelList; model != 0; model = model->mNext) {
        if (modelProcessAt60fps(model) == 1)
            GSmodelAdvanceAnimation(model, 1.0f);
    }
    GSgfxPerfbarCPUEntry(perfbarEntry);
}

void _modelPostUpdate(f32 delta)
{
    u32 perfbarEntry = GSgfxPerfbarCPUEntry(GSGFX_PERFBAR_MODEL_ANIMATION);
    _GSmodel *model;
    for (model = gsModelList; model != 0; model = model->mNext) {
        if (modelProcessAt60fps(model) == 0)
            GSmodelAdvanceAnimation(model, delta);
        modelDustUpdate(model);
    }
    GSgfxPerfbarCPUEntry(perfbarEntry);
}

void _modelPreUpdate(f32)
{
    u32 perfbarEntry;
    _GSmodel *model;
    _GSmodel *updateModel;

    GSgfxProjectSetupParameters();
    perfbarEntry = GSgfxPerfbarCPUEntry(GSGFX_PERFBAR_MODEL_ANIMATION);
    for (model = gsModelList; model != 0; model = model->mNext) {
        model->mFlags &= ~MODEL_FLAG_FRAME_START_CLEAR_MASK;
        if (model->mFlags & MODEL_FLAG_VISIBLE) {
            if (model->mRenderCamera == 0 &&
                (model->mFlags & MODEL_FLAG_CHECK_BOUND) != 0) {
                if (GSboundIsInViewFast(&model->mBound))
                    model->mFlags |= MODEL_FLAG_FRAME_VISIBLE;
            } else {
                model->mFlags |= MODEL_FLAG_FRAME_VISIBLE;
            }
            if ((model->mFlags & MODEL_FLAG_FRAME_VISIBLE) != 0 ||
                (model->mAnimFlags & MODEL_ANIM_FLAG_UPDATE_OUTSIDE_VIEW) != 0)
                GSmodelUpdateAnimation(model);
        }
    }
    for (updateModel = gsModelList; updateModel != 0; updateModel = updateModel->mNext) {
        if (updateModel->mFlags & MODEL_FLAG_VISIBLE) {
            modelUpdateAttachments(updateModel);
            modelUpdateTransform(updateModel);
        }
    }
    for (updateModel = gsModelList; updateModel != 0; updateModel = updateModel->mNext) {
        if (updateModel->mFlags & MODEL_FLAG_FRAME_VISIBLE)
            modelApplyPartAnimMixes(updateModel);
    }
    modelShadowUpdate();
    GSgfxPerfbarCPUEntry(perfbarEntry);
}

extern "C" void GSmodelIterateList(void (*callback)(GSmodel *, void *),
                                   void *userContext)
{
    for (GSmodel *model = (GSmodel *)gsModelList; model != 0;
         model = model->mNext)
        callback(model, userContext);
}

extern "C" _GSmodel *GSmodelSearchModelList(_HSD_JObj *root)
{
    _GSmodel *model;
    for (model = gsModelList; model != 0; model = model->mNext) {
        _HSD_JObj *modelRoot = (model->mFlags & MODEL_FLAG_BLEND_ACTIVE)
                                   ? model->mBlendJObjTo : model->mJObj;
        if (modelRoot == root) return model;
    }
    return 0;
}

extern "C" void GSmodelDestroyLinkedParticles(_GSmodel *model)
{
    if (model->mLinkedParticleBank != 0)
        GSparticleDestroyAllGenerators(model->mLinkedParticleBank, true);
}

extern "C" GSattachMode GSmodelGetGSparticleLinkAttachMode(_GSmodel *model)
{
    return model->mParticleLinkAttachMode;
}

extern "C" _GSparticleBank *GSmodelGetLinkedGSparticleBank(_GSmodel *model)
{
    return model->mLinkedParticleBank;
}

extern "C" void GSmodelSetGSparticleLinkAttachMode(_GSmodel *model,
                                                    GSattachMode mode)
{
    model->mParticleLinkAttachMode = mode;
}

extern "C" void GSmodelLinkToGSparticleBank(_GSmodel *model,
                                             _GSparticleBank *bank)
{
    if (model->mLinkedParticleBank == bank) return;
    model->mLinkedParticleBank = bank;
    if (model->mLinkedParticleBank != 0) {
        model->mAnimFlags |= MODEL_ANIM_FLAG_UPDATE_OUTSIDE_VIEW;
        GSmodelSetAnimFrame(model, model->mAnim.mFrame);
        GSmodelForceAnimTransformUpdate(model);
    } else {
        model->mAnimFlags &= ~MODEL_ANIM_FLAG_UPDATE_OUTSIDE_VIEW;
    }
}

extern "C" GSbound *GSmodelGetBound(_GSmodel *model)
{
    return &model->mBound;
}

extern "C" void GSmodelSetBoundCheck(_GSmodel *model, u8 enabled)
{
    if (enabled) {
        model->mFlags |= MODEL_FLAG_CHECK_BOUND;
    } else {
        model->mFlags &= ~MODEL_FLAG_CHECK_BOUND;
    }
}

extern "C" void GSmodelSetFogState(_GSmodel *model, u8 opaqueFogEnabled, u8 translucentFogEnabled)
{
    if (opaqueFogEnabled) {
        model->mFlags |= MODEL_FLAG_OPAQUE_FOG_ENABLED;
    } else {
        model->mFlags &= ~MODEL_FLAG_OPAQUE_FOG_ENABLED;
    }
    if (translucentFogEnabled) {
        model->mFlags |= MODEL_FLAG_TRANSLUCENT_FOG_ENABLED;
    } else {
        model->mFlags &= ~MODEL_FLAG_TRANSLUCENT_FOG_ENABLED;
    }
}

extern "C" GSvec *GSmodelGetScalePtr(_GSmodel *model)
{
    return &model->mScale;
}

extern "C" GSvec *GSmodelGetRotationPtr(_GSmodel *model)
{
    return &model->mRotation;
}

extern "C" GSvec *GSmodelGetPositionPtr(_GSmodel *model)
{
    return &model->mPosition;
}

extern "C" void GSmodelUpdate(_GSmodel *model)
{
    u32 perfbarEntry = GSgfxPerfbarCPUEntry(GSGFX_PERFBAR_MODEL_UPDATE);
    modelUpdateAttachments(model);
    modelUpdateTransform(model);
    GSgfxPerfbarCPUEntry(perfbarEntry);
}

extern "C" bool GSmodelGetVisibility(_GSmodel *model)
{
    return (model->mFlags & MODEL_FLAG_VISIBLE) != 0;
}

extern "C" void GSmodelGetScale(_GSmodel *model, GSvec *scale)
{
    scale->x = model->mScale.x;
    scale->y = model->mScale.y;
    scale->z = model->mScale.z;
}

extern "C" void GSmodelGetRotation(_GSmodel *model, GSvec *rotation)
{
    rotation->x = model->mRotation.x;
    rotation->y = model->mRotation.y;
    rotation->z = model->mRotation.z;
}

extern "C" void GSmodelGetPosition(_GSmodel *model, GSvec *position)
{
    position->x = model->mPosition.x;
    position->y = model->mPosition.y;
    position->z = model->mPosition.z;
}

extern "C" void GSmodelAddPosition(_GSmodel *model, GSvec *position)
{
    GSvec result;
    if (model->mAttachmentType == GS_ATTACH_NONE) {
        result = model->mPosition;
    } else {
        result = model->mAttachedPosition;
    }
    PSVECAdd((Vec *)&result, (Vec *)position, (Vec *)&result);
    if (model->mAttachmentType == GS_ATTACH_NONE) {
        if (!(model->mFlags & MODEL_FLAG_ROOT_JOINT_POSE)) {
            model->mPosition = result;
        }
    } else {
        model->mAttachedPosition = result;
    }
    model->mFlags &= ~MODEL_FLAG_TRANSFORM_UPDATED;
}

extern "C" void GSmodelSetVisibility(_GSmodel *model, bool visible)
{
    if (visible) {
        model->mFlags |= MODEL_FLAG_VISIBLE;
        if (model->mLinkedParticleBank) {
            GSparticleSetBankVisibility(model->mLinkedParticleBank, 1);
        }
    } else {
        model->mFlags &= ~MODEL_FLAG_VISIBLE;
        if (model->mLinkedParticleBank) {
            GSparticleSetBankVisibility(model->mLinkedParticleBank, 0);
        }
    }
}

extern "C" void GSmodelSetScale(_GSmodel *model, GSvec *scale)
{
    if (model->mAttachmentType == GS_ATTACH_NONE) {
        if (!(model->mFlags & MODEL_FLAG_ROOT_JOINT_POSE)) {
            model->mScale = *scale;
        }
    } else {
        model->mAttachedScale = *scale;
    }
    model->mFlags &= ~MODEL_FLAG_TRANSFORM_UPDATED;
}

extern "C" void GSmodelSetRotation(_GSmodel *model, GSvec *rotation)
{
    if (model->mAttachmentType == GS_ATTACH_NONE) {
        if (!(model->mFlags & MODEL_FLAG_ROOT_JOINT_POSE)) {
            model->mRotation = *rotation;
        }
    } else {
        model->mAttachedRotation = *rotation;
    }
    model->mFlags &= ~MODEL_FLAG_TRANSFORM_UPDATED;
}

extern "C" void GSmodelSetPosition(_GSmodel *model, GSvec *position)
{
    if (model->mAttachmentType == GS_ATTACH_NONE) {
        if (!(model->mFlags & MODEL_FLAG_ROOT_JOINT_POSE)) {
            model->mPosition = *position;
        }
    } else {
        model->mAttachedPosition = *position;
    }
    model->mFlags &= ~MODEL_FLAG_TRANSFORM_UPDATED;
}

extern "C" GSmtx *GSmodelGetMatrixPtr(_GSmodel *model)
{
    HSD_JObjSetupMatrix(model->mJObj);
    return (GSmtx *)&model->mJObj->mMatrix;
}

extern "C" void GSmodelSetMatrix(_GSmodel *model, GSmtx *matrix)
{
    _HSD_JObj *joint = model->mJObj;
    HSD_JObjSetMatrix(joint, matrix);
}

extern "C" void GSmodelDetachFromGSpart(_GSmodel *model, u8 keepTransform)
{
    if (model->mAttachmentType != GS_ATTACH_NONE) {
        _HSD_JObj *jobj = model->mJObj;
        HSD_JObjClearMatrix(jobj);
        model->mAttachedModel = 0;
        model->mAttachedPartIndex = 0;
        model->mFlags &= ~MODEL_FLAG_ATTACHMENT_DETACH;
        model->mAttachmentType = GS_ATTACH_NONE;
        if (keepTransform) {
            GSmodelSetPosition(model, &model->mAttachedPosition);
            GSmodelSetRotation(model, &model->mAttachedRotation);
            GSmodelSetScale(model, &model->mAttachedScale);
        }
    }
}

extern "C" void GSmodelAttachToGSpart(_GSmodel *model, GSpart *part,
                                        GSattachMode mode, u8 detachAfterUpdate,
                                        u8 snapToPart)
{
    if (model->mAttachmentType == GS_ATTACH_NONE && mode != GS_ATTACH_NONE) {
        model->mAttachedModel = part->mModel;
        model->mAttachedPartIndex = part->mIndex;
        if (snapToPart == 1) {
            model->mAttachedPosition.x = 0.0f;
            model->mAttachedPosition.y = 0.0f;
            model->mAttachedPosition.z = 0.0f;
            model->mAttachedRotation.x = 0.0f;
            model->mAttachedRotation.y = 0.0f;
            model->mAttachedRotation.z = 0.0f;
            model->mAttachedScale.x = 1.0f;
            model->mAttachedScale.y = 1.0f;
            model->mAttachedScale.z = 1.0f;
        } else {
            GSvec partPosition;
            GSvec partRotation;
            GSvec partScale;
            GSvec offset;
            GSvec inverseRotation;
            f32 quaternion[4];
            GSpartGetTransform(part, &partPosition, &partRotation, &partScale);
            PSVECSubtract((Vec *)&model->mPosition, (Vec *)&partPosition, (Vec *)&offset);
            PSVECScale((Vec *)&partRotation, (Vec *)&inverseRotation, -1.0f);
            EulerToQuat(&inverseRotation, quaternion);
            GSvecTransformQuat(&model->mAttachedPosition, quaternion, &offset);
            PSVECSubtract((Vec *)&model->mRotation, (Vec *)&partRotation,
                          (Vec *)&model->mAttachedRotation);
            model->mAttachedScale.x = model->mScale.x * partScale.x;
            model->mAttachedScale.y = model->mScale.y * partScale.y;
            model->mAttachedScale.z = model->mScale.z * partScale.z;
        }
        model->mAttachmentType = mode;
        if (detachAfterUpdate) {
            model->mFlags |= MODEL_FLAG_ATTACHMENT_DETACH;
        }
        model->mFlags &= ~MODEL_FLAG_ATTACHMENT_UPDATED;
        model->mFlags &= ~MODEL_FLAG_TRANSFORM_UPDATED;
    }
}


_GSmodel *_modelLoad(_HSD_SceneModelSet *resource, _HSD_Joint *joint,
                     GSboundAnimHeader *boundHeader);

extern "C" void GSmodelFree(_GSmodel *model)
{
    GSgfxRenderWaitForDrawComplete();
    modelShadowFreeModelList(model);
    GSmaterial *material = model->mMaterials;
    u32 materialCount = model->mMaterialCount;
    if (materialCount != 0 && material != 0) {
        for (u32 i = 0; i < materialCount; ++i, ++material) {
            modelMaterialFree(material);
        }
        GSmemFree(model->mMaterials);
        model->mMaterialCount = 0;
        model->mMaterials = 0;
    }
    GSmodelRemoveNull(model);
    modelRemoveCenterNull(model);
    GSmodelDisableAnimBlend(model);
    HSD_JObjUnref(model->mJObj);
    if (model->mLinkedParticleBank != 0) {
        GSparticleDestroyAllGenerators(model->mLinkedParticleBank, 1);
    }
    GSmodelFreeDistortion(model);
    GSboundFree(&model->mBound);
    --_modelInUseCount;
    modelBankReleaseModel(model);
}

extern "C" _GSmodel *GSmodelClone(_GSmodel *model)
{
    _HSD_Joint joint;

    joint.mClassName = 0;
    joint.mJointFlags = model->mRoot->mJoint->mJointFlags | HSD_JOBJ_TRAVERSAL_BARRIER;
    joint.mChild = model->mRoot->mJoint;
    joint.mNext = 0;
    joint.mContents = 0;
    joint.mRotate.x = 0.0f;
    joint.mRotate.y = 0.0f;
    joint.mRotate.z = 0.0f;
    joint.mScale.x = 1.0f;
    joint.mScale.y = 1.0f;
    joint.mScale.z = 1.0f;
    joint.mTranslate.x = 0.0f;
    joint.mTranslate.y = 0.0f;
    joint.mTranslate.z = 0.0f;
    joint.mInverseBindMatrix = 0;
    joint.mRObj = 0;

    if (GSboundHasAnimData(&model->mBound) == 0) {
        return _modelLoad(model->mRoot, &joint, 0);
    }
    GSboundAnimHeader *boundHeader = model->mBound.mAnimHeader;
    return _modelLoad(model->mRoot, &joint, boundHeader);
}

extern "C" _GSmodel *GSmodelLoad(_HSD_SceneModelSet *resource,
                                GSboundAnimHeader *boundHeader)
{
    modelLoading = 1;
    return _modelLoad(resource, resource->mJoint, boundHeader);
}

extern "C" void GSmodelInit(void)
{
    GSgfxRenderModule renderModule = {
        0,
        _modelPreUpdate,
        _modelRender,
        _modelFastRender,
        _modelPostUpdate,
        _modelPostUpdate60fps,
        GSGFX_RENDER_MODULE_SIGNATURE,
    };

    modelBankInit();
    modelShadowInit();
    GSgfxRenderModuleRegister(kModelRenderPriority, &renderModule, 0);
    HSD_SetVertexBuffer(GSmemAlloc(2000 * sizeof(Vec)), 2000);
    HSD_SetNormalBuffer(GSmemAlloc(2000 * sizeof(Vec)), 2000);
    HSD_SetExpArgBuffer(GSmemAlloc(100 * sizeof(f32)), 100);
}

void modelSetScl(_GSmodel *model, GSvec *scale)
{
    if (!(model->mFlags & MODEL_FLAG_ROOT_JOINT_POSE)) {
        model->mScale = *scale;
    }
}

void modelSetRot(_GSmodel *model, GSvec *rotation)
{
    if (!(model->mFlags & MODEL_FLAG_ROOT_JOINT_POSE)) {
        model->mRotation = *rotation;
    }
}

void modelSetPos(_GSmodel *model, GSvec *position)
{
    if (!(model->mFlags & MODEL_FLAG_ROOT_JOINT_POSE)) {
        model->mPosition = *position;
    }
}

_HSD_JObj *modelGetRenderJObj(_GSmodel *model)
{
    if (model->mFlags & MODEL_FLAG_BLEND_ACTIVE) {
        return model->mBlendJObjTo;
    }
    return model->mJObj;
}

_GSmodel *_modelLoad(_HSD_SceneModelSet *resource, _HSD_Joint *joint,
                     GSboundAnimHeader *boundHeader)
{
    _GSmodel *model = modelBankGetFreeModel();
    if (model == 0) {
        return 0;
    }

    model->mFlags |= MODEL_FLAG_TEXANIM_LINKED;
    model->mRenderCamera = 0;
    model->mMaxAnimIndex = 0;
    model->mMaxTexAnimIndex = 0;
    model->mAnimEndedCB = 0;
    model->mMaterialCount = 0;
    model->mMaterials = 0;
    for (u32 i = 0; i < 8; ++i) {
        model->mRenderLayers[i] = GSGFX_LAYER_INVALID;
    }
    model->mRenderLayers[0] = GSGFX_LAYER_DEFAULT;
    model->mDustParticleBank = 0;
    model->mDustScale = 1.0f;
    model->mDust[0].mPrevY = 0.0f;
    model->mDust[0].mEmitter = 0;
    model->mDust[0].mPart = 0;
    model->mDust[0].mPending = 0;
    model->mDust[0].mActive = 0;
    model->mDust[1].mPrevY = 0.0f;
    model->mDust[1].mEmitter = 0;
    model->mDust[1].mPart = 0;
    model->mDust[1].mPending = 0;
    model->mDust[1].mActive = 0;
    model->mDistortionFlags = 0;
    model->mDistortionPart = 0;
    model->mDistortionMaterial = 0;
    model->mDistortionPrimitive = 0;
    model->mDistortionTexture = 0;
    model->mDistortionVF = 0;
    model->_5E8[0] = 0;
    model->_5E8[1] = 0;
    model->_5E8[2] = 0;
    model->_5F4 = 0.0f;
    model->mAttachmentType = GS_ATTACH_NONE;
    model->mAttachedModel = 0;
    model->mAttachedPartIndex = 0;
    model->mLinkedParticleBank = 0;
    model->mParticleLinkAttachMode = GS_ATTACH_POSITION;
    model->mRoot = resource;
    model->mJObj = HSD_JObjLoadJoint(joint);
    model->mBlendJObj = 0;
    model->mBlendJObjTo = 0;
    model->mPosition = GSvec(model->mJObj->mTrans[0], model->mJObj->mTrans[1],
                             model->mJObj->mTrans[2]);
    model->mRotation = GSvec(model->mJObj->mRotate[0], model->mJObj->mRotate[1],
                             model->mJObj->mRotate[2]);
    model->mScale = GSvec(model->mJObj->mScale[0], model->mJObj->mScale[1],
                          model->mJObj->mScale[2]);

    for (u32 i = 0; i < 16; ++i) {
        model->mPartMix[i].mType = GS_MODEL_PART_MIX_NONE;
    }
    for (u32 i = 0; i < 8; ++i) {
        model->mPartAnim[i].mAnimation = 0;
    }
    for (u32 i = 0; i < 4; ++i) {
        model->mPartTexAnim[i].mAnimation = 0;
    }
    model->mHasPartAnim = 0;
    model->mHasPartTexAnim = 0;

    if (model->mRoot->mShapeAnimations != 0) {
        model->mFlags |= MODEL_FLAG_SHAPE_ANIM_PRESENT;
    }
    if (model->mRoot->mAnimations != 0) {
        model->mFlags |= MODEL_FLAG_ANIM_PRESENT;
        model->mAnim.mIndex = -1;
        while (model->mRoot->mAnimations[model->mMaxAnimIndex] != 0) {
            ++model->mMaxAnimIndex;
        }
        GSmodelSetAnimIndex(model, 0);
        GSmodelSetAnimRate(model, 0.5f);
        GSmodelSetAnimType(model, GS_MODEL_ANIM_LOOP);
    }
    if (model->mRoot->mTextureAnimations != 0) {
        model->mFlags |= MODEL_FLAG_TEXANIM_PRESENT;
        model->mTexAnim.mIndex = -1;
        while (model->mRoot->mTextureAnimations[model->mMaxTexAnimIndex] != 0) {
            ++model->mMaxTexAnimIndex;
        }
        GSmodelSetTexAnimIndex(model, 0);
        GSmodelSetTexAnimRate(model, 0.5f);
        GSmodelSetTexAnimType(model, GS_MODEL_ANIM_LOOP);
    }

    GSboundCreate(&model->mBound, boundHeader);
    GSboundSetDataPtr(&model->mBound, GS_BOUND_DATA_MODEL, model);
    GSmodelRecalculateBound(model);

    u32 materialCount;
    GSpart *part;
    u32 i;
    u32 partCount;
    GSmaterial *material;
    u32 count;
    u32 j;
    materialCount = 0;
    partCount = GSmodelGetPartCount(model);
    for (i = 0; i < partCount; ++i) {
        part = GSmodelGetPart(model, i);
        materialCount += GSpartGetMaterialCount(part);
        GSpartFree(part);
    }
    if (materialCount != 0) {
        if ((material = static_cast<GSmaterial *>(GSmemAlloc(materialCount * sizeof(GSmaterial)))) != 0) {
            model->mMaterials = material;
            model->mMaterialCount = materialCount;
            for (j = 0; j < partCount; ++j) {
                part = GSmodelGetPart(model, j);
                count = GSpartGetMaterialCount(part);
                for (i = 0; i < count; ++i, ++material) {
                    modelMaterialInit(material);
                    GSpartInitMaterial(material, part, i);
                }
                GSpartFree(part);
            }
        }
    }
    HSD_JObjCountVertices(model->mJObj, &model->mVertexCount,
                          &model->mPolygonCount);
    ++_modelInUseCount;
    if (_modelInUseCount > _modelMaxUsedCount) {
        _modelMaxUsedCount = _modelInUseCount;
    }
    return model;
}
