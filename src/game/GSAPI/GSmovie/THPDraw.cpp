#include <GSAPI/GSmovie/THPDraw.h>
#include <GSgfx.h>
#include <GSgfxVideo.h>
#include <GStexture.h>
#include <GSmtx.h>

static GSgfxLayer *thpLayer;

void THPGXSetRenderLayer(GSgfxLayer *layer)
{
    thpLayer = layer;
}

void THPGXRestore(void)
{
}

void THPGXYuv2RgbSetup(void)
{
    u16 width, height;
    f32 projection[4][4];
    GSgfxVideoGetEFBSize(&width, &height);
    GSgfxLayer *layer = thpLayer;
    GSmtx44Ortho(projection, 0.0f, (s32)height, 0.0f, (s32)width, 0.0f, -30000.0f);
    layer->orthographicProjection(projection);
    layer->mMatrixStack->identity();
    layer->projectionDepthRange(0.0f, 30000.0f);
    thpLayer->scissor();
    thpLayer->pixelMode(true, false, false);
    thpLayer->zCompare(true, GSGFX_Z_COMPARE_ALWAYS);
    thpLayer->blendMode(GSGFX_BLEND_NONE, GSGFX_BLEND_ONE, GSGFX_BLEND_ONE, true);
    thpLayer->cunitCount(0);
    thpLayer->tgenCount(2);
    thpLayer->tgenFunc(0, GS_GFX_TGEN_MTX2X4, GS_GFX_TGEN_SRC_TEX0, 0, false, 0);
    thpLayer->tgenFunc(1, GS_GFX_TGEN_MTX2X4, GS_GFX_TGEN_SRC_TEX0, 0, false, 0);
    thpLayer->mRenderFlags |= GSGFX_RENDER_FLAG_INVALIDATE_TEXTURE_CACHE;
    thpLayer->tunitCount(4);

    thpLayer->tunitMapping(0, 1, 1, GS_GFX_COLOR_CHANNEL_NULL);
    thpLayer->tunitColorArg(0, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_KONST, GSGFX_TUNIT_C_C0);
    thpLayer->tunitColorOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, false, GSGFX_TUNIT_REG_PREV);
    thpLayer->tunitAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_KONST, GSGFX_TUNIT_A_A0);
    thpLayer->tunitAlphaOp(0, GSGFX_TUNIT_OP_SUBTRACT, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, false, GSGFX_TUNIT_REG_PREV);
    thpLayer->tunitKRegSel(0, GSGFX_KREG0);
    thpLayer->tunitSwapMode(0, GX_TEV_SWAP0, GX_TEV_SWAP0);

    thpLayer->tunitMapping(1, 1, 2, GS_GFX_COLOR_CHANNEL_NULL);
    thpLayer->tunitColorArg(1, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_KONST, GSGFX_TUNIT_C_PREV);
    thpLayer->tunitColorOp(1, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_TWO, false, GSGFX_TUNIT_REG_PREV);
    thpLayer->tunitAlphaArg(1, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_KONST, GSGFX_TUNIT_A_PREV);
    thpLayer->tunitAlphaOp(1, GSGFX_TUNIT_OP_SUBTRACT, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, false, GSGFX_TUNIT_REG_PREV);
    thpLayer->tunitKRegSel(1, GSGFX_KREG1);
    thpLayer->tunitSwapMode(1, GX_TEV_SWAP0, GX_TEV_SWAP0);

    thpLayer->tunitMapping(2, 0, 0, GS_GFX_COLOR_CHANNEL_NULL);
    thpLayer->tunitColorArg(2, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_ONE, GSGFX_TUNIT_C_PREV);
    thpLayer->tunitColorOp(2, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
    thpLayer->tunitAlphaArg(2, GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_PREV);
    thpLayer->tunitAlphaOp(2, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
    thpLayer->tunitSwapMode(2, GX_TEV_SWAP0, GX_TEV_SWAP0);

    thpLayer->tunitMapping(3, GSGFX_TUNIT_NO_TEXTURE, GSGFX_TUNIT_NO_TEXTURE, GS_GFX_COLOR_CHANNEL_NULL);
    thpLayer->tunitColorArg(3, GSGFX_TUNIT_C_APREV, GSGFX_TUNIT_C_PREV, GSGFX_TUNIT_C_KONST, GSGFX_TUNIT_C_ZERO);
    thpLayer->tunitColorOp(3, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
    thpLayer->tunitAlphaArg(3, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_ZERO);
    thpLayer->tunitAlphaOp(3, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true, GSGFX_TUNIT_REG_PREV);
    thpLayer->tunitSwapMode(3, GX_TEV_SWAP0, GX_TEV_SWAP0);
    thpLayer->tunitKColorSel(3, GSGFX_KCOLOR_K2);

    GXColorS10 bias = { -90, 0, -114, 135 };
    thpLayer->tunitRegColor(GSGFX_TUNIT_REG0, bias);
    GScolor uCoefficient = { 0x0000E258 };
    thpLayer->tunitKColor(0, uCoefficient);
    GScolor vCoefficient = { 0xB30000B6 };
    thpLayer->tunitKColor(1, vCoefficient);
    GScolor scale = { 0xFF00FF80 };
    thpLayer->tunitKColor(2, scale);
    thpLayer->tunitSwapTable(0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
}

u16 THPGXYuv2RgbDraw(void *yImage, void *uImage, void *vImage, s16 x, s16 y,
                     s16 textureWidth, s16 textureHeight, s16 polygonWidth, s16 polygonHeight)
{
    GStexture yTexture, uTexture, vTexture;

    GStextureCreateFromMemory(&yTexture, textureWidth, textureHeight, yImage,
                              GS_TEXTURE_FORMAT_I8, GS_TEXTURE_TLUT_FORMAT_NONE, 0);
    thpLayer->texSelect(0, &yTexture);
    GStextureCreateFromMemory(&uTexture, textureWidth >> 1, textureHeight >> 1, uImage,
                              GS_TEXTURE_FORMAT_I8, GS_TEXTURE_TLUT_FORMAT_NONE, 0);
    thpLayer->texSelect(1, &uTexture);
    GStextureCreateFromMemory(&vTexture, textureWidth >> 1, textureHeight >> 1, vImage,
                              GS_TEXTURE_FORMAT_I8, GS_TEXTURE_TLUT_FORMAT_NONE, 0);
    thpLayer->texSelect(2, &vTexture);

    thpLayer->mVertexFormat = GSgfxVFGetPredef(GSGFX_VF_PREDEFINED_XY_S16_TEXCOORD_U8);
    thpLayer->mPrimitiveType = GSGFX_QUADS;
    thpLayer->beginSurface(4);
    GSgfxFifo.s16 = x;
    GSgfxFifo.s16 = y;
    GSgfxFifo.u8 = 0;
    GSgfxFifo.u8 = 0;
    GSgfxFifo.s16 = x + polygonWidth;
    GSgfxFifo.s16 = y;
    GSgfxFifo.u8 = 1;
    GSgfxFifo.u8 = 0;
    GSgfxFifo.s16 = x + polygonWidth;
    GSgfxFifo.s16 = y + polygonHeight;
    GSgfxFifo.u8 = 1;
    GSgfxFifo.u8 = 1;
    GSgfxFifo.s16 = x;
    GSgfxFifo.s16 = y + polygonHeight;
    GSgfxFifo.u8 = 0;
    GSgfxFifo.u8 = 1;
    thpLayer->endSurface();
    return thpLayer->tokenMark();
}

