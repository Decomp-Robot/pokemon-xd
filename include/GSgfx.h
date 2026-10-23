#ifndef GS_GFX_H
#define GS_GFX_H

#include <GSmathTypes.h>
#include <types.h>
#include <GSgfxC.h>
#include <libc/memory.h>
#include <GSvec.h>
#include <GStexture.h>

struct GSgfxVF {
    u32 reserved[0x110];
};

struct GSgfxLight;
enum GSgfxMatrixFlags { GS_MATRIX_INVERSE_TRANSPOSE_DIRTY = 1 };

struct GSgfxMatrixStackEntry {
    u32 mFlags;
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
    void identity()
    {
        memcpy(&mCurrentEntry->mMatrix, mtxIdent, 0x30);
        mCurrentEntry->mFlags |= GS_MATRIX_INVERSE_TRANSPOSE_DIRTY;
    }
    void mult(GSmtx *matrix)
    {
        PSMTXConcat((const f32 (*)[4])&mCurrentEntry->mMatrix,
                    (const f32 (*)[4])matrix,
                    (f32 (*)[4])&mCurrentEntry->mMatrix);
        mCurrentEntry->mFlags |= GS_MATRIX_INVERSE_TRANSPOSE_DIRTY;
    }
    u8 reservedHeader[4];
    GSgfxMatrixStackEntry *mCurrentEntry;
};


union GScolor { u32 rgba; struct { u8 r, g, b, a; } channels; };
enum GSgfxTunitKReg { GSGFX_KREG0, GSGFX_KREG1, GSGFX_KREG2, GSGFX_KREG3 };
// Texel filter a layer loads its texture with (GStextureSetFilter): 0 keeps the
// nearest texel, 2 blends the neighbours linearly.
enum GStextureFilter { GS_TEXTURE_FILTER_NEAREST = 0, GS_TEXTURE_FILTER_LINEAR = 2 };
enum GSgfxCunitAlpha { GSGFX_CUNIT_COLOR, GSGFX_CUNIT_COLOR_AND_ALPHA = 2 };
enum GSgfxCunitSrc { GSGFX_CUNIT_REGISTER, GSGFX_CUNIT_VERTEX_COLOR = 1 };
enum GSgfxLightType { GSGFX_LIGHT_DIRECTIONAL, GSGFX_LIGHT_SPOT, GSGFX_LIGHT_POINT };
enum GSgfxDiffuseFunc { GSGFX_DIFFUSE_NONE, GSGFX_DIFFUSE_SIGN, GSGFX_DIFFUSE_CLAMP };
enum GSgfxTunitCArg {
    GSGFX_TUNIT_C_PREV = 0,
    GSGFX_TUNIT_C_TEXC = 8,
    GSGFX_TUNIT_C_RASC = 10,
    GSGFX_TUNIT_C_ONE = 12,
    GSGFX_TUNIT_C_KONST = 14,
    GSGFX_TUNIT_C_ZERO = 15
};
enum GSgfxTunitAArg {
    GSGFX_TUNIT_A_PREV = 0,
    GSGFX_TUNIT_A_TEXA = 4,
    GSGFX_TUNIT_A_RASA = 5,
    GSGFX_TUNIT_A_KONST = 6,
    GSGFX_TUNIT_A_ZERO = 7
};
enum GSgfxKColorSelection { GSGFX_KCOLOR_K0 = 12, GSGFX_KCOLOR_K1 = 13, GSGFX_KCOLOR_K2 = 14, GSGFX_KCOLOR_K3 = 15 };
enum GSgfxKAlphaSelection { GSGFX_KALPHA_K0 = 28, GSGFX_KALPHA_K1 = 29, GSGFX_KALPHA_K2 = 30, GSGFX_KALPHA_K3 = 31 };
enum GSgfxTunitSentinel { GSGFX_TUNIT_NO_TEXTURE = 255 };
enum GSgfxTunitChannel { GSGFX_TUNIT_CHANNEL_COLOR1A = 1 };
struct GSgfxTunitOrder { u32 texCoord, texMap, channel; };
struct GSgfxTunitInputs { u32 a, b, c, d; };
// Texture-unit operation state stored by each layer stage.
struct GSgfxTunitOpState { u32 operation, bias, scale; u8 clamp; u8 reserved[3]; u32 output; };

