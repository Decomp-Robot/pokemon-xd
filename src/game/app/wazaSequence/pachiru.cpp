#include <app/wazaSequence/pachiru.h>
#include <GSlog.h>
#include <GSmaterial.h>
#include <GSmodel.h>
#include <GSpart.h>
#include <GSres.h>
#include <GStexture.h>
#include <dolphin/os/OSCache.h>
#include <string.h>

static const u32 pachiruResourceGroup = 903;
static const s32 pachiruPartIndex = 2;
static const s32 pachiruMaterialIndex = 0;
static const s32 pachiruSpotCount = 4;

enum PachiruResource {
    PACHIRU_BASE_TEXTURE_RESOURCE = 0x0F7D1200,
    PACHIRU_SPOT_TEXTURE_RESOURCE = 0x0F7E1200
};

struct PachiruSpotOrigin {
    u8 x;
    u8 y;
};

static PachiruSpotOrigin pachiruSpotOrigins[pachiruSpotCount] = {
    {6, 7}, {91, 6}, {30, 65}, {68, 65}
};

GStexture* _pachiruEffectCreateTexture(GStexture* baseTexture,
                                       GStexture* spotTexture,
                                       u32 personality);

void pachiruEffectFree(PachiruEffect* effect)
{
    effect->mMaterial = NULL;
    if (effect->mTexture != NULL) {
        GStextureFree(effect->mTexture);
        effect->mTexture = NULL;
    }
}

u8 pachiruEffectCreate(PachiruEffect* effect, _GSmodel* model,
                       u32 personality)
{
    u8 created = 0;
    if (model == NULL) {
        return 0;
    }
    GSpart* part = GSmodelGetPart(model, pachiruPartIndex);
    if (part == NULL) {
        return 0;
    }
    effect->mMaterial = NULL;
    effect->mTexture = NULL;
    effect->mMaterial = (GSmaterial*)GSpartGetMaterial(part, pachiruMaterialIndex);
    if (effect->mMaterial != NULL) {
        GStexture* baseTexture = (GStexture*)GSresGetResource(
            pachiruResourceGroup, PACHIRU_BASE_TEXTURE_RESOURCE);
        if (baseTexture != NULL) {
            GStexture* spotTexture = (GStexture*)GSresGetResource(
                pachiruResourceGroup, PACHIRU_SPOT_TEXTURE_RESOURCE);
            if (spotTexture != NULL) {
                effect->mTexture = _pachiruEffectCreateTexture(
                    baseTexture, spotTexture, personality);
                if (effect->mTexture != NULL) {
                    GSmaterialSetTexture(effect->mMaterial, effect->mTexture);
                    created = 1;
                }
            }
        }
    }
    if (!created) {
        if (effect->mTexture != NULL) {
            GStextureFree(effect->mTexture);
            effect->mTexture = NULL;
        }
        effect->mMaterial = NULL;
    }
    GSpartFree(part);
    return created;
}

GStexture* _pachiruEffectCreateTexture(GStexture* baseTexture,
                                       GStexture* spotTexture,
                                       u32 personality)
{
    GStexture* texture;
    u16* destImage;
    u16* sourceImage = NULL;
    s16 width;
    s16 height;
    s32 pixelCount;
    u16* last;
    s16 spotWidth;
    s16 spotHeight;
    u16* source;
    u16* dest;
    s32 x;
    s32 y;
    s16 origin;
    s16 row;
    s16 column;
    s16 spotIndex;
    width = GStextureGetXsize(baseTexture);
    height = GStextureGetYsize(baseTexture);
    texture = GStextureCreate(width, height, GStextureGetFormat(baseTexture),
                              GStextureGetTlutFormat(baseTexture),
                              GStextureGetMiplevels(baseTexture));
    destImage = (u16*)GStextureLockImage(texture, 0);
    if (destImage == NULL) {
        goto failed;
    }
    sourceImage = (u16*)GStextureLockImage(baseTexture, 0);
    if (sourceImage == NULL) {
        goto failed;
    }
    pixelCount = width * height;
    memcpy(destImage, sourceImage, pixelCount * sizeof(u16));
    DCFlushRange(destImage, pixelCount * sizeof(u16));
    GStextureUnlockImage(baseTexture);
    GStextureConvertFromHW(texture);
    GStextureConvertFromHW(spotTexture);
    sourceImage = (u16*)GStextureLockImage(spotTexture, 0);
    if (sourceImage == NULL) {
        goto failed;
    }
    spotWidth = GStextureGetXsize(spotTexture);
    spotHeight = GStextureGetYsize(spotTexture);
    if (spotWidth > (width >> 2)) {
        goto failed;
    }
    if (spotHeight > (height >> 2)) {
        goto failed;
    }
    last = destImage + (pixelCount - 1);
    for (spotIndex = 0; spotIndex < pachiruSpotCount; spotIndex++) {
        s16 dx = (s16)(personality & 0xF) - 8;
        s16 dy = (s16)((personality >> 4) & 0xF) - 8;
        x = 3.4f * dx;
        y = 3.4f * dy;
        personality >>= 8;
        x += pachiruSpotOrigins[spotIndex].x;
        y += pachiruSpotOrigins[spotIndex].y;
        origin = y * width + x;
        for (row = 0; row < spotHeight; row++) {
            s16 rowStart = row * spotWidth;
            s16 line = origin + row * width;
            if (line >= 0) {
                dest = &destImage[line];
                if (dest <= last) {
                    source = &sourceImage[rowStart];
                    for (column = 0; column < spotWidth; column++, dest++,
                        source++) {
                        if (dest <= last && *source > *dest) {
                            *dest = *source;
                        }
                    }
                }
            }
        }
    }
    GStextureUnlockImage(texture);
    GStextureUnlockImage(spotTexture);
    GStextureConvertToHW(spotTexture);
    GStextureConvertToHW(texture);
    return texture;

failed:
    GSlogWrite("Failed to create Patchiru texture\n");
    if (destImage != NULL) {
        GStextureUnlockImage(texture);
    }
    if (sourceImage != NULL) {
        GStextureUnlockImage(spotTexture);
    }
    if (texture != NULL) {
        GStextureFree(texture);
    }
    return NULL;
}
