#include <app/effects/blur.h>
#include <dolphin/os/OSFastCast.h>
#include <GScamera.h>
#include <GSmodel.h>
#include <GSmtx.h>
#include <GSparticle.h>
#include <GSgfx.h>
#include <GStexture.h>
#include <hsd/state.h>

// Called by the back framebuffer capture with the number of frames captured so
// far; returning zero ends the capture.
u8 blurEffectRenderFBCallback(GStexture *texture, u32 frames, void *param);

blurEffect::~blurEffect()
{
    Stop();
    if (mFramebuffer && mCapture != NULL) {
        if (mToken != 0) {
            GSgfxTokenWaitForClear(mToken);
            mToken = 0;
        }
        GStextureFree(mCapture);
    }
}

void blurEffect::_stop()
{
    if (mFramebuffer) {
        GSgfxEndBackFBCapture(mCapture);
        mCaptureReady = false;
    }
}

void blurEffect::_render(GSgfxLayer *layer, GSgfxLayerPass pass)
{
    s32 history;
    f32 rate;
    f32 step;
    f32 frame;
    s32 i;
    if (mFramebuffer) {
        if (layer->mLayerID == GSGFX_LAYER_FILTER && pass == GSGFX_LAYER_PASS_XLU) {
            RenderFB(layer);
        }
        return;
    }
    if (mModel == NULL) return;
    if (layer->mLayerID != GSGFX_LAYER_DEFAULT) return;
    if (pass != GSGFX_LAYER_PASS_OPAQUE) return;
    if (!GSmodelIsAnimating(mModel)) return;

    static HSD_PEDesc pedesc = {
        HSD_PE_COLOR_UPDATE | HSD_PE_ALPHA_UPDATE | HSD_PE_Z_COMP,
        0, 0, 255,
        GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET,
        GX_LESS, GX_ALWAYS, GX_AOP_AND, GX_ALWAYS
    };

    history = (s32)(0.5f + 20.0f * mStrength);
    if (history == 0) return;

    frame = GSmodelGetAnimFrame(mModel);
    rate = GSmodelGetAnimRate(mModel);
    f32 start = frame - rate * history;
    if (start < 0.0f) {
        history = (s32)(frame / rate);
        start = 0.0f;
    }
    if (history == 0) return;
    if (start >= frame) return;

    GSmodelSetPEdescr(mModel, &pedesc);
    enum { BLUR_RENDER_ALPHA_MAT = 0x20 };
    GSmodelSetRenderFlags(mModel, BLUR_RENDER_ALPHA_MAT);
    GSmodelInitMaterialAlpha(mModel);
    GScameraLoadActiveCameraMatrix(GSGFX_LAYER_INVALID);
    _GSparticleBank *bank = GSmodelGetLinkedGSparticleBank(mModel);
    if (bank != NULL) {
        GSparticleDisactivateBank(bank);
        GSparticleSetBankVisibility(bank, false);
    }
    GSmodelSetAnimFrame(mModel, start);

    i = 1;
    step = 1.0f / history;
    while (start + rate * 0.5f < frame) {
        start += rate;
        f32 weight = i * step;
        i++;
        GSmodelSetMaterialAlpha(mModel, 0.5f * weight * weight);
        GSmodelForceAnimTransformUpdate(mModel);
        GSmodelDrawModel(mModel, layer, pass);
        GSmodelAdvanceAnimation(mModel, 1.0f);
    }

    GSmodelResetMaterialAlpha(mModel);
    GSmodelResetRenderFlags(mModel);
    GSmodelResetPEdescr(mModel);
    GSmodelSetAnimFrame(mModel, frame);
    GSmodelForceAnimTransformUpdate(mModel);
    GSmodelDrawModel(mModel, layer, pass);
    if (bank != NULL) {
        GSparticleActivateBank(bank);
        GSparticleSetBankVisibility(bank, true);
    }
}

bool blurEffect::_start()
{
    if (mFramebuffer) {
        if (mCapture == NULL) return false;
        mCaptureReady = false;
        GSgfxBeginBackFBCapture(mCapture, blurEffectRenderFBCallback, this);
    }
    return true;
}

blurEffect::blurEffect(bool framebuffer)
{
    mCapture = NULL;
    mModel = NULL;
    mToken = 0;
    mCaptureReady = false;
    mFramebuffer = framebuffer;
    mStrength = 0.0f;
    if (framebuffer) {
        mCapture = GStextureCreate(0, 0, GS_TEXTURE_FORMAT_RGB565, 0, 0);
    }
}

void blurEffect::RenderFB(GSgfxLayer *layer)
{
    if (!mCaptureReady) return;

    u8 alpha = __OSf32tou8(255.0f * mStrength);
    f32 mtx[4][4];
    GSmtx44Ortho(mtx, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    layer->orthographicProjection(mtx);
    layer->mMatrixStack->identity();
    layer->projectionDepthRange(0.0f, 30000.0f);
    layer->SetRenderPass(GSGFX_LAYER_PASS_XLU);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, false);
    layer->zCompare(false, GSGFX_Z_COMPARE_ALWAYS);
    layer->pixelMode(true, true, false);
    layer->cullMode(GSGFX_CULL_NONE);
    enum { BLUR_VERTEX_FORMAT_POSITION_RGBA8_TEXCOORD = 5 };
    layer->setRenderStateAndSelect(GSgfxVFGetPredef(BLUR_VERTEX_FORMAT_POSITION_RGBA8_TEXCOORD));
    layer->texSelect(0, mCapture);
    layer->mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    layer->beginSurface(4);
    // The captured frame covers the whole screen as a single quad.
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 640.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 480.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.f32 = 640.0f;
    GSgfxFifo.f32 = 480.0f;
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = 255;
    GSgfxFifo.u8 = alpha;
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.f32 = 1.0f;
    layer->endSurface();
    mToken = layer->tokenMark();
}

u8 blurEffectRenderFBCallback(GStexture *texture, u32 frames, void *param)
{
    blurEffect *effect = (blurEffect *)param;
    effect->mCaptureReady = (texture == effect->mCapture);
    return effect->mCaptureReady;
}
