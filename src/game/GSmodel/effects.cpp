#include <GSmodelInternal.h>

enum {
    DISTORTION_TEXTURE_STORAGE_BYTES = 0x80,
    DISTORTION_PASS_ZERO_FIFO_BYTES = 0,
    DISTORTION_PASS_ONE_FIFO_BYTES = 0x1400,
    DISTORTION_MATRIX_STACK_CAPACITY = 2
};

/* Name of the GSgfx layer the distortion pass draws into, and the number
 * of models currently using it. The layer is removed from its config once
 * the last user has gone. */
extern const char gsDistortionLayerName[];
/* Number of models that currently own a distortion pass; the layer is
 * dropped from the config once the last one goes. */
extern u8 gsDistortionUserCount;

/* Live models, linked through mNext. */
extern _GSmodel *gsModelList;

/* Sets up the distortion layer for one frame: builds a light-perspective
 * camera, configures the render state, the texture environment and the
 * texture generators, then hands the pass to _modelDrawDistortionSub. Each
 * cached setting below also raises the matching dirty bit so the layer
 * re-uploads the GS state; the writes are in the order the pass sets them. */
void modelDistortionRender(void)
{
    bool fog;
    GSgfxLayer *layer;
    void *cam;
    void *tex;
    u32 token;
    _GSmodel *m;
    u8 wanted;
    f32 fovY;
    f32 aspect;
    f32 nearZ;
    f32 farZ;
    GSvec camPos;
    GSmtx camMtx;

    layer = (GSgfxLayer *)GSgfxLayerFindByName(gsDistortionLayerName);
    cam = GScameraGetActiveCamera();
    if (cam == NULL) {
        return;
    }
    if (layer == NULL) {
        return;
    }
    if (!layer->IsLayerVisible()) {
        return;
    }
    tex = GSgfxLayerGetFrameBufferTexture();
    if (tex == NULL) {
        return;
    }
    /* Nothing to draw unless some model still holds the distortion class;
     * the pass is skipped entirely in that case, timing included. */
    wanted = 0;
    for (m = (_GSmodel *)gsModelList; m != NULL;
         m = m->mNext) {
        if ((m->mFlags & MODEL_FLAG_FRAME_VISIBLE) != 0 &&
            (m->mFlags & MODEL_FLAG_DISTORTION_CLASS) == MODEL_FLAG_DISTORTION_CLASS) {
            wanted = 1;
            break;
        }
    }
    if (wanted == 0) {
        return;
    }
    token = GSgfxPerfbarCPUEntry(GSGFX_PERFBAR_MODEL_DISTORTION);
    fog = layer->mFogEnabled;
    layer->fog(false);
    GScameraLoadActiveCameraMatrix(layer->mLayerID);
    GScameraGetPerspective(cam, &fovY, &aspect, &nearZ, &nearZ);
    GScameraGetPosition(cam, &camPos);
    C_MTXLightPerspective(camMtx.sdk(), fovY, aspect, 0.5f, (-0.5f),
                          0.5f, 0.5f);
    layer->cullMode(GS_GFX_CULL_NONE);
    layer->clipMode(GS_GFX_CLIP_ENABLED);
    layer->coplanar(false);
    layer->mScissorX = 0;
    layer->mScissorY = 0;
    GSgfxVideoGetEFBSize(&layer->mScissorWidth, &layer->mScissorHeight);
    layer->markDirty(layer->mRasterStateDirtyA, layer->mRasterStateDirtyB, GS_GFX_RASTER_DIRTY_SCISSOR, GS_GFX_DIRTY_RASTER);
    layer->pixelMode(true, false, false);
    layer->zCompare(true, GS_GFX_Z_COMPARE_LEQUAL);
    layer->alphaCompare(false, GS_GFX_A_COMPARE_ALWAYS, 0, GS_GFX_ALPHA_AND, GS_GFX_A_COMPARE_ALWAYS, 0);
    layer->destinationAlpha(false, 0xFF);
    layer->dither(true);
    layer->blendMode(GS_GFX_BLEND_ALPHA, GS_GFX_FACTOR_ONE, GS_GFX_FACTOR_ZERO, true);
    layer->tunitCount(1);
    layer->indTexCount(1);
    layer->tgenCount(2);
    layer->channelCount(0);
    layer->tunitFunc(0, GS_GFX_TUNIT_TEXTURE_REPLACE);
    layer->tunitMapping(0, 1, 1, GS_GFX_COLOR_CHANNEL_NULL);
    layer->texSelect(1, (GStexture *)tex);
    layer->mIndirectTexCoord0 = 0;
    layer->mIndirectTextureMap0 = 0;
    layer->markDirty(layer->mIndirectOrderDirtyA, layer->mIndirectOrderDirtyB, GS_GFX_STAGE_ZERO_MASK, GS_GFX_DIRTY_INDIRECT);
    layer->mIndirectCoordScaleS0 = GS_GFX_INDIRECT_SCALE_ONE;
    layer->mIndirectCoordScaleT0 = GS_GFX_INDIRECT_SCALE_ONE;
    layer->markDirty(layer->mIndirectCoordScaleDirtyA, layer->mIndirectCoordScaleDirtyB, GS_GFX_STAGE_ZERO_MASK, GS_GFX_DIRTY_INDIRECT);
    /* Warp the framebuffer coordinates with indirect texture stage zero. */
    layer->mTevIndirectOperation0 = GS_GFX_INDIRECT_WARP;
    layer->mTevIndirectStage0 = 0;
    layer->mTevWarpSignedOffset0 = 1;
    layer->mTevWarpReplaceMode0 = 0;
    layer->mTevWarpMatrix0 = 1;
    layer->markDirty(layer->mTevIndirectDirtyA, layer->mTevIndirectDirtyB, GS_GFX_STAGE_ZERO_MASK, GS_GFX_DIRTY_TEV);
    layer->tgenFunc(0, GS_GFX_TGEN_MTX2X4, GS_GFX_TGEN_SRC_TEX0, NULL, false, NULL);
    layer->mTexCoordLineOffset0 = 0;
    layer->mTexCoordPointOffset0 = 0;
    layer->markDirty(layer->mTexCoordOffsetDirtyA, layer->mTexCoordOffsetDirtyB, GS_GFX_TEXCOORD_ZERO_MASK, GS_GFX_DIRTY_GEOMETRY);
    /* Every model that still holds the distortion class draws its pass. */
    for (m = (_GSmodel *)gsModelList; m != NULL;
         m = m->mNext) {
        if ((m->mFlags & MODEL_FLAG_FRAME_VISIBLE) != 0 &&
            (m->mFlags & MODEL_FLAG_DISTORTION_CLASS) == MODEL_FLAG_DISTORTION_CLASS) {
            _modelDrawDistortionSub(m, layer, &camMtx, &camPos);
        }
    }
    layer->mTevIndirectOperation0 = GS_GFX_INDIRECT_DIRECT;
    layer->markDirty(layer->mTevIndirectDirtyA, layer->mTevIndirectDirtyB, GS_GFX_STAGE_ZERO_MASK, GS_GFX_DIRTY_TEV);
    layer->indTexCount(0);
    if (fog) {
        layer->fog(fog);
    }
    GSgfxPerfbarCPUEntry(token);
}

