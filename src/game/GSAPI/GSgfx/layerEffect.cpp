#include <GSgfx.h>
#include <GSgfxVideo.h>

#include <dolphin/gx/GXCommandList.h>
#include <dolphin/gx/GXDraw.h>
#include <dolphin/gx/GXGeometry.h>
#include <dolphin/gx/GXTexture.h>
#include <dolphin/gx/GXTransform.h>
#include <dolphin/gx/GXVert.h>

#include <string.h>

// Copy-filter taps used by the EFB-to-texture copy path.
static u8 _defaultCopyFilter[8] = { 8, 8, 10, 12, 10, 8, 8, 0 };

GStexture *GSgfxLayer::fbTexPtr;
GStexture *GSgfxLayer::distTexPtr;
u16 GSgfxLayer::fbTexRefCount;
u16 GSgfxLayer::distTexRefCount;

static __declspec(section ".sdata2") GScolor _zeroClearColor;


void GSgfxLayer::textureCopy(GSgfxLayerPass pass, GStexture *texture, bool enableCopy,
                             u8 *copyConfig, bool clearColor, bool clearAlpha,
                             bool clearDepth)
{
    SetRenderPass(pass);
    EffectTextureCopy(texture, enableCopy, copyConfig, clearColor, clearAlpha,
                      clearDepth);
}

void GSgfxLayer::EffectTextureCopy(GStexture *texture, bool enableCopy, u8 *copyConfig,
                                   bool clearColor, bool clearAlpha, bool clearDepth)
{
    GXRenderModeObj *renderMode;
    GSgfxCopyTexParams params;
    u16 copyWidth;
    u16 copyHeight;
    bool clear;
    u8 savedColor;
    u8 savedAlpha;
    u8 savedDepth;
    u8 savedDepthCompare;
    GSgfxZCmpFunc savedDepthFunc;

    switch (texture->mFormat) {
    case GS_TEXTURE_FORMAT_I4:
    case GS_TEXTURE_FORMAT_IA4:
    case GS_TEXTURE_FORMAT_I8:
    case GS_TEXTURE_FORMAT_IA8:
    case GS_TEXTURE_FORMAT_RGB565:
    case GS_TEXTURE_FORMAT_32BIT:
    case GS_TEXTURE_FORMAT_RGB5A3:
    case GS_TEXTURE_FORMAT_A8:
    case GS_TEXTURE_FORMAT_R8:
    case GS_TEXTURE_FORMAT_G8:
    case GS_TEXTURE_FORMAT_B8:
        break;
    default:
        return;
    }

    renderMode = GSgfxVideoGetRenderMode();
    GSgfxVideoGetEFBSize(&copyWidth, &copyHeight);
    copyWidth = texture->mXsize < copyWidth ? texture->mXsize : copyWidth;
    copyHeight = texture->mYsize < copyHeight ? texture->mYsize : copyHeight;
    clear = clearColor || clearAlpha || clearDepth;

    if (clear == true) {
        savedColor = mColorUpdateEnabled != 0;
        savedAlpha = mAlphaUpdateEnabled != 0;
        savedDepth = mDepthUpdateEnabled != 0;
        savedDepthCompare = mDepthCompareEnabled != 0;
        savedDepthFunc = mDepthCompareFunction;

        if (clearColor == true)
            mColorUpdateEnabled = true;
        else
            mColorUpdateEnabled = false;
        if (clearAlpha == true)
            mAlphaUpdateEnabled = true;
        else
            mAlphaUpdateEnabled = false;
        if (clearDepth == true)
            mDepthUpdateEnabled = true;
        else
            mDepthUpdateEnabled = false;
        mPassState[0].mGpuFlags |= GSGFX_GPU_PIXEL_MODE;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mGpuFlags |= GSGFX_GPU_PIXEL_MODE;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        zCompare(true, GSGFX_Z_COMPARE_ALWAYS);
    }

    FlushState();

    if (enableCopy) {
        if (copyConfig == NULL)
            GSgfx_GCWriteCopyFilter(renderMode->aa, renderMode->sample_pattern[0],
                                    true, _defaultCopyFilter);
        else
            GSgfx_GCWriteCopyFilter(renderMode->aa, renderMode->sample_pattern[0],
                                    true, copyConfig);
    } else {
        GSgfx_GCWriteCopyFilter(renderMode->aa, renderMode->sample_pattern[0],
                                false, NULL);
    }

    params.mLeft = 0;
    params.mTop = 0;
    params.mWidth = copyWidth;
    params.mHeight = copyHeight;
    params.mDestWidth = copyWidth;
    params.mFormat = GStextureGetGXformat(texture, false);
    params.mMode = 3;
    params.mMipmap = 0;
    params.mClear = clear;
    GSgfx_GCWriteCopyTex(&params, texture->mImage[0]);

    if (clear == true) {
        if (savedColor == true)
            mColorUpdateEnabled = true;
        else
            mColorUpdateEnabled = false;
        if (savedAlpha == true)
            mAlphaUpdateEnabled = true;
        else
            mAlphaUpdateEnabled = false;
        if (savedDepth == true)
            mDepthUpdateEnabled = true;
        else
            mDepthUpdateEnabled = false;
        mPassState[0].mGpuFlags |= GSGFX_GPU_PIXEL_MODE;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mGpuFlags |= GSGFX_GPU_PIXEL_MODE;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        if (savedDepthCompare == true)
            mDepthCompareEnabled = true;
        else
            mDepthCompareEnabled = false;
        mDepthCompareFunction = savedDepthFunc;
        mPassState[0].mGpuFlags |= GSGFX_GPU_Z_COMPARE;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mGpuFlags |= GSGFX_GPU_Z_COMPARE;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    }
    mRenderFlags |= GSGFX_RENDER_FLAG_INVALIDATE_TEXTURE_CACHE | 0x20000000;
}

