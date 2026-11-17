#ifndef HSD_STATE_H
#define HSD_STATE_H

#include <types.h>
#include <dolphin/gx/GXEnum.h>
#include <dolphin/gx/GXStruct.h>

enum HSD_StateInvalidateFlags {
    HSD_STATE_PRIMITIVE = 0x01,
    HSD_STATE_VTX_ATTR = 0x02,
    HSD_STATE_COLOR_CHANNEL = 0x04,
    HSD_STATE_TEV_STAGE = 0x08,
    HSD_STATE_TEV_REGISTER = 0x10,
    HSD_STATE_TEXCOORD_GEN = 0x20,
    HSD_STATE_RENDER_MODE = 0x40
};

enum HSD_RenderModeFlags {
    RENDER_DIFFUSE_MAT = 0x01,
    RENDER_DIFFUSE_VTX = 0x02,
    RENDER_DIFFUSE_BOTH = 0x03,
    RENDER_DIFFUSE_MASK = 0x03,
    RENDER_DIFFUSE = 0x04,
    RENDER_SPECULAR = 0x08,
    RENDER_ALPHA_MAT = 0x2000,
    RENDER_ALPHA_VTX = 0x4000,
    RENDER_ALPHA_MASK = 0x6000,
    RENDER_ALPHA_SHIFT = 13,
    RENDER_NO_ZTEST = 0x08000000,
    RENDER_NO_ZUPDATE = 0x20000000,
    RENDER_XLU = 0x40000000
};

enum {
    HSD_PE_COLOR_UPDATE = 0x01,
    HSD_PE_ALPHA_UPDATE = 0x02,
    HSD_PE_DST_ALPHA = 0x04,
    HSD_PE_BEFORE_TEX = 0x08,
    HSD_PE_Z_COMP = 0x10,
    HSD_PE_Z_UPDATE = 0x20,
    HSD_PE_DITHER = 0x40
};

typedef struct _HSD_PEDesc {
    u8 flags;
    u8 ref0;
    u8 ref1;
    u8 dstAlpha;
    u8 blendMode;
    u8 srcFactor;
    u8 dstFactor;
    u8 logicOp;
    u8 zComp;
    u8 alphaComp0;
    u8 alphaOp;
    u8 alphaComp1;
} HSD_PEDesc;

#ifdef __cplusplus
extern "C" {
#endif
void HSD_InitColorSwapTable(u8 table, u8 red, u8 green, u8 blue, u8 alpha);
void HSD_ResetColorSwapTables(void);
void HSD_StateInvalidate(u32 flags);
void _HSD_StateInvalidateRenderMode(void);
void _HSD_StateInvalidateVtxAttr(void);
void _HSD_StateInvalidatePrimitive(void);
void HSD_StateSetCullMode(GXCullMode mode);
void HSD_SetMaterialShininess(f32 shininess);
void HSD_SetMaterialColor(GXColor ambient, GXColor diffuse, GXColor specular, f32 alpha);
void HSD_SetupRenderModeWithCustomPE(u32 rendermode, HSD_PEDesc *pe);
void HSD_SetupPEMode(u32 rendermode, HSD_PEDesc *pe);
void HSD_SetupChannelMode(u32 rendermode);
#ifdef __cplusplus
}
#endif

#endif