/* Draws the distortion pass for one model: reads the part the model marked
 * as the distortion source, folds its position against the camera into the
 * indirect-texture scale matrix, pushes the part transform and submits the
 * display list of the distortion primitive. */
void _modelDrawDistortionSub(_GSmodel *model, GSgfxLayer *layer, GSmtx *mtx,
                            GSvec *pos)
{
    GSpart *partHandle;
    _HSD_PObj *src;      /* primitive supplying the distortion display list. */
    GSvec v;        /* part position the distance is measured from. */
    f32 scale[6];   /* indirect-texture scale matrix the pass installs. */
    GSmtx m;        /* part transform read from the distortion part. */
    f32 d;
    f32 recip;
    GStexture *tex;

    if ((model->mFlags & MODEL_FLAG_DISTORTION_CLASS) != MODEL_FLAG_DISTORTION_CLASS) {
        return;
    }
    if (model->mDistortionVF == NULL) {
        return;
    }
    src = model->mDistortionPrimitive;
    if (src == NULL) {
        return;
    }
    partHandle = GSmodelGetPart(model, model->mDistortionPart);
    if (partHandle == NULL) {
        return;
    }
    GSpartGetMatrix(partHandle, &m);
    v.x = m.m[0][3];
    v.y = m.m[1][3];
    v.z = m.m[2][3];
    GSpartFree(partHandle);
    /* The effect fades with the distance to the camera; the reciprocal is
     * clamped so a part right on the camera cannot blow the scale up. */
    d = PSVECDistance((Vec *)&v, (Vec *)pos);
    if (d < 1.0f) {
        d = 1.0f;
    }
    recip = 1.0f / d;
    memset(scale, 0, 0x18);
    if (model->mDistortionFlags & GS_MODEL_WARP_S) {
        if (model->mDistortionFlags & GS_MODEL_WARP_REVERSE_S) {
            scale[1] = (-0.5f) * recip;
        } else {
            scale[1] = 0.5f * recip;
        }
    }
    if (model->mDistortionFlags & GS_MODEL_WARP_T) {
        if (model->mDistortionFlags & GS_MODEL_WARP_REVERSE_T) {
            scale[5] = (-0.5f) * recip;
        } else {
            scale[5] = 0.5f * recip;
        }
    }
    tex = (GStexture *)model->mDistortionTexture;
    layer->texSelect(0, tex);
    layer->mMatrixStack->pushMult(&m);
    GSmtx cameraTransform = *mtx * *layer->mMatrixStack->currentMatrix();
    PSMTXCopy(cameraTransform.sdk(), m.sdk());
    layer->tgenFunc(1, GS_GFX_TGEN_MTX3X4, GS_GFX_TGEN_SRC_POSITION, &m, false, NULL);
    layer->tunitIndTexMtx(GS_GFX_IND_MTX0, (f32 (*)[3])scale, 1);
    layer->mVertexFormat = model->mDistortionVF;
    layer->dlDrawExt((u8 *)src->mDisplayList, src->mDisplayListBlocks << 5);
    layer->mMatrixStack->pop();
}

