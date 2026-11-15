#include <GSgfx.h>
#include <HSD.h>
#include <GSmem.h>
#include <GSlog.h>
#include <dolphin/gx/GXDispList.h>
#include <dolphin/gx/GXManage.h>
#include <dolphin/os.h>

u32 _dlPrimType = GSGFX_PRIM_TRIANGLES;
GSgfxDL *_dlCurPtr;
static u8 _dlRecording;

u8 *_dlParseSurface(GSgfxPrimType type, _HSD_VtxDescList *vtxAttr, u8 *data,
                    u16 count, GSgfxParseCallbackList *callbacks, void *userContext);
u8 *_dlParseVertex(_HSD_VtxDescList *desc, u8 *data, GSgfxParseCallbackList *callbacks, void *userContext);
u16 _dlCalculateStride(GSgfxVF *vf);

enum { DL_COMMAND_NOP = 0, DL_COMMAND_LOAD_BP = 0x61, DL_COMMAND_PRIMITIVE_MASK = 0xF8 };
enum { DL_ALIGNMENT = 32, DL_RECORDING_SURFACE_BYTES = 32, DL_RECORDING_OVERHEAD = 64 };

extern "C" void GSgfxParseDisplayList(_HSD_VtxDescList *vtxAttr, void *dl, u32 size,
                                     GSgfxParseCallbackList *callbacks, void *userContext)
{
    u8 *data = (u8 *)dl;
    GSgfxPrimType primitive;
    u8 *end = data + size;
    u16 count;
    u8 command;
    while (data < end) {
        command = *data++ & DL_COMMAND_PRIMITIVE_MASK;
        switch (command) {
        case GX_POINTS:
        case GX_LINES:
        case GX_LINESTRIP:
        case GX_TRIANGLES:
        case GX_TRIANGLESTRIP:
        case GX_TRIANGLEFAN:
        case GX_QUADS: {
            count = *((u16 *)data)++;
            switch (command) {
            case GX_POINTS: primitive = GSGFX_PRIM_POINTS; break;
            case GX_LINES: primitive = GSGFX_PRIM_LINES; break;
            case GX_LINESTRIP: primitive = GSGFX_PRIM_LINE_STRIP; break;
            case GX_TRIANGLES: primitive = GSGFX_PRIM_TRIANGLES; break;
            case GX_TRIANGLESTRIP: primitive = GS_GFX_PRIM_TRIANGLE_STRIP; break;
            case GX_TRIANGLEFAN: primitive = GSGFX_PRIM_TRIANGLE_FAN; break;
            case GX_QUADS: primitive = GSGFX_QUADS; break;
            }
            data = _dlParseSurface(primitive, vtxAttr, data, count, callbacks, userContext);
            break;
        }
        case DL_COMMAND_LOAD_BP: data += 4; break;
        case DL_COMMAND_NOP: break;
        default: return;
        }
    }
}

extern "C" void GSgfxDLEndSurface() {}

extern "C" void GSgfxDLBeginSurface(u16 vertexCount)
{
    u32 primitive = _dlPrimType;
    GXVtxFmt format = _dlCurPtr->mVertexFormat->fmtIndex;
    primitive |= format;
    GSgfxFifo.u8 = primitive;
    GSgfxFifo.u16 = vertexCount;
}

extern "C" void GSgfxDLPrimitiveType(u32 primitive)
{
    _dlPrimType = primitive;
}

extern "C" void GSgfxDLEnd()
{
    u32 size = GXEndDisplayList();
    GSmemShrink(_dlCurPtr->mData, size);
    _dlCurPtr->mSize = size;
    _dlRecording = 0;
    _dlCurPtr = 0;
}

