#include <hsd/state.h>
#include <hsd/tev.h>
#include <hsd/lobj.h>
#include <hsd/pobj.h>
#include <hsd/util.h>
#include <dolphin/gx/GXCull.h>
#include <dolphin/gx/GXPixel.h>
#include <dolphin/gx/GXTev.h>


static struct {
    GXColor ambient;
    GXColor diffuse;
    GXColor specular;
    u8 alpha;
    f32 shininess;
} matstate;

static u8 state_primitive_unknown_byte;
static GXBool state_primitive_unknown_bool;
static GXCullMode state_cull_mode;
static GXBlendMode state_blend_type;
static GXBlendFactor state_src_factor;
static GXBlendFactor state_dst_factor;
static GXLogicOp state_logic_op;
static GXBool state_z_enable;
static GXCompare state_z_func;
static GXBool state_z_update;
static GXCompare state_alpha_comp0;
static u8 state_alpha_ref0;
static GXAlphaOp state_alpha_op;
static GXCompare state_alpha_comp1;
static u8 state_alpha_ref1;
static GXBool state_color_update;
static GXBool state_alpha_update;
static GXBool state_dst_alpha_enable;
static u8 state_dst_alpha;
static GXBool state_z_comp_loc;
static GXBool state_dither;

void HSD_SetupChannelMode(u32 rendermode)
{
    enum HSD_RenderModeFlags diffuse, alpha;
    int specular = 0;
    int vertex_alpha = 0;
    diffuse = rendermode & RENDER_DIFFUSE_MASK;
    if (!diffuse) {
        diffuse = RENDER_DIFFUSE_MAT;
    }
    alpha = rendermode & RENDER_ALPHA_MASK;
    if (!alpha) {
        alpha = diffuse << RENDER_ALPHA_SHIFT;
    }
    if (rendermode & RENDER_SPECULAR) {
        static HSD_Chan ch = {
            NULL, GX_COLOR1, 0, {0, 0, 0, 0}, {255, 255, 255, 255},
            GX_TRUE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_SPEC, 0
        };
        int i, n;
        ch.light_mask = HSD_LObjGetLightMaskSpecular();
        HSD_SetupChannel(&ch);
        specular = 1;
        n = HSD_LObjGetNbActive();
        for (i = 0; i < n; i++) {
            _HSD_LObj *lobj = HSD_LObjGetActiveByIndex(i);
            if (lobj) {
                HSD_LObjSetup(lobj, lobj->color, matstate.shininess);
            }
        }
    }
    if (rendermode & RENDER_DIFFUSE) {
        static HSD_Chan ch = {
            NULL, GX_COLOR0, 0, {0, 0, 0, 0}, {255, 255, 255, 255},
            GX_TRUE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_SPOT, 0
        };
        static HSD_Chan ch_a = {
            NULL, GX_ALPHA0, 0, {0, 0, 0, 0}, {0, 0, 0, 0},
            GX_TRUE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_SPOT, 0
        };
        static HSD_Chan ch_vtx_a = {
            NULL, GX_ALPHA0, 0, {0, 0, 0, 0}, {0, 0, 0, 0},
            GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_CLAMP, GX_AF_SPOT, 0
        };
        static GXColor dark_matter = {0, 0, 0, 255};
        _HSD_LObj *ambient = HSD_LObjGetActiveByID(GX_MAX_LIGHT);
        int amb_alpha;
        if (ambient && (ambient->flags & HSD_LOBJ_DIFFUSE)) {
            HSD_MulColor(&matstate.ambient, &ambient->color, &ch.amb_color);
        } else {
            ch.amb_color = dark_matter;
        }
        ch.mat_src = (diffuse & RENDER_DIFFUSE_VTX) ? GX_SRC_VTX : GX_SRC_REG;
        ch.light_mask = HSD_LObjGetLightMaskDiffuse();
        HSD_SetupChannel(&ch);
        if (alpha & RENDER_ALPHA_VTX) {
            ch_a.chan = GX_ALPHA1;
            vertex_alpha = 1;
            HSD_SetupChannel(&ch_vtx_a);
        } else {
            ch_a.chan = GX_ALPHA0;
        }
        ch_a.light_mask = HSD_LObjGetLightMaskAlpha();
        if (ambient && (ambient->flags & HSD_LOBJ_ALPHA)) {
            amb_alpha = ambient->color.a;
        } else {
            amb_alpha = 0;
        }
        if (ch_a.light_mask) {
            ch_a.enable = GX_TRUE;
            ch_a.mat_color.a = 255;
            ch_a.amb_color.a = amb_alpha;
        } else {
            ch_a.enable = GX_FALSE;
            ch_a.mat_color.a = amb_alpha;
        }
        HSD_SetupChannel(&ch_a);
    } else {
        static HSD_Chan ch = {
            NULL, GX_COLOR0, 0, {0, 0, 0, 0}, {255, 255, 255, 255},
            GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE, 0
        };
        static HSD_Chan ch_a = {
            NULL, GX_ALPHA0, 0, {0, 0, 0, 0}, {0, 0, 0, 255},
            GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE, 0
        };
        ch.mat_src = (diffuse & RENDER_DIFFUSE_VTX) ? GX_SRC_VTX : GX_SRC_REG;
        HSD_SetupChannel(&ch);
        ch_a.mat_src = (alpha & RENDER_ALPHA_VTX) ? GX_SRC_VTX : GX_SRC_REG;
        HSD_SetupChannel(&ch_a);
    }
    if (specular) {
        if (!vertex_alpha) {
            HSD_DisableChannelLighting(GX_ALPHA1);
        }
        HSD_StateSetNumChans(2);
    } else if (vertex_alpha) {
        HSD_DisableChannelLighting(GX_COLOR1);
        HSD_StateSetNumChans(2);
    } else {
        HSD_DisableChannelLighting(GX_COLOR1A1);
        HSD_StateSetNumChans(1);
    }
}

