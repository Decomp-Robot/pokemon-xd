#ifndef GS_MATERIAL_H
#define GS_MATERIAL_H

#include <types.h>
#include <hsd/state.h>
#include <hsd/tobj.h>
#include <GSgfx.h>

struct GStexture;

struct GSmaterial {
    u16 mSavedAlpha;
    u16 mEnabledExtensions; // GS_MATERIAL_EXT_* flags currently on.
    u32 mSavedFlags;        // Material flags saved while an extension is on.
    _HSD_MObj *mMaterial;
    GScolor mModulate;      // Colour the texture is modulated with.
    u8 mColorChannels[4];   // Source channel of each output colour channel.
    _HSD_TObj *mEnvMapTexture;    // Texture object of the environment map.
    _HSD_ImageDesc *mEnvMapImage; // Image the environment map was built from.
    GStexture *mEnvMapSource;     // Texture the environment map was set up for.
    s32 mEnvMapMode;        // Environment map mapping mode.
    s32 mEnvMapType;        // Environment map blending type.
    f32 mEnvMapBlend;       // Environment map blend factor.
    _HSD_ImageDesc *mImageDesc; // Image the texture change put in.
    HSD_PEDesc *mPEDesc;    // PE descriptor the material renders with.
};

/* Marks a material descriptor slot that no descriptor was installed in yet. */
#define GS_MATERIAL_PEDESC_NONE ((HSD_PEDesc *)0xFEFEFEFE)
#define GS_MATERIAL_IMAGEDESC_NONE ((_HSD_ImageDesc *)0xFEFEFEFE)

/* Extra rendering modes a material can carry. */
enum GSmaterialExtension {
    GS_MATERIAL_EXT_MODULATION = 0x1,
    GS_MATERIAL_EXT_COLOR_SWAP = 0x2,
    GS_MATERIAL_EXT_ENV_MAP = 0x4,
    GS_MATERIAL_EXT_ALL = 0xFFFF
};

extern "C" void *GSmaterialGetTexture(void *material, void *tex);

extern "C" {
void GSmaterialResetPEdescr(GSmaterial *material);
void GSmaterialSetPEdescr(GSmaterial *material, HSD_PEDesc *descriptor);
void GSmaterialResetAlpha(GSmaterial *material);
void GSmaterialStoreAlpha(GSmaterial *material);
void GSmaterialSetAlpha(GSmaterial *material, f32 alpha);
void GSmaterialResetFlags(GSmaterial *material);
u32 GSmaterialGetFlags(GSmaterial *material);
void GSmaterialSetFlags(GSmaterial *material, u32 flags);
void GSmaterialResetTexture(GSmaterial *material);
void GSmaterialSetTexture(GSmaterial *material, GStexture *texture);
void GSmaterialGetModulate(GSmaterial *material, GScolor *color);
void GSmaterialSetModulate(GSmaterial *material, GScolor *color);
void GSmaterialSetColorChannels(GSmaterial *material, GScolor *color);
void GSmaterialSetEnvMapParams(GSmaterial *material, s32 mode, s32 type,
                               GStexture *texture, f32 blend);
void GSmaterialSetEnvMapBlendValue(GSmaterial *material, f32 blend);
void GSmaterialDisableExtension(GSmaterial *material, u32 extension);
void GSmaterialEnableExtension(GSmaterial *material, u32 extension);
u16 GSmaterialGetEnabledExtensions(GSmaterial *material);
}

#endif