extern "C" void GSgfxDLBegin(GSgfxDL *list, GSgfxVF *vf, u16 blockCount, u16 vertexCount)
{
    list->mVertexStride = _dlCalculateStride(vf);
    list->mSize = (blockCount * DL_RECORDING_SURFACE_BYTES +
                  vertexCount * list->mVertexStride + DL_RECORDING_OVERHEAD + DL_ALIGNMENT - 1) & ~(DL_ALIGNMENT - 1);
    list->mData = (char *)GSmemAllocAlign(list->mSize, DL_ALIGNMENT);
    memset(list->mData, 0, list->mSize);
    DCFlushRange(list->mData, list->mSize);
    list->mVertexFormat = vf;
    GXBeginDisplayList(list->mData, list->mSize);
    GXResetWriteGatherPipe();
    list->mFlags |= GS_DISPLAY_LIST_ALLOCATED;
    _dlRecording = 1;
    _dlCurPtr = list;
}

extern "C" void GSgfxDLFree(GSgfxDL *list)
{
    if (list->mFlags & GS_DISPLAY_LIST_ALLOCATED) {
        GSmemFree(list->mData);
        list->mData = 0;
        list->mFlags &= ~GS_DISPLAY_LIST_ALLOCATED;
    }
}

extern "C" void GSgfxDLCreate(GSgfxDL *list, GSgfxPrimType primitive, GSgfxVF *vf, u16 vertexCount)
{
    list->mVertexStride = _dlCalculateStride(vf);
    list->mSize = (vertexCount * list->mVertexStride + GS_DISPLAY_LIST_HEADER_BYTES + DL_ALIGNMENT - 1) & ~(DL_ALIGNMENT - 1);
    list->mData = (char *)GSmemAllocAlign(list->mSize, DL_ALIGNMENT);
    if (list->mData == 0) {
        GSlogWrite("GSgfxDL Warning: unable to allocate display list memory of (%d) bytes\n", list->mSize);
        list->mSize = 0;
    } else {
        memset(list->mData, 0, list->mSize);
        list->mVertexFormat = vf;
        u8 *data = (u8 *)list->mData;
        switch (primitive) {
        case GSGFX_PRIM_POINTS: *data = GX_POINTS; break;
        case GSGFX_PRIM_LINES: *data = GX_LINES; break;
        case GSGFX_PRIM_LINE_STRIP: *data = GX_LINESTRIP; break;
        case GSGFX_PRIM_TRIANGLES: *data = GX_TRIANGLES; break;
        case GS_GFX_PRIM_TRIANGLE_STRIP: *data = GX_TRIANGLESTRIP; break;
        case GSGFX_PRIM_TRIANGLE_FAN: *data = GX_TRIANGLEFAN; break;
        case GSGFX_QUADS: *data = GX_QUADS; break;
        }
        *data |= vf->fmtIndex;
        *(u16 *)(data + 1) = vertexCount;
        list->mFlags |= GS_DISPLAY_LIST_ALLOCATED | GS_DISPLAY_LIST_DIRTY | GS_DISPLAY_LIST_SINGLE_SURFACE;
    }
}

