#include <string.h>
#include <GSres.h>
#include <WorldMap.h>
#include <floor.h>
#include <GSlight.h>
#include <GSmodel.h>
#include <GSparticle.h>
#include <GSmath.h>
#include <GSlog.h>
#include <scene.h>


/* Map resource loader messages. */
static const char sModelLoadError[0x24] = "Error! %s: couldn't load model %d\n";
static const char sModelLoadErrorJp[0x18] = "\x83\x82\x83\x66\x83\x8b\x82\xcc\x8f\x89\x8a\xfa\x89\xbb\x82\xc9\x8e\xb8\x94\x73\n";
static const char sLightLoadError[0x38] = "Error! floorReadMapPostFunc: couldn't load light %d\n";
static const char sLightLoadErrorJp[0x18] = "\x83\x89\x83\x43\x83\x67\x82\xcc\x8f\x89\x8a\xfa\x89\xbb\x82\xc9\x8e\xb8\x94\x73\n";
static const char sSceneDataName[] = "scene_data";

static bool _unloadModel(void* model, unsigned long group, unsigned long resourceID);
static bool _unloadLight(void* light, unsigned long group, unsigned long resourceID);

#pragma force_active on

/* Release the offscreen pass: clear layer 2's copy and clear state, move the
 * render config back and free the texture. */
void WorldMapModel::releaseOffscreenData(void)
{
    GSgfxLayer* layer = GSgfxLayerFindByID(GSGFX_LAYER_DEFAULT);
    layer->SetClearFlags(false, false, false);
    layer->SetTextureCopy(0, false, 0);
    GSgfxLayerConfig::Move(GSgfxLayerID(2), GSgfxLayerID(4), GS_LAYER_INSERT_BEFORE);
    if (mOffscreenTexture != 0) {
        GStextureFree(mOffscreenTexture);
        mOffscreenTexture = 0;
    }
}

