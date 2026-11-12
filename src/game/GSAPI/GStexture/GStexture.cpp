#include <GStexture.h>
#include <GSgfx.h>
#include <GSmem.h>
#include <GSlog.h>
#include <libc/memory.h>
#include <dolphin/gx/GXTexture.h>
#include <dolphin/os/OSCache.h>

static GStexture* _textureStructList;
static u32 _textureStructCount;

enum { GS_TEXTURE_TLUT_BITS_PER_ENTRY = 16 };

/* One row of 4x4 texel tiles for a 16-bit texture up to 640 texels wide. */
static u16 _texXformBuf[2560];

/* Rearrange a linear 16-bit image into the GX 4x4 tiled layout, in place. */
extern "C" void GStextureConvertToHW(GStexture* tex)
{
    u16* image;
    u16* src;
    u16* dst;
    s32 x;
    s32 y;
    s32 row;
    s32 col;

    if (tex->mFormat != GS_TEXTURE_FORMAT_RGB565 && tex->mFormat != GS_TEXTURE_FORMAT_RGB5A3) {
        return;
    }
    if ((tex->mXsize & 2) || (tex->mYsize & 2)) {
        return;
    }

    tex->mLockCount++;
    image = (u16*)tex->mImage[0];
    for (y = 0; y < tex->mYsize; y += 4) {
        dst = _texXformBuf;
        for (x = 0; x < tex->mXsize; x += 4) {
            src = &image[y * tex->mXsize] + x;
            for (row = 0; row < 4; row++) {
                for (col = 0; col < 4; col++) {
                    *dst++ = *src++;
                }
                src = src + tex->mXsize - 4;
            }
        }
        memcpy(&image[y * tex->mXsize], _texXformBuf, tex->mXsize * 4 * sizeof(u16));
    }
    DCStoreRange(tex->mImage[0], tex->mDataSize);
    GXInvalidateTexAll();
    tex->mLockCount--;
}

/* Rearrange a GX 4x4 tiled 16-bit image back into linear rows, in place. */
extern "C" void GStextureConvertFromHW(GStexture* tex)
{
    u16* image;
    u16* src;
    u16* dst;
    s32 x;
    s32 y;
    s32 row;
    s32 col;

    if (tex->mFormat != GS_TEXTURE_FORMAT_RGB565 && tex->mFormat != GS_TEXTURE_FORMAT_RGB5A3) {
        return;
    }
    if ((tex->mXsize & 2) || (tex->mYsize & 2)) {
        return;
    }

    tex->mLockCount++;
    image = (u16*)tex->mImage[0];
    for (y = 0; y < tex->mYsize; y += 4) {
        src = &image[y * tex->mXsize];
        for (x = 0; x < tex->mXsize; x += 4) {
            dst = &_texXformBuf[x];
            for (row = 0; row < 4; row++) {
                for (col = 0; col < 4; col++) {
                    *dst++ = *src++;
                }
                dst = dst + tex->mXsize - 4;
            }
        }
        memcpy(&image[y * tex->mXsize], _texXformBuf, tex->mXsize * 4 * sizeof(u16));
    }
    DCStoreRange(tex->mImage[0], tex->mDataSize);
    GXInvalidateTexAll();
    tex->mLockCount--;
}

extern "C" GXTexFmt GStextureGetGXformat(GStexture* tex, u8 asTexture)
{
    switch (tex->mFormat) {
    case GS_TEXTURE_FORMAT_CI4:    return (GXTexFmt)GX_TF_C4;
    case GS_TEXTURE_FORMAT_CI8:    return (GXTexFmt)GX_TF_C8;
    case GS_TEXTURE_FORMAT_CI14X2: return (GXTexFmt)GX_TF_C14X2;
    case GS_TEXTURE_FORMAT_I4:     return GX_TF_I4;
    case GS_TEXTURE_FORMAT_IA4:    return GX_TF_IA4;
    case GS_TEXTURE_FORMAT_I8:     return GX_TF_I8;
    case GS_TEXTURE_FORMAT_IA8:    return GX_TF_IA8;
    case GS_TEXTURE_FORMAT_RGB565: return GX_TF_RGB565;
    case GS_TEXTURE_FORMAT_32BIT:  return GX_TF_RGBA8;
    case GS_TEXTURE_FORMAT_RGB5A3: return GX_TF_RGB5A3;
    case GS_TEXTURE_FORMAT_CMPR:   return GX_TF_CMPR;
    case GS_TEXTURE_FORMAT_A8:     return asTexture ? GX_TF_I8 : GX_TF_A8;
    case GS_TEXTURE_FORMAT_R8:     return asTexture ? GX_TF_I8 : GX_CTF_R8;
    case GS_TEXTURE_FORMAT_G8:     return asTexture ? GX_TF_I8 : GX_CTF_G8;
    case GS_TEXTURE_FORMAT_B8:     return asTexture ? GX_TF_I8 : GX_CTF_B8;
    }
    return (GXTexFmt)-1;
}

