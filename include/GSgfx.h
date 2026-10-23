#ifndef GS_GFX_H
#define GS_GFX_H

#include <types.h>
#include <GSvec.h>
#include <GStexture.h>

// Write-gather FIFO the GS layers emit their primitives into.
typedef union {
    u8 u8;
    u16 u16;
    u32 u32;
    s8 s8;
    s16 s16;
    s32 s32;
    f32 f32;
} PPCWGPipe;

#define GXWGFifo (*(volatile PPCWGPipe *)0xCC008000)

// Layer commands of the frame (GSgfxLayer +0x04): the bits the render modules
// set and the layer's own setup pass consumes one by one.
enum GSgfxRenderFlag {
    GSGFX_RENDER_FLAG_ZDRAW_BANK = 0x08000000,      // particle bank started
    GSGFX_RENDER_FLAG_ZDRAW_GENERATOR = 0x10000000, // particle generator started
};

// Setup groups of a pass state (GSgfxPassState +0x00): every group owns the
// dirty word behind it and runs when its bit is set. Only the group the GPU
// state setup consumes (0x40) and the light group (0x4) show what they do.
enum GSgfxPassFlag {
    GSGFX_PASS_FLAG_GROUP_A = 0x1,
    GSGFX_PASS_FLAG_GROUP_B = 0x2,
    GSGFX_PASS_FLAG_LIGHT = 0x4,
    GSGFX_PASS_FLAG_GPU_STATE = 0x40,
};

// Dirty bits the particle module sets in the pass state's first GX group.
enum GSgfxPassGroupAFlag {
    GSGFX_PASS_GROUP_A_PARTICLES = 0x1,
};

// Dirty bits the particle module sets in the pass state's second GX group.
enum GSgfxPassGroupBFlag {
    GSGFX_PASS_GROUP_B_PARTICLES = 0x4,
};

// Dirty bits of the GPU state group (GSgfxPassState +0x64).
enum GSgfxGpuFlag {
    GSGFX_GPU_FOG = 0x40,
    GSGFX_GPU_FOG_ADJ_TABLE = 0x100,
    GSGFX_GPU_FOG_RANGE_ADJ = 0x200,
};

// Render state a layer keeps for one render pass: the pass is set up from it
// before it draws, and each pass owns one of these. The word at 0x00 asks for
// the setup groups behind it (bit 0x40 runs the GPU state setup), the words at
// 0x0C/0x10/0x64 carry the dirty bits of the groups they name.
struct GSgfxPassState {
    u32 mFlags;          // 0x00: setup groups the pass still has to run.
    u8 _04[0x08];
    u32 mGroupAFlags;    // 0x0C: dirty bits of the pass state's first group.
    u32 mGroupBFlags;    // 0x10: dirty bits of the pass state's second group.
    u8 _14[0x50];
    u32 mGpuFlags;       // 0x64: dirty bits of the GPU state setup.
    u8 _68[0x2C];
    u32 mTextureState;   // 0x94: bits of the texture slots the pass loaded.
    GStexture *mTexture; // 0x98: the texture slots, one entry per slot.
    u8 _9C[0x1C];
};


enum GSgfxDLFlags {
    GS_DISPLAY_LIST_ALLOCATED = 1,
    GS_DISPLAY_LIST_DIRTY = 2,
};

enum { GS_DISPLAY_LIST_HEADER_BYTES = 3 };

struct GSgfxVF;
struct GSgfxDL {
    u16 mFlags;
    u16 mVertexStride;
    char *mData;
    u32 mSize;
    GSgfxVF *mVertexFormat;

    char *VertexData(u16 vertex) {
        if (mFlags & GS_DISPLAY_LIST_ALLOCATED)
            return mData + vertex * mVertexStride + GS_DISPLAY_LIST_HEADER_BYTES;
        return 0;
    }
    GSgfxVF *VertexFormat() {
        if (mFlags & GS_DISPLAY_LIST_ALLOCATED)
            return mVertexFormat;
        return 0;
    }
    void Invalidate() { mFlags |= GS_DISPLAY_LIST_DIRTY; }
};

struct GSgfxVF {
    u8 storage[0x440];
};

