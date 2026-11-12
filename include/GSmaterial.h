#ifndef GS_MATERIAL_H
#define GS_MATERIAL_H

#include <types.h>

union GScolor;
struct GStexture;
struct _HSD_MObj;

struct GSmaterial {
    u8 _00[2];
    u16 mEnabledExtensions; // 0x002: GS_MATERIAL_EXT_* flags currently on.
    u8 _04[4];
    _HSD_MObj *mMaterial;
    u8 _0C[40];
};

/* Extra rendering modes a material can carry. */
enum GSmaterialExtension {
    GS_MATERIAL_EXT_MODULATION = 0x1,
    GS_MATERIAL_EXT_COLOR_SWAP = 0x2,
    GS_MATERIAL_EXT_ENV_MAP = 0x4
};

extern "C" void *GSmaterialGetTexture(void *material, void *tex);

extern "C" {
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
