#include <string.h>
#include <hsd/tev.h>
#include <hsd/state.h>
#include <hsd/objalloc.h>
#include <hsd/assert.h>
#include <dolphin/gx/GXBump.h>
#include <dolphin/gx/GXGeometry.h>
#include <dolphin/gx/GXLighting.h>
#include <dolphin/gx/GXTev.h>

#define CHAN_RGB_MASK 0xffffff00
#define CHAN_ALPHA_MASK 0xff

typedef struct {
    GXColorS10 color;
    int isDirty;
} TevRegEntry;

TevRegEntry TevReg[4] = { 0 };

HSD_Chan prev_ch[4] = {
    { NULL, GX_COLOR0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE, 0 },
    { NULL, GX_COLOR1, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE, 0 },
    { NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE, 0 },
    { NULL, GX_ALPHA1, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE, 0 },
};

HSD_ObjAllocData rd_alloc_data;
HSD_ObjAllocData reg_alloc_data;
HSD_ObjAllocData ch_alloc_data;
GXColor _def_swap_tables[4];
GXColor _cur_swap_tables[4];

int current_tev;
int prev_amb_invalid[2];
int prev_mat_invalid[2];
int prev_num_chans;
int num_tex_gens;


void HSD_RenderInitAllocData(void)
{
    HSD_ObjAllocInit(&rd_alloc_data, 28, 4);
    HSD_ObjAllocInit(&reg_alloc_data, 20, 4);
    HSD_ObjAllocInit(&ch_alloc_data, sizeof(HSD_Chan), 4);
}

HSD_ObjAllocData *HSD_RenderGetAllocData(void)
{
    return &rd_alloc_data;
}

HSD_ObjAllocData *HSD_TevRegGetAllocData(void)
{
    return &reg_alloc_data;
}

HSD_ObjAllocData *HSD_ChanGetAllocData(void)
{
    return &ch_alloc_data;
}

void HSD_SetupChannel(HSD_Chan *chan)
{
    int id;
    int n;
    int idx;

    if (chan == NULL || chan->chan == GX_COLOR_NULL)
        return;
    id = chan->chan;
    n = id & 3;
    idx = id & 1;
    if (chan->enable != 0 && chan->amb_src == GX_SRC_REG) {
        if (prev_amb_invalid[idx] != 0) {
            prev_amb_invalid[idx] = 0;
            GXSetChanAmbColor(GX_COLOR0A0 + idx, chan->amb_color);
            prev_ch[idx].amb_color = chan->amb_color;
            goto amb_end;
        }
        if (id == GX_COLOR0A0 || id == GX_COLOR1A1) {
            if (*(u32 *)&chan->amb_color == *(u32 *)&prev_ch[idx].amb_color)
                goto amb_end;
            prev_ch[idx].amb_color = chan->amb_color;
        } else if (id == GX_COLOR0 || id == GX_COLOR1) {
            u32 prev_color = *(u32 *)&prev_ch[idx].amb_color;

            if (!((*(u32 *)&chan->amb_color ^ *(u32 *)&prev_ch[idx].amb_color) & CHAN_RGB_MASK))
                goto amb_end;
            *(u32 *)&prev_ch[idx].amb_color = (prev_color & CHAN_ALPHA_MASK) |
                (*(u32 *)&chan->amb_color & CHAN_RGB_MASK);
        } else {
            if (chan->amb_color.a == prev_ch[idx].amb_color.a)
                goto amb_end;
            prev_ch[idx].amb_color.a = chan->amb_color.a;
        }
        GXSetChanAmbColor(id, chan->amb_color);
    }
amb_end:
    if (chan->mat_src == GX_SRC_REG) {
        if (prev_mat_invalid[idx] != 0) {
            prev_mat_invalid[idx] = 0;
            GXSetChanMatColor(GX_COLOR0A0 + idx, chan->mat_color);
            prev_ch[idx].mat_color = chan->mat_color;
            goto mat_end;
        }
        if (id == GX_COLOR0A0 || id == GX_COLOR1A1) {
            if (*(u32 *)&chan->mat_color == *(u32 *)&prev_ch[idx].mat_color)
                goto mat_end;
            prev_ch[idx].mat_color = chan->mat_color;
        } else if (id == GX_COLOR0 || id == GX_COLOR1) {
            u32 prev_color = *(u32 *)&prev_ch[idx].mat_color;

            if (!((*(u32 *)&chan->mat_color ^ *(u32 *)&prev_ch[idx].mat_color) & CHAN_RGB_MASK))
                goto mat_end;
            *(u32 *)&prev_ch[idx].mat_color = (prev_color & CHAN_ALPHA_MASK) |
                (*(u32 *)&chan->mat_color & CHAN_RGB_MASK);
        } else {
            if (chan->mat_color.a == prev_ch[idx].mat_color.a)
                goto mat_end;
            prev_ch[idx].mat_color.a = chan->mat_color.a;
        }
        GXSetChanMatColor(id, chan->mat_color);
    }
mat_end:
    if (chan->enable == prev_ch[n].enable && chan->amb_src == prev_ch[n].amb_src &&
        chan->mat_src == prev_ch[n].mat_src && chan->light_mask == prev_ch[n].light_mask &&
        chan->diff_fn == prev_ch[n].diff_fn && chan->attn_fn == prev_ch[n].attn_fn)
        return;
    GXSetChanCtrl(id, chan->enable, chan->amb_src, chan->mat_src, chan->light_mask,
        chan->diff_fn, chan->attn_fn);
    prev_ch[n].enable = chan->enable;
    prev_ch[n].amb_src = chan->amb_src;
    prev_ch[n].mat_src = chan->mat_src;
    prev_ch[n].light_mask = chan->light_mask;
    prev_ch[n].diff_fn = chan->diff_fn;
    prev_ch[n].attn_fn = chan->attn_fn;
    if (id == GX_COLOR0A0 || id == GX_COLOR1A1) {
        prev_ch[n + 2].enable = chan->enable;
        prev_ch[n + 2].amb_src = chan->amb_src;
        prev_ch[n + 2].mat_src = chan->mat_src;
        prev_ch[n + 2].light_mask = chan->light_mask;
        prev_ch[n + 2].diff_fn = chan->diff_fn;
        prev_ch[n + 2].attn_fn = chan->attn_fn;
    }
}

