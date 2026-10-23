#ifndef GS_TEXTURE_H
#define GS_TEXTURE_H

#include <types.h>

// Texture the layers cache and the game sprites draw.
struct GStexture {
    u8 mReservedHeader[0x07];
    u8 mTextureObjectDirty;
};

enum GStextureWrapMode { GS_TEXTURE_WRAP_CLAMP = 0 };
enum GStextureFilterMode { GS_TEXTURE_FILTER_LINEAR = 2 };
enum GStextureMipFilterMode { GS_TEXTURE_MIP_FILTER_NONE = 0 };

extern "C" {
u16 GStextureGetXsize(GStexture* texture);
u16 GStextureGetYsize(GStexture* texture);
void GStextureFree(void *tex);
void GStextureSetWrap(void *tex, s32 wrapU, s32 wrapV);
void GStextureSetFilter(void *tex, s32 minFilter, s32 magFilter, s32 mipFilter);
}

#endif
