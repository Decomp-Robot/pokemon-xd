#include <GSgfx.h>

enum {
    VF_FLAG_USED          = 0x1, // created and not yet freed
    VF_FLAG_FMT_DIRTY     = 0x2, // vertex attribute formats need rewriting
    VF_FLAG_DESC_DIRTY    = 0x4, // vertex descriptors/arrays need rewriting
    VF_FLAG_INVALIDATE_VC = 0x8, // indexed data: invalidate the vertex cache
    VF_FLAG_PREDEFINED    = 0x8000
};

enum {
    VF_ARRAY_FLAG_FLUSH = 0x1 // array lives in cached RAM; store it before drawing
};

static GSgfxVF _vfPredefFmt[] = {
    {
        VF_FLAG_PREDEFINED | VF_FLAG_USED, 1, 0, GX_VTXFMT7,
        { { GX_VA_POS, GX_POS_XYZ, GX_F32, 0 }, { GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0 }, { GX_VA_NULL } },
        { { GX_VA_POS, GX_DIRECT }, { GX_VA_CLR0, GX_DIRECT }, { GX_VA_NULL } },
        { { 0, 0, GX_VA_NULL } }
    },
    {
        VF_FLAG_PREDEFINED | VF_FLAG_USED, 1, 0, GX_VTXFMT7,
        { { GX_VA_POS, GX_POS_XYZ, GX_S16, 0 }, { GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0 }, { GX_VA_NULL } },
        { { GX_VA_POS, GX_DIRECT }, { GX_VA_CLR0, GX_DIRECT }, { GX_VA_NULL } },
        { { 0, 0, GX_VA_NULL } }
    },
    {
        VF_FLAG_PREDEFINED | VF_FLAG_USED, 1, 0, GX_VTXFMT7,
        { { GX_VA_POS, GX_POS_XY, GX_F32, 0 }, { GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0 }, { GX_VA_NULL } },
        { { GX_VA_POS, GX_DIRECT }, { GX_VA_CLR0, GX_DIRECT }, { GX_VA_NULL } },
        { { 0, 0, GX_VA_NULL } }
    },
    {
        VF_FLAG_PREDEFINED | VF_FLAG_USED, 1, 0, GX_VTXFMT7,
        { { GX_VA_POS, GX_POS_XY, GX_S16, 0 }, { GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0 }, { GX_VA_NULL } },
        { { GX_VA_POS, GX_DIRECT }, { GX_VA_CLR0, GX_DIRECT }, { GX_VA_NULL } },
        { { 0, 0, GX_VA_NULL } }
    },
    {
        VF_FLAG_PREDEFINED | VF_FLAG_USED, 0, 1, GX_VTXFMT7,
        { { GX_VA_POS, GX_POS_XYZ, GX_F32, 0 }, { GX_VA_TEX0, GX_TEX_ST, GX_F32, 0 }, { GX_VA_NULL } },
        { { GX_VA_POS, GX_DIRECT }, { GX_VA_TEX0, GX_DIRECT }, { GX_VA_NULL } },
        { { 0, 0, GX_VA_NULL } }
    },
    {
        VF_FLAG_PREDEFINED | VF_FLAG_USED, 1, 1, GX_VTXFMT7,
        { { GX_VA_POS, GX_POS_XYZ, GX_F32, 0 }, { GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0 }, { GX_VA_TEX0, GX_TEX_ST, GX_F32, 0 }, { GX_VA_NULL } },
        { { GX_VA_POS, GX_DIRECT }, { GX_VA_CLR0, GX_DIRECT }, { GX_VA_TEX0, GX_DIRECT }, { GX_VA_NULL } },
        { { 0, 0, GX_VA_NULL } }
    },
    {
        VF_FLAG_PREDEFINED | VF_FLAG_USED, 1, 1, GX_VTXFMT7,
        { { GX_VA_POS, GX_POS_XY, GX_F32, 0 }, { GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0 }, { GX_VA_TEX0, GX_TEX_ST, GX_F32, 0 }, { GX_VA_NULL } },
        { { GX_VA_POS, GX_DIRECT }, { GX_VA_CLR0, GX_DIRECT }, { GX_VA_TEX0, GX_DIRECT }, { GX_VA_NULL } },
        { { 0, 0, GX_VA_NULL } }
    },
    {
        VF_FLAG_PREDEFINED | VF_FLAG_USED, 1, 1, GX_VTXFMT7,
        { { GX_VA_POS, GX_POS_XY, GX_S16, 0 }, { GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0 }, { GX_VA_TEX0, GX_TEX_ST, GX_F32, 0 }, { GX_VA_NULL } },
        { { GX_VA_POS, GX_DIRECT }, { GX_VA_CLR0, GX_DIRECT }, { GX_VA_TEX0, GX_DIRECT }, { GX_VA_NULL } },
        { { 0, 0, GX_VA_NULL } }
    },
    {
        VF_FLAG_PREDEFINED | VF_FLAG_USED, 0, 1, GX_VTXFMT7,
        { { GX_VA_POS, GX_POS_XY, GX_S16, 0 }, { GX_VA_TEX0, GX_TEX_ST, GX_U8, 0 }, { GX_VA_NULL } },
        { { GX_VA_POS, GX_DIRECT }, { GX_VA_TEX0, GX_DIRECT }, { GX_VA_NULL } },
        { { 0, 0, GX_VA_NULL } }
    }
};