void HSD_DisableChannelLighting(GXChannelID chan)
{
    int id = chan & 3;

    switch (chan) {
    case GX_COLOR0A0:
    case GX_COLOR1A1:
        if (prev_ch[id].enable != 0 || prev_ch[id + 2].enable != 0) {
            GXColorSrc amb_src;
            GXColorSrc mat_src;
            GXDiffuseFn diff_fn;
            GXAttnFn attn_fn;

            prev_ch[id + 2].enable = 0;
            prev_ch[id].enable = 0;
            prev_ch[id + 2].light_mask = 0;
            prev_ch[id].light_mask = 0;
            amb_src = prev_ch[id].amb_src;
            mat_src = prev_ch[id].mat_src;
            diff_fn = prev_ch[id].diff_fn;
            attn_fn = prev_ch[id].attn_fn;
            prev_ch[id + 2].amb_src = amb_src;
            prev_ch[id + 2].mat_src = mat_src;
            prev_ch[id + 2].diff_fn = diff_fn;
            prev_ch[id + 2].attn_fn = attn_fn;
            GXSetChanCtrl(chan, GX_DISABLE, amb_src, mat_src, 0, diff_fn, attn_fn);
        }
        break;
    default:
        if (prev_ch[id].enable != 0) {
            prev_ch[id].enable = 0;
            prev_ch[id].light_mask = 0;
            GXSetChanCtrl(chan, GX_DISABLE, prev_ch[id].amb_src, prev_ch[id].mat_src, 0,
                prev_ch[id].diff_fn, prev_ch[id].attn_fn);
        }
        break;
    }
}

void HSD_StateSetNumChans(int num_chans)
{
    if (prev_num_chans != num_chans) {
        GXSetNumChans(num_chans);
        prev_num_chans = num_chans;
    }
}

void HSD_SetupChannelAll(HSD_Chan *chan)
{
    int max = 0;

    for (; chan != NULL; chan = chan->next) {
        int id;
        switch (chan->chan) {
        case GX_COLOR0:
            id = 1;
            break;
        case GX_COLOR1:
            id = 2;
            break;
        case GX_ALPHA0:
            id = 1;
            break;
        case GX_ALPHA1:
            id = 2;
            break;
        case GX_COLOR0A0:
            id = 1;
            break;
        case GX_COLOR1A1:
            id = 2;
            break;
        case GX_COLOR_NULL:
            id = 0;
            break;
        default:
            HSD_ASSERTLINE(795, 0);
            id = 0;
            break;
        }
        if (id > max)
            max = id;
        HSD_SetupChannel(chan);
    }
    if (prev_num_chans != (u8)max) {
        GXSetNumChans(max);
        prev_num_chans = (u8)max;
    }
}

