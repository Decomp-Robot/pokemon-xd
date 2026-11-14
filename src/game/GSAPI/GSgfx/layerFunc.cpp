#include <GSgfx.h>
#include <dolphin/gx/GXTransform.h>

void GSgfxLayer::SetDistortionParams(f32 (*matrix)[3], signed char scaleExponent)
{
    mDistortionMatrix[0][0] = matrix[0][0];
    mDistortionMatrix[0][1] = matrix[0][1];
    mDistortionMatrix[0][2] = matrix[0][2];
    mDistortionMatrix[1][0] = matrix[1][0];
    mDistortionMatrix[1][1] = matrix[1][1];
    mDistortionMatrix[1][2] = matrix[1][2];
    mDistortionScale = scaleExponent;
}

u16 GSgfxLayer::tokenMark()
{
    SetupFifo();
    return GSgfxTokenMark_Sub();
}

void GSgfxLayer::tunitIndTexMtx(GSgfxIndTexMtxID id, f32 (*matrix)[3], signed char scaleExponent)
{
    GSgfxLayerPass pass;
    GXIndTexMtxID matrixID;
    SetupFifo();
    matrixID = (GXIndTexMtxID)id;
    GSgfx_GCWriteIndTexMtx(matrixID, matrix, scaleExponent);
    pass = mCurrentPass;
    GSgfxLayerPass otherPass = (GSgfxLayerPass)!pass;
    if (mDrawPasses[otherPass].mFifoBuffer != 0) {
        SetRenderPass(otherPass);
        SetupFifo();
        GSgfx_GCWriteIndTexMtx(matrixID, matrix, scaleExponent);
        SetRenderPass(pass);
        SetupFifo();
    }
}

void GSgfxLayer::tgenFunc(u8 texGen, GSgfxTgenFunc func, GSgfxTgenSrc src,
                         GSmtx *textureMatrix, bool normalize, GSmtx *postMatrix)
{
    mTexGen[texGen].mFunction = func;
    mTexGen[texGen].mSource = src;
    mTexGen[texGen].mNormalize = normalize;
    mTexGen[texGen].mFlags = 0;
    u8 loadMatrix = 0;
    if (textureMatrix == 0) {
        mTexGen[texGen].mMatrix = GX_IDENTITY;
    } else {
        mTexGen[texGen].mMatrix = GX_TEXMTX0 + texGen * 3;
        loadMatrix = 1;
    }
    if (postMatrix == 0) {
        mTexGen[texGen].mPostMatrix = GX_PTIDENTITY;
    } else {
        mTexGen[texGen].mPostMatrix = GX_PTTEXMTX0 + texGen * 3;
        loadMatrix = 1;
    }
    if (loadMatrix == 1) {
        SetupFifo();
        if (textureMatrix != 0) {
            if (func == GS_GFX_TGEN_MTX2X4)
                GXLoadTexMtxImm(textureMatrix->sdk(), mTexGen[texGen].mMatrix, GX_MTX2x4);
            else
                GXLoadTexMtxImm(textureMatrix->sdk(), mTexGen[texGen].mMatrix, GX_MTX3x4);
        }
        if (postMatrix != 0)
            GXLoadTexMtxImm(textureMatrix->sdk(), mTexGen[texGen].mPostMatrix, GX_MTX3x4);
        GSgfxLayerPass pass = mCurrentPass;
        GSgfxLayerPass otherPass = (GSgfxLayerPass)!pass;
        if (mDrawPasses[otherPass].mFifoBuffer != 0) {
            SetRenderPass(otherPass);
            SetupFifo();
            if (textureMatrix != 0) {
                if (func == GS_GFX_TGEN_MTX2X4)
                    GXLoadTexMtxImm(textureMatrix->sdk(), mTexGen[texGen].mMatrix, GX_MTX2x4);
                else
                    GXLoadTexMtxImm(textureMatrix->sdk(), mTexGen[texGen].mMatrix, GX_MTX3x4);
            }
            if (postMatrix != 0)
                GXLoadTexMtxImm(textureMatrix->sdk(), mTexGen[texGen].mPostMatrix, GX_MTX3x4);
            SetRenderPass(pass);
            SetupFifo();
        }
    }
    mPassState[0].mTexGenFlags |= 1 << texGen;
    mPassState[0].mFlags |= GSGFX_PASS_FLAG_GEOMETRY;
    mPassState[1].mTexGenFlags |= 1 << texGen;
    mPassState[1].mFlags |= GSGFX_PASS_FLAG_GEOMETRY;
}