// Draw the captured map as a full-screen quad.
void WorldMapModel::renderOffscreenData(f32 alpha)
{
    u8 brightness = (u8)(255.0f * alpha);
    if (mOffscreenTexture == 0) {
        return;
    }
    GSgfxLayer* layer = GSgfxLayerFindByID(static_cast<GSgfxLayerID>(4));
    if (layer == 0) {
        return;
    }
    GSgfxVF* vf = GSgfxVFBuildTemp(0, 2, 0, 0, 1, 5, 1, 1, 0);
    spriteSetOffset(4, 0, 0);
    spriteClearScissor(4);
    spriteSetEnv(4);
    f32 matrix[4][4];
    GSmtx44Ortho(matrix, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    layer->orthographicProjection(matrix);
    GSgfxMatrixStack* stack = layer->mMatrixStack;
    memcpy(stack->currentMatrix(), mtxIdent, sizeof(GSmtx));
    f32 layerDepthMax;
    f32 layerDepthMin;
    layerDepthMin = 0.0f;
    layerDepthMax = 30000.0f;
    stack->mCurrentEntry->mFlags |= GS_MATRIX_INVERSE_TRANSPOSE_DIRTY;
    layer->projectionDepthRange(layerDepthMin, layerDepthMax);
    layer->pixelMode(true, false, false);
    layer->zCompare(false, GSGFX_Z_COMPARE_ALWAYS);
    layer->cullMode(GSGFX_CULL_BACK);
    layer->mPrimitiveType = GSGFX_QUADS;
    layer->mVertexFormat = vf;
    layer->texCoordGeneratorCount(1);
    layer->tgenFunc(0, GS_GFX_TGEN_MTX2X4, GS_GFX_TGEN_SRC_TEX0, 0, false, 0);
    layer->cunitCount(1);
    layer->cunitChannelGen(0, GSGFX_CUNIT_COLOR_AND_ALPHA, GSGFX_CUNIT_VERTEX_COLOR, GSGFX_CUNIT_VERTEX_COLOR, 0,
                           0, static_cast<GSgfxLightType>(2), GSGFX_DIFFUSE_NONE);
    layer->tunitCount(1);
    layer->tunitMapping(0, 0, 0, 0);
    layer->tunitColorOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
    layer->tunitColorArg(0, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_TEXC);
    layer->tunitAlphaOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
    layer->tunitAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_RASA, GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->texSelect(0, mOffscreenTexture);
    layer->beginSurface(4);
    GSgfxFifo.u16 = 0;
    GSgfxFifo.u16 = 0;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = brightness;
    GSgfxFifo.u8 = 0;
    GSgfxFifo.u8 = 0;
    GSgfxFifo.u16 = 640;
    GSgfxFifo.u16 = 0;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = brightness;
    GSgfxFifo.u8 = 1;
    GSgfxFifo.u8 = 0;
    GSgfxFifo.u16 = 640;
    GSgfxFifo.u16 = 480;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = brightness;
    GSgfxFifo.u8 = 1;
    GSgfxFifo.u8 = 1;
    GSgfxFifo.u16 = 0;
    GSgfxFifo.u16 = 480;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = brightness;
    GSgfxFifo.u8 = 0;
    GSgfxFifo.u8 = 1;
    layer->endSurface();
}

/* Set up the offscreen pass: allocate the capture texture and point layer 2
 * at it. */
void WorldMapModel::initializeOffscreenData(void)
{
    mOffscreenTexture = GStextureCreate(0, 0, GS_TEXTURE_FORMAT_32BIT, 0, 0);
    if (mOffscreenTexture != 0) {
        GSgfxLayerConfig::Move(GSgfxLayerID(2), GSgfxLayerID(0), GS_LAYER_INSERT_BEFORE);
        GSgfxLayer* layer = GSgfxLayerFindByID(GSGFX_LAYER_DEFAULT);
        layer->SetTextureCopy(mOffscreenTexture, false, 0);
        layer->SetClearFlags(true, true, true);
    }
}

/* Load every model and light of the map resource into the floor group and
 * register them with the free callbacks. */
_HSD_SceneDesc* WorldMapModel::SetModel(unsigned long group, unsigned long resource, bool visible)
{
    GSgfxLayerID layerID[1] = { GSgfxLayerID(0) };
    _HSD_SceneDesc* header;
    long modelIndex = 0;
    long lightIndex = 0;
    _GSlight* light;
    _GSmodel* model;
    header = makeArchiveHeader(resource);
    if (header == 0) {
        return 0;
    }
    if (header->mModels != 0) {
        long resID = floorReadMakeModelResID(resource);
        for (; header->mModels[modelIndex] != 0; modelIndex++) {
            model = GSmodelLoad(header->mModels[modelIndex], 0);
            GSresFreeResource(group, resID | modelIndex);
            GSresRegisterResource(model, group, resID | modelIndex, _unloadModel);
            GSmodelSetRenderLayer(model, 1, layerID);
            if (model == 0) {
                GSlogWrite(sModelLoadError, __FUNCTION__);
                GSlogWrite(sModelLoadErrorJp);
            } else {
                GSmodelSetVisibility(model, 1);
                if (GSmodelCanAnimate(model) != 0) {
                    GSmodelSetAnimIndex(model, 0);
                    GSmodelSetAnimRate(model, 0.5f);
                    GSmodelStartAnimation(model);
                }
                if (GSmodelCanTexAnimate(model) != 0) {
                    GSmodelSetTexAnimIndex(model, 0);
                    GSmodelSetTexAnimRate(model, 0.5f);
                    GSmodelStartTexAnimation(model);
                }
            }
        }
    }
    if (header->mLights != 0) {
        long resID = floorReadMakeLightResID(resource);
        for (; header->mLights[lightIndex] != 0; lightIndex++) {
            light = GSlightLoad(header->mLights[lightIndex]);
            if (light == 0) {
                GSlogWrite(sLightLoadError, lightIndex);
                GSlogWrite(sLightLoadErrorJp);
                continue;
            }
            GSresRegisterResource(light, group, resID | lightIndex, _unloadLight);
            if (visible && light != 0) {
                if (lightIndex == 0) {
                    GSlightSetType(light, 0);
                } else {
                    GSlightSetType(light, 1);
                }
                GSlightSetActive(light, 1);
                if (GSlightCanAnimate(light) != 0) {
                    GSlightSetAnimIndex(light, 0);
                    GSlightSetAnimRate(light, 0.5f);
                    GSlightStartAnimation(light);
                }
            }
        }
    }
    return header;
}

/* Free every model and light resource the map loaded and clear the light
 * state. */
void WorldMapModel::UnloadModel(unsigned long group, unsigned long resource)
{
    long modelIndex = 0;
    long lightIndex = 0;
    long resID;
    _HSD_SceneDesc* header = makeArchiveHeader(resource);
    if (header == 0) {
        return;
    }
    if (header->mModels != 0) {
        resID = floorReadMakeModelResID(resource);
        for (; header->mModels[modelIndex] != 0; modelIndex++) {
            GSresFreeResource(group, resID | modelIndex);
        }
    }
    if (header->mLights != 0) {
        resID = floorReadMakeLightResID(resource);
        for (; header->mLights[lightIndex] != 0; lightIndex++) {
            GSresFreeResource(group, resID | lightIndex);
        }
        floorSetLightInfo(resID, lightIndex);
    } else {
        floorSetLightInfo(0, 0);
    }
}

/* Show or hide every model of the map. */
void WorldMapModel::ObjectVisibility(unsigned long model, bool visible)
{
    _HSD_SceneDesc* header = makeArchiveHeader(model);
    long resID = floorReadMakeModelResID(model);
    if (header->mModels != 0) {
        for (long modelIndex = 0; header->mModels[modelIndex] != 0; modelIndex++) {
            Visibility(resID, visible);
            resID++;
        }
    }
}

/* Show or hide one map model resource. */
void WorldMapModel::Visibility(unsigned long model, bool visible)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(model));
    if (res == 0) {
        return;
    }
    GSmodelSetVisibility(res, visible);
}