extern "C" {
void *GSgfxVFCreate(void *vf);
void GSgfxVFFree(void *vf);
void GSgfxVFInit(void *vf);
void GSgfxVFSetElement(void *vf, s32 attribute, u32 componentCount, u32 componentFormat, u8 fractionalBits);
void GSgfxVFSetArray(void *vf, s32 attribute, u32 inputType, void *arrayBase, u32 byteStride, s32 arrayMetadata);
}
void GSgfxRenderWaitForDrawComplete();
class GSgfxLayerConfig;
/* Layer registration offset. */
enum GSgfxLayerOffset { GS_LAYER_INSERT_AFTER = 2 };

/* Registered graphics layer handle. */
enum GSgfxLayerID { };

/* GX surface primitive type. */
enum GSgfxPrimType { GSGFX_QUADS = 0x80, GS_GFX_PRIM_TRIANGLE_STRIP = 0x98 };

/* Operation the texture generator performs. */
enum GSgfxTgenFunc { GS_GFX_TGEN_MTX3X4 = 0, GS_GFX_TGEN_MTX2X4 = 1 };

/* Coordinate source the texture generator samples from. */
enum GSgfxTgenSrc { GS_GFX_TGEN_SRC_POSITION = 0, GS_GFX_TGEN_SRC_TEX0 = 4 };

/* TEV texture-unit combiner preset. */
enum GSgfxTunitFunc { GS_GFX_TUNIT_TEXTURE_REPLACE = 3 };

/* Indirect-texture matrix selector. */
enum GSgfxIndTexMtxID { GS_GFX_IND_MTX0 = 1 };

/* Kind of a GSgfx layer. */
enum GSgfxLayerType { GS_GFX_LAYER_FRAMEBUFFER_COPY = 4 };

/* One registered layer slot of the layer registry. */
class GSgfxLayerConfig {
public:
    static void Remove(GSgfxLayerID id);
    /* Registers a layer and hands back the id it was filed under, which the
     * callers pass straight on to GSgfxLayerFindByID. */
    static GSgfxLayerID Add(char *name, unsigned long pass0FifoSize, unsigned long pass1FifoSize,
                   unsigned long matrixStackCapacity, GSgfxLayerType type, bool clearColor, bool clearAlpha,
                   bool clearDepth, GSgfxLayerID relativeLayerID, GSgfxLayerOffset offset);
};


/* Stack the layers push their world transforms onto. */
struct GSgfxMatrixStackEntry {
    u8 reservedHeader[4];
    GSmtx mMatrix;
    u8 reserved[0x30];
};
class GSgfxMatrixStack {
public:
    void push(GSmtx *mtx);
    void pushMult(GSmtx *mtx);
    void pop(void);
    GSmtx *top(void);
    GSmtx *currentMatrix() { return &mCurrentEntry->mMatrix; }
    u8 reservedHeader[4];
    GSgfxMatrixStackEntry *mCurrentEntry;
};

