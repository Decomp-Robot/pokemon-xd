#ifndef GS_GFX_H
#define GS_GFX_H

#include <types.h>
#include <GSvec.h>

struct GSgfxVF;

// Layer slot registered by the graphics layer setup.
enum GSgfxLayerID { };

// Surface primitive operation, using the GX primitive encoding.
enum GSgfxPrimType { };

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

// Texture the layers cache and the game sprites draw.
struct GStexture {
    u8 _00[0x07];
    u8 mFormat; // 0x07: nonzero when the texture differs from the cached one.
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

// One GS graphics layer. Only the fields the texture path and the particle
// render module touch are named; the rest of the object stays sized padding.
class GSgfxLayer {
public:
    void setRenderStateAndSelect(GSgfxVF *vf);
    void beginSurface(u16 surface);
    void primitiveType(GSgfxPrimType type);

    void InvalidateState();
    void fog(bool enable)
    {
        mFog = enable;
        mPassState[0].mGpuFlags |= GSGFX_GPU_FOG;
        mPassState[1].mGpuFlags |= GSGFX_GPU_FOG;
        if (enable) {
            mPassState[0].mGpuFlags |= GSGFX_GPU_FOG_RANGE_ADJ;
            mPassState[1].mGpuFlags |= GSGFX_GPU_FOG_RANGE_ADJ;
            if (mFogRangeAdj != 0) {
                mPassState[0].mGpuFlags |= GSGFX_GPU_FOG_ADJ_TABLE;
                mPassState[1].mGpuFlags |= GSGFX_GPU_FOG_ADJ_TABLE;
            }
        }
        mPassState[0].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
        mPassState[1].mFlags |= GSGFX_PASS_FLAG_GPU_STATE;
    }

    u8 _0000[0x04];
    u32 mRenderFlags;      // 0x04: layer commands of the current frame.
    u8 _0008[0x20];
    GSgfxLayerID mLayerID; // 0x28: layer the display list belongs to.
    u8 _002C[0xD18];
    u8 mGroupACache0;      // 0xD44: cached value of the first group's setup.
    u8 mGroupACache1;      // 0xD45: cached value of the first group's setup.
    u8 _0D46[0x0E];
    u8 mProjection[0x3C];  // 0xD54: projection psDispSetProjection is given.
    u32 mGroupBCache;      // 0xD90: cached value of the second group's setup.
    u8 _0D94[0x912];
    u8 mFogRangeAdj;       // 0x16A6: fog range adjustment is set up.
    u8 mFog;               // 0x16A7: the layer draws with fog.
    GSgfxPassState mPassState[2]; // 0x16A8: one state per render pass.
    u8 _1818[0x1998 - 0x1818];
    GSgfxPrimType mPrimitiveType; // 0x1998: primitive op the surface emits.
};

// Which of the layer's two passes a render module is drawing for. The layer
// builds and prerenders pass 0 before pass 1 and keeps a display list and a
// render state per pass; blendMode() switches the current pass to 0 for an
// unblended surface and to 1 for a blended one. The particles belong to the
// second pass.
enum GSgfxLayerPass {
    GSGFX_LAYER_PASS_OPAQUE = 0,
    GSGFX_LAYER_PASS_XLU = 1,
};

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