void GSgfxLayer::tunitFunc(u8 stage, GSgfxTunitFunc func)
{
    switch (func) {
    case GS_GFX_TUNIT_MODULATE:
        tunitColorOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitColorArg(stage, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_RASC,
                      GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_ZERO);
        tunitAlphaOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitAlphaArg(stage, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_RASA,
                      GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO);
        break;
    case GS_GFX_TUNIT_DECAL:
        tunitColorOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitColorArg(stage, GSGFX_TUNIT_C_RASC, GSGFX_TUNIT_C_TEXC,
                      GSGFX_TUNIT_C_TEXA, GSGFX_TUNIT_C_ZERO);
        tunitAlphaOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitAlphaArg(stage, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO,
                      GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_RASA);
        break;
    case GS_GFX_TUNIT_BLEND:
        tunitColorOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitColorArg(stage, GSGFX_TUNIT_C_RASC, GSGFX_TUNIT_C_ONE,
                      GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_ZERO);
        tunitAlphaOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitAlphaArg(stage, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_RASA,
                      GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO);
        break;
    case GS_GFX_TUNIT_TEXTURE_REPLACE:
        tunitColorOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitColorArg(stage, GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_ZERO,
                      GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO);
        tunitAlphaOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitAlphaArg(stage, GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO,
                      GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO);
        break;
    case GS_GFX_TUNIT_COLOR_REPLACE:
        tunitColorOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitColorArg(stage, GSGFX_TUNIT_C_RASC, GSGFX_TUNIT_C_ZERO,
                      GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_ZERO);
        tunitAlphaOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitAlphaArg(stage, GSGFX_TUNIT_A_RASA, GSGFX_TUNIT_A_ZERO,
                      GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO);
        break;
    case GS_GFX_TUNIT_TEXTURE_ADD:
        tunitColorOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitColorArg(stage, GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_ZERO,
                      GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_PREV);
        tunitAlphaOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitAlphaArg(stage, GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO,
                      GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_PREV);
        break;
    case GS_GFX_TUNIT_COLOR_ADD:
        tunitColorOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitColorArg(stage, GSGFX_TUNIT_C_RASC, GSGFX_TUNIT_C_ZERO,
                      GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_PREV);
        tunitAlphaOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitAlphaArg(stage, GSGFX_TUNIT_A_RASA, GSGFX_TUNIT_A_ZERO,
                      GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_PREV);
        break;
    case GS_GFX_TUNIT_MODULATE_ADD:
        tunitColorOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitColorArg(stage, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_RASC,
                      GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_PREV);
        tunitAlphaOp(stage, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO,
                     GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
        tunitAlphaArg(stage, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_RASA,
                      GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_PREV);
        break;
    default:
        return;
    }
}

void GSgfxLayer::cunitChannelGen(u8 channel, GSgfxCunitAlpha alpha,
                                GSgfxCunitSrc ambient, GSgfxCunitSrc material,
                                unsigned long lightCount, GSgfxLight **lights,
                                GSgfxLightType type, GSgfxDiffuseFunc diffuse)
{
    u32 i;
    u32 colorChannel = channel;
    u32 alphaChannel = colorChannel + 2;
    u8 enabled = 0;
    if (lights != 0 && lightCount != 0) enabled = 1;
    if (!enabled) {
        type = GSGFX_LIGHT_DIFFUSE;
        diffuse = GSGFX_DIFFUSE_NONE;
    } else if (type == GSGFX_LIGHT_SPECULAR) {
        diffuse = GSGFX_DIFFUSE_NONE;
    }
    if (alpha == GSGFX_CUNIT_COLOR || alpha == GSGFX_CUNIT_COLOR_AND_ALPHA) {
        mChannels[colorChannel].mLightingEnabled = enabled;
        mChannels[colorChannel].mAmbientSource = ambient;
        mChannels[colorChannel].mMaterialSource = material;
        mChannels[colorChannel].mLightMask = 0;
        mChannels[colorChannel].mDiffuseFunction = diffuse;
        mChannels[colorChannel].mLightType = type;
        if (enabled) {
            for (i = 0; i < 8; i++) {
                if (i < lightCount) mChannels[colorChannel].mLights[i] = lights[i];
                else mChannels[colorChannel].mLights[i] = 0;
            }
        } else {
            memset(mChannels[colorChannel].mLights, 0, sizeof(mChannels[colorChannel].mLights));
        }
        mPassState[0].mChannelFlags |= 1 << colorChannel;
        mPassState[1].mChannelFlags |= 1 << colorChannel;
    }
    if (alpha == GSGFX_CUNIT_ALPHA || alpha == GSGFX_CUNIT_COLOR_AND_ALPHA) {
        mChannels[alphaChannel].mLightingEnabled = enabled;
        mChannels[alphaChannel].mAmbientSource = ambient;
        mChannels[alphaChannel].mMaterialSource = material;
        mChannels[alphaChannel].mLightMask = 0;
        mChannels[alphaChannel].mDiffuseFunction = diffuse;
        mChannels[alphaChannel].mLightType = type;
        if (enabled) {
            for (i = 0; i < 8; i++) {
                if (i < lightCount) mChannels[colorChannel].mLights[i] = lights[i];
                else mChannels[colorChannel].mLights[i] = 0;
            }
        } else {
            memset(mChannels[colorChannel].mLights, 0, sizeof(mChannels[colorChannel].mLights));
        }
        mPassState[0].mChannelFlags |= 1 << alphaChannel;
        mPassState[1].mChannelFlags |= 1 << alphaChannel;
    }
    mPassState[0].mFlags |= GSGFX_PASS_FLAG_LIGHT;
    mPassState[1].mFlags |= GSGFX_PASS_FLAG_LIGHT;
}
