#ifndef GSTEXTURE_H
#define GSTEXTURE_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <dolphin/gx/GXStruct.h>
#ifdef __cplusplus
}
#endif

enum GStextureFormat {
    GS_TEXTURE_FORMAT_CI4 = 0, GS_TEXTURE_FORMAT_CI8 = 1, GS_TEXTURE_FORMAT_CI14X2 = 48,
    GS_TEXTURE_FORMAT_I4 = 64, GS_TEXTURE_FORMAT_IA4 = 65, GS_TEXTURE_FORMAT_I8 = 66, GS_TEXTURE_FORMAT_IA8 = 67,
    GS_TEXTURE_FORMAT_RGB565 = 68, GS_TEXTURE_FORMAT_32BIT = 69, GS_TEXTURE_FORMAT_RGB5A3 = 144,
    GS_TEXTURE_FORMAT_A8 = 160, GS_TEXTURE_FORMAT_R8 = 161, GS_TEXTURE_FORMAT_G8 = 162, GS_TEXTURE_FORMAT_B8 = 163,
    GS_TEXTURE_FORMAT_CMPR = 176
};
enum GStextureFilter { GS_TEXTURE_FILTER_NEAREST = 0, GS_TEXTURE_FILTER_LINEAR = 2 };
enum GStextureTlutFormat { GS_TEXTURE_TLUT_FORMAT_NONE = 0, GS_TEXTURE_TLUT_FORMAT_IA8 = 1, GS_TEXTURE_TLUT_FORMAT_RGB565 = 2, GS_TEXTURE_TLUT_FORMAT_RGB5A3 = 3 };

enum GStextureWrapMode { GS_TEXTURE_WRAP_CLAMP = 0, GS_TEXTURE_WRAP_REPEAT = 1, GS_TEXTURE_WRAP_MIRROR = 2 };
enum GStextureMipFilterMode { GS_TEXTURE_MIP_FILTER_NONE = 0, GS_TEXTURE_MIP_FILTER_NEAREST = 1, GS_TEXTURE_MIP_FILTER_LINEAR = 2 };

typedef struct GStexture {
    u16 mXsize;
    u16 mYsize;
    u8 mBitsPerPixel;
    u8 mMipCount;
    u8 mInUse;
    u8 mStateDirty;
    s32 mFormat;
    s32 mTlutFormat;
    s32 mWrapU;
    s32 mWrapV;
    s32 mMinFilter;
    s32 mMagFilter;
    s32 mMipFilter;
    void* mAllocation;
    void* mImage[8];
    void* mTlut;
    u32 mDataSize;
    u16 mLockCount;
    u16 mUnknown;
    GXTexObj mTexObj;
    GXTlutObj mTlutObj;
} GStexture;

#ifdef __cplusplus
extern "C" {
#endif
void GStextureConvertToHW(GStexture* texture);
void GStextureConvertFromHW(GStexture* texture);
GXTexFmt GStextureGetGXformat(GStexture* texture, u8 asTexture);
s32 GStextureGetTlutFormat(GStexture* texture);
s32 GStextureGetFormat(GStexture* texture);
u8 GStextureGetMiplevels(GStexture* texture);
u16 GStextureGetYsize(GStexture* texture);
u16 GStextureGetXsize(GStexture* texture);
void GStextureUnlockImage(GStexture* texture);
void* GStextureLockImage(GStexture* texture, u8 level);
void GStextureSetFilter(GStexture* texture, s32 minFilter, s32 magFilter, s32 mipFilter);
void GStextureSetWrap(GStexture* texture, s32 wrapU, s32 wrapV);
void GStextureFree(GStexture* texture);
int GStextureCreateFromMemory(GStexture* texture, u16 width, u16 height, void* image, s32 format, s32 tlutFormat, u8 mipLevels);
GStexture* GStextureCreate(u16 width, u16 height, s32 format, s32 tlutFormat, u8 mipLevels);
GStexture* GStextureLoad(void* data);
void GStextureInit(u32 count);
s32 GXFmtToGStextureFmt(GXTexFmt format);
#ifdef __cplusplus
}
#endif
#endif