enum GSgfxCullType { GSGFX_CULL_NONE = 0, GSGFX_CULL_BACK = 2 , GS_GFX_CULL_NONE = 0, GSGFX_CULL_FRONT = 1, GSGFX_CULL_ALL = 3 };
enum GSgfxClipMode { GSGFX_CLIP_ENABLED = 0 , GS_GFX_CLIP_ENABLED = 0 };
enum GSgfxZCmpFunc { GSGFX_Z_COMPARE_LESS = 1, GSGFX_Z_COMPARE_ALWAYS = 7, GSGFX_Z_COMPARE_LEQUAL = 3, GS_GFX_Z_COMPARE_LEQUAL = 3 };
enum GSgfxACmpFunc { GSGFX_A_COMPARE_ALWAYS = 7 , GS_GFX_A_COMPARE_ALWAYS = 7 };
enum GSgfxAlphaOp { GSGFX_ALPHA_AND = 0 , GS_GFX_ALPHA_AND = 0 };
enum GSgfxTunitOp { GSGFX_TUNIT_OP_ADD = 0 };
enum GSgfxTunitBias { GSGFX_TUNIT_BIAS_ZERO = 0 };
enum GSgfxTunitScale { GSGFX_TUNIT_SCALE_ONE = 0 };
enum GSgfxTunitReg { GSGFX_TUNIT_REG_PREV = 0 };


// Layer selected for a primitive submission.
#include <GSgfxTypes.h>

// Primitive operation emitted by a surface.
enum GSgfxPrimType { GSGFX_PRIM_TRIANGLE_FAN = 0xA0 , GSGFX_QUADS = 0x80, GS_GFX_PRIM_TRIANGLE_STRIP = 0x98 };
enum GSgfxLogicOp { GSGFX_LOGIC_NOOP = 5 };
enum GSgfxBlendType { GSGFX_BLEND_NONE, GSGFX_BLEND_BLEND, GSGFX_BLEND_LOGIC, GSGFX_BLEND_SUBTRACT , GS_GFX_BLEND_NONE = 0, GS_GFX_BLEND_ALPHA = 1 };
enum GSgfxBlendFactor { GSGFX_BLEND_ZERO, GSGFX_BLEND_ONE, GSGFX_BLEND_SRC_COLOR, GSGFX_BLEND_INV_SRC_COLOR, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, GSGFX_BLEND_DST_ALPHA, GSGFX_BLEND_INV_DST_ALPHA , GS_GFX_FACTOR_ZERO = 0, GS_GFX_FACTOR_ONE = 1 };
enum GSgfxLayerPass {
    GSGFX_LAYER_PASS_OPAQUE = 0,
    GSGFX_LAYER_PASS_XLU = 1, GS_GFX_PASS_OPAQUE = 0, GS_GFX_PASS_TRANSLUCENT = 1 };
struct GSgfxDrawPass { u8 _00[0xBC]; };

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

#define GSgfxFifo (*(PPCWGPipe *)0xCC008000)
#define GXWGFifo (*(volatile PPCWGPipe *)0xCC008000)

// Texture the layers cache and the game sprites draw.
struct GStexture {
    u8 _00[0x07];
    u8 mStateDirty; // 0x07: nonzero when cached sampler state must be updated.
};

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
    GSGFX_PASS_FLAG_TUNIT = 0x8,
    GSGFX_PASS_FLAG_KCOLOR = 0x10,
    GSGFX_PASS_FLAG_GPU_STATE = 0x40,
    GSGFX_PASS_FLAG_INDIRECT = 0x80,
};

// Dirty bits the particle module sets in the pass state's first GX group.
enum GSgfxPassGroupAFlag {
    GSGFX_PASS_GROUP_A_PARTICLES = 0x1,
};

// Dirty bits the particle module sets in the pass state's second GX group.
enum GSgfxPassGroupBFlag {
    GSGFX_PASS_GROUP_B_PARTICLES = 0x4,
    GSGFX_PASS_GROUP_B_CULL = 0x2,
};

enum GSgfxLightStateFlag { GSGFX_LIGHT_STATE_CHANNEL_COUNT = 1, GSGFX_LIGHT_STATE_POSITION = 2, GSGFX_LIGHT_STATE_DIRECTION = 4, GSGFX_LIGHT_STATE_LIGHTS = 16 };
enum GSgfxTunitCountFlag { GSGFX_TUNIT_COUNT_DIRTY = 1 };