// Partial layout of the graphics layer and its two render-pass caches.
enum GSgfxCachedStateConstants {
    GS_GFX_DIRTY_GEOMETRY = 0x1,
    GS_GFX_DIRTY_RASTER = 0x2,
    GS_GFX_DIRTY_CHANNELS = 0x4,
    GS_GFX_DIRTY_TEV = 0x8,
    GS_GFX_DIRTY_TEV_KONST = 0x10,
    GS_GFX_DIRTY_TEV_SWAP = 0x20,
    GS_GFX_DIRTY_PIXEL = 0x40,
    GS_GFX_DIRTY_INDIRECT = 0x80,
    GS_GFX_PIXEL_DIRTY_BLEND = 0x1,
    GS_GFX_PIXEL_DIRTY_DESTINATION_ALPHA = 0x2,
    GS_GFX_PIXEL_DIRTY_WRITE_ENABLES = 0x4,
    GS_GFX_PIXEL_DIRTY_DEPTH_COMPARE = 0x8,
    GS_GFX_PIXEL_DIRTY_ALPHA_COMPARE = 0x10,
    GS_GFX_PIXEL_DIRTY_DITHER = 0x20,
    GS_GFX_PIXEL_DIRTY_FOG = 0x40,
    GS_GFX_PIXEL_DIRTY_FOG_COLOR = 0x80,
    GS_GFX_PIXEL_DIRTY_FOG_RANGE_TABLE = 0x100,
    GS_GFX_PIXEL_DIRTY_FOG_RANGE_UPLOAD = 0x200,
    GS_GFX_RASTER_DIRTY_SCISSOR = 0x1,
    GS_GFX_RASTER_DIRTY_CULL = 0x2,
    GS_GFX_RASTER_DIRTY_CLIP = 0x4,
    GS_GFX_RASTER_DIRTY_COPLANAR = 0x8,
    GS_GFX_GEOMETRY_DIRTY_TEXGEN_COUNT = 0x1,
    GS_GFX_CHANNEL_DIRTY_COUNT = 0x1,
    GS_GFX_TEV_DIRTY_COUNT = 0x1,
    GS_GFX_INDIRECT_DIRTY_COUNT = 0x1,
    GS_GFX_STAGE_ZERO_MASK = 0x1,
    GS_GFX_TEXCOORD_ZERO_MASK = 0x1,
    GS_GFX_TEXTURE_MAP_ZERO_MASK = 0x1,
    GS_GFX_TEXTURE_MAP_ONE_MASK = 0x2,
    GS_GFX_INDIRECT_DIRECT = 0x0,
    GS_GFX_INDIRECT_WARP = 0x2,
    GS_GFX_BLEND_ENABLED = 0x1,
    GS_GFX_BLEND_FACTOR_ZERO = 0x0,
    GS_GFX_BLEND_FACTOR_ONE = 0x1,
    GS_GFX_LOGIC_NOOP = 0x5,
    GS_GFX_COMPARE_LEQUAL = 0x3,
    GS_GFX_COMPARE_ALWAYS = 0x7,
    GS_GFX_ALPHA_COMPARE_AND = 0x0,
    GS_GFX_INDIRECT_SCALE_ONE = 0x0,
    GS_GFX_TEV_ORDER_TEXCOORD = 0x0,
    GS_GFX_TEV_ORDER_TEXTURE_MAP = 0x1,
    GS_GFX_TEV_ORDER_COLOR_CHANNEL = 0x2,
    GS_GFX_COLOR_CHANNEL_NULL = 0xFF,
};

enum GSgfxCullType {
    GS_GFX_CULL_NONE = 0,
    GSGFX_CULL_NONE = GS_GFX_CULL_NONE,
    GSGFX_CULL_FRONT = 1,
    GSGFX_CULL_BACK = 2,
    GSGFX_CULL_ALL = 3,
};
enum GSgfxClipMode { GS_GFX_CLIP_ENABLED = 0 };
enum GSgfxZCmpFunc { GS_GFX_Z_COMPARE_LEQUAL = 3 };
enum GSgfxACmpFunc { GS_GFX_A_COMPARE_ALWAYS = 7 };
enum GSgfxAlphaOp { GS_GFX_ALPHA_AND = 0 };
enum GSgfxBlendType { GS_GFX_BLEND_NONE = 0, GS_GFX_BLEND_ALPHA = 1 };
enum GSgfxBlendFactor { GS_GFX_FACTOR_ZERO = 0, GS_GFX_FACTOR_ONE = 1 };
enum GSgfxLayerPass {
    GS_GFX_PASS_OPAQUE = 0, GS_GFX_PASS_TRANSLUCENT = 1,
    GSGFX_LAYER_PASS_OPAQUE = GS_GFX_PASS_OPAQUE,
    GSGFX_LAYER_PASS_XLU = GS_GFX_PASS_TRANSLUCENT
};
struct GSgfxLayerFifo { u8 storage[0xBC]; };