u8 *_dlParseSurface(GSgfxPrimType type, _HSD_VtxDescList *vtxAttr, u8 *data, u16 count,
                    GSgfxParseCallbackList *callbacks, void *userContext)
{
    _HSD_VtxDescList *desc;
    unsigned long attributeMask = 0;

    for (desc = vtxAttr; desc->mAttribute != GX_VA_NULL; desc++) {
        switch (desc->mAttribute) {
        case GX_VA_PNMTXIDX: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_PNMTXIDX; break;
        case GX_VA_TEX0MTXIDX: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_TEXMTXIDX; break;
        case GX_VA_POS: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_POSITION; break;
        case GX_VA_NRM: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_NORMAL; break;
        case GX_VA_NBT: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_NBT; break;
        case GX_VA_CLR0: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_COLOR0; break;
        case GX_VA_CLR1: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_COLOR1; break;
        case GX_VA_TEX0: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_TEX0; break;
        case GX_VA_TEX1: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_TEX1; break;
        case GX_VA_TEX2: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_TEX2; break;
        case GX_VA_TEX3: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_TEX3; break;
        case GX_VA_TEX4: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_TEX4; break;
        case GX_VA_TEX5: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_TEX5; break;
        case GX_VA_TEX6: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_TEX6; break;
        case GX_VA_TEX7: attributeMask |= GS_GFX_PARSE_ATTRIBUTE_TEX7; break;
        }
    }

    if (callbacks->mBeginSurface) {
        callbacks->mBeginSurface(type, count, attributeMask, userContext);
    }
    while (count--) {
        if (callbacks->mBeginVertex) {
            callbacks->mBeginVertex(userContext);
        }
        for (desc = vtxAttr; desc->mAttribute != GX_VA_NULL; desc++) {
            data = _dlParseVertex(desc, data, callbacks, userContext);
        }
        if (callbacks->mEndVertex) {
            callbacks->mEndVertex(userContext);
        }
    }
    if (callbacks->mEndSurface) {
        callbacks->mEndSurface(userContext);
    }
    return data;
}