void HSD_StateRegisterTexGen(GXTexCoordID texgen)
{
    int res;

    switch (texgen) {
    case GX_TEXCOORD0:
        res = 1;
        break;
    case GX_TEXCOORD1:
        res = 2;
        break;
    case GX_TEXCOORD2:
        res = 3;
        break;
    case GX_TEXCOORD3:
        res = 4;
        break;
    case GX_TEXCOORD4:
        res = 5;
        break;
    case GX_TEXCOORD5:
        res = 6;
        break;
    case GX_TEXCOORD6:
        res = 7;
        break;
    case GX_TEXCOORD7:
        res = 8;
        break;
    case GX_TEXCOORD_NULL:
        res = 0;
        break;
    default:
        HSD_ASSERTLINE(1149, 0);
        res = 0;
        break;
    }
    if (res > num_tex_gens)
        num_tex_gens = res;
}

void HSD_StateSetNumTexGens(void)
{
    GXSetNumTexGens(num_tex_gens);
    num_tex_gens = 0;
}

void HSD_StateInitTev(void)
{
    current_tev = 0;
}

int HSD_StateGetNumTevStages(void)
{
    return current_tev;
}

GXTevStageID HSD_StateAssignTev(void)
{
    GXTevStageID stage = current_tev;

    current_tev = stage + 1;
    return HSD_Index2TevStage(stage);
}

void HSD_StateSetNumTevStages(void)
{
    GXSetNumTevStages(current_tev);
    current_tev = 0;
}

void HSD_InitColorSwapTable(u8 table, u8 r, u8 g, u8 b, u8 a)
{
    _cur_swap_tables[table].r = r;
    _def_swap_tables[table].r = r;
    _cur_swap_tables[table].g = g;
    _def_swap_tables[table].g = g;
    _cur_swap_tables[table].b = b;
    _def_swap_tables[table].b = b;
    _cur_swap_tables[table].a = a;
    _def_swap_tables[table].a = a;
}

void HSD_ResetColorSwapTables(void)
{
    int i;

    for (i = 0; i < 4; i++) {
        s32 defval = *(s32 *)&_def_swap_tables[i];

        if (defval != *(s32 *)&_cur_swap_tables[i]) {
            GXSetTevSwapModeTable(i, _def_swap_tables[i].r, _def_swap_tables[i].g,
                _def_swap_tables[i].b, _def_swap_tables[i].a);
            *(s32 *)&_cur_swap_tables[i] = defval;
        }
    }
}