class GSgfxLayer {
public:
    void markDirty(u32 &stateA, u32 &stateB, u32 stateMask, u32 groupMask)
    {
        stateA |= stateMask;
        mDirtyStateA |= groupMask;
        stateB |= stateMask;
        mDirtyStateB |= groupMask;
    }
    void tunitCount(u8 count)
    {
        mTevStageCount = count;
        markDirty(mTevStateDirtyA, mTevStateDirtyB, GS_GFX_TEV_DIRTY_COUNT, GS_GFX_DIRTY_TEV);
    }
    void indTexCount(u8 count)
    {
        mIndirectStageCount = count;
        markDirty(mIndirectStateDirtyA, mIndirectStateDirtyB, GS_GFX_INDIRECT_DIRTY_COUNT, GS_GFX_DIRTY_INDIRECT);
    }
    void tgenCount(u8 count)
    {
        mTexGenCount = count;
        markDirty(mGeometryStateDirtyA, mGeometryStateDirtyB, GS_GFX_GEOMETRY_DIRTY_TEXGEN_COUNT, GS_GFX_DIRTY_GEOMETRY);
    }
    void channelCount(u8 count)
    {
        mChannelCount = count;
        markDirty(mChannelStateDirtyA, mChannelStateDirtyB, GS_GFX_CHANNEL_DIRTY_COUNT, GS_GFX_DIRTY_CHANNELS);
    }
    void destinationAlpha(bool on, u8 alpha)
    {
        mDestinationAlphaEnabled = on;
        mDestinationAlpha = alpha;
        markDirty(mPixelStateDirtyA, mPixelStateDirtyB, GS_GFX_PIXEL_DIRTY_DESTINATION_ALPHA, GS_GFX_DIRTY_PIXEL);
    }
    void dither(bool on)
    {
        mDitherEnabled = on;
        markDirty(mPixelStateDirtyA, mPixelStateDirtyB, GS_GFX_PIXEL_DIRTY_DITHER, GS_GFX_DIRTY_PIXEL);
    }
    void SetRenderPass(GSgfxLayerPass pass)
    {
        if (mRenderPass != pass) {
            mRenderPass = pass;
            mCurrentRenderPassFifo = &mRenderPassFifos[mRenderPass];
        }
    }
    void cullMode(GSgfxCullType mode)
    {
        mCullType = mode;
        markDirty(mRasterStateDirtyA, mRasterStateDirtyB, GS_GFX_RASTER_DIRTY_CULL, GS_GFX_DIRTY_RASTER);
    }
    void clipMode(GSgfxClipMode mode)
    {
        mClipMode = mode;
        markDirty(mRasterStateDirtyA, mRasterStateDirtyB, GS_GFX_RASTER_DIRTY_CLIP, GS_GFX_DIRTY_RASTER);
    }
    void coplanar(bool on)
    {
        mCoplanarEnabled = on;
        markDirty(mRasterStateDirtyA, mRasterStateDirtyB, GS_GFX_RASTER_DIRTY_COPLANAR, GS_GFX_DIRTY_RASTER);
    }
    void pixelMode(bool color, bool alpha, bool depth)
    {
        mColorUpdateEnabled = color == true ? 1 : 0;
        mAlphaUpdateEnabled = alpha == true ? 1 : 0;
        mDepthUpdateEnabled = depth == true ? 1 : 0;
        markDirty(mPixelStateDirtyA, mPixelStateDirtyB, GS_GFX_PIXEL_DIRTY_WRITE_ENABLES, GS_GFX_DIRTY_PIXEL);
    }
    void zCompare(bool on, GSgfxZCmpFunc func)
    {
        mDepthCompareEnabled = on == true ? 1 : 0;
        mDepthCompareFunction = func;
        markDirty(mPixelStateDirtyA, mPixelStateDirtyB, GS_GFX_PIXEL_DIRTY_DEPTH_COMPARE, GS_GFX_DIRTY_PIXEL);
    }
    void alphaCompare(bool on, GSgfxACmpFunc func0, u8 ref0, GSgfxAlphaOp op, GSgfxACmpFunc func1, u8 ref1)
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
        markDirty(mPixelStateDirtyA, mPixelStateDirtyB, GS_GFX_PIXEL_DIRTY_ALPHA_COMPARE, GS_GFX_DIRTY_PIXEL);
    }
    void blendMode(GSgfxBlendType type, GSgfxBlendFactor src, GSgfxBlendFactor dst, bool selectPass)
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
        markDirty(mPixelStateDirtyA, mPixelStateDirtyB, GS_GFX_PIXEL_DIRTY_BLEND, GS_GFX_DIRTY_PIXEL);
    }
    void InvalidateState();
    void setRenderStateAndSelect(GSgfxVF *vf);
    void beginSurface(u16 vertexCount);
    void primitiveType(GSgfxPrimType type);
    void endSurface(void);
    void dlDraw(GSgfxDL *displayList);
    /* Returns whether the layer is visible. */
    bool IsLayerVisible(void);
    void SetLayerVisibility(bool on);

    /* Enable fog and refresh its range adjustment when needed. */
    void fog(bool on)
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

    void tunitMapping(u8 stage, u8 texCoord, u8 textureMap, u8 colorChannel)
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

    void texSelect(u8 textureMap, GStexture *texture)
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

    void tunitFunc(u8 textureUnit, GSgfxTunitFunc func);
    void tgenFunc(u8 texGen, GSgfxTgenFunc func, GSgfxTgenSrc src, GSmtx *textureMatrix,
                  bool normalize, GSmtx *postMatrix);
    void tunitIndTexMtx(GSgfxIndTexMtxID id, f32 (*m)[3], signed char scaleExponent);
    void dlDrawExt(u8 *dl, unsigned long size);