extern "C" s32 GStextureGetTlutFormat(GStexture* tex) { return tex->mTlutFormat; }
extern "C" s32 GStextureGetFormat(GStexture* tex) { return tex->mFormat; }
extern "C" u8 GStextureGetMiplevels(GStexture* tex) { return tex->mMipCount - 1; }
extern "C" u16 GStextureGetYsize(GStexture* tex) { return tex->mYsize; }
extern "C" u16 GStextureGetXsize(GStexture* tex) { return tex->mXsize; }
extern "C" void GStextureUnlockImage(GStexture* tex)
{
    DCStoreRange(tex->mImage[0], tex->mDataSize);
    GXInvalidateTexAll();
    tex->mLockCount--;
}
extern "C" void* GStextureLockImage(GStexture* tex, u8 level)
{
    if (level >= 8) return NULL;
    tex->mLockCount++;
    return tex->mImage[level];
}

extern "C" void GStextureSetFilter(GStexture* tex, s32 minFilter, s32 magFilter, s32 mipFilter)
{
    GXTexFilter gxMinFilter;

    tex->mMinFilter = minFilter;
    tex->mMagFilter = magFilter;
    tex->mMipFilter = mipFilter;

    switch (tex->mMipFilter) {
    case GS_TEXTURE_MIP_FILTER_NONE:
        if (tex->mMinFilter == GS_TEXTURE_FILTER_LINEAR) {
            gxMinFilter = GX_LINEAR;
        } else {
            gxMinFilter = GX_NEAR;
        }
        break;
    case GS_TEXTURE_MIP_FILTER_NEAREST:
        if (tex->mMinFilter == GS_TEXTURE_FILTER_LINEAR) {
            gxMinFilter = GX_LIN_MIP_NEAR;
        } else {
            gxMinFilter = GX_NEAR_MIP_NEAR;
        }
        break;
    case GS_TEXTURE_MIP_FILTER_LINEAR:
        if (tex->mMinFilter == GS_TEXTURE_FILTER_LINEAR) {
            gxMinFilter = GX_LIN_MIP_LIN;
        } else {
            gxMinFilter = GX_NEAR_MIP_LIN;
        }
        break;
    }
    GXInitTexObjLOD(&tex->mTexObj, gxMinFilter,
                    tex->mMagFilter == GS_TEXTURE_FILTER_LINEAR ? GX_LINEAR : GX_NEAR,
                    0.0f, tex->mMipCount - 1, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    tex->mStateDirty = TRUE;
}

extern "C" void GStextureSetWrap(GStexture* tex, s32 wrapU, s32 wrapV)
{
    GXTexWrapMode gxWrapU;
    GXTexWrapMode gxWrapV;

    tex->mWrapU = wrapU;
    tex->mWrapV = wrapV;

    switch (tex->mWrapU) {
    case GS_TEXTURE_WRAP_CLAMP: gxWrapU = GX_CLAMP; break;
    case GS_TEXTURE_WRAP_REPEAT: gxWrapU = GX_REPEAT; break;
    case GS_TEXTURE_WRAP_MIRROR: gxWrapU = GX_MIRROR; break;
    }
    switch (tex->mWrapV) {
    case GS_TEXTURE_WRAP_CLAMP: gxWrapV = GX_CLAMP; break;
    case GS_TEXTURE_WRAP_REPEAT: gxWrapV = GX_REPEAT; break;
    case GS_TEXTURE_WRAP_MIRROR: gxWrapV = GX_MIRROR; break;
    }
    GXInitTexObjWrapMode(&tex->mTexObj, gxWrapU, gxWrapV);
    tex->mStateDirty = TRUE;
}

extern "C" void GStextureFree(GStexture* tex)
{
    if (tex->mInUse) {
        GSgfxRenderWaitForDrawComplete();
        tex->mInUse = FALSE;
        if (tex->mAllocation != NULL) GSmemFree(tex->mAllocation);
    }
}

static inline GXCITexFmt GStextureGetGXCIformat(const GStexture* tex)
{
    switch (tex->mFormat) {
    case GS_TEXTURE_FORMAT_CI4:    return GX_TF_C4;
    case GS_TEXTURE_FORMAT_CI8:    return GX_TF_C8;
    case GS_TEXTURE_FORMAT_CI14X2: return GX_TF_C14X2;
    }
    return (GXCITexFmt)-1;
}

static inline void GStextureInitObjects(GStexture* tex)
{
    GXTlutFmt gxTlutFormat;
    u16 numEntries;

    numEntries = 0;
    if (tex->mTlut != NULL) {
        GXCITexFmt ciFormat;

        ciFormat = GStextureGetGXCIformat(tex);
        switch (tex->mFormat) {
        case GS_TEXTURE_FORMAT_CI4:    numEntries = 16; break;
        case GS_TEXTURE_FORMAT_CI8:    numEntries = 256; break;
        case GS_TEXTURE_FORMAT_CI14X2: numEntries = 1024; break;
        }
        switch (tex->mTlutFormat) {
        case GS_TEXTURE_TLUT_FORMAT_IA8:    gxTlutFormat = GX_TL_IA8; break;
        case GS_TEXTURE_TLUT_FORMAT_RGB565: gxTlutFormat = GX_TL_RGB565; break;
        case GS_TEXTURE_TLUT_FORMAT_RGB5A3: gxTlutFormat = GX_TL_RGB5A3; break;
        }
        GXInitTlutObj(&tex->mTlutObj, tex->mTlut, gxTlutFormat, numEntries);
        GXInitTexObjCI(&tex->mTexObj, tex->mImage[0], tex->mXsize, tex->mYsize, ciFormat,
                       GX_CLAMP, GX_CLAMP, tex->mMipCount > 1, GX_TLUT0);
    } else {
        GXInitTexObj(&tex->mTexObj, tex->mImage[0], tex->mXsize, tex->mYsize, GStextureGetGXformat(tex, TRUE),
                     GX_CLAMP, GX_CLAMP, tex->mMipCount > 1);
    }

}

extern "C" int GStextureCreateFromMemory(GStexture* tex, u16 xsize, u16 ysize, void* data, s32 format, s32 tlutFormat, u8 mipLevels)
{
    s32 tlutEntries = 0;
    int i;
    u32 size;

    memset(tex, 0, sizeof(GStexture));
    tex->mXsize = xsize;
    tex->mYsize = ysize;

    switch (format) {
    case GS_TEXTURE_FORMAT_CI4:
        tlutEntries = 16;
    case GS_TEXTURE_FORMAT_I4:
    case GS_TEXTURE_FORMAT_CMPR:
        tex->mBitsPerPixel = 4;
        break;
    case GS_TEXTURE_FORMAT_CI8:
        tlutEntries = 256;
    case GS_TEXTURE_FORMAT_IA4:
    case GS_TEXTURE_FORMAT_I8:
    case GS_TEXTURE_FORMAT_A8:
    case GS_TEXTURE_FORMAT_R8:
    case GS_TEXTURE_FORMAT_G8:
    case GS_TEXTURE_FORMAT_B8:
        tex->mBitsPerPixel = 8;
        break;
    case GS_TEXTURE_FORMAT_CI14X2:
        tlutEntries = 1024;
    case GS_TEXTURE_FORMAT_IA8:
    case GS_TEXTURE_FORMAT_RGB565:
    case GS_TEXTURE_FORMAT_RGB5A3:
        tex->mBitsPerPixel = 16;
        break;
    case GS_TEXTURE_FORMAT_32BIT:
        tex->mBitsPerPixel = 32;
        break;
    default:
        return FALSE;
    }

    tex->mMipCount = mipLevels + 1;
    tex->mInUse = TRUE;
    tex->mStateDirty = TRUE;
    tex->mFormat = format;
    tex->mTlutFormat = tlutFormat;
    tex->mWrapU = GS_TEXTURE_WRAP_CLAMP;
    tex->mWrapV = GS_TEXTURE_WRAP_CLAMP;
    tex->mMinFilter = GS_TEXTURE_FILTER_LINEAR;
    tex->mMagFilter = GS_TEXTURE_FILTER_LINEAR;
    tex->mMipFilter = GS_TEXTURE_MIP_FILTER_NONE;
    tex->mAllocation = NULL;
    tex->mImage[0] = data;
    tex->mImage[1] = NULL;
    tex->mTlut = NULL;

    size = tex->mBitsPerPixel * (u32)(xsize * ysize) >> 3;
    tex->mDataSize = 0;
    for (i = 0; i < mipLevels; i++) {
        tex->mDataSize += (size + 31) & ~31;
        size >>= 1;
    }

    if (tlutEntries != 0) {
        switch (tlutFormat) {
        case GS_TEXTURE_TLUT_FORMAT_IA8:
        case GS_TEXTURE_TLUT_FORMAT_RGB565:
        case GS_TEXTURE_TLUT_FORMAT_RGB5A3:
            tex->mDataSize += tlutEntries * GS_TEXTURE_TLUT_BITS_PER_ENTRY >> 3;
            break;
        case GS_TEXTURE_TLUT_FORMAT_NONE:
        default:
            return FALSE;
        }
    }

    size = tex->mBitsPerPixel * (u32)(xsize * ysize) >> 3;
    for (i = 1; i < 8; i++) {
        if (i < tex->mMipCount) {
            tex->mImage[i] = (u8*)tex->mImage[i - 1] + size;
            size >>= 1;
        } else {
            tex->mImage[i] = NULL;
        }
    }

    switch (tlutFormat) {
    case GS_TEXTURE_TLUT_FORMAT_IA8:
    case GS_TEXTURE_TLUT_FORMAT_RGB565:
    case GS_TEXTURE_TLUT_FORMAT_RGB5A3:
        tex->mTlut = (u8*)tex->mImage[tex->mMipCount - 1] + size;
        break;
    default:
        tex->mTlut = NULL;
        break;
    }

    GStextureInitObjects(tex);

    DCStoreRange(tex->mImage[0], tex->mDataSize);
    return TRUE;
}

static inline GStexture* GStextureFindFree(void)
{
    u32 i;

    for (i = 0; i < _textureStructCount; i++) {
        GStexture* tex = &_textureStructList[i];
        if (!tex->mInUse) {
            return tex;
        }
    }
    return NULL;
}

extern "C" GStexture* GStextureCreate(u16 xsize, u16 ysize, s32 format, s32 tlutFormat, u8 mipLevels)
{
    GStexture* tex;
    u32 alignment;
    u16 alignedXsize;
    u16 alignedYsize;
    u16 mipXsize;
    u16 mipYsize;
    u8 maxMipLevels;
    s32 tlutEntries;
    u32 size;
    int i;

    // a zero size asks for a texture the size of the embedded frame buffer
    if (xsize == 0 && ysize == 0) {
        GSgfxVideoGetEFBSize(&xsize, &ysize);
    }
    if (xsize > 1024 || ysize > 1024 || xsize < 4 || ysize < 4) {
        return NULL;
    }

    switch (format) {
    case GS_TEXTURE_FORMAT_CI4:
    case GS_TEXTURE_FORMAT_I4:
    case GS_TEXTURE_FORMAT_IA4:
    case GS_TEXTURE_FORMAT_CMPR:
        alignment = 8;
        break;
    case GS_TEXTURE_FORMAT_CI8:
    case GS_TEXTURE_FORMAT_I8:
    case GS_TEXTURE_FORMAT_IA8:
    case GS_TEXTURE_FORMAT_A8:
    case GS_TEXTURE_FORMAT_R8:
    case GS_TEXTURE_FORMAT_G8:
    case GS_TEXTURE_FORMAT_B8:
        alignment = 4;
        break;
    case GS_TEXTURE_FORMAT_CI14X2:
    case GS_TEXTURE_FORMAT_RGB565:
    case GS_TEXTURE_FORMAT_32BIT:
    case GS_TEXTURE_FORMAT_RGB5A3:
        alignment = 4;
        break;
    default:
        GSlogWrite("GStexture: invalid texture format\n");
        return NULL;
    }

    // round the size up to whole texture tiles
    alignedXsize = (xsize + alignment - 1) & ~(alignment - 1);
    alignedYsize = (ysize + alignment - 1) & ~(alignment - 1);
    if (alignedXsize != xsize || alignedYsize != ysize) {
        GSlogWrite("GStexture: warning -- texture size adjusted from [%d,%d] to [%d,%d]\n",
                   xsize, ysize, alignedXsize, alignedYsize);
    }
    xsize = alignedXsize;
    ysize = alignedYsize;

    // no mip level may be smaller than 4x4
    mipXsize = xsize;
    mipYsize = ysize;
    maxMipLevels = 0;
    while (mipXsize > 4 && mipYsize > 4 && maxMipLevels < 7) {
        mipXsize >>= 1;
        mipYsize >>= 1;
        maxMipLevels++;
    }
    if (mipLevels > maxMipLevels) {
        mipLevels = maxMipLevels;
    }

    tex = GStextureFindFree();
    if (tex == NULL) {
        return NULL;
    }

    tlutEntries = 0;
    switch (format) {
    case GS_TEXTURE_FORMAT_CI4:
        tlutEntries = 16;
    case GS_TEXTURE_FORMAT_I4:
    case GS_TEXTURE_FORMAT_CMPR:
        tex->mBitsPerPixel = 4;
        break;
    case GS_TEXTURE_FORMAT_CI8:
        tlutEntries = 256;
    case GS_TEXTURE_FORMAT_IA4:
    case GS_TEXTURE_FORMAT_I8:
    case GS_TEXTURE_FORMAT_A8:
    case GS_TEXTURE_FORMAT_R8:
    case GS_TEXTURE_FORMAT_G8:
    case GS_TEXTURE_FORMAT_B8:
        tex->mBitsPerPixel = 8;
        break;
    case GS_TEXTURE_FORMAT_CI14X2:
        tlutEntries = 1024;
    case GS_TEXTURE_FORMAT_IA8:
    case GS_TEXTURE_FORMAT_RGB565:
    case GS_TEXTURE_FORMAT_RGB5A3:
        tex->mBitsPerPixel = 16;
        break;
    case GS_TEXTURE_FORMAT_32BIT:
        tex->mBitsPerPixel = 32;
        break;
    default:
        return NULL;
    }

    mipLevels++;
    size = tex->mBitsPerPixel * (u32)(xsize * ysize) >> 3;
    tex->mDataSize = 0;
    for (i = 0; i < mipLevels; i++) {
        tex->mDataSize += (size + 31) & ~31;
        size >>= 1;
    }

    if (tlutEntries != 0) {
        switch (tlutFormat) {
        case GS_TEXTURE_TLUT_FORMAT_IA8:
        case GS_TEXTURE_TLUT_FORMAT_RGB565:
        case GS_TEXTURE_TLUT_FORMAT_RGB5A3:
            tex->mDataSize += tlutEntries * GS_TEXTURE_TLUT_BITS_PER_ENTRY >> 3;
            break;
        case GS_TEXTURE_TLUT_FORMAT_NONE:
        default:
            return NULL;
        }
    }

    tex->mAllocation = GSmemAllocAlign(tex->mDataSize, 32);
    if (tex->mAllocation == NULL) {
        return NULL;
    }
    tex->mImage[0] = tex->mAllocation;
    DCStoreRange(tex->mImage[0], tex->mDataSize);

    tex->mInUse = TRUE;
    tex->mXsize = xsize;
    tex->mYsize = ysize;
    tex->mMipCount = mipLevels;
    tex->mFormat = format;
    tex->mTlutFormat = tlutFormat;
    tex->mWrapU = GS_TEXTURE_WRAP_CLAMP;
    tex->mWrapV = GS_TEXTURE_WRAP_CLAMP;
    tex->mMinFilter = GS_TEXTURE_FILTER_LINEAR;
    tex->mMagFilter = GS_TEXTURE_FILTER_LINEAR;
    if (mipLevels > 1) {
        tex->mMipFilter = GS_TEXTURE_MIP_FILTER_LINEAR;
    } else {
        tex->mMipFilter = GS_TEXTURE_MIP_FILTER_NONE;
    }
    tex->mLockCount = 0;
    tex->mUnknown = 0;

    size = tex->mBitsPerPixel * (u32)(xsize * ysize) >> 3;
    for (i = 1; i < 8; i++) {
        if (i < tex->mMipCount) {
            tex->mImage[i] = (u8*)tex->mImage[i - 1] + size;
            size >>= 1;
        } else {
            tex->mImage[i] = NULL;
        }
    }

    switch (tlutFormat) {
    case GS_TEXTURE_TLUT_FORMAT_IA8:
    case GS_TEXTURE_TLUT_FORMAT_RGB565:
    case GS_TEXTURE_TLUT_FORMAT_RGB5A3:
        tex->mTlut = (u8*)tex->mImage[tex->mMipCount - 1] + size;
        break;
    default:
        tex->mTlut = NULL;
        break;
    }

    GStextureInitObjects(tex);

    tex->mStateDirty = TRUE;
    return tex;
}

// Fixes up a texture image that was converted offline: the file starts with a
// GStexture header whose image and TLUT pointers are byte offsets from the
// start of the header, followed by the texel and palette data.
extern "C" GStexture* GStextureLoad(void* data)
{
    GStexture* tex = (GStexture*)data;
    int i;

    for (i = 0; i < tex->mMipCount; i++) {
        tex->mImage[i] = (u8*)tex + (u32)tex->mImage[i];
    }
    for (i = tex->mMipCount; i < 8; i++) {
        tex->mImage[i] = NULL;
    }
    if (tex->mTlut != NULL) {
        tex->mTlut = (u8*)tex + (u32)tex->mTlut;
    }

    GStextureInitObjects(tex);
    GStextureSetWrap(tex, tex->mWrapU, tex->mWrapV);
    GStextureSetFilter(tex, tex->mMinFilter, tex->mMagFilter, tex->mMipFilter);
    tex->mStateDirty = TRUE;
    tex->mAllocation = NULL;

    DCStoreRange(tex->mImage[0], tex->mDataSize);
    return tex;
}

extern "C" void GStextureInit(u32 count)
{
    _textureStructCount = count;
    _textureStructList = (GStexture*)GSmemAlloc(count * sizeof(GStexture));
    if (_textureStructList != NULL) {
        for (u32 i = 0; i < _textureStructCount; i++) {
            _textureStructList[i].mInUse = FALSE;
        }
    }
}
extern "C" s32 GXFmtToGStextureFmt(GXTexFmt format)
{
    switch (format) {
    case GX_TF_C4: return GS_TEXTURE_FORMAT_CI4;
    case GX_TF_C8: return GS_TEXTURE_FORMAT_CI8;
    case GX_TF_C14X2: return GS_TEXTURE_FORMAT_CI14X2;
    case GX_TF_I4: return GS_TEXTURE_FORMAT_I4;
    case GX_TF_IA4: return GS_TEXTURE_FORMAT_IA4;
    case GX_TF_I8: return GS_TEXTURE_FORMAT_I8;
    case GX_TF_IA8: return GS_TEXTURE_FORMAT_IA8;
    case GX_TF_RGB565: return GS_TEXTURE_FORMAT_RGB565;
    case GX_TF_RGBA8: return GS_TEXTURE_FORMAT_32BIT;
    case GX_TF_RGB5A3: return GS_TEXTURE_FORMAT_RGB5A3;
    case GX_TF_CMPR: return GS_TEXTURE_FORMAT_CMPR;
    case GX_TF_A8: return GS_TEXTURE_FORMAT_A8;
    case GX_CTF_R8: return GS_TEXTURE_FORMAT_R8;
    case GX_CTF_G8: return GS_TEXTURE_FORMAT_G8;
    case GX_CTF_B8: return GS_TEXTURE_FORMAT_B8;
    }
    return -1;
}
