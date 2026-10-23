#ifndef GS_GFX_H
#define GS_GFX_H

#include <types.h>

struct GSgfxVF;

// Layer the GS engine routes a primitive to, passed by value to the layer
// entry points. The enumerator names were not recovered.
enum GSgfxLayerID { };

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

// One GS graphics layer. Only the fields the texture path touches are named;
// the rest of the object stays sized padding.
class GSgfxLayer {
public:
    void setRenderStateAndSelect(GSgfxVF *vf);
    void beginSurface(u16 surface);

    u8 _0000[0x173C];
    u32 mTextureStateA;   // 0x173C: dirty bits of the first cached texture.
    GStexture *mTextureA; // 0x1740: first cached texture.
    u8 _1744[0xB0];
    u32 mTextureStateB;   // 0x17F4: dirty bits of the second cached texture.
    GStexture *mTextureB; // 0x17F8: second cached texture.
    u8 _17FC[0x19C];
    u32 mSurfaceID;       // 0x1998: surface the quad is emitted into.
};

#endif
