#include <GSmodelInternal.h>

/* Position-matrix selection persists between parser invocations. */
extern void *const gsBoundParserCallbacks[6];

struct GSmodelBoundCtx {
    _GSmodel *mModel;
    GSmtx *mPositionMatrix;
    GSmtx *mMatrixPalette;
    u32 mAttributeMask;
};

extern "C" void GSmodelRecalculateBound(_GSmodel *model)
{
    _GSparticleBank *particleBank;
    f32 positionX, positionY, positionZ;
    f32 rotationX, rotationY, rotationZ;
    f32 scaleX, scaleY, scaleZ;

    if (GSboundHasAnimData(&model->mBound)) {
        return;
    }
    GSboundReset(&model->mBound);
    particleBank = model->mLinkedParticleBank;
    model->mLinkedParticleBank = 0;
    positionX = model->mPosition[0];
    positionY = model->mPosition[1];
    positionZ = model->mPosition[2];
    model->mPosition[0] = 0.0f;
    model->mPosition[1] = 0.0f;
    model->mPosition[2] = 0.0f;
    rotationX = model->mRotation[0];
    rotationY = model->mRotation[1];
    rotationZ = model->mRotation[2];
    model->mRotation[0] = 0.0f;
    model->mRotation[1] = 0.0f;
    model->mRotation[2] = 0.0f;
    scaleX = model->mScale[0];
    scaleY = model->mScale[1];
    scaleZ = model->mScale[2];
    model->mScale[0] = 1.0f;
    model->mScale[1] = 1.0f;
    model->mScale[2] = 1.0f;
    GSmodelForceAnimTransformUpdate(model);
    GSmodelParse(model, 1, _modelBoundPObj, model);
    model->mLinkedParticleBank = particleBank;
    model->mPosition[0] = positionX;
    model->mPosition[1] = positionY;
    model->mPosition[2] = positionZ;
    model->mRotation[0] = rotationX;
    model->mRotation[1] = rotationY;
    model->mRotation[2] = rotationZ;
    model->mScale[0] = scaleX;
    model->mScale[1] = scaleY;
    model->mScale[2] = scaleZ;
}

/* GSmodelParse callback: walks one primitive object display list so its
 * vertices contribute to the model bound. */
void _modelBoundPObj(_HSD_PObj *pobj, GSmtx *referenceMatrix, GSmtx *positionMatrix,
                            GSmtx *matrixPalette, void *userContext)
{
    GSmodelBoundCtx ctx;

    ctx.mModel = (_GSmodel *)userContext;
    ctx.mPositionMatrix = positionMatrix;
    ctx.mMatrixPalette = matrixPalette;
    GSgfxParseDisplayList(pobj->mVtxAttr, pobj->mDisplayList, pobj->mDisplayListBlocks << 5,
                          gsBoundParserCallbacks, &ctx);
}

/* Accumulates position attributes using the selected position matrix. */
void _modelBoundVertex(unsigned long attributeFlags, void *attributeData, void *userContext)
{
    GSmodelBoundCtx *ctx = (GSmodelBoundCtx *)userContext;
    GSmtx *mtx;
    f32 *src = (f32 *)attributeData;

    static s32 gsBoundPositionMatrixIndex = 0;
    if (attributeFlags & GS_GFX_PARSE_ATTRIBUTE_PNMTXIDX) {
        gsBoundPositionMatrixIndex = ((u8 *)attributeData)[0] / GS_GFX_POSITION_MATRIX_ROWS;
    }
    if (!(attributeFlags & GS_GFX_PARSE_ATTRIBUTE_POSITION)) {
        return;
    }
    /* A matrix-index attribute selects the surface's matrix palette. */
    if (ctx->mAttributeMask & GS_GFX_PARSE_ATTRIBUTE_PNMTXIDX) {
        mtx = &ctx->mMatrixPalette[gsBoundPositionMatrixIndex];
    } else {
        mtx = ctx->mPositionMatrix;
    }
    GSvec v(src[0], src[1], src[2]);
    PSMTXMultVec(mtx->sdk(), (Vec *)&v, (Vec *)&v);
    GSboundAddPoint(GSmodelGetBound(ctx->mModel), v.x, v.y, v.z);
}

/* Records the attributes present on this surface. */
void _modelBoundBeginSurface(GSgfxPrimType primitiveType, u16 vertexCount, unsigned long attributeMask,
                                    void *userContext)
{
    GSmodelBoundCtx *ctx = (GSmodelBoundCtx *)userContext;

    ctx->mAttributeMask = attributeMask;
}
