#include <GSprim.h>

static inline void primEmitVertex(f32 x, f32 y, const GSgfxColAu8 &color)
{
    GSgfxFifo.f32 = x;
    GSgfxFifo.f32 = y;
    GSgfxColAu8 packedColor = color;
    GSgfxFifo.u32 = *(u32 *)&packedColor;
}

static inline void primEmitVertex(GSvec2 pos, const GSgfxColAu8 &color)
{
    GSgfxFifo.f32 = pos.x;
    GSgfxFifo.f32 = pos.y;
    GSgfxColAu8 packedColor = color;
    GSgfxFifo.u32 = *(u32 *)&packedColor;
}

// Emits the four corners of an axis-aligned quad as one primitive.
static inline void primDrawQuadVerts(f32 x, f32 y, f32 width, f32 height, const GSgfxColAu8 &color)
{
    {
        GSgfxFifo.f32 = x;
        GSgfxFifo.f32 = y;
        GSgfxColAu8 packedColor = color;
        GSgfxFifo.u32 = *(u32 *)&packedColor;
    }
    {
        GSgfxFifo.f32 = x + width;
        GSgfxFifo.f32 = y;
        GSgfxColAu8 packedColor = color;
        GSgfxFifo.u32 = *(u32 *)&packedColor;
    }
    {
        GSgfxFifo.f32 = x;
        GSgfxFifo.f32 = y + height;
        GSgfxColAu8 packedColor = color;
        GSgfxFifo.u32 = *(u32 *)&packedColor;
    }
    {
        GSgfxFifo.f32 = x + width;
        GSgfxFifo.f32 = y + height;
        GSgfxColAu8 packedColor = color;
        GSgfxFifo.u32 = *(u32 *)&packedColor;
    }
}

// Immediate-mode 2D primitives pushed straight into the write-gather FIFO of
// the layer tracked by primData.

static inline void primDrawTexCoord(f32 x, f32 y)
{
    GSgfxFifo.f32 = x;
    GSgfxFifo.f32 = y;
}

void GSprimDraw2DTexQuad4VCA(GSvec2 *vertices, GSvec2 *texCoords, GStexture *texture,
                             const GSgfxColAu8 *color, GSrectS *clipArea)
{
    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(6));
    primData.layer->texSelect(0, texture);
    GSprimLayerClipRect(primData.layer, *clipArea);
    primData.layer->tunitColorOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true,
                        GSGFX_TUNIT_REG_PREV);
    primData.layer->tunitColorArg(0, GSGFX_TUNIT_C_ZERO, GSGFX_TUNIT_C_RASC, GSGFX_TUNIT_C_TEXC, GSGFX_TUNIT_C_C0);
    primData.layer->tunitAlphaOp(0, GSGFX_TUNIT_OP_ADD, GSGFX_TUNIT_BIAS_ZERO, GSGFX_TUNIT_SCALE_ONE, true,
                        GSGFX_TUNIT_REG_PREV);
    primData.layer->tunitAlphaArg(0, GSGFX_TUNIT_A_ZERO, GSGFX_TUNIT_A_RASA, GSGFX_TUNIT_A_TEXA, GSGFX_TUNIT_A_A0);
    primData.layer->mPrimitiveType = primData.primitiveType;
    primData.layer->beginSurface(4);

    primEmitVertex(vertices[0], *color);
    primDrawTexCoord(texCoords[0].x, texCoords[0].y);
    primEmitVertex(vertices[1], *color);
    primDrawTexCoord(texCoords[1].x, texCoords[1].y);
    primEmitVertex(vertices[2], *color);
    primDrawTexCoord(texCoords[2].x, texCoords[2].y);
    primEmitVertex(vertices[3], *color);
    primDrawTexCoord(texCoords[3].x, texCoords[3].y);
    primData.layer->endSurface();
}

void GSprimDraw2DTexQuad4V(GSvec2 *vertices, GSvec2 *texCoords, GStexture *texture, GSgfxColAu8 color)
{
    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(6));
    primData.layer->texSelect(0, texture);
    primData.layer->mPrimitiveType = primData.primitiveType;
    primData.layer->beginSurface(4);

    primEmitVertex(vertices[0], color);
    primDrawTexCoord(texCoords[0].x, texCoords[0].y);
    primEmitVertex(vertices[1], color);
    primDrawTexCoord(texCoords[1].x, texCoords[1].y);
    primEmitVertex(vertices[2], color);
    primDrawTexCoord(texCoords[2].x, texCoords[2].y);
    primEmitVertex(vertices[3], color);
    primDrawTexCoord(texCoords[3].x, texCoords[3].y);
    primData.layer->endSurface();
}

extern "C" void GSprimDraw2DTexQuad(GStexture *texture, const GSgfxColAu8 *color, f32 x1, f32 y1, f32 x2, f32 y2)
{
    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(6));
    primData.layer->texSelect(0, texture);
    primData.layer->mPrimitiveType = primData.primitiveType;
    primData.layer->beginSurface(4);

    primEmitVertex(x1, y1, *color);
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 0.0f;
    primEmitVertex(x2, y1, *color);
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.f32 = 0.0f;
    primEmitVertex(x1, y2, *color);
    GSgfxFifo.f32 = 0.0f;
    GSgfxFifo.f32 = 1.0f;
    primEmitVertex(x2, y2, *color);
    GSgfxFifo.f32 = 1.0f;
    GSgfxFifo.f32 = 1.0f;
    primData.layer->endSurface();
}