// Dirty bits of the GPU state group (GSgfxPassState +0x64).
enum GSgfxGpuFlag {
    GSGFX_GPU_BLEND = 0x1,
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
    u32 mGeometryFlags;  // 0x04
    u8 _08[0x04];
    u32 mGroupAFlags;    // 0x0C: dirty bits of the pass state's first group.
    u32 mGroupBFlags;    // 0x10: dirty bits of the pass state's second group.
    u32 mLightFlags;     // 0x14
    u8 _18[0x04];
    u32 mAmbientFlags;   // 0x1C
    u32 mMaterialFlags;  // 0x20
    u32 mTunitCountFlags;// 0x24
    u32 mTunitOrderFlags;// 0x28
    u32 mColorInputFlags;// 0x2C
    u32 mAlphaInputFlags;// 0x30
    u32 mColorOpFlags;   // 0x34
    u32 mAlphaOpFlags;   // 0x38
    u8 _3C[4];
    u32 mIndirectOperationFlags; // 0x40
    u32 mIndirectStageFlags; // 0x44
    u32 mIndirectOrderFlags; // 0x48
    u32 mIndirectScaleFlags; // 0x4C
    u32 mKColorFlags;    // 0x50
    u32 mKColorSelFlags; // 0x54
    u32 mKAlphaSelFlags; // 0x58
    u8 _5C[0x08];
    u32 mGpuFlags;       // 0x64: dirty bits of the GPU state setup.
    u8 _68[0x2C];
    u32 mTextureState;   // 0x94: bits of the texture slots the pass loaded.
    GStexture *mTextures[8]; // 0x98: textures cached by this render pass.

};

enum GSgfxDLFlags {
    GS_DISPLAY_LIST_ALLOCATED = 1,
    GS_DISPLAY_LIST_DIRTY = 2,
};

enum { GS_DISPLAY_LIST_HEADER_BYTES = 3 };
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

enum GSgfxProjectionType { GS_PROJECTION_PERSPECTIVE = 0, GS_PROJECTION_ORTHOGRAPHIC = 1 };

struct GSgfxProjectionState {
    GSgfxProjectionType mType;
    f32 mParameters[6];
    u16 mScissorLeft;
    u16 mScissorTop;
    u16 mScissorWidth;
    u16 mScissorHeight;
    f32 mViewportLeft;
    f32 mViewportTop;
    f32 mViewportCentreX;
    f32 mViewportCentreY;
};
enum GSgfxGeometryFlag { GSGFX_GEOMETRY_TEXGEN_COUNT = 1, GSGFX_GEOMETRY_PROJECTION = 0x20 };

enum GSgfxLayerOffset { GS_LAYER_INSERT_BEFORE = 1, GS_LAYER_INSERT_AFTER = 2 };


enum GSgfxTgenFunc { GS_GFX_TGEN_MTX3X4 = 0, GS_GFX_TGEN_MTX2X4 = 1 };

enum GSgfxTgenSrc { GS_GFX_TGEN_SRC_POSITION = 0, GS_GFX_TGEN_SRC_TEX0 = 4 };

enum GSgfxTunitFunc { GS_GFX_TUNIT_TEXTURE_REPLACE = 3 };

enum GSgfxIndTexMtxID { GS_GFX_IND_MTX0 = 1 };

enum GSgfxLayerType { GS_GFX_LAYER_FRAMEBUFFER_COPY = 4 };

class GSgfxLayerConfig {
public:
    static void Remove(GSgfxLayerID id);
    static void Move(GSgfxLayerID from, GSgfxLayerID to, GSgfxLayerOffset offset);
    /* Registers a layer and hands back the id it was filed under, which the
     * callers pass straight on to GSgfxLayerFindByID. */
    static GSgfxLayerID Add(char *name, unsigned long pass0FifoSize, unsigned long pass1FifoSize,
                   unsigned long matrixStackCapacity, GSgfxLayerType type, bool clearColor, bool clearAlpha,
                   bool clearDepth, GSgfxLayerID relativeLayerID, GSgfxLayerOffset offset);
};

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

