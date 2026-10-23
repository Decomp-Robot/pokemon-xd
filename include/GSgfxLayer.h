#ifndef GS_GFX_LAYER_H
#define GS_GFX_LAYER_H

#include <types.h>
#include <GSvecDecl.h>
#include <GSmtx.h>
#include <GStexture.h>

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
    u8 reservedBeforeGroupAFlags[0x08];
    u32 mGroupAFlags;    // 0x0C: dirty bits of the pass state's first group.
    u32 mGroupBFlags;    // 0x10: dirty bits of the pass state's second group.
    u8 reservedBeforeGpuFlags[0x50];
    u32 mGpuFlags;       // 0x64: dirty bits of the GPU state setup.
    u8 reservedBeforeTextureState[0x2C];
    u32 mTextureState;   // 0x94: bits of the texture slots the pass loaded.
    GStexture *mTexture; // 0x98: the texture slots, one entry per slot.
    u8 reservedPassTail[0x1C];
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
enum GSgfxPrimType { GS_GFX_PRIM_QUADS = 0x80, GSGFX_QUADS = GS_GFX_PRIM_QUADS, GS_GFX_PRIM_TRIANGLE_STRIP = 0x98, GS_GFX_PRIM_TRIANGLE_FAN = 0xA0 };

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
    GSmtx *currentMatrix();
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

enum GSgfxCullType { GS_GFX_CULL_NONE = 0, GSGFX_CULL_NONE = GS_GFX_CULL_NONE, GSGFX_CULL_FRONT = 1, GSGFX_CULL_BACK = 2, GSGFX_CULL_ALL = 3 };
enum GSgfxClipMode { GS_GFX_CLIP_ENABLED = 0 };
enum GSgfxZCmpFunc { GS_GFX_Z_COMPARE_LEQUAL = 3, GS_GFX_Z_COMPARE_ALWAYS = 7 };
enum GSgfxACmpFunc { GS_GFX_A_COMPARE_ALWAYS = 7 };
enum GSgfxAlphaOp { GS_GFX_ALPHA_AND = 0 };
enum GSgfxBlendType { GS_GFX_BLEND_NONE = 0, GS_GFX_BLEND_ALPHA = 1 };
enum GSgfxBlendFactor { GS_GFX_FACTOR_ZERO = 0, GS_GFX_FACTOR_ONE = 1, GS_GFX_FACTOR_SOURCE_ALPHA = 4 };
enum GSgfxLayerPass {
    GS_GFX_PASS_OPAQUE = 0, GS_GFX_PASS_TRANSLUCENT = 1,
    GSGFX_LAYER_PASS_OPAQUE = GS_GFX_PASS_OPAQUE,
    GSGFX_LAYER_PASS_XLU = GS_GFX_PASS_TRANSLUCENT
};
struct GSgfxLayerFifo { u8 storage[0xBC]; };

class GSgfxLayer {
public:
    void markDirty(u32 &stateA, u32 &stateB, u32 stateMask, u32 groupMask);
    void tunitCount(u8 count);
    void indTexCount(u8 count);
    void tgenCount(u8 count);
    void channelCount(u8 count);
    void destinationAlpha(bool on, u8 alpha);
    void dither(bool on);
    void SetRenderPass(GSgfxLayerPass pass);
    void cullMode(GSgfxCullType mode);
    void clipMode(GSgfxClipMode mode);
    void coplanar(bool on);
    void pixelMode(bool color, bool alpha, bool depth);
    void zCompare(bool on, GSgfxZCmpFunc func);
    void alphaCompare(bool on, GSgfxACmpFunc func0, u8 ref0, GSgfxAlphaOp op, GSgfxACmpFunc func1, u8 ref1);
    void blendMode(GSgfxBlendType type, GSgfxBlendFactor src, GSgfxBlendFactor dst, bool selectPass);
    void InvalidateState();
    void setRenderStateAndSelect(GSgfxVF *vf);
    void beginSurface(u16 vertexCount);
    void primitiveType(GSgfxPrimType type);
    void color4u8(u8 red, u8 green, u8 blue, u8 alpha);
    void vertex3f(f32 x, f32 y, f32 z);
    void endSurface(void);
    /* Returns whether the layer is visible. */
    bool IsLayerVisible(void);
    void SetLayerVisibility(bool on);

    /* Enable fog and refresh its range adjustment when needed. */
    void fog(bool on);

    void tunitMapping(u8 stage, u8 texCoord, u8 textureMap, u8 colorChannel);

    void texSelect(u8 textureMap, GStexture *texture);

