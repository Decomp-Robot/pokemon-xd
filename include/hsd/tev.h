#ifndef HSD_TEV_H
#define HSD_TEV_H

#include <dolphin/gx/GXEnum.h>
#include <dolphin/gx/GXStruct.h>

typedef struct _HSD_Chan {
    struct _HSD_Chan *next;
    GXChannelID chan;
    u32 unknown_08;
    GXColor amb_color;
    GXColor mat_color;
    GXBool enable;
    GXColorSrc amb_src;
    GXColorSrc mat_src;
    GXLightID light_mask;
    GXDiffuseFn diff_fn;
    GXAttnFn attn_fn;
    u32 unknown_2c;
} HSD_Chan;

typedef struct _HSD_TevDesc {
    struct _HSD_TevDesc *next;
    u32 flags;
    GXTevStageID stage;
    GXTexCoordID coord;
    GXTexMapID map;
    GXChannelID color;
    union {
        GXTevMode tevop;
        struct {
            GXTevOp color_op;
            GXTevColorArg color_a, color_b, color_c, color_d;
            GXTevScale color_scale;
            GXTevBias color_bias;
            GXBool color_clamp;
            GXTevRegID color_reg;
            GXTevOp alpha_op;
            GXTevAlphaArg alpha_a, alpha_b, alpha_c, alpha_d;
            GXTevScale alpha_scale;
            GXTevBias alpha_bias;
            GXBool alpha_clamp;
            GXTevRegID alpha_reg;
        } tevconf;
    } u;
    u32 unknown_60;
    GXTevSwapSel ras_swap;
    GXTevSwapSel tex_swap;
    GXTevKColorSel kcsel;
    GXTevKAlphaSel kasel;
    GXTevColorChan swap_r, swap_g, swap_b, swap_a;
} HSD_TevDesc;

#ifdef __cplusplus
extern "C" {
#endif
void _HSD_StateInvalidateTexCoordGen(void);
void _HSD_StateInvalidateTevRegister(void);
void _HSD_StateInvalidateTevStage(void);
void _HSD_StateInvalidateColorChannel(void);
int HSD_StateGetNumTevStages(void);
GXTevStageID HSD_StateAssignTev(void);
void HSD_StateSetNumTevStages(void);
void HSD_StateSetNumTexGens(void);
void HSD_SetTevRegAll(void);
void HSD_SetupTevStage(HSD_TevDesc *desc);
void HSD_SetupChannel(HSD_Chan *chan);
void HSD_DisableChannelLighting(GXChannelID chan);
void HSD_StateSetNumChans(int num_chans);
#ifdef __cplusplus
}
#endif

#endif
