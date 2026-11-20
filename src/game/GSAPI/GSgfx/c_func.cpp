#include <GSgfx.h>
#include <GScamera.h>
#include <GSprim.h>
#include <dolphin/mtx.h>

extern "C" {

f32 (*GSgfxMatrixGetPtr(GSgfxMatrixSelector matrix))[4]
{
    GSgfxMatrixStack *stack;

    switch (matrix) {
    case GSGFX_MATRIX_CURRENT:
        return gsgfxState->activeLayer->mMatrixStack->currentMatrix()->sdk();
    case GSGFX_MATRIX_CURRENT_INVERSE_TRANSPOSE:
        stack = gsgfxState->activeLayer->mMatrixStack;
        if (stack->mCurrentEntry->mFlags & GS_MATRIX_INVERSE_TRANSPOSE_DIRTY) {
            PSMTXInvXpose(stack->mCurrentEntry->mMatrix.sdk(),
                          stack->mCurrentEntry->mInverseTranspose.sdk());
            stack->mCurrentEntry->mFlags &= ~GS_MATRIX_INVERSE_TRANSPOSE_DIRTY;
        }
        return stack->mCurrentEntry->mInverseTranspose.sdk();
    default: {
        GScamera *camera = GScameraGetActiveCamera();

        if (camera == NULL) {
            return NULL;
        }
        switch (matrix) {
        case GSGFX_MATRIX_VIEW:
            return GScameraGetMatrixPtr(camera);
        case GSGFX_MATRIX_VIEW_INVERSE:
            return GScameraGetInvMatrixPtr(camera);
        case GSGFX_MATRIX_PROJECTION:
            return GScameraGetProjMatrixPtr(camera);
        default:
            return NULL;
        }
    }
    }
}

u16 GSgfxTokenMark(void)
{
    return gsgfxState->activeLayer->tokenMark();
}

void GSgfxDLDraw(GSgfxDL *displayList)
{
    gsgfxState->activeLayer->dlDraw(displayList);
}

void GSgfxSetupRenderAndSelect(GSgfxVF *format)
{
    gsgfxState->activeLayer->setRenderStateAndSelect(format);
}

void GSgfxSetTUnitUserAlphaArg(u8 stage, u32 a, u32 b, u32 c, u32 d)
{
    gsgfxState->activeLayer->tunitAlphaArg(stage, (GSgfxTunitAArg)a, (GSgfxTunitAArg)b,
                                           (GSgfxTunitAArg)c, (GSgfxTunitAArg)d);
}

void GSgfxSetTUnitUserAlphaOp(u8 stage, u32 operation, u32 bias, u32 scale, u8 clamp, u32 output)
{
    gsgfxState->activeLayer->tunitAlphaOp(stage, (GSgfxTunitOp)operation, (GSgfxTunitBias)bias,
                                          (GSgfxTunitScale)scale, clamp != 0, (GSgfxTunitReg)output);
}

void GSgfxSetTUnitUserColorArg(u8 stage, u32 a, u32 b, u32 c, u32 d)
{
    gsgfxState->activeLayer->tunitColorArg(stage, (GSgfxTunitCArg)a, (GSgfxTunitCArg)b,
                                           (GSgfxTunitCArg)c, (GSgfxTunitCArg)d);
}

void GSgfxSetTUnitUserColorOp(u8 stage, u32 operation, u32 bias, u32 scale, u8 clamp, u32 output)
{
    gsgfxState->activeLayer->tunitColorOp(stage, (GSgfxTunitOp)operation, (GSgfxTunitBias)bias,
                                          (GSgfxTunitScale)scale, clamp != 0, (GSgfxTunitReg)output);
}

void GSgfxSelectTexture(u8 textureMap, GStexture *texture)
{
    gsgfxState->activeLayer->texSelect(textureMap, texture);
}

void GSgfxCullMode(s32 mode)
{
    gsgfxState->activeLayer->cullMode((GSgfxCullType)mode);
}

void GSgfxAlphaCompare(u8 enable, s32 compare0, u8 reference0, s32 operation,
                       s32 compare1, u8 reference1)
{
    gsgfxState->activeLayer->alphaCompare(enable == 1, (GSgfxACmpFunc)compare0, reference0,
                                          (GSgfxAlphaOp)operation, (GSgfxACmpFunc)compare1,
                                          reference1);
}

void GSgfxZCompare(u8 enable, s32 comparison)
{
    GSgfxLayer *layer = gsgfxState->activeLayer;

    if (enable == 1) {
        layer->mDepthCompareEnabled = true;
    } else {
        layer->mDepthCompareEnabled = false;
    }
    layer->mDepthCompareFunction = (GSgfxZCmpFunc)comparison;
    layer->mPassState[0].mGpuFlags |= GSGFX_GPU_Z_COMPARE;
    layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    layer->mPassState[1].mGpuFlags |= GSGFX_GPU_Z_COMPARE;
    layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
}

void GSgfxPixelMode(u8 colorUpdate, u8 alphaUpdate, u8 depthUpdate)
{
    GSgfxLayer *layer = gsgfxState->activeLayer;

    if (colorUpdate == 1) {
        layer->mColorUpdateEnabled = true;
    } else {
        layer->mColorUpdateEnabled = false;
    }
    if (alphaUpdate == 1) {
        layer->mAlphaUpdateEnabled = true;
    } else {
        layer->mAlphaUpdateEnabled = false;
    }
    if (depthUpdate == 1) {
        layer->mDepthUpdateEnabled = true;
    } else {
        layer->mDepthUpdateEnabled = false;
    }
    layer->mPassState[0].mGpuFlags |= GSGFX_GPU_PIXEL_MODE;
    layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    layer->mPassState[1].mGpuFlags |= GSGFX_GPU_PIXEL_MODE;
    layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
}

void GSgfxBlendMode(s32 mode, s32 sourceFactor, s32 destinationFactor)
{
    gsgfxState->activeLayer->blendMode((GSgfxBlendType)mode, (GSgfxBlendFactor)sourceFactor,
                                       (GSgfxBlendFactor)destinationFactor, true);
}

void GSgfxSetLineWidth(f32 width)
{
    GSprimLayerLineWidth(gsgfxState->activeLayer, (u8)(6.0f * width));
}

void GSgfxTexCoord2f(f32 s, f32 t)
{
    GSgfxFifo.f32 = s;
    GSgfxFifo.f32 = t;
}

void GSgfxColor4u8(u8 r, u8 g, u8 b, u8 a)
{
    GSgfxFifo.u8 = r;
    GSgfxFifo.u8 = g;
    GSgfxFifo.u8 = b;
    GSgfxFifo.u8 = a;
}

void GSgfxVertex2s16(s16 x, s16 y)
{
    GSgfxFifo.s16 = x;
    GSgfxFifo.s16 = y;
}

void GSgfxVertex2f(f32 x, f32 y)
{
    GSgfxFifo.f32 = x;
    GSgfxFifo.f32 = y;
}

void GSgfxVertex3f(f32 x, f32 y, f32 z)
{
    GSgfxFifo.f32 = x;
    GSgfxFifo.f32 = y;
    GSgfxFifo.f32 = z;
}

void GSgfxEndSurface(void)
{
    gsgfxState->activeLayer->endSurface();
}

void GSgfxBeginSurface(u16 vertexCount)
{
    gsgfxState->activeLayer->beginSurface(vertexCount);
}

void GSgfxSetPrimitiveType(s32 type)
{
    gsgfxState->activeLayer->mPrimitiveType = (GSgfxPrimType)type;
}

}
