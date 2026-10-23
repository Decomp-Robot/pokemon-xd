#ifndef GS_GFX_INLINE_H
#define GS_GFX_INLINE_H

#include <GSgfxLayer.h>

extern "C" {
    static inline void GXParam1u8(u8 value);
    static inline void GXParam1f32(f32 value);
}

inline void GSgfxLayer::color4u8(u8 red, u8 green, u8 blue, u8 alpha)
{
    GXParam1u8(red); GXParam1u8(green); GXParam1u8(blue); GXParam1u8(alpha);
}

inline void GSgfxLayer::vertex3f(f32 x, f32 y, f32 z)
{
    GXParam1f32(x); GXParam1f32(y); GXParam1f32(z);
}

inline void GSgfxLayer::primitiveType(GSgfxPrimType type) { mPrimitiveType = type; }

inline void GSgfxLayer::blendMode(GSgfxBlendType type, GSgfxBlendFactor src, GSgfxBlendFactor dst, bool selectPass)
{
    if (type == GS_GFX_BLEND_NONE) {
        if (selectPass == true) SetRenderPass(GS_GFX_PASS_OPAQUE);
        mBlendType = GS_GFX_BLEND_NONE;
        mBlendSourceFactor = GS_GFX_BLEND_FACTOR_ONE;
        mBlendDestinationFactor = GS_GFX_BLEND_FACTOR_ONE;
        mBlendLogicOperation = GS_GFX_LOGIC_NOOP;
    } else {
        if (selectPass == true) SetRenderPass(GS_GFX_PASS_TRANSLUCENT);
        mBlendType = type;
        mBlendSourceFactor = src;
        mBlendDestinationFactor = dst;
        mBlendLogicOperation = GS_GFX_LOGIC_NOOP;
    }
    mPixelStateDirtyA |= GS_GFX_PIXEL_DIRTY_BLEND;
    mDirtyStateA |= GS_GFX_DIRTY_PIXEL;
    mPixelStateDirtyB |= GS_GFX_PIXEL_DIRTY_BLEND;
    mDirtyStateB |= GS_GFX_DIRTY_PIXEL;
}

inline void GSgfxLayer::SetRenderPass(GSgfxLayerPass pass)
{
    if (mRenderPass != pass) {
        mRenderPass = pass;
        mCurrentRenderPassFifo = &mRenderPassFifos[mRenderPass];
    }
}

inline void GSgfxLayer::cullMode(GSgfxCullType mode)
{
    mCullType = mode;
    mRasterStateDirtyA |= GS_GFX_RASTER_DIRTY_CULL;
    mDirtyStateA |= GS_GFX_DIRTY_RASTER;
    mRasterStateDirtyB |= GS_GFX_RASTER_DIRTY_CULL;
    mDirtyStateB |= GS_GFX_DIRTY_RASTER;
}

inline void GSgfxLayer::alphaCompare(bool on, GSgfxACmpFunc func0, u8 ref0, GSgfxAlphaOp op, GSgfxACmpFunc func1, u8 ref1)
{
    if (on == true) {
        mAlphaCompareFunction0 = func0;
        mAlphaCompareReference0 = ref0;
        mAlphaCompareOperation = op;
        mAlphaCompareFunction1 = func1;
        mAlphaCompareReference1 = ref1;
        mDepthCompareBeforeTexture = 0;
    } else {
        mAlphaCompareFunction0 = GS_GFX_COMPARE_ALWAYS;
        mAlphaCompareReference0 = 0;
        mAlphaCompareOperation = GS_GFX_ALPHA_COMPARE_AND;
        mAlphaCompareFunction1 = GS_GFX_COMPARE_ALWAYS;
        mAlphaCompareReference1 = 0;
        mDepthCompareBeforeTexture = 1;
    }
    mPixelStateDirtyA |= GS_GFX_PIXEL_DIRTY_ALPHA_COMPARE;
    mDirtyStateA |= GS_GFX_DIRTY_PIXEL;
    mPixelStateDirtyB |= GS_GFX_PIXEL_DIRTY_ALPHA_COMPARE;
    mDirtyStateB |= GS_GFX_DIRTY_PIXEL;
}

inline void GSgfxLayer::zCompare(bool on, GSgfxZCmpFunc func)
{
    if (on == true) mDepthCompareEnabled = 1; else mDepthCompareEnabled = 0;
    mDepthCompareFunction = func;
    mPixelStateDirtyA |= GS_GFX_PIXEL_DIRTY_DEPTH_COMPARE;
    mDirtyStateA |= GS_GFX_DIRTY_PIXEL;
    mPixelStateDirtyB |= GS_GFX_PIXEL_DIRTY_DEPTH_COMPARE;
    mDirtyStateB |= GS_GFX_DIRTY_PIXEL;
}

inline void GSgfxLayer::pixelMode(bool color, bool alpha, bool depth)
{
    if (color == true) mColorUpdateEnabled = 1; else mColorUpdateEnabled = 0;
    if (alpha == true) mAlphaUpdateEnabled = 1; else mAlphaUpdateEnabled = 0;
    if (depth == true) mDepthUpdateEnabled = 1; else mDepthUpdateEnabled = 0;
    mPixelStateDirtyA |= GS_GFX_PIXEL_DIRTY_WRITE_ENABLES;
    mDirtyStateA |= GS_GFX_DIRTY_PIXEL;
    mPixelStateDirtyB |= GS_GFX_PIXEL_DIRTY_WRITE_ENABLES;
    mDirtyStateB |= GS_GFX_DIRTY_PIXEL;
}