// Graphics state and draw commands for one layer.
class GSgfxLayer {
public:
    void SetClearFlags(bool color, bool alpha, bool depth);
    void SetTextureCopy(GStexture* texture, bool flag, u8* image);
    void perspectiveProjection(const f32 matrix[4][4])
    {
        mProjectionState.mType = GS_PROJECTION_PERSPECTIVE;
        mProjectionState.mParameters[0] = matrix[0][0];
        mProjectionState.mParameters[1] = matrix[0][2];
        mProjectionState.mParameters[2] = matrix[1][1];
        mProjectionState.mParameters[3] = matrix[1][2];
        mProjectionState.mParameters[4] = matrix[2][2];
        mProjectionState.mParameters[5] = matrix[2][3];
    }
    void orthographicProjection(const f32 matrix[4][4]) {
        mProjectionState.mType = GS_PROJECTION_ORTHOGRAPHIC;
        mProjectionState.mParameters[0] = matrix[0][0];
        mProjectionState.mParameters[1] = matrix[0][3];
        mProjectionState.mParameters[2] = matrix[1][1];
        mProjectionState.mParameters[3] = matrix[1][3];
        mProjectionState.mParameters[4] = matrix[2][2];
        mProjectionState.mParameters[5] = matrix[2][3];
    }
    void projectionDepthRange(f32 nearDistance, f32 farDistance) {
        mProjectionNear = nearDistance;
        mProjectionFar = farDistance;
        if (mFog != 0) {
            if (mFogRangeAdj != 0) {
                mPassState[0].mGpuFlags |= GSGFX_GPU_FOG_ADJ_TABLE;
                mPassState[1].mGpuFlags |= GSGFX_GPU_FOG_ADJ_TABLE;
            }
            mPassState[0].mGpuFlags |= GSGFX_GPU_FOG;
            mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
            mPassState[1].mGpuFlags |= GSGFX_GPU_FOG;
            mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        }
        mPassState[0].mGeometryFlags |= GSGFX_GEOMETRY_PROJECTION;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
        mPassState[1].mGeometryFlags |= GSGFX_GEOMETRY_PROJECTION;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
    }
    void texCoordGeneratorCount(u8 count) {
        mTexGenCount = count;
        mPassState[0].mGeometryFlags |= GSGFX_GEOMETRY_TEXGEN_COUNT;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
        mPassState[1].mGeometryFlags |= GSGFX_GEOMETRY_TEXGEN_COUNT;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
    }
    void FlushState();
    void endSurface();
    void beginSurface(u16 count);
    void beginSurface(GSgfxPrimType type, GSgfxVF *format, u16 count) {
        mPrimitiveType = type;
        setRenderStateAndSelect(format);
        beginSurface(count);
    }
    void SetRenderPass(GSgfxLayerPass pass) {
        if (mCurrentPass != pass) {
            mCurrentPass = pass;
            mDrawPass = &mDrawPasses[mCurrentPass];
        }
    }
    void blendMode(GSgfxBlendType type, GSgfxBlendFactor src, GSgfxBlendFactor dst, bool selectPass) {
        if (type == GSGFX_BLEND_NONE) {
            if (selectPass) SetRenderPass(GSGFX_LAYER_PASS_OPAQUE);
            mBlendType = GSGFX_BLEND_NONE;
            mBlendSrc = GSGFX_BLEND_ONE;
            mBlendDst = GSGFX_BLEND_ONE;
            mBlendLogic = GSGFX_LOGIC_NOOP;
        } else {
            if (selectPass) SetRenderPass(GSGFX_LAYER_PASS_XLU);
            mBlendType = type;
            mBlendSrc = src;
            mBlendDst = dst;
            mBlendLogic = GSGFX_LOGIC_NOOP;
        }
        mPassState[0].mGpuFlags |= GSGFX_GPU_BLEND;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mGpuFlags |= GSGFX_GPU_BLEND;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    }