static inline void StateSetColorUpdate(GXBool enable)
{
    enable = enable ? GX_TRUE : GX_FALSE;
    if (state_color_update != enable) {
        GXSetColorUpdate(enable);
        state_color_update = enable;
    }
}

static inline void StateSetAlphaUpdate(GXBool enable)
{
    enable = enable ? GX_TRUE : GX_FALSE;
    if (state_alpha_update != enable) {
        GXSetAlphaUpdate(enable);
        state_alpha_update = enable;
    }
}

static inline void StateSetDstAlpha(GXBool enable, u8 alpha)
{
    enable = enable ? GX_TRUE : GX_FALSE;
    if (state_dst_alpha_enable != enable || state_dst_alpha != alpha) {
        GXSetDstAlpha(enable, alpha);
        state_dst_alpha_enable = enable;
        state_dst_alpha = alpha;
    }
}

static inline void StateSetBlendMode(GXBlendMode type, GXBlendFactor src, GXBlendFactor dst, GXLogicOp op)
{
    if (state_blend_type != type || state_src_factor != src ||
        state_dst_factor != dst || state_logic_op != op) {
        GXSetBlendMode(type, src, dst, op);
        state_blend_type = type;
        state_src_factor = src;
        state_dst_factor = dst;
        state_logic_op = op;
    }
}

static inline void StateSetZMode(GXBool enable, GXCompare func, GXBool update)
{
    enable = enable ? GX_TRUE : GX_FALSE;
    update = update ? GX_TRUE : GX_FALSE;
    if (state_z_enable != enable || state_z_func != func || state_z_update != update) {
        GXSetZMode(enable, func, update);
        state_z_enable = enable;
        state_z_func = func;
        state_z_update = update;
    }
}

static inline void StateSetZCompLoc(GXBool before_tex)
{
    before_tex = before_tex ? GX_TRUE : GX_FALSE;
    if (state_z_comp_loc != before_tex) {
        GXSetZCompLoc(before_tex);
        state_z_comp_loc = before_tex;
    }
}

static inline void StateSetAlphaCompare(GXCompare comp0, u8 ref0, GXAlphaOp op, GXCompare comp1, u8 ref1)
{
    if (state_alpha_comp0 != comp0 || state_alpha_ref0 != ref0 ||
        state_alpha_op != op || state_alpha_comp1 != comp1 || state_alpha_ref1 != ref1) {
        GXSetAlphaCompare(comp0, ref0, op, comp1, ref1);
        state_alpha_comp0 = comp0;
        state_alpha_ref0 = ref0;
        state_alpha_op = op;
        state_alpha_comp1 = comp1;
        state_alpha_ref1 = ref1;
    }
}

static inline void StateSetDither(GXBool enable)
{
    enable = enable ? GX_TRUE : GX_FALSE;
    if (state_dither != enable) {
        GXSetDither(enable);
        state_dither = enable;
    }
}