void GSgfxLayer::EffectScreenClear(bool color, bool alpha, bool depth)
{
    f32 projection[4][4];

    if (!color && !alpha && !depth)
        return;

    if (color == true)
        mColorUpdateEnabled = true;
    else
        mColorUpdateEnabled = false;
    if (alpha == true)
        mAlphaUpdateEnabled = true;
    else
        mAlphaUpdateEnabled = false;
    if (depth == true)
        mDepthUpdateEnabled = true;
    else
        mDepthUpdateEnabled = false;

    mPassState[0].mGpuFlags |= GSGFX_GPU_PIXEL_MODE;
    mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    mPassState[1].mGpuFlags |= GSGFX_GPU_PIXEL_MODE;
    mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;

    GSmtx44Ortho(projection, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    orthographicProjection(projection);
    mMatrixStack->identity();
    projectionDepthRange(0.0f, 30000.0f);

    blendMode(GSGFX_BLEND_NONE, GSGFX_BLEND_ONE, GSGFX_BLEND_ONE, false);
    zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND,
                 GSGFX_A_COMPARE_ALWAYS, 0);
    cullMode(GSGFX_CULL_NONE);
    clipMode(GSGFX_CLIP_DISABLED);
    setRenderStateAndSelect(GSgfxVFGetPredef(GSGFX_VF_PREDEFINED_POSITION_COLOR));
    mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    beginSurface(4);

    f32 quadHeight = 480.0f;
    f32 quadWidth = 640.0f;
    f32 quadDepth = 29999.0f;
    f32 zero = 0.0f;
    GXPosition3f32(zero, zero, quadDepth);
    GXColor4u8(0, 0, 0, 0);
    GXPosition3f32(quadWidth, zero, quadDepth);
    GXColor4u8(0, 0, 0, 0);
    GXPosition3f32(zero, quadHeight, quadDepth);
    GXColor4u8(0, 0, 0, 0);
    GXPosition3f32(quadWidth, quadHeight, quadDepth);
    GXColor4u8(0, 0, 0, 0);
    endSurface();
}