    void tunitKRegSel(u8 stage, GSgfxTunitKReg reg);
    void cunitChannelGen(u8 channel, GSgfxCunitAlpha alpha,
                         GSgfxCunitSrc ambient, GSgfxCunitSrc material,
                         unsigned long lightMask, GSgfxLight **lights,
                         GSgfxLightType type, GSgfxDiffuseFunc diffuse);
    void drawCube();
    void drawCylinder(u8 slices);
    void drawDodecaedron();
    void drawIcosahedron();
    void drawOctahedron();
    void drawSphere(u8 slices);
    void drawSphere(u8 stacks, u8 slices);
    void drawTorus(f32 radius, u8 stacks, u8 slices);
    void cullMode(GSgfxCullType mode) {
        mCullMode = mode;
        mPassState[0].mGroupBFlags |= GSGFX_PASS_GROUP_B_CULL;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
        mPassState[1].mGroupBFlags |= GSGFX_PASS_GROUP_B_CULL;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
    }
    void cunitCount(u8 count) {
        mCunitCount = count;
        mPassState[0].mLightFlags |= GSGFX_LIGHT_STATE_CHANNEL_COUNT;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_LIGHT;
        mPassState[1].mLightFlags |= GSGFX_LIGHT_STATE_CHANNEL_COUNT;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_LIGHT;
    }
    void tunitCount(u8 count) {
        mTunitCount = count;
        mPassState[0].mTunitCountFlags |= GSGFX_TUNIT_COUNT_DIRTY;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_TUNIT;
        mPassState[1].mTunitCountFlags |= GSGFX_TUNIT_COUNT_DIRTY;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    }
    void tunitMapping(u8 stage, u8 coord, u8 map, u8 channel) {
        mTunitOrder[stage].texCoord = coord;
        mTunitOrder[stage].texMap = map;
        if (channel != GSGFX_TUNIT_NO_TEXTURE)
            mTunitOrder[stage].channel = channel + 4;
        else
            mTunitOrder[stage].channel = GSGFX_TUNIT_NO_TEXTURE;
        mPassState[0].mTunitOrderFlags |= 1 << stage;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_TUNIT;
        mPassState[1].mTunitOrderFlags |= 1 << stage;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    }
    void tunitColorArg(u8 stage, GSgfxTunitCArg a, GSgfxTunitCArg b, GSgfxTunitCArg c, GSgfxTunitCArg d) {
        mTunitColorInputs[stage].a = a;
        mTunitColorInputs[stage].b = b;
        mTunitColorInputs[stage].c = c;
        mTunitColorInputs[stage].d = d;
        mPassState[0].mColorInputFlags |= 1 << stage;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_TUNIT;
        mPassState[1].mColorInputFlags |= 1 << stage;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    }
    void tunitAlphaArg(u8 stage, GSgfxTunitAArg a, GSgfxTunitAArg b, GSgfxTunitAArg c, GSgfxTunitAArg d) {
        mTunitAlphaInputs[stage].a = a;
        mTunitAlphaInputs[stage].b = b;
        mTunitAlphaInputs[stage].c = c;
        mTunitAlphaInputs[stage].d = d;
        mPassState[0].mAlphaInputFlags |= 1 << stage;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_TUNIT;
        mPassState[1].mAlphaInputFlags |= 1 << stage;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    }
    void tunitColorOp(u8 stage, GSgfxTunitOp operation, GSgfxTunitBias bias, GSgfxTunitScale scale, bool clamp, GSgfxTunitReg output) {
        mTunitColorOp[stage].operation = operation;
        mTunitColorOp[stage].bias = bias;
        mTunitColorOp[stage].scale = scale;
        mTunitColorOp[stage].clamp = clamp;
        mTunitColorOp[stage].output = output;
        mPassState[0].mColorOpFlags |= 1 << stage;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_TUNIT;
        mPassState[1].mColorOpFlags |= 1 << stage;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    }
    void tunitAlphaOp(u8 stage, GSgfxTunitOp operation, GSgfxTunitBias bias, GSgfxTunitScale scale, bool clamp, GSgfxTunitReg output) {
        mTunitAlphaOp[stage].operation = operation;
        mTunitAlphaOp[stage].bias = bias;
        mTunitAlphaOp[stage].scale = scale;
        mTunitAlphaOp[stage].clamp = clamp;
        mTunitAlphaOp[stage].output = output;
        mPassState[0].mAlphaOpFlags |= 1 << stage;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_TUNIT;
        mPassState[1].mAlphaOpFlags |= 1 << stage;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    }
    void tunitKColorSel(u8 stage, u32 select) {
        mTunitKColorSel[stage] = select;
        mPassState[0].mKColorSelFlags |= 1 << stage;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_KCOLOR;
        mPassState[1].mKColorSelFlags |= 1 << stage;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_KCOLOR;
    }
    void tunitKAlphaSel(u8 stage, u32 select) {
        mTunitKAlphaSel[stage] = select;
        mPassState[0].mKAlphaSelFlags |= 1 << stage;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_KCOLOR;
        mPassState[1].mKAlphaSelFlags |= 1 << stage;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_KCOLOR;
    }
    void tunitKColor(u8 reg, GScolor color) {
        mTunitKColor[reg].rgba = color.rgba;
        mPassState[0].mKColorFlags |= 1 << reg;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_KCOLOR;
        mPassState[1].mKColorFlags |= 1 << reg;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_KCOLOR;
    }
    void cunitColor(u8 channel, GScolor ambient, GScolor material) {
        mAmbientColor[channel].rgba = ambient.rgba;
        mMaterialColor[channel].rgba = material.rgba;
        mPassState[0].mAmbientFlags |= 1 << channel;
        mPassState[0].mMaterialFlags |= 1 << channel;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_LIGHT;
        mPassState[1].mAmbientFlags |= 1 << channel;
        mPassState[1].mMaterialFlags |= 1 << channel;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_LIGHT;
    }
    void cunitLights(u8 count, GSgfxLight **lights) {
        mLightCount = count;
        for (u8 i = 0; i < 8; ++i) {
            if (i < count) mLights[i] = lights[i];
            else mLights[i] = 0;
        }
        mPassState[0].mLightFlags |= GSGFX_LIGHT_STATE_LIGHTS;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_LIGHT;
        mPassState[1].mLightFlags |= GSGFX_LIGHT_STATE_LIGHTS;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_LIGHT;
    }