void GSprimDraw2DQuad4V(GSvec2 *vertices, GSgfxColAu8 color)
{
    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(2));
    primData.layer->mPrimitiveType = primData.primitiveType;
    primData.layer->beginSurface(4);

    primEmitVertex(vertices[0], color);
    primEmitVertex(vertices[1], color);
    primEmitVertex(vertices[2], color);
    primEmitVertex(vertices[3], color);
    primData.layer->endSurface();
}

void GSprimDraw2DQuadWH(const GSgfxColAu8 *color, f32 x, f32 y, f32 width, f32 height)
{
    GSgfxColAu8 col = *color;

    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(2));
    primData.layer->mPrimitiveType = primData.primitiveType;
    primData.layer->beginSurface(4);

    primDrawQuadVerts(x, y, width, height, col);
    primData.layer->endSurface();
}

void GSprimDraw2DQuad(GSgfxColAu8 color, f32 x1, f32 y1, f32 x2, f32 y2)
{
    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(2));
    primData.layer->mPrimitiveType = primData.primitiveType;
    primData.layer->beginSurface(4);

    primEmitVertex(x1, y1, color);
    primEmitVertex(x2, y1, color);
    primEmitVertex(x1, y2, color);
    primEmitVertex(x2, y2, color);
    primData.layer->endSurface();
}

void GSprimDraw2DPixelListC(GSivec2 *positions, u32 count, GSgfxColAu8 *colors)
{
    u32 i;

    if (count == 0)
        return;

    GSprimLayerPointSize(primData.layer, 6);
    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(3));
    primData.layer->mPrimitiveType = (GSgfxPrimType)GSprimPrimPoints;
    primData.layer->beginSurface((u16)count);

    for (i = count; i != 0; --i) {
        s16 x = positions->x;
        s16 y = positions->y;
        GSgfxFifo.s16 = x;
        GSgfxFifo.s16 = y;
        GSgfxFifo.u32 = *(u32 *)colors;
        ++positions;
        ++colors;
    }
    primData.layer->endSurface();
}

void GSprimDraw2DLineListC(GSvec2 *positions, u32 count, GSgfxColAu8 *colors)
{
    u32 i;

    if (count < 1)
        return;

    if (primData.linePushMode != 1) {
        GSprimLayerLineWidth(primData.layer, (u8)(6.0f * primData.lineWidth));
        primData.linePushMode = 1;
    }

    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(2));
    primData.layer->mPrimitiveType = (GSgfxPrimType)GSprimPrimLines;
    primData.layer->beginSurface((u16)(count * 2));

    for (i = count; i != 0; --i) {
        primEmitVertex(positions[0], colors[0]);
        primEmitVertex(positions[1], colors[1]);
        positions += 2;
        colors += 2;
    }
    primData.layer->endSurface();
}

// Emits both endpoints of a line primitive.
static inline void primDrawLineVerts(f32 x1, f32 y1, GSgfxColAu8 c1, f32 x2, f32 y2, GSgfxColAu8 c2)
{
    {
        GSgfxFifo.f32 = x1;
        GSgfxFifo.f32 = y1;
        GSgfxColAu8 packedColor = c1;
        GSgfxFifo.u32 = *(u32 *)&packedColor;
    }
    {
        GSgfxFifo.f32 = x2;
        GSgfxFifo.f32 = y2;
        GSgfxColAu8 packedColor = c2;
        GSgfxFifo.u32 = *(u32 *)&packedColor;
    }
}

void GSprimDraw2DVLine(const GSgfxColAu8 *color, f32 x, f32 top, f32 bottom)
{
    GSgfxColAu8 bottomColor = *color;
    GSgfxColAu8 topColor = *color;

    if (primData.linePushMode != 1) {
        GSprimLayerLineWidth(primData.layer, (u8)(6.0f * primData.lineWidth));
        primData.linePushMode = 1;
    }

    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(2));
    primData.layer->mPrimitiveType = (GSgfxPrimType)GSprimPrimLines;
    primData.layer->beginSurface(2);

    primDrawLineVerts(x, top, topColor, x, bottom, bottomColor);
    primData.layer->endSurface();
}

void GSprimDraw2DHLine(const GSgfxColAu8 *color, f32 left, f32 right, f32 y)
{
    GSgfxColAu8 rightColor = *color;
    GSgfxColAu8 leftColor = *color;

    if (primData.linePushMode != 1) {
        GSprimLayerLineWidth(primData.layer, (u8)(6.0f * primData.lineWidth));
        primData.linePushMode = 1;
    }

    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(2));
    primData.layer->mPrimitiveType = (GSgfxPrimType)GSprimPrimLines;
    primData.layer->beginSurface(2);

    primDrawLineVerts(left, y, leftColor, right, y, rightColor);
    primData.layer->endSurface();
}

void GSprimDraw2DLine(const GSgfxColAu8 *startColor, const GSgfxColAu8 *endColor, f32 x1, f32 y1, f32 x2, f32 y2)
{
    if (primData.linePushMode != 1) {
        GSprimLayerLineWidth(primData.layer, (u8)(6.0f * primData.lineWidth));
        primData.linePushMode = 1;
    }

    primData.layer->setRenderStateAndSelect(GSgfxVFGetPredef(2));
    primData.layer->mPrimitiveType = (GSgfxPrimType)GSprimPrimLines;
    primData.layer->beginSurface(2);

    primEmitVertex(x1, y1, *startColor);
    primEmitVertex(x2, y2, *endColor);
    primData.layer->endSurface();
}