static GXAttr _vfElemTable[] = {
    GX_VA_PNMTXIDX, GX_VA_TEX0MTXIDX, GX_VA_TEX1MTXIDX, GX_VA_TEX2MTXIDX,
    GX_VA_TEX3MTXIDX, GX_VA_TEX4MTXIDX, GX_VA_TEX5MTXIDX, GX_VA_TEX6MTXIDX,
    GX_VA_TEX7MTXIDX, GX_VA_POS, GX_VA_NRM, GX_VA_NBT, GX_VA_CLR0, GX_VA_CLR1,
    GX_VA_TEX0, GX_VA_TEX1, GX_VA_TEX2, GX_VA_TEX3,
    GX_VA_TEX4, GX_VA_TEX5, GX_VA_TEX6, GX_VA_TEX7, GX_VA_NULL
};

GSgfxVF _vfLocal;
GXVtxFmt _vfAllocIndex;

static GXVtxAttrFmtList *_vfFindFmt(GSgfxVF *vf, GXAttr attr)
{
    GXVtxAttrFmtList *fmt;
    for (fmt = vf->fmt; fmt->attr != GX_VA_NULL; fmt++) {
        if (fmt->attr == attr) {
            return fmt;
        }
    }
    return NULL;
}

static GXVtxDescList *_vfFindDesc(GSgfxVF *vf, GXAttr attr)
{
    GXVtxDescList *desc;
    for (desc = vf->desc; desc->attr != GX_VA_NULL; desc++) {
        if (desc->attr == attr) {
            return desc;
        }
    }
    return NULL;
}

static GSgfxVFArray *_vfFindArray(GSgfxVF *vf, GXAttr attr)
{
    GSgfxVFArray *array;
    for (array = vf->array; array->attr != GX_VA_NULL; array++) {
        if (array->attr == attr) {
            return array;
        }
    }
    return NULL;
}

static u32 _vfElemIndex(GXAttr attr)
{
    u32 index;
    for (index = 0; _vfElemTable[index] != GX_VA_NULL; index++) {
        if (attr == _vfElemTable[index]) {
            return index;
        }
    }
    return 0;
}

static void _vfUpdate(GSgfxVF *vf)
{
    u8 i;
    u8 j;

    vf->colorCount = 0;
    vf->textureCount = 0;
    vf->flags &= ~VF_FLAG_INVALIDATE_VC;

    for (i = 0; i < 2; i++) {
        if (_vfFindFmt(vf, (GXAttr)(GX_VA_CLR0 + i)) == NULL) {
            break;
        }
        vf->colorCount++;
    }
    for (i = 0; i < 8; i++) {
        if (_vfFindFmt(vf, (GXAttr)(GX_VA_TEX0 + i)) == NULL) {
            break;
        }
        vf->textureCount++;
    }
    for (j = 0; j < GX_VA_MAX_ATTR + 1; j++) {
        if (vf->desc[j].attr == GX_VA_NULL) {
            break;
        }
        if (vf->desc[j].type == GX_INDEX8 || vf->desc[j].type == GX_INDEX16) {
            vf->flags |= VF_FLAG_INVALIDATE_VC;
            break;
        }
    }
    vf->flags |= VF_FLAG_FMT_DIRTY | VF_FLAG_DESC_DIRTY;
}