void HSD_SetupPEMode(u32 rendermode, HSD_PEDesc *pe)
{
    if (pe) {
        StateSetColorUpdate((pe->flags & HSD_PE_COLOR_UPDATE) != 0);
        StateSetAlphaUpdate((pe->flags & HSD_PE_ALPHA_UPDATE) != 0);
        StateSetDstAlpha((pe->flags & HSD_PE_DST_ALPHA) != 0, pe->dstAlpha);
        StateSetBlendMode(pe->blendMode, pe->srcFactor, pe->dstFactor, pe->logicOp);
        StateSetZMode((pe->flags & HSD_PE_Z_COMP) != 0, pe->zComp,
                      pe->flags & HSD_PE_Z_UPDATE);
        StateSetZCompLoc((pe->flags & HSD_PE_BEFORE_TEX) != 0);
        StateSetAlphaCompare(pe->alphaComp0, pe->ref0, pe->alphaOp, pe->alphaComp1, pe->ref1);
        StateSetDither((pe->flags & HSD_PE_DITHER) != 0);
    } else {
        StateSetColorUpdate(GX_TRUE);
        StateSetAlphaUpdate(GX_FALSE);
        StateSetDstAlpha(GX_FALSE, 0);
        StateSetBlendMode((rendermode & RENDER_XLU) ? GX_BM_BLEND : GX_BM_NONE,
                          GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
        StateSetZMode(GX_TRUE, (rendermode & RENDER_NO_ZTEST) ? GX_ALWAYS : GX_LEQUAL,
                      (rendermode & RENDER_NO_ZUPDATE) ? GX_FALSE : GX_TRUE);
        if (!(rendermode & RENDER_NO_ZUPDATE) && (rendermode & RENDER_XLU)) {
            StateSetZCompLoc(GX_FALSE);
            StateSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_GREATER, 0);
        } else {
            StateSetZCompLoc(GX_TRUE);
            StateSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        }
        StateSetDither(GX_FALSE);
    }
}

void HSD_SetupRenderModeWithCustomPE(u32 rendermode, HSD_PEDesc *pe)
{
    if (HSD_StateGetNumTevStages() == 0) {
        HSD_TevDesc tev;
        tev.flags = 0;
        tev.stage = HSD_StateAssignTev();
        tev.coord = GX_TEXCOORD_NULL;
        tev.map = GX_TEXMAP_NULL;
        tev.color = GX_COLOR0A0;
        tev.u.tevop = GX_PASSCLR;
        HSD_SetupTevStage(&tev);
    }
    HSD_SetupPEMode(rendermode, pe);
    HSD_SetTevRegAll();
    HSD_StateSetNumTevStages();
    HSD_StateSetNumTexGens();
    HSD_SetupChannelMode(rendermode);
}

static inline f32 ClampAlpha(f32 alpha)
{
    if (alpha <= 0.0f) {
        return 0.0f;
    }
    if (alpha >= 1.0f) {
        return 1.0f;
    }
    return alpha;
}

void HSD_SetMaterialColor(GXColor ambient, GXColor diffuse, GXColor specular, f32 alpha)
{
    matstate.ambient = ambient;
    matstate.diffuse = diffuse;
    matstate.specular = specular;
    matstate.alpha = 255.0f * ClampAlpha(alpha);
}

void HSD_SetMaterialShininess(f32 shininess)
{
    matstate.shininess = shininess;
}

void HSD_StateSetCullMode(GXCullMode mode)
{
    if (state_cull_mode != mode) {
        GXSetCullMode(mode);
        state_cull_mode = mode;
    }
}

void _HSD_StateInvalidatePrimitive(void)
{
    state_primitive_unknown_bool = 0;
    state_cull_mode = -1;
    state_primitive_unknown_byte = 0xFF;
}

void _HSD_StateInvalidateVtxAttr(void)
{
    HSD_ClearVtxDesc();
}

void _HSD_StateInvalidateRenderMode(void)
{
    state_blend_type = -1;
    state_src_factor = -1;
    state_dst_factor = -1;
    state_logic_op = -1;
    state_z_enable = 0xFF;
    state_z_func = -1;
    state_z_update = 0xFF;
    state_alpha_comp0 = -1;
    state_alpha_ref0 = 0;
    state_alpha_op = -1;
    state_alpha_comp1 = -1;
    state_alpha_ref1 = 0;
    state_color_update = 0xFF;
    state_alpha_update = 0xFF;
    state_dst_alpha_enable = 0xFF;
    state_dst_alpha = 0;
    state_z_comp_loc = 0xFF;
    state_dither = 0xFF;
}

void HSD_StateInvalidate(u32 flags)
{
    static struct {
        enum HSD_StateInvalidateFlags mask;
        void (*invalidate)(void);
    } invalidate_funcs[] = {
        { HSD_STATE_PRIMITIVE, _HSD_StateInvalidatePrimitive },
        { HSD_STATE_VTX_ATTR, _HSD_StateInvalidateVtxAttr },
        { HSD_STATE_COLOR_CHANNEL, _HSD_StateInvalidateColorChannel },
        { HSD_STATE_TEV_STAGE, _HSD_StateInvalidateTevStage },
        { HSD_STATE_TEV_REGISTER, _HSD_StateInvalidateTevRegister },
        { HSD_STATE_TEXCOORD_GEN, _HSD_StateInvalidateTexCoordGen },
        { HSD_STATE_RENDER_MODE, _HSD_StateInvalidateRenderMode },
        { 0, NULL }
    };
    int i;
    for (i = 0; invalidate_funcs[i].mask; i++) {
        if (flags & invalidate_funcs[i].mask) {
            invalidate_funcs[i].invalidate();
        }
    }
}