extern "C" void GSmodelFreeDistortion(_GSmodel *model)
{
    GSgfxLayer *layer;

    if ((model->mFlags & MODEL_FLAG_DISTORTION_CLASS) == MODEL_FLAG_DISTORTION_CLASS) {
        model->mFlags &= ~MODEL_FLAG_DISTORTION_CLASS;
        layer = (GSgfxLayer *)GSgfxLayerFindByName(gsDistortionLayerName);
        if (layer != NULL) {
            layer->SetLayerVisibility(false);
        }
    }
    model->mDistortionPart = 0;
    model->mDistortionMaterial = 0;
    model->mDistortionPrimitive = NULL;
    if (model->mDistortionTexture != NULL) {
        GSmemFree(model->mDistortionTexture);
        model->mDistortionTexture = NULL;
        if (gsDistortionUserCount != 0) {
            gsDistortionUserCount--;
            if (gsDistortionUserCount == 0) {
                layer = (GSgfxLayer *)GSgfxLayerFindByName(gsDistortionLayerName);
                if (layer != NULL) {
                    GSgfxLayerConfig::Remove(layer->mLayerID);
                }
            }
        }
    }
    if (model->mDistortionVF != NULL) {
        GSgfxVFFree(model->mDistortionVF);
        operator delete(model->mDistortionVF);
        model->mDistortionVF = NULL;
    }
}

/* Marks up the material texture the distortion pass samples with: the
 * texture is copied out, clamped in both directions and filtered
 * without mipmaps, and the source part attribute array is turned into a
 * vertex formatter. */