static GSgfxVFArray *_vfFindArrayInsert(GSgfxVF *vf, GXAttr attr)
{
    u32 index = _vfElemIndex(attr);
    GSgfxVFArray *prev;
    while (index != 0) {
        index--;
        prev = _vfFindArray(vf, _vfElemTable[index]);
        if (prev != NULL) {
            return prev + 1;
        }
    }
    return NULL;
}

static GSgfxVFArray *_vfInsertArray(GSgfxVF *vf, GXAttr attr)
{
    GSgfxVFArray *insert = _vfFindArrayInsert(vf, attr);
    GSgfxVFArray *last;

    if (insert == NULL) {
        insert = vf->array;
    }
    for (last = insert; last->attr != GX_VA_NULL; last++) {
    }
    for (; last >= insert; last--) {
        last[1].attr = last[0].attr;
        last[1].base = last[0].base;
        last[1].stride = last[0].stride;
    }
    return insert;
}

extern "C" {

u8 GSgfxVFGetTextureCount(GSgfxVF *vf)
{
    return vf->textureCount;
}

u8 GSgfxVFGetColorCount(GSgfxVF *vf)
{
    return vf->colorCount;
}

void GSgfxVFSetArray(GSgfxVF *vf, s32 attribute, u32 type, void *base, u32 stride, s32 arrayBytes)
{
    GXAttr attr = (GXAttr)attribute;
    u32 size = (u32)arrayBytes;
    GSgfxVFArray *array;

    if (base == NULL && size != (u32)-1) {
        if (base == NULL) {
            return;
        }
        if (size == 0) {
            return;
        }
    }

    _vfFindDesc(vf, attr)->type = (GXAttrType)type;

    array = _vfFindArray(vf, attr);
    if (array == NULL) {
        array = _vfInsertArray(vf, attr);
    }

    array->flags = 0;
    array->attr = attr;
    array->base = base;
    array->stride = stride;
    array->size = size;
    if (((u32)array->base & 0xC0000000) == 0x80000000) {
        array->flags |= VF_ARRAY_FLAG_FLUSH;
    }
    vf->flags |= VF_FLAG_DESC_DIRTY | VF_FLAG_INVALIDATE_VC;
}

static void _vfRemoveDesc(GXVtxDescList *desc)
{
    GXVtxDescList *next = desc + 1;
    while (desc->attr != GX_VA_NULL) {
        *desc++ = *next++;
    }
}

static void _vfRemoveFmt(GXVtxAttrFmtList *fmt)
{
    GXVtxAttrFmtList *next = fmt + 1;
    while (fmt->attr != GX_VA_NULL) {
        fmt->attr = next->attr;
        fmt->cnt = next->cnt;
        fmt->type = next->type;
        fmt->frac = next->frac;
        next++;
        fmt++;
    }
}

static void _vfRemoveArray(GSgfxVFArray *array)
{
    GSgfxVFArray *next = array + 1;
    while (array->attr != GX_VA_NULL) {
        array->attr = next->attr;
        array->base = next->base;
        array->stride = next->stride;
        next++;
        array++;
    }
}

void GSgfxVFRemoveElement(GSgfxVF *vf, GXAttr attr)
{
    u8 removed = FALSE;
    GXVtxDescList *desc;
    GXVtxAttrFmtList *fmt;
    GSgfxVFArray *array;

    desc = _vfFindDesc(vf, attr);
    if (desc != NULL) {
        _vfRemoveDesc(desc);
        removed = TRUE;
    }

    if (attr >= GX_VA_POS || attr < GX_VA_PNMTXIDX) {
        fmt = _vfFindFmt(vf, attr);
        if (fmt != NULL) {
            _vfRemoveFmt(fmt);
            removed = TRUE;
        }

        array = _vfFindArray(vf, attr);
        if (array != NULL) {
            _vfRemoveArray(array);
            removed = TRUE;
        }
    }

    if (removed == TRUE) {
        _vfUpdate(vf);
    }
}

static GXVtxDescList *_vfFindDescInsert(GSgfxVF *vf, GXAttr attr)
{
    u32 index = _vfElemIndex(attr);
    GXVtxDescList *prev;
    while (index != 0) {
        index--;
        prev = _vfFindDesc(vf, _vfElemTable[index]);
        if (prev != NULL) {
            return prev + 1;
        }
    }
    return NULL;
}

static GXVtxAttrFmtList *_vfFindFmtInsert(GSgfxVF *vf, GXAttr attr)
{
    u32 index = _vfElemIndex(attr);
    GXVtxAttrFmtList *prev;
    while (index != 0) {
        index--;
        prev = _vfFindFmt(vf, _vfElemTable[index]);
        if (prev != NULL) {
            return prev + 1;
        }
    }
    return NULL;
}

static GXVtxDescList *_vfInsertDesc(GSgfxVF *vf, GXAttr attr)
{
    GXVtxDescList *insert = _vfFindDescInsert(vf, attr);
    GXVtxDescList *last;

    if (insert == NULL) {
        insert = vf->desc;
    }
    for (last = insert; last->attr != GX_VA_NULL; last++) {
    }
    for (; last >= insert; last--) {
        last[1] = last[0];
    }
    return insert;
}

static GXVtxAttrFmtList *_vfInsertFmt(GSgfxVF *vf, GXAttr attr)
{
    GXVtxAttrFmtList *insert = _vfFindFmtInsert(vf, attr);
    GXVtxAttrFmtList *last;

    if (insert == NULL) {
        insert = vf->fmt;
    }
    for (last = insert; last->attr != GX_VA_NULL; last++) {
    }
    for (; last >= insert; last--) {
        last[1].attr = last[0].attr;
        last[1].cnt = last[0].cnt;
        last[1].type = last[0].type;
        last[1].frac = last[0].frac;
    }
    return insert;
}

void GSgfxVFSetElement(GSgfxVF *vf, s32 attribute, u32 cnt, u32 compType, u8 shift)
{
    GXAttr attr = (GXAttr)attribute;
    GXVtxDescList *desc;
    GXVtxAttrFmtList *fmt;

    GSgfxVFRemoveElement(vf, attr);

    desc = _vfInsertDesc(vf, attr);
    desc->attr = attr;
    desc->type = GX_DIRECT;

    if (attr >= GX_VA_POS || attr < GX_VA_PNMTXIDX) {
        fmt = _vfInsertFmt(vf, attr);
        fmt->attr = attr;
        fmt->cnt = (GXCompCnt)cnt;
        fmt->type = (GXCompType)compType;
        fmt->frac = shift;
    }

    _vfUpdate(vf);
}

bool GSgfxVFHasElement(GSgfxVF *vf, GXAttr attr)
{
    if (_vfFindDesc(vf, attr) != NULL) {
        return true;
    }
    return false;
}


void GSgfxVFFree(GSgfxVF *vf)
{
    GSgfxLayer *layer;

    if (vf == &_vfLocal) {
        return;
    }
    if (!(vf->flags & VF_FLAG_USED)) {
        return;
    }
    vf->flags &= ~VF_FLAG_USED;
    for (layer = GSgfxLayerGetLayerList(); layer != NULL; layer = layer->mNext) {
        layer->ResetVertexFormat(vf);
    }
}

BOOL GSgfxVFIsUsed(GSgfxVF *vf)
{
    return vf->flags & VF_FLAG_USED;
}

GSgfxVF *GSgfxVFGetPredef(u32 format)
{
    return &_vfPredefFmt[format];
}

static GSgfxVF *_vfCreate(GSgfxVF *vf)
{
    int i;
    GXVtxFmt fmtIndex = _vfAllocIndex;
    u32 next = _vfAllocIndex + 1;

    if (next >= GX_MAX_VTXFMT) {
        next = 0;
    }
    _vfAllocIndex = (GXVtxFmt)next;
    vf->fmtIndex = fmtIndex;
    vf->flags |= VF_FLAG_USED;
    for (i = 0; i < GX_VA_MAX_ATTR + 1; i++) {
        vf->fmt[i].attr = GX_VA_NULL;
    }
    for (i = 0; i < GX_VA_MAX_ATTR + 1; i++) {
        vf->desc[i].attr = GX_VA_NULL;
    }
    for (i = 0; i < GX_VA_MAX_ATTR + 1; i++) {
        vf->array[i].attr = GX_VA_NULL;
    }
    vf->flags |= VF_FLAG_FMT_DIRTY | VF_FLAG_DESC_DIRTY;
    return vf;
}

GSgfxVF *GSgfxVFCreate(GSgfxVF *vf)
{
    return _vfCreate(vf);
}

void GSgfxVFInit(GSgfxVF *vf)
{
    memset(vf, 0, sizeof(GSgfxVF));
}

static inline void _vfSetDirectDesc(GXVtxDescList *desc, s32 attribute)
{
    desc->attr = (GXAttr)attribute;
    desc->type = GX_DIRECT;
}

GSgfxVF *GSgfxVFBuildTemp(s32 positionComponents, s32 positionType,
                         u8 hasNormal, s32 normalComponents,
                         u8 colorCount, s32 colorFormat,
                         u8 textureCount, s32 textureComponents,
                         s32 textureFormat)
{
    GXCompCnt posCnt = (GXCompCnt)positionComponents;
    GXCompType posType = (GXCompType)positionType;
    GXCompCnt nrmCnt = (GXCompCnt)normalComponents;
    GXCompType colorType = (GXCompType)colorFormat;
    GXCompCnt texCnt = (GXCompCnt)textureComponents;
    GXCompType texType = (GXCompType)textureFormat;
    GSgfxVF *vf;
    GXVtxAttrFmtList *fmt;
    GXVtxDescList *desc;
    int i;

    if (nrmCnt == GX_NRM_NBT3) {
        return NULL;
    }
    if (colorCount > 2) {
        return NULL;
    }
    if (textureCount > 8) {
        return NULL;
    }
    if (colorType < GX_RGB565) {
        return NULL;
    }
    if (texType > GX_F32) {
        return NULL;
    }

    vf = &_vfLocal;
    fmt = vf->fmt;
    desc = vf->desc;

    fmt->attr = GX_VA_POS;
    fmt->cnt = posCnt;
    fmt->type = posType;
    fmt->frac = 0;
    fmt++;
    _vfSetDirectDesc(desc, GX_VA_POS);
    desc++;

    if (hasNormal == TRUE) {
        switch (nrmCnt) {
        case GX_NRM_XYZ:
            fmt->cnt = GX_NRM_XYZ;
            fmt->attr = GX_VA_NRM;
            desc->attr = GX_VA_NRM;
            break;
        case GX_NRM_NBT:
            fmt->cnt = GX_NRM_NBT;
            fmt->attr = GX_VA_NBT;
            desc->attr = GX_VA_NBT;
            break;
        }
        fmt->type = GX_F32;
        fmt->frac = 0;
        fmt++;
        desc->type = GX_DIRECT;
        desc++;
    }

    for (i = 0; i < colorCount; i++) {
        switch (colorType) {
        case GX_RGB565:
        case GX_RGB8:
            fmt->cnt = GX_CLR_RGB;
            break;
        case GX_RGBX8:
        case GX_RGBA4:
        case GX_RGBA6:
        case GX_RGBA8:
            fmt->cnt = GX_CLR_RGBA;
            break;
        }
        fmt->attr = (GXAttr)(GX_VA_CLR0 + i);
        fmt->type = colorType;
        fmt->frac = 0;
        fmt++;
        _vfSetDirectDesc(desc, GX_VA_CLR0 + i);
        desc++;
    }

    for (i = 0; i < textureCount; i++) {
        fmt->attr = (GXAttr)(GX_VA_TEX0 + i);
        fmt->cnt = texCnt;
        fmt->type = texType;
        fmt->frac = 0;
        fmt++;
        _vfSetDirectDesc(desc, GX_VA_TEX0 + i);
        desc++;
    }

    fmt->attr = GX_VA_NULL;
    desc->attr = GX_VA_NULL;
    vf->array[0].attr = GX_VA_NULL;
    _vfLocal.colorCount = colorCount;
    _vfLocal.textureCount = textureCount;
    _vfLocal.flags |= VF_FLAG_FMT_DIRTY | VF_FLAG_DESC_DIRTY;
    return &_vfLocal;
}

}

void GSgfxVFInit(void)
{
    _vfAllocIndex = GX_VTXFMT0;
    _vfCreate(&_vfLocal);
}