void HSD_SetupTevStage(HSD_TevDesc *desc)
{
    GXSetTevOrder(desc->stage, desc->coord, desc->map, desc->color);
    if (desc->flags == 0) {
        GXSetTevOp(desc->stage, desc->u.tevop);
        GXSetTevSwapMode(desc->stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    } else {
        GXSetTevColorOp(desc->stage, desc->u.tevconf.color_op, desc->u.tevconf.color_bias,
            desc->u.tevconf.color_scale, desc->u.tevconf.color_clamp, desc->u.tevconf.color_reg);
        GXSetTevColorIn(desc->stage, desc->u.tevconf.color_a, desc->u.tevconf.color_b,
            desc->u.tevconf.color_c, desc->u.tevconf.color_d);
        GXSetTevAlphaOp(desc->stage, desc->u.tevconf.alpha_op, desc->u.tevconf.alpha_bias,
            desc->u.tevconf.alpha_scale, desc->u.tevconf.alpha_clamp, desc->u.tevconf.alpha_reg);
        GXSetTevAlphaIn(desc->stage, desc->u.tevconf.alpha_a, desc->u.tevconf.alpha_b,
            desc->u.tevconf.alpha_c, desc->u.tevconf.alpha_d);
        GXSetTevSwapModeTable(desc->ras_swap, desc->swap_r, desc->swap_g, desc->swap_b, desc->swap_a);
        _cur_swap_tables[desc->ras_swap].r = desc->swap_r;
        _cur_swap_tables[desc->ras_swap].g = desc->swap_g;
        _cur_swap_tables[desc->ras_swap].b = desc->swap_b;
        _cur_swap_tables[desc->ras_swap].a = desc->swap_a;
        if (desc->tex_swap != desc->ras_swap) {
            GXSetTevSwapModeTable(desc->tex_swap, desc->swap_r, desc->swap_g, desc->swap_b, desc->swap_a);
            _cur_swap_tables[desc->ras_swap].r = desc->swap_r;
            _cur_swap_tables[desc->ras_swap].g = desc->swap_g;
            _cur_swap_tables[desc->ras_swap].b = desc->swap_b;
            _cur_swap_tables[desc->ras_swap].a = desc->swap_a;
        }
        GXSetTevSwapMode(desc->stage, desc->ras_swap, desc->tex_swap);
        GXSetTevKColorSel(desc->stage, desc->kcsel);
        GXSetTevKAlphaSel(desc->stage, desc->kasel);
    }
}

void HSD_SetupTevStageAll(HSD_TevDesc *desc)
{
    int max = 0;

    for (; desc != NULL; desc = desc->next) {
        int counter;
        switch (desc->stage) {
        case GX_TEVSTAGE0:
            counter = 1;
            break;
        case GX_TEVSTAGE1:
            counter = 2;
            break;
        case GX_TEVSTAGE2:
            counter = 3;
            break;
        case GX_TEVSTAGE3:
            counter = 4;
            break;
        case GX_TEVSTAGE4:
            counter = 5;
            break;
        case GX_TEVSTAGE5:
            counter = 6;
            break;
        case GX_TEVSTAGE6:
            counter = 7;
            break;
        case GX_TEVSTAGE7:
            counter = 8;
            break;
        case GX_TEVSTAGE8:
            counter = 9;
            break;
        case GX_TEVSTAGE9:
            counter = 10;
            break;
        case GX_TEVSTAGE10:
            counter = 11;
            break;
        case GX_TEVSTAGE11:
            counter = 12;
            break;
        case GX_TEVSTAGE12:
            counter = 13;
            break;
        case GX_TEVSTAGE13:
            counter = 14;
            break;
        case GX_TEVSTAGE14:
            counter = 15;
            break;
        case GX_TEVSTAGE15:
            counter = 16;
            break;
        default:
            HSD_ASSERTLINE(932, 0);
            counter = 0;
            break;
        }
        if (counter > max)
            max = counter;
        HSD_SetupTevStage(desc);
    }
    current_tev = max;
    HSD_StateSetNumTevStages();
}

GXTevStageID HSD_Index2TevStage(int index)
{
    switch (index) {
    case 0:
        return GX_TEVSTAGE0;
    case 1:
        return GX_TEVSTAGE1;
    case 2:
        return GX_TEVSTAGE2;
    case 3:
        return GX_TEVSTAGE3;
    case 4:
        return GX_TEVSTAGE4;
    case 5:
        return GX_TEVSTAGE5;
    case 6:
        return GX_TEVSTAGE6;
    case 7:
        return GX_TEVSTAGE7;
    case 8:
        return GX_TEVSTAGE8;
    case 9:
        return GX_TEVSTAGE9;
    case 10:
        return GX_TEVSTAGE10;
    case 11:
        return GX_TEVSTAGE11;
    case 12:
        return GX_TEVSTAGE12;
    case 13:
        return GX_TEVSTAGE13;
    case 14:
        return GX_TEVSTAGE14;
    case 15:
        return GX_TEVSTAGE15;
    default:
        HSD_ASSERTLINE(848, 0);
        return GX_TEVSTAGE15;
    }
}

static inline GXTevRegID HSD_Index2TevRegID(u32 index)
{
    switch (index) {
    case 0:
        return GX_TEVREG0;
    case 1:
        return GX_TEVREG1;
    case 2:
        return GX_TEVREG2;
    case 3:
        return GX_TEVPREV;
    default:
        return GX_TEVREG0;
    }
}

void HSD_SetTevRegAll(void)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        if (TevReg[i].isDirty != 0) {
            GXSetTevColorS10(HSD_Index2TevRegID(i), TevReg[i].color);
            TevReg[i].isDirty = 0;
        }
    }
}

void _HSD_StateInvalidateColorChannel(void)
{
    static HSD_Chan invalid_prev_ch[4] = {
        { NULL, GX_COLOR0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE, 0 },
        { NULL, GX_COLOR1, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE, 0 },
        { NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE, 0 },
        { NULL, GX_ALPHA1, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE, 0 },
    };

    memcpy(prev_ch, invalid_prev_ch, sizeof(HSD_Chan) * 4);
    prev_amb_invalid[0] = 1;
    prev_amb_invalid[1] = 1;
    prev_mat_invalid[0] = 1;
    prev_mat_invalid[1] = 1;
    prev_num_chans = -1;
}

void _HSD_StateInvalidateTevStage(void)
{
    int i;

    for (i = 0; i < 16; i++)
        GXSetTevDirect(i);
    GXSetNumIndStages(0);
    current_tev = 0;
}

void _HSD_StateInvalidateTevRegister(void)
{
    int i;

    for (i = 0; i < 4; i++)
        TevReg[i].isDirty = 0;
}

void _HSD_StateInvalidateTexCoordGen(void)
{
    num_tex_gens = 0;
}