/* Detach the map model from its part again. */
bool WorldMapModel::ModelDetach(unsigned long model)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(model));
    if (res == 0) {
        return false;
    }
    GSmodelDetachFromGSpart(res, 1);
    return true;
}

/* Attach one map model to another's part. */
bool WorldMapModel::ModelAttach(unsigned long model, unsigned long part, long mode, GSattachMode attachMode)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(model));
    if (res == 0) {
        return false;
    }
    _GSmodel* partRes = static_cast<_GSmodel*>(GSresGetResourceAllGroup(part));
    if (partRes == 0) {
        return false;
    }
    GSpart* gsPart = GSmodelGetPart(partRes, mode);
    GSmodelAttachToGSpart(res, gsPart, GS_ATTACH_ALL, 0, 1);
    GSpartFree(gsPart);
    return true;
}

/* Create a particle generator on a part of the map model and return it. */
_GSparticleGen* WorldMapModel::SetModelPartPaticle(unsigned long model, unsigned long particleResource, long part,
                                                 GSattachMode attachMode, bool flagA, bool flagB, bool flagC)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(model));
    _GSparticleGen* gen;
    GSpart* gsPart = GSmodelGetPart(res, part);
    _GSparticleBank* particleBank = static_cast<_GSparticleBank*>(GSresGetResourceAllGroup(particleResource));
    gen = GSparticleCreateGenerator(particleBank, 0, 0);
    GSparticleAttachGeneratorToGSpart(gen, gsPart, attachMode, flagA, flagB, flagC);
    GSpartFree(gsPart);
    return gen;
}

/* Detach a particle generator from its part again. */
void WorldMapModel::DetachModelPartPaticle(_GSparticleGen* gen, bool flag)
{
    GSparticleDetachGeneratorFromGSpart(gen, flag);
}

/* Resource free callback for models. */
static bool _unloadModel(void* model, unsigned long, unsigned long)
{
    GSmodelFree(static_cast<_GSmodel*>(model));
    return 1;
}

/* Resource free callback for lights. */
static bool _unloadLight(void* light, unsigned long, unsigned long)
{
    GSlightFree(static_cast<_GSlight*>(light));
    return 1;
}

/* Fetch the map resource's archive header. */
_HSD_SceneDesc* WorldMapModel::makeArchiveHeader(unsigned long resource)
{
    void* archive = GSresGetResourceAllGroup(resource);
    return (_HSD_SceneDesc*)HSD_ArchiveGetPublicAddress(archive, sSceneDataName);
}

/* Frame count of one map model animation. */
f32 WorldMapModel::GetAnimeFrameCount(unsigned long motion, unsigned long index)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(motion));
    if (res == 0) {
        return 0.0f;
    }
    return GSmodelGetAnimFrameCount(res, index);
}

/* Current frame of one map model animation. */
f32 WorldMapModel::GetAnimeFrame(unsigned long motion)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(motion));
    if (res == 0) {
        return 0.0f;
    }
    return GSmodelGetAnimFrame(res);
}

/* Stop one map model animation. */
void WorldMapModel::StopMotion(unsigned long motion)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(motion));
    if (res == 0) {
        return;
    }
    GSmodelStopAnimation(res);
}

/* True while one map model animation is still running. */
bool WorldMapModel::CheckMotion(unsigned long motion)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(motion));
    if (res == 0) {
        return false;
    }
    return GSmodelIsAnimating(res);
}

/* Select a map model animation and its starting frame. */
void WorldMapModel::SetMotion(unsigned long modelResourceId, short animationIndex, f32 startFrame, bool loop)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(modelResourceId));
    if (res == 0) {
        return;
    }
    GSmodelSetAnimIndex(res, animationIndex);
    GSmodelSetAnimFrame(res, startFrame);
    GSmodelSetAnimRate(res, 0.5f);
    if (loop) {
        GSmodelSetAnimType(res, 1);
    } else {
        GSmodelSetAnimType(res, 0);
    }
    GSmodelStartAnimation(res);
}

/* Move one map model to a render layer. */
void WorldMapModel::SetLayerID(unsigned long model, long, GSgfxLayerID* layer)
{
    _GSmodel* res = static_cast<_GSmodel*>(GSresGetResourceAllGroup(model));
    if (res == 0) {
        return;
    }
    GSmodelSetRenderLayer(res, 1, layer);
}

/* Loaded model payload for a map model resource. */
_GSmodel* WorldMapModel::GetModelPtr(unsigned long modelResourceID)
{
    return static_cast<_GSmodel*>(GSresGetResourceAllGroup(modelResourceID));
}

#pragma force_active reset