    void tgenCount(u8 count) {
        mTexGenCount = count;
        mPassState[0].mGeometryFlags |= GSGFX_GEOMETRY_TEXGEN_COUNT;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
        mPassState[1].mGeometryFlags |= GSGFX_GEOMETRY_TEXGEN_COUNT;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
    }
    void channelCount(u8 count) {
        mCunitCount = count;
        mPassState[0].mLightFlags |= GSGFX_LIGHT_STATE_CHANNEL_COUNT;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_LIGHT;
        mPassState[1].mLightFlags |= GSGFX_LIGHT_STATE_CHANNEL_COUNT;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_LIGHT;
    }
    void indTexCount(u8 count) {
        mIndirectStageCount = count;
        mPassState[0].mIndirectStageFlags |= 1;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_INDIRECT;
        mPassState[1].mIndirectStageFlags |= 1;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_INDIRECT;
    }
    void destinationAlpha(bool on, u8 alpha) {
        mDestinationAlphaEnabled = on;
        mDestinationAlpha = alpha;
        mPassState[0].mGpuFlags |= 0x2;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mGpuFlags |= 0x2;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    }
    void dither(bool on) {
        mDitherEnabled = on;
        mPassState[0].mGpuFlags |= 0x20;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mGpuFlags |= 0x20;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    }
    void clipMode(GSgfxClipMode mode) {
        mClipMode = mode;
        mPassState[0].mGroupBFlags |= 0x4;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
        mPassState[1].mGroupBFlags |= 0x4;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
    }
    void coplanar(bool on) {
        mCoplanarEnabled = on;
        mPassState[0].mGroupBFlags |= 0x8;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
        mPassState[1].mGroupBFlags |= 0x8;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_B;
    }
    void pixelMode(bool color, bool alpha, bool depth) {
        mColorUpdateEnabled = color;
        mAlphaUpdateEnabled = alpha;
        mDepthUpdateEnabled = depth;
        mPassState[0].mGpuFlags |= 0x4;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mGpuFlags |= 0x4;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    }
    void zCompare(bool enable, GSgfxZCmpFunc function) {
        mDepthCompareEnabled = enable;
        mDepthCompareFunction = function;
        mPassState[0].mGpuFlags |= 0x8;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mGpuFlags |= 0x8;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    }
    void alphaCompare(bool enable, GSgfxACmpFunc function0, u8 reference0,
                      GSgfxAlphaOp operation, GSgfxACmpFunc function1, u8 reference1) {
        if (enable) {
            mAlphaCompareFunction0 = function0;
            mAlphaCompareReference0 = reference0;
            mAlphaCompareOperation = operation;
            mAlphaCompareFunction1 = function1;
            mAlphaCompareReference1 = reference1;
            mDepthCompareBeforeTexture = 0;
        } else {
            mAlphaCompareFunction0 = GSGFX_A_COMPARE_ALWAYS;
            mAlphaCompareReference0 = 0;
            mAlphaCompareOperation = GSGFX_ALPHA_AND;
            mAlphaCompareFunction1 = GSGFX_A_COMPARE_ALWAYS;
            mAlphaCompareReference1 = 0;
            mDepthCompareBeforeTexture = 1;
        }
        mPassState[0].mGpuFlags |= 0x10;
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mGpuFlags |= 0x10;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    }
    void fog(bool on) {
        mFog = on;
        mPassState[0].mGpuFlags |= GSGFX_GPU_FOG;
        mPassState[1].mGpuFlags |= GSGFX_GPU_FOG;
        if (on) {
            mPassState[0].mGpuFlags |= GSGFX_GPU_FOG_RANGE_ADJ;
            mPassState[1].mGpuFlags |= GSGFX_GPU_FOG_RANGE_ADJ;
            if (mFogRangeAdj) {
                mPassState[0].mGpuFlags |= GSGFX_GPU_FOG_ADJ_TABLE;
                mPassState[1].mGpuFlags |= GSGFX_GPU_FOG_ADJ_TABLE;
            }
        }
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    }
    void texSelect(u8 textureMap, GStexture *texture) {
        if (mPassState[0].mTextures[textureMap] != texture ||
            mPassState[1].mTextures[textureMap] != texture || texture->mStateDirty != 0) {
            mPassState[0].mTextures[textureMap] = texture;
            mPassState[0].mTextureState |= 1 << textureMap;
            mPassState[1].mTextures[textureMap] = texture;
            mPassState[1].mTextureState |= 1 << textureMap;
        }
    }
    void InvalidateState();
    void setRenderStateAndSelect(GSgfxVF *vf);
    void primitiveType(GSgfxPrimType type);
    void dlDraw(GSgfxDL *displayList);
    bool IsLayerVisible(void);
    void SetLayerVisibility(bool on);
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
    GSgfxProjectionState mProjectionState; // 0xD54
    u8 mCoplanarEnabled; // 0xD88
    u8 _0D89[3];
    u32 mCullMode;       // 0xD8C
    GSgfxClipMode mClipMode; // 0xD90
    u8 mLightCount;        // 0xD94
    u8 _0D95[3];
    GSgfxLight *mLights[8];// 0xD98
    u8 mCunitCount;        // 0xDB8
    u8 _0DB9[0xE9C - 0xDB9];
    GScolor mAmbientColor[2]; // 0xE9C
    GScolor mMaterialColor[2];// 0xEA4
    u8 mTunitCount;        // 0xEAC
    u8 _0EAD[3];
    GSgfxTunitOrder mTunitOrder[16]; // 0xEB0
    GSgfxTunitInputs mTunitColorInputs[16]; // 0xF70
    GSgfxTunitInputs mTunitAlphaInputs[16]; // 0x1070
    GSgfxTunitOpState mTunitColorOp[16]; // 0x1170
    GSgfxTunitOpState mTunitAlphaOp[16]; // 0x12B0
    u8 _13F0[0x30];
    u32 mTevIndirectOperation0;
    u8 mTevIndirectStage0;
    u8 mTevWarpSignedOffset0;
    u8 mTevWarpReplaceMode0;
    u8 _1427;
    u32 mTevWarpMatrix0;
    u8 _142C[0x84];
    u8 mIndirectStageCount; // 0x14B0
    u8 _14B1[3];
    u32 mIndirectTexCoord0;
    u32 mIndirectTextureMap0;
    u8 _14BC[0x18];
    u32 mIndirectCoordScaleS0;
    u32 mIndirectCoordScaleT0;
    u8 _14DC[0x18];
    GScolor mTunitKColor[4]; // 0x14F4
    u32 mTunitKColorSel[16]; // 0x1504
    u32 mTunitKAlphaSel[16]; // 0x1544
    u8 _1584[0x1644 - 0x1584];
    GSgfxBlendType mBlendType; // 0x1644
    GSgfxBlendFactor mBlendSrc; // 0x1648
    GSgfxBlendFactor mBlendDst; // 0x164C
    GSgfxLogicOp mBlendLogic; // 0x1650
    u8 mDestinationAlphaEnabled; // 0x1654
    u8 mDestinationAlpha; // 0x1655
    u8 mColorUpdateEnabled; // 0x1656
    u8 mAlphaUpdateEnabled; // 0x1657
    u8 mDepthUpdateEnabled; // 0x1658
    u8 _1659[3];
    u8 mDepthCompareEnabled; // 0x165C
    u8 _165D[3];
    GSgfxZCmpFunc mDepthCompareFunction; // 0x1660
    GSgfxACmpFunc mAlphaCompareFunction0; // 0x1664
    GSgfxAlphaOp mAlphaCompareOperation; // 0x1668
    GSgfxACmpFunc mAlphaCompareFunction1; // 0x166C
    u8 mAlphaCompareReference0; // 0x1670
    u8 mAlphaCompareReference1; // 0x1671
    u8 mDepthCompareBeforeTexture; // 0x1672
    u8 _1673;
    u8 mDitherEnabled; // 0x1674
    u8 _1675[0x0F];
    f32 mProjectionNear; // 0x1684
    f32 mProjectionFar; // 0x1688
    u8 _168C[0x1A];
    u8 mFogRangeAdj;       // 0x16A6: fog range adjustment is set up.
    u8 mFog;               // 0x16A7: the layer draws with fog.
    GSgfxPassState mPassState[2]; // 0x16A8: one state per render pass.
    GSgfxDrawPass mDrawPasses[2]; // 0x1818
    GSgfxDrawPass *mDrawPass; // 0x1990
    GSgfxLayerPass mCurrentPass; // 0x1994
    GSgfxPrimType mPrimitiveType; // 0x1998: primitive op the surface emits.
    GSgfxVF *mVertexFormat; // 0x199C
    GSgfxMatrixStack *mMatrixStack; // 0x19A0
};