extern "C" void GSmodelCreateDistortion(_GSmodel *model, u16 part, u8 material,
                                u32 flags)
{
    GSgfxLayer *layer;
    GSpart *partHandle;
    void *mat;
    _HSD_PObj *distortionPrimitive;
    _HSD_JObj *joint;
    _HSD_VtxDescList *vtx;

    if (model->mDistortionTexture != NULL) {
        return;
    }
    if (part == 0) {
        return;
    }
    partHandle = GSmodelGetPart(model, part);
    if (partHandle == NULL) {
        return;
    }
    joint = partHandle->mJoint;
    mat = GSpartGetMaterial(partHandle, material);
    GSpartFree(partHandle);
    if (mat == NULL) {
        return;
    }
    long ok = (joint->flags & (HSD_JOBJ_FLAG_PARTICLE | HSD_JOBJ_FLAG_SPLINE)) ? 0 : 1;
    if (ok == 0) {
        return;
    }
    if (joint->mDrawable == NULL) {
        return;
    }
    distortionPrimitive = joint->mDrawable->mPrimitive;
    if (distortionPrimitive == NULL) {
        return;
    }
    if (distortionPrimitive->mVtxAttr == NULL) {
        return;
    }
    if (distortionPrimitive->mDisplayListBlocks == 0) {
        return;
    }
    if (distortionPrimitive->mDisplayList == NULL) {
        return;
    }
    if ((distortionPrimitive->mFlags & HSD_POBJ_TYPE_MASK) == HSD_POBJ_SHAPE_ANIM) {
        return;
    }
    model->mDistortionPart = (u16)part;
    model->mDistortionMaterial = (u8)material;
    model->mDistortionPrimitive = distortionPrimitive;
    model->mDistortionTexture = (void *)GSmemAlloc(DISTORTION_TEXTURE_STORAGE_BYTES);
    if (model->mDistortionTexture == NULL) {
        return;
    }
    /* A zero low byte defaults to positive S warp. */
    if ((u8)flags == 0) {
        flags = GS_MODEL_WARP_S;
    }
    model->mDistortionFlags = (u8)flags;
    GSmaterialGetTexture(mat, model->mDistortionTexture);
    GStextureSetWrap(model->mDistortionTexture, GS_TEXTURE_WRAP_CLAMP, GS_TEXTURE_WRAP_CLAMP);
    GStextureSetFilter(model->mDistortionTexture, GS_TEXTURE_FILTER_LINEAR,
                       GS_TEXTURE_FILTER_LINEAR, GS_TEXTURE_MIP_FILTER_NONE);
    layer = GSgfxLayerFindByName(gsDistortionLayerName);
    if (layer == NULL) {
        layer = GSgfxLayerFindByID(GSgfxLayerConfig::Add((char *)gsDistortionLayerName,
                                                         DISTORTION_PASS_ZERO_FIFO_BYTES,
                                                         DISTORTION_PASS_ONE_FIFO_BYTES,
                                                         DISTORTION_MATRIX_STACK_CAPACITY,
                                                         GS_GFX_LAYER_FRAMEBUFFER_COPY, false,
                                                         false, false,
                                                         DISTORTION_RELATIVE_LAYER_ID,
                                                         GS_LAYER_INSERT_AFTER));
        if (layer != NULL) {
            layer->SetLayerVisibility(0);
        }
    }
    gsDistortionUserCount++;
    if (layer == NULL) {
        GStextureFree(model->mDistortionTexture);
        model->mDistortionTexture = NULL;
        model->mDistortionPrimitive = NULL;
    }
    model->mDistortionVF = new GSgfxVF;
    if (model->mDistortionVF == NULL) {
        return;
    }
    GSgfxVFInit(model->mDistortionVF);
    GSgfxVFCreate(model->mDistortionVF);
    vtx = distortionPrimitive->mVtxAttr;
    while (vtx->mAttribute != HSD_VTX_ATTR_NULL) {
        if (vtx->mAttribute >= HSD_VTX_ATTR_POSITION || vtx->mAttribute < 0) {
            GSgfxVFSetElement(model->mDistortionVF, vtx->mAttribute, vtx->mComponentCount,
                              vtx->mComponentFormat, vtx->mFractionalBits);
        }
        if (vtx->mInputType != HSD_VTX_INPUT_DIRECT) {
            GSgfxVFSetArray(model->mDistortionVF, vtx->mAttribute, vtx->mInputType,
                            vtx->mArrayBase, vtx->mStride & 0xFF, 0);
        }
        vtx++;
    }
}
extern "C" void GSmodelEnableDistortion(_GSmodel *model)
{
    GSgfxLayer *layer;

    /* Enable only a fully configured distortion source. */
    u32 cls = model->mFlags & MODEL_FLAG_DISTORTION_CLASS;

    if (cls == MODEL_FLAG_DISTORTION_CLASS) {
        return;
    }
    if (model->mDistortionPrimitive == NULL) {
        return;
    }
    if (model->mDistortionTexture == NULL) {
        return;
    }
    layer = (GSgfxLayer *)GSgfxLayerFindByName(gsDistortionLayerName);
    if (layer == NULL) {
        return;
    }
    layer->SetLayerVisibility(true);
    model->mFlags |= MODEL_FLAG_DISTORTION_CLASS;
}

void modelDustUpdate(_GSmodel *model)
{
    GSpart *part;
    GSvec modelPosition;
    GSvec partPosition;
    GSvec modelRotation;
    GSvec scale;
    GSmodelDustEntry *e;
    _GSparticleGen *gen;
    f32 y;
    s32 i;

    if (model->mDustParticleBank == NULL) {
        return;
    }
    GSmodelGetPosition(model, &modelPosition);
    GSmodelGetRotation(model, &modelRotation);
    scale.x = model->mDustScale;
    scale.y = model->mDustScale;
    scale.z = model->mDustScale;
    for (i = 0, e = &model->mDust[0]; i < 2; i++, e++) {
        if (e->mActive == 0) {
            continue;
        }
        part = GSmodelGetPart(model, e->mEmitter);
        if (part == NULL) {
            continue;
        }
        GSpartGetTransform(part, &partPosition, 0, 0);
        GSpartFree(part);
        y = partPosition.y - modelPosition.y;
        if (y < e->mPrevY - 0.00001f) {
            e->mPending = 1;
        } else if (e->mPending != 0) {
            e->mPending = 0;
            gen = GSparticleCreateGenerator(model->mDustParticleBank, 0, 0);
            if (gen != NULL) {
                part = GSmodelGetPart(model, e->mPart);
                if (part == NULL) {
                    continue;
                }
                GSpartGetTransform(part, &partPosition, 0, 0);
                GSpartFree(part);
                GSparticleSetGeneratorPosition(gen, &partPosition);
                GSparticleSetGeneratorRotation(gen, &modelRotation);
                GSparticleSetGeneratorScale(gen, &scale, 0, 0);
            }
        }
        e->mPrevY = y;
    }
}