u8 *_dlParseVertex(_HSD_VtxDescList *desc, u8 *data, GSgfxParseCallbackList *callbacks, void *userContext)
{
    GXCompType componentFormat;
    u32 size;
    unsigned long attributeFlag;
    GSgfxParseFlags formatFlag;
    unsigned long componentFlag;
    u16 index;
    void *attributeData;

    switch (desc->mAttribute) {
    case GX_VA_PNMTXIDX:
    case GX_VA_TEX0MTXIDX:
    case GX_VA_TEX1MTXIDX:
    case GX_VA_TEX2MTXIDX:
    case GX_VA_TEX3MTXIDX:
    case GX_VA_TEX4MTXIDX:
    case GX_VA_TEX5MTXIDX:
    case GX_VA_TEX6MTXIDX:
    case GX_VA_TEX7MTXIDX:
        attributeData = data;
        formatFlag = GS_GFX_PARSE_FORMAT_U8;
        componentFlag = GS_GFX_PARSE_COMPONENTS_1;
        switch (desc->mAttribute) {
        case GX_VA_PNMTXIDX: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_PNMTXIDX; break;
        case GX_VA_TEX0MTXIDX: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEXMTXIDX; break;
        case GX_VA_TEX1MTXIDX: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEXMTXIDX; break;
        case GX_VA_TEX2MTXIDX: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEXMTXIDX; break;
        case GX_VA_TEX3MTXIDX: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEXMTXIDX; break;
        case GX_VA_TEX4MTXIDX: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEXMTXIDX; break;
        case GX_VA_TEX5MTXIDX: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEXMTXIDX; break;
        case GX_VA_TEX6MTXIDX: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEXMTXIDX; break;
        case GX_VA_TEX7MTXIDX: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEXMTXIDX; break;
        }
        data += 1;
        break;
    case GX_VA_POS:
    case GX_VA_NRM:
    case GX_VA_CLR0:
    case GX_VA_CLR1:
    case GX_VA_TEX0:
    case GX_VA_TEX1:
    case GX_VA_TEX2:
    case GX_VA_TEX3:
    case GX_VA_TEX4:
    case GX_VA_TEX5:
    case GX_VA_TEX6:
    case GX_VA_TEX7:
    case GX_VA_NBT:
        if (desc->mAttribute == GX_VA_CLR0 || desc->mAttribute == GX_VA_CLR1) {
            attributeFlag = (desc->mAttribute == GX_VA_CLR0) ? GS_GFX_PARSE_ATTRIBUTE_COLOR0
                                                                 : GS_GFX_PARSE_ATTRIBUTE_COLOR1;
            componentFormat = desc->mComponentFormat;
            componentFlag = GS_GFX_PARSE_COMPONENTS_1;
            switch (componentFormat) {
            case GX_RGB565: formatFlag = GS_GFX_PARSE_FORMAT_RGB565; break;
            case GX_RGB8: formatFlag = GS_GFX_PARSE_FORMAT_RGB8; break;
            case GX_RGBX8: formatFlag = GS_GFX_PARSE_FORMAT_RGBX8; break;
            case GX_RGBA4: formatFlag = GS_GFX_PARSE_FORMAT_RGBA4; break;
            case GX_RGBA6: formatFlag = GS_GFX_PARSE_FORMAT_RGBA6; break;
            case GX_RGBA8: formatFlag = GS_GFX_PARSE_FORMAT_RGBA8; break;
            }
        } else {
            unsigned long scalarFormatFlag;
            GXCompType rawComponentFormat = desc->mComponentFormat;
            switch (rawComponentFormat) {
            case GX_U8: scalarFormatFlag = GS_GFX_PARSE_FORMAT_U8; break;
            case GX_S8: scalarFormatFlag = GS_GFX_PARSE_FORMAT_S8; break;
            case GX_U16: scalarFormatFlag = GS_GFX_PARSE_FORMAT_U16; break;
            case GX_S16: scalarFormatFlag = GS_GFX_PARSE_FORMAT_S16; break;
            case GX_F32: scalarFormatFlag = GS_GFX_PARSE_FORMAT_F32; break;
            }
            formatFlag = (GSgfxParseFlags)scalarFormatFlag;
            switch (desc->mAttribute) {
            case GX_VA_POS:
                attributeFlag = GS_GFX_PARSE_ATTRIBUTE_POSITION;
                if (desc->mComponentCount == GX_POS_XY) {
                    componentFlag = GS_GFX_PARSE_COMPONENTS_2;
                } else {
                    componentFlag = GS_GFX_PARSE_COMPONENTS_3;
                }
                break;
            case GX_VA_NRM:
                attributeFlag = GS_GFX_PARSE_ATTRIBUTE_NORMAL;
                if (desc->mComponentCount == GX_NRM_XYZ) {
                    componentFlag = GS_GFX_PARSE_COMPONENTS_3;
                }
                break;
            case GX_VA_NBT:
                attributeFlag = GS_GFX_PARSE_ATTRIBUTE_NBT;
                if (desc->mComponentCount == GX_NRM_NBT) {
                    componentFlag = GS_GFX_PARSE_COMPONENTS_3;
                }
                break;
            case GX_VA_TEX0:
            case GX_VA_TEX1:
            case GX_VA_TEX2:
            case GX_VA_TEX3:
            case GX_VA_TEX4:
            case GX_VA_TEX5:
            case GX_VA_TEX6:
            case GX_VA_TEX7:
                switch (desc->mAttribute) {
                case GX_VA_TEX0: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEX0; break;
                case GX_VA_TEX1: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEX1; break;
                case GX_VA_TEX2: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEX2; break;
                case GX_VA_TEX3: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEX3; break;
                case GX_VA_TEX4: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEX4; break;
                case GX_VA_TEX5: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEX5; break;
                case GX_VA_TEX6: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEX6; break;
                case GX_VA_TEX7: attributeFlag = GS_GFX_PARSE_ATTRIBUTE_TEX7; break;
                }
                if (desc->mComponentCount == GX_TEX_S) {
                    componentFlag = GS_GFX_PARSE_COMPONENTS_1;
                } else {
                    componentFlag = GS_GFX_PARSE_COMPONENTS_2;
                }
                break;
            }
        }

        if (desc->mInputType == GX_DIRECT) {
            attributeData = data;
            switch (formatFlag) {
            case GS_GFX_PARSE_FORMAT_U8:
            case GS_GFX_PARSE_FORMAT_S8:
                size = 1;
                break;
            case GS_GFX_PARSE_FORMAT_U16:
            case GS_GFX_PARSE_FORMAT_S16:
            case GS_GFX_PARSE_FORMAT_RGB565:
            case GS_GFX_PARSE_FORMAT_RGBA4:
                size = 2;
                break;
            case GS_GFX_PARSE_FORMAT_RGB8:
            case GS_GFX_PARSE_FORMAT_RGBA6:
                size = 3;
                break;
            case GS_GFX_PARSE_FORMAT_F32:
            case GS_GFX_PARSE_FORMAT_RGBX8:
            case GS_GFX_PARSE_FORMAT_RGBA8:
                size = 4;
                break;
            }
            switch (componentFlag) {
            case GS_GFX_PARSE_COMPONENTS_1:
                break;
            case GS_GFX_PARSE_COMPONENTS_2:
                size *= 2;
                break;
            case GS_GFX_PARSE_COMPONENTS_3:
                size *= 3;
                break;
            case GS_GFX_PARSE_COMPONENTS_4:
                size *= 4;
                break;
            }
            data += size;
        } else {
            if (desc->mInputType == GX_INDEX8) {
                index = *data;
                data += 1;
            } else {
                index = *(u16 *)data;
                data += 2;
            }
            attributeData = (u8 *)desc->mArrayBase + index * desc->mStride;
        }
        break;
    }

    if (callbacks->mVertex) {
        callbacks->mVertex(attributeFlag | formatFlag | componentFlag, attributeData, userContext);
    }
    return data;
}