struct GSgfxLight {
    GSgfxLight();
    void MakeDiff(GSgfxColAu8 *color, GSvec *position, bool attenuation);
    void MakeSpec(GSgfxColAu8 *color, GSvec *direction, f32 shininess, GSvec *position);
    u16 mFlags;
    u8 _02[2];
    GSgfxLightType mType;
    u8 _08[4];
    GSvec mPosition;
    GSvec mDirection;
    u8 _24[0x80 - 0x24];

    void position(GSvec *pos) {
        switch (mType) {
        case GSGFX_LIGHT_POINT: mPosition = *pos; break;
        case GSGFX_LIGHT_SPOT: mPosition = *pos; break;
        case GSGFX_LIGHT_DIRECTIONAL: return;
        }
        mFlags |= GSGFX_LIGHT_STATE_POSITION;
    }
    void direction(GSvec *dir) {
        switch (mType) {
        case GSGFX_LIGHT_DIRECTIONAL:
        case GSGFX_LIGHT_SPOT:
            mDirection = *dir;
            mFlags |= GSGFX_LIGHT_STATE_DIRECTION;
            break;
        }
    }
};
extern "C" GSgfxVF *GSgfxVFGetPredef(u32 format);
extern "C" GSgfxVF *GSgfxVFBuildTemp(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
void GSprimSetup2DRender(GSgfxLayer *layer);
extern "C" GSgfxLayer *GSgfxLayerFindByID(GSgfxLayerID id);

// Which of the layer's two passes a render module is drawing for. The layer
// builds and prerenders pass 0 before pass 1 and keeps a display list and a
// render state per pass; blendMode() switches the current pass to 0 for an
// unblended surface and to 1 for a blended one. The particles belong to the
// second pass.


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
s32 GSgfxGPUSetMode(s32 mode);
void GSgfxRender(void);
void GSgfxDLPrimitiveType(u32 primitive);
GSgfxVF* GSgfxVFBuildTemp(s32 positionComponents, s32 positionType,
                         s32 hasNormal, s32 normalComponents,
                         s32 colorCount, s32 colorType,
                         s32 texCoordCount, s32 texCoordComponents,
                         s32 texCoordType);
}


