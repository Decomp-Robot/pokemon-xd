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
    positionX = model->mPosition.x;
    positionY = model->mPosition.y;
    positionZ = model->mPosition.z;
    model->mPosition.x = 0.0f;
    model->mPosition.y = 0.0f;
    model->mPosition.z = 0.0f;
    rotationX = model->mRotation.x;
    rotationY = model->mRotation.y;
    rotationZ = model->mRotation.z;
    model->mRotation.x = 0.0f;
    model->mRotation.y = 0.0f;
    model->mRotation.z = 0.0f;
    scaleX = model->mScale.x;
    scaleY = model->mScale.y;
    scaleZ = model->mScale.z;
    model->mScale.x = 1.0f;
    model->mScale.y = 1.0f;
    model->mScale.z = 1.0f;
    GSmodelForceAnimTransformUpdate(model);
    GSmodelParse(model, 1, _modelBoundPObj, model);
    model->mLinkedParticleBank = particleBank;
    model->mPosition.x = positionX;
    model->mPosition.y = positionY;
    model->mPosition.z = positionZ;
    model->mRotation.x = rotationX;
    model->mRotation.y = rotationY;
    model->mRotation.z = rotationZ;
    model->mScale.x = scaleX;
    model->mScale.y = scaleY;
    model->mScale.z = scaleZ;
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