u16 _dlCalculateStride(GSgfxVF *vf)
{
    GXVtxAttrFmtList *element = vf->fmt;
    GXVtxDescList *array = vf->desc;
    u16 stride = 0;
    u16 components;

    for (; array->attr != GX_VA_NULL; element++, array++) {
        switch (array->type) {
        case GX_DIRECT:
            switch (element->attr) {
            case GX_VA_PNMTXIDX:
                stride += 1;
                break;
            case GX_VA_POS:
                if (element->cnt == GX_POS_XY) {
                    components = 2;
                } else if (element->cnt == GX_POS_XYZ) {
                    components = 3;
                }
                switch (element->type) {
                case GX_U8:
                case GX_S8:
                    stride += components;
                    break;
                case GX_U16:
                case GX_S16:
                    stride += components * 2;
                    break;
                case GX_F32:
                    stride += components * 4;
                    break;
                }
                break;
            case GX_VA_NRM:
                if (element->cnt == GX_NRM_XYZ) {
                    components = 3;
                }
                switch (element->type) {
                case GX_S8:
                    stride += components;
                    break;
                case GX_S16:
                    stride += components * 2;
                    break;
                case GX_F32:
                    stride += components * 4;
                    break;
                }
                break;
            case GX_VA_CLR0:
            case GX_VA_CLR1:
                switch (element->type) {
                case GX_RGB565:
                case GX_RGBA4:
                    stride += 2;
                    break;
                case GX_RGB8:
                case GX_RGBA6:
                    stride += 3;
                    break;
                case GX_RGBX8:
                case GX_RGBA8:
                    stride += 4;
                    break;
                }
                break;
            case GX_VA_TEX0:
            case GX_VA_TEX1:
            case GX_VA_TEX2:
            case GX_VA_TEX3:
            case GX_VA_TEX4:
            case GX_VA_TEX5:
            case GX_VA_TEX6:
            case GX_VA_TEX7:
                if (element->cnt == GX_TEX_S) {
                    components = 1;
                } else if (element->cnt == GX_TEX_ST) {
                    components = 2;
                }
                switch (element->type) {
                case GX_U8:
                case GX_S8:
                    stride += components;
                    break;
                case GX_U16:
                case GX_S16:
                    stride += components * 2;
                    break;
                case GX_F32:
                    stride += components * 4;
                    break;
                }
                break;
            case GX_VA_NBT:
                break;
            }
            break;
        case GX_INDEX8:
            stride += 1;
            break;
        case GX_INDEX16:
            stride += 2;
            break;
        }
    }
    return stride;
}