inline void GSgfxLayer::markDirty(u32 &stateA, u32 &stateB, u32 stateMask, u32 groupMask)
{
    stateA |= stateMask;
    mDirtyStateA |= groupMask;
    stateB |= stateMask;
    mDirtyStateB |= groupMask;
}

inline void GSgfxLayer::tunitCount(u8 count)
{
    mTevStageCount = count;
    markDirty(mTevStateDirtyA, mTevStateDirtyB, GS_GFX_TEV_DIRTY_COUNT, GS_GFX_DIRTY_TEV);
}

inline void GSgfxLayer::indTexCount(u8 count)
{
    mIndirectStageCount = count;
    markDirty(mIndirectStateDirtyA, mIndirectStateDirtyB, GS_GFX_INDIRECT_DIRTY_COUNT, GS_GFX_DIRTY_INDIRECT);
}

inline void GSgfxLayer::tgenCount(u8 count)
{
    mTexGenCount = count;
    markDirty(mGeometryStateDirtyA, mGeometryStateDirtyB, GS_GFX_GEOMETRY_DIRTY_TEXGEN_COUNT, GS_GFX_DIRTY_GEOMETRY);
}

inline void GSgfxLayer::channelCount(u8 count)
{
    mChannelCount = count;
    markDirty(mChannelStateDirtyA, mChannelStateDirtyB, GS_GFX_CHANNEL_DIRTY_COUNT, GS_GFX_DIRTY_CHANNELS);
}

inline void GSgfxLayer::destinationAlpha(bool on, u8 alpha)
{
    mDestinationAlphaEnabled = on;
    mDestinationAlpha = alpha;
    markDirty(mPixelStateDirtyA, mPixelStateDirtyB, GS_GFX_PIXEL_DIRTY_DESTINATION_ALPHA, GS_GFX_DIRTY_PIXEL);
}

inline void GSgfxLayer::dither(bool on)
{
    mDitherEnabled = on;
    markDirty(mPixelStateDirtyA, mPixelStateDirtyB, GS_GFX_PIXEL_DIRTY_DITHER, GS_GFX_DIRTY_PIXEL);
}

inline void GSgfxLayer::clipMode(GSgfxClipMode mode)
{
    mClipMode = mode;
    markDirty(mRasterStateDirtyA, mRasterStateDirtyB, GS_GFX_RASTER_DIRTY_CLIP, GS_GFX_DIRTY_RASTER);
}

inline void GSgfxLayer::coplanar(bool on)
{
    mCoplanarEnabled = on;
    markDirty(mRasterStateDirtyA, mRasterStateDirtyB, GS_GFX_RASTER_DIRTY_COPLANAR, GS_GFX_DIRTY_RASTER);
}

inline void GSgfxLayer::fog(bool on)
{
    mFogEnabled = on;
    mPixelStateDirtyA |= GS_GFX_PIXEL_DIRTY_FOG;
    mPixelStateDirtyB |= GS_GFX_PIXEL_DIRTY_FOG;
    if (on) {
        mPixelStateDirtyA |= GS_GFX_PIXEL_DIRTY_FOG_RANGE_UPLOAD;
        mPixelStateDirtyB |= GS_GFX_PIXEL_DIRTY_FOG_RANGE_UPLOAD;
        if (mFogRangeAdjustEnabled) {
            mPixelStateDirtyA |= GS_GFX_PIXEL_DIRTY_FOG_RANGE_TABLE;
            mPixelStateDirtyB |= GS_GFX_PIXEL_DIRTY_FOG_RANGE_TABLE;
        }
    }
    mDirtyStateA |= GS_GFX_DIRTY_PIXEL;
    mDirtyStateB |= GS_GFX_DIRTY_PIXEL;
}

inline void GSgfxLayer::tunitMapping(u8 stage, u8 texCoord, u8 textureMap, u8 colorChannel)
{
    mTevOrders[stage][GS_GFX_TEV_ORDER_TEXCOORD] = texCoord;
    mTevOrders[stage][GS_GFX_TEV_ORDER_TEXTURE_MAP] = textureMap;
    if (colorChannel != GS_GFX_COLOR_CHANNEL_NULL) {
        mTevOrders[stage][GS_GFX_TEV_ORDER_COLOR_CHANNEL] = colorChannel + 4;
    } else {
        mTevOrders[stage][GS_GFX_TEV_ORDER_COLOR_CHANNEL] = GS_GFX_COLOR_CHANNEL_NULL;
    }
    markDirty(mTevOrderDirtyA, mTevOrderDirtyB, 1 << stage, GS_GFX_DIRTY_TEV);
}

inline void GSgfxLayer::texSelect(u8 textureMap, GStexture *texture)
{
    if (mTexturesA[textureMap] != texture ||
    mTexturesB[textureMap] != texture ||
    texture->mTextureObjectDirty != 0) {
        mTexturesA[textureMap] = texture;
        mTextureDirtyA |= 1 << textureMap;
        mTexturesB[textureMap] = texture;
        mTextureDirtyB |= 1 << textureMap;
    }
}

inline GSmtx *GSgfxMatrixStack::currentMatrix()
{
    return &mCurrentEntry->mMatrix;
}

#endif