/* Layer registry entry and cached render state. */
    u8 _0000[4];
    u32 mRenderFlags;
    u8 _0008[0x20];
    GSgfxLayerID mLayerID; // 0x028: Slot the layer was registered in.
    u8 _002C[0xC44];
    u8 mTexGenCount;
    u8 _0C71[0xD3];
    u8 mTexCoordLineOffset0;
    u8 mTexCoordPointOffset0;
    u8 _0D46[0x0E];
    u8 mProjection[0x1C];
    u16 mScissorX; // Scissor X origin.
    u16 mScissorY; // Scissor Y origin.
    u16 mScissorWidth; // Scissor width.
    u16 mScissorHeight; // Scissor height.
    u8 _0D78[0x10];
    u8 mCoplanarEnabled;
    u8 _0D89[0x3];
    u32 mCullType;         // 0xD8C: Face culling mode (cullMode stores here).
    u32 mClipMode;
    u8 _0D94[0x24];
    u8 mChannelCount;
    u8 _0DB9[0xF3];
    u8 mTevStageCount;
    u8 _0EAD[0x3];
    s32 mTevOrders[16][3];
    u8 reservedTevState[0x4B0];
    u32 mTevIndirectOperation0;
    u8 mTevIndirectStage0;
    u8 mTevWarpSignedOffset0;
    u8 mTevWarpReplaceMode0;
    u8 _1427[0x1];
    u32 mTevWarpMatrix0;
    u8 _142C[0x84];
    u8 mIndirectStageCount;
    u8 _14B1[0x3];
    u32 mIndirectTexCoord0;
    u32 mIndirectTextureMap0;
    u8 _14BC[0x18];
    u32 mIndirectCoordScaleS0;
    u32 mIndirectCoordScaleT0;
    u8 _14DC[0x18];
    u32 mKonstColors[2]; // 0x14F4: Packed constant colors.
    u8 _14FC[0x148];
    u32 mBlendType;
    u32 mBlendSourceFactor;
    u32 mBlendDestinationFactor;
    u32 mBlendLogicOperation;
    u8 mDestinationAlphaEnabled;
    u8 mDestinationAlpha;
    u8 mColorUpdateEnabled;
    u8 mAlphaUpdateEnabled;
    u8 mDepthUpdateEnabled;
    u8 _1659[0x3];
    u8 mDepthCompareEnabled;
    u8 _165D[0x3];
    u32 mDepthCompareFunction;
    u32 mAlphaCompareFunction0;
    u32 mAlphaCompareOperation;
    u32 mAlphaCompareFunction1;
    u8 mAlphaCompareReference0;
    u8 mAlphaCompareReference1;
    u8 mDepthCompareBeforeTexture;
    u8 _1673[0x1];
    u8 mDitherEnabled;
    u8 _1675[0x31];
    u8 mFogRangeAdjustEnabled;
    u8 mFogEnabled;
    u32 mDirtyStateA;
    u32 mGeometryStateDirtyA;
    u8 _16B0[0x4];
    u32 mTexCoordOffsetDirtyA;
    u32 mRasterStateDirtyA;
    u32 mChannelStateDirtyA;
    u8 _16C0[0xC];
    u32 mTevStateDirtyA;
    u32 mTevOrderDirtyA;
    u8 _16D4[0x14];
    u32 mTevIndirectDirtyA;
    u32 mIndirectStateDirtyA;
    u32 mIndirectOrderDirtyA;
    u32 mIndirectCoordScaleDirtyA;
    u32 mTevKonstColorsDirtyA;
    u8 _16FC[0x10];
    u32 mPixelStateDirtyA;
    u8 _1710[0x2C];
    u32 mTextureDirtyA;
    GStexture *mTexturesA[8];
    u32 mDirtyStateB;
    u32 mGeometryStateDirtyB;
    u8 _1768[0x4];
    u32 mTexCoordOffsetDirtyB;
    u32 mRasterStateDirtyB;
    u32 mChannelStateDirtyB;
    u8 _1778[0xC];
    u32 mTevStateDirtyB;
    u32 mTevOrderDirtyB;
    u8 _178C[0x14];
    u32 mTevIndirectDirtyB;
    u32 mIndirectStateDirtyB;
    u32 mIndirectOrderDirtyB;
    u32 mIndirectCoordScaleDirtyB;
    u32 mTevKonstColorsDirtyB;
    u8 _17B4[0x10];
    u32 mPixelStateDirtyB;
    u8 _17C8[0x2C];
    u32 mTextureDirtyB;
    GStexture *mTexturesB[8];
    GSgfxLayerFifo mRenderPassFifos[2];
    GSgfxLayerFifo *mCurrentRenderPassFifo;
    GSgfxLayerPass mRenderPass;
    GSgfxPrimType mPrimitiveType; // 0x1998: Primitive op the surface emits.
    void * mVertexFormat;
    GSgfxMatrixStack * mMatrixStack; // 0x19A0: Transform stack of the pass.

};