    void tunitFunc(u8 textureUnit, GSgfxTunitFunc func);
    void tgenFunc(u8 texGen, GSgfxTgenFunc func, GSgfxTgenSrc src, GSmtx *textureMatrix,
                  bool normalize, GSmtx *postMatrix);
    void tunitIndTexMtx(GSgfxIndTexMtxID id, f32 (*m)[3], signed char scaleExponent);
    void dlDraw(GSgfxDL *displayList);
    void dlDrawExt(u8 *dl, unsigned long size);

/* Layer registry entry and cached render state. */
    u8 reservedBeforeRenderFlags[4];
    u32 mRenderFlags;
    u8 reservedBeforeLayerID[0x20];
    GSgfxLayerID mLayerID; // 0x028: Slot the layer was registered in.
    u8 reservedBeforeTexGenCount[0xC44];
    u8 mTexGenCount;
    u8 reservedBeforeTexCoordLineOffset0[0xD3];
    u8 mTexCoordLineOffset0;
    u8 mTexCoordPointOffset0;
    u8 reservedBeforeProjection[0x0E];
    u8 mProjection[0x1C];
    u16 mScissorX; // Scissor X origin.
    u16 mScissorY; // Scissor Y origin.
    u16 mScissorWidth; // Scissor width.
    u16 mScissorHeight; // Scissor height.
    u8 reservedBeforeCoplanarEnabled[0x10];
    u8 mCoplanarEnabled;
    u8 reservedBeforeCullType[0x3];
    u32 mCullType;         // 0xD8C: Face culling mode (cullMode stores here).
    u32 mClipMode;
    u8 reservedBeforeChannelCount[0x24];
    u8 mChannelCount;
    u8 reservedBeforeTevStageCount[0xF3];
    u8 mTevStageCount;
    u8 reservedBeforeTevOrders[0x3];
    s32 mTevOrders[16][3];
    u8 reservedTevState[0x4B0];
    u32 mTevIndirectOperation0;
    u8 mTevIndirectStage0;
    u8 mTevWarpSignedOffset0;
    u8 mTevWarpReplaceMode0;
    u8 reservedBeforeTevWarpMatrix0[0x1];
    u32 mTevWarpMatrix0;
    u8 reservedBeforeIndirectStageCount[0x84];
    u8 mIndirectStageCount;
    u8 reservedBeforeIndirectTexCoord0[0x3];
    u32 mIndirectTexCoord0;
    u32 mIndirectTextureMap0;
    u8 reservedBeforeIndirectCoordScaleS0[0x18];
    u32 mIndirectCoordScaleS0;
    u32 mIndirectCoordScaleT0;
    u8 reservedBeforeKonstColors[0x18];
    u32 mKonstColors[2]; // 0x14F4: Packed constant colors.
    u8 reservedBeforeBlendType[0x148];
    u32 mBlendType;
    u32 mBlendSourceFactor;
    u32 mBlendDestinationFactor;
    u32 mBlendLogicOperation;
    u8 mDestinationAlphaEnabled;
    u8 mDestinationAlpha;
    u8 mColorUpdateEnabled;
    u8 mAlphaUpdateEnabled;
    u8 mDepthUpdateEnabled;
    u8 reservedBeforeDepthCompareEnabled[0x3];
    u8 mDepthCompareEnabled;
    u8 reservedBeforeDepthCompareFunction[0x3];
    u32 mDepthCompareFunction;
    u32 mAlphaCompareFunction0;
    u32 mAlphaCompareOperation;
    u32 mAlphaCompareFunction1;
    u8 mAlphaCompareReference0;
    u8 mAlphaCompareReference1;
    u8 mDepthCompareBeforeTexture;
    u8 reservedBeforeDitherEnabled[0x1];
    u8 mDitherEnabled;
    u8 reservedBeforeFogRangeAdjustEnabled[0x31];
    u8 mFogRangeAdjustEnabled;
    u8 mFogEnabled;
    u32 mDirtyStateA;
    u32 mGeometryStateDirtyA;
    u8 reservedBeforeTexCoordOffsetDirtyA[0x4];
    u32 mTexCoordOffsetDirtyA;
    u32 mRasterStateDirtyA;
    u32 mChannelStateDirtyA;
    u8 reservedBeforeTevStateDirtyA[0xC];
    u32 mTevStateDirtyA;
    u32 mTevOrderDirtyA;
    u8 reservedBeforeTevIndirectDirtyA[0x14];
    u32 mTevIndirectDirtyA;
    u32 mIndirectStateDirtyA;
    u32 mIndirectOrderDirtyA;
    u32 mIndirectCoordScaleDirtyA;
    u32 mTevKonstColorsDirtyA;
    u8 reservedBeforePixelStateDirtyA[0x10];
    u32 mPixelStateDirtyA;
    u8 reservedBeforeTextureDirtyA[0x2C];
    u32 mTextureDirtyA;
    GStexture *mTexturesA[8];
    u32 mDirtyStateB;
    u32 mGeometryStateDirtyB;
    u8 reservedBeforeTexCoordOffsetDirtyB[0x4];
    u32 mTexCoordOffsetDirtyB;
    u32 mRasterStateDirtyB;
    u32 mChannelStateDirtyB;
    u8 reservedBeforeTevStateDirtyB[0xC];
    u32 mTevStateDirtyB;
    u32 mTevOrderDirtyB;
    u8 reservedBeforeTevIndirectDirtyB[0x14];
    u32 mTevIndirectDirtyB;
    u32 mIndirectStateDirtyB;
    u32 mIndirectOrderDirtyB;
    u32 mIndirectCoordScaleDirtyB;
    u32 mTevKonstColorsDirtyB;
    u8 reservedBeforePixelStateDirtyB[0x10];
    u32 mPixelStateDirtyB;
    u8 reservedBeforeTextureDirtyB[0x2C];
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