void GSgfxLayer::EffectDistortion()
{
    f32 projection[4][4];

    if (mFog)
        fog(false);
    SetRenderPass(GSGFX_LAYER_PASS_XLU);
    EffectTextureCopy(distTexPtr, true, NULL, false, false, false);
    GSgfxVideoResetViewport(true);

    tunitSwapTable(0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    cullMode(GSGFX_CULL_BACK);
    blendMode(GSGFX_BLEND_NONE, GSGFX_BLEND_ONE, GSGFX_BLEND_ONE, false);
    pixelMode(true, false, false);
    zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND,
                 GSGFX_A_COMPARE_ALWAYS, 0);
    clipMode(GSGFX_CLIP_ENABLED);
    texCoordGeneratorCount(1);
    tgenFunc(0, GS_GFX_TGEN_MTX2X4, GS_GFX_TGEN_SRC_TEX0, NULL, false, NULL);
    tunitCount(1);
    indTexCount(1);
    channelCount(0);
    indTexOrder(0, 0, 1);
    indTexCoordScale(0, 0, 0);
    tunitIndTexMtx(GS_GFX_IND_MTX0, mDistortionMatrix, mDistortionScale);
    tunitIndirect(0, GS_GFX_INDIRECT_WARP, 0, 0, 0, GS_GFX_IND_MTX0);
    tunitMapping(0, 0, 0, GSGFX_TUNIT_NO_TEXTURE);
    tunitColorOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                 GSGFX_TUNIT_SCALE_ONE, false, GSGFX_TUNIT_REG_PREV);
    tunitAlphaOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                 GSGFX_TUNIT_SCALE_ONE, false, GSGFX_TUNIT_REG_PREV);
    tunitColorArg(0, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO,
                  GSGFX_TUNIT_C_TEXC);
    tunitAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO,
                  GSGFX_TUNIT_A_ZERO);
    texSelect(0, fbTexPtr);
    texSelect(1, distTexPtr);

    mVertexFormat = GSgfxVFGetPredef(GSGFX_VF_PREDEFINED_XY_S16_TEXCOORD_U8);
    GSmtx44Ortho(projection, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    orthographicProjection(projection);
    mMatrixStack->identity();
    projectionDepthRange(0.0f, 30000.0f);
    mPrimitiveType = GS_GFX_PRIM_TRIANGLE_STRIP;
    beginSurface(4);
    GXPosition2u16(0, 0);
    GXTexCoord2u8(0, 0);
    GXPosition2u16(640, 0);
    GXTexCoord2u8(1, 0);
    GXPosition2u16(0, 480);
    GXTexCoord2u8(0, 1);
    GXPosition2u16(640, 480);
    GXTexCoord2u8(1, 1);
    endSurface();

    indTexCount(0);
    mTevIndirectOperation0 = GS_GFX_INDIRECT_DIRECT;
    mPassState[0].mIndirectOperationFlags |= GS_GFX_STAGE_ZERO_MASK;
    mPassState[0].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    mPassState[1].mIndirectOperationFlags |= GS_GFX_STAGE_ZERO_MASK;
    mPassState[1].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    FlushState();
    GScolor savedClearColor = mSavedClearColor;
    GSgfxVideoSetClearColor(&savedClearColor);
    GSgfxVideoResetViewport(false);
}

void GSgfxLayer::PrepareDistortion()
{
    GScolor black = _zeroClearColor;
    GScolor savedClearColor;
    GScolor color;

    if ((mRenderFlags & GSGFX_RENDER_FLAG_TEXTURE_COPY) != 0 &&
        mTextureCopy == fbTexPtr)
        return;

    SetRenderPass(GSGFX_LAYER_PASS_XLU);
    tunitSwapTable(0, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    savedClearColor = mSavedClearColor;
    GSgfxVideoGetClearColor(&savedClearColor);
    color = black;
    GSgfxVideoSetClearColor(&color);
    EffectTextureCopy(fbTexPtr, false, NULL, true, true, false);
}

void GSgfxLayer::PrepareFBCopy()
{
    if ((mRenderFlags & GSGFX_RENDER_FLAG_TEXTURE_COPY) != 0 &&
        mTextureCopy == fbTexPtr)
        return;

    SetRenderPass(GSGFX_LAYER_PASS_XLU);
    EffectTextureCopy(fbTexPtr, false, NULL, false, false, false);
}