extern "C" {
u8 GSgfxVideoGetVsyncRate(void);
void GSgfxVideoSetLockFrameRate(long frameRateLock);
}


extern "C" {
void GSgfxLayerSetActive(GSgfxLayerID layer);

}


extern "C" f32 GSgfxVideoGetLastRenderTime(void);
void GSgfxRenderWaitForDrawComplete();
typedef void (*GSgfxParseVertexCB)(unsigned long attributeFlags, void *attributeData, void *userContext);
typedef void (*GSgfxParseSurfaceCB)(GSgfxPrimType type, u16 count, unsigned long attributeMask, void *userContext);
#define GS_GFX_PARSE_ATTRIBUTE_PNMTXIDX 0x1u
#define GS_GFX_PARSE_ATTRIBUTE_POSITION 0x2u
#define GS_GFX_POSITION_MATRIX_ROWS 3
extern "C" {
void GSgfxParseDisplayList(void *vtxAttr, void *dl, u32 size, const void *callbacks, void *ctx);
u32 GSgfxPerfbarCPUEntry(u32 id);
void GSgfxVideoGetEFBSize(u16 *width, u16 *height);
GSgfxLayer *GSgfxLayerFindByName(const char *name);
void *GSgfxLayerGetFrameBufferTexture(void);
void *GSgfxVFCreate(void *vf);
void GSgfxVFFree(void *vf);
void GSgfxVFInit(void *vf);
void GSgfxVFSetElement(void *vf, s32 attribute, u32 componentCount, u32 componentFormat, u8 fractionalBits);
void GSgfxVFSetArray(void *vf, s32 attribute, u32 inputType, void *arrayBase, u32 byteStride, s32 arrayMetadata);
}
#endif