extern "C" {
typedef void (*GSgfxParseVertexCB)(unsigned long attributeFlags, void *attributeData, void *userContext);
typedef void (*GSgfxParseSurfaceCB)(GSgfxPrimType type, u16 count, unsigned long attributeMask, void *userContext);

void GSgfxParseDisplayList(void *vtxAttr, void *dl, u32 size, const void *callbacks, void *ctx);
u32 GSgfxPerfbarCPUEntry(u32 id);
/* Either dimension output may be null. */
void GSgfxVideoGetEFBSize(u16 *width, u16 *height);
GSgfxLayer *GSgfxLayerFindByName(const char *name);
GSgfxLayer *GSgfxLayerFindByID(s32 id);
void *GSgfxLayerGetFrameBufferTexture(void);
}

/* Display-list parser attribute-presence flags. */
#define GS_GFX_PARSE_ATTRIBUTE_PNMTXIDX 0x1u
#define GS_GFX_PARSE_ATTRIBUTE_POSITION 0x2u
#define GS_GFX_POSITION_MATRIX_ROWS 3

// One entry of the render module list GSgfxRenderModuleRegister builds. The
// engine calls the hooks once per frame; fastRender is used on the frames the
// engine skips the normal render path.
struct GSgfxRenderModule {
    void (*init)(void *param);                            // 0x00
    void (*preUpdate)(f32 delta);                         // 0x04
    void (*render)(GSgfxLayer *layer, GSgfxLayerPass pass); // 0x08
    void (*fastRender)(void);                             // 0x0C
    void (*postUpdate)(f32 delta);                        // 0x10
    void (*postFrame)(void);                              // 0x14
    u32 signature;                                        // 0x18
};

#define GSGFX_RENDER_MODULE_SIGNATURE 0xB29A48E1
extern "C" {
GSgfxVF* GSgfxVFBuildTemp(s32 positionComponents, s32 positionType,
                         s32 hasNormal, s32 normalComponents,
                         s32 colorCount, s32 colorType,
                         s32 texCoordCount, s32 texCoordComponents,
                         s32 texCoordType);
void GSgfxZCompare(s32 enable, s32 comparison);
void GSgfxPixelMode(s32 colorUpdate, s32 alphaUpdate, s32 depthUpdate);
void GSgfxCullMode(s32 mode);
void GSgfxBlendMode(s32 mode, s32 sourceFactor, s32 destinationFactor);
}


#endif
