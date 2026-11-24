#include <flashcalc.h>
#include <GSprim.h>
#include <vecGraph.h>

struct _GRADIENT;

struct VEC_GRAPH_DATA {
    s32 fillEnabled[2];
    s32 fillIndex[2];
    s32 lineIndex[2];
    s16 left, top, right, bottom;
    s16 centerX, centerY, width, height;
    u32 color;
    FLASHCXFORM* colorTransform;
    u32 transformedWhite;
    u32 fillStyleCount;
    _FILLSTYLE* fillStyles;
    _LINESTYLE* lineStyles;
    _GRADIENT* gradient;
    s32 gradientX, gradientY;
    f32 gradientScale;
    f32 gradientAngle;
    f32 sinAngle, cosAngle;
    f32 sinNegAngle, cosNegAngle;
    u32 colors[256];
    s16 gradientCross[256][8];
};

extern VEC_GRAPH_DATA gVecDat;
extern u32 scanline_buffer[480][80];

s32 debugstep = 1;
GSgfxBlendType _type = GSGFX_BLEND_BLEND;
GSgfxBlendFactor _src = GSGFX_BLEND_SRC_ALPHA;
GSgfxBlendFactor _dst = GSGFX_BLEND_INV_SRC_ALPHA;
GSgfxBlendType _typemask = GSGFX_BLEND_BLEND;
GSgfxBlendFactor _srcmask = GSGFX_BLEND_SRC_ALPHA;
GSgfxBlendFactor _dstmask = GSGFX_BLEND_INV_SRC_ALPHA;

void vecExec(u32, _FILLSTYLEARRAY*, _LINESTYLEARRAY*, _SHAPERECORD*,
             FLASHMATRIXF*, FLASHMATRIXF*, FLASHCXFORM*);

enum VEC_FLAG {
    VEC_FLAG_MASK_BLEND = 0x40
};

void vectroGraphSetBlendMode(GSgfxBlendFactor source, GSgfxBlendFactor destination)
{
    _src = source;
    _dst = destination;
}

void vectorGraph(u32 characterId, _FILLSTYLEARRAY* fills, _LINESTYLEARRAY* lines,
                 _SHAPERECORD* records, FLASHMATRIXF* objectMatrix,
                 FLASHMATRIXF* parentMatrix, FLASHCXFORM* colorTransform, u8 flags)
{
    if (debugstep == 24)
        return;

    GSprimSetup2DRender(0);
    GSgfxLayer* layer = GSgfxLayerFindByID(GSgfxLayerGetActive());
    layer->dither(true);
    if (flags & VEC_FLAG_MASK_BLEND) {
        layer->blendMode(_typemask, _srcmask, _dstmask, true);
    } else {
        layer->blendMode(_type, _src, _dst, true);
    }
    GSprimSetQuadVertexOrder(GS_PRIM_VERTEX_ORDER_PERIMETER_QUADS);

    for (s32 i = 0; i < 2; ++i) {
        gVecDat.fillEnabled[i] = 0;
        gVecDat.fillIndex[i] = -1;
        gVecDat.lineIndex[i] = -1;
    }
    gVecDat.fillStyleCount = fills->count;
    gVecDat.colorTransform = colorTransform;
    gVecDat.transformedWhite = flcalcMultiTransformColor(colorTransform, 0xFFFFFFFF);
    gVecDat.fillStyles = fills->styles;
    gVecDat.lineStyles = lines->styles;
    gVecDat.top = 32767;
    gVecDat.left = 32767;
    gVecDat.bottom = -32768;
    gVecDat.right = -32768;
    memset(scanline_buffer, 0, sizeof(scanline_buffer));
    vecExec(characterId, fills, lines, records, objectMatrix, parentMatrix, colorTransform);
}
