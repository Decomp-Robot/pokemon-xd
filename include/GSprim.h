#ifndef GSPRIM_H
#define GSPRIM_H
#include <GSgfx.h>
#include <GSmathTypes.h>
#include <GStexture.h>

#include <GSgfxTypes.h>

typedef struct GSrectS { s16 left, top, right, bottom; } GSrectS;

#ifdef __cplusplus
void GSprimSetup3DRender(GSgfxLayer *layer);

#endif

#ifdef __cplusplus
extern "C" {
#endif
enum {
    GS_PRIM_VERTEX_ORDER_TRIANGLE_STRIP = 0,
    GS_PRIM_VERTEX_ORDER_PERIMETER_QUADS = 1
};
void GSprimSetQuadVertexOrder(s32 order);
void GSprimDraw2DQuad(GSgfxColAu8 color, f32 x, f32 y, f32 width, f32 height);
void GSprimDraw2DQuad4V(GSvec2 *vertices, GSgfxColAu8 color);
void GSprimSetTextColor(GSgfxColAu8 color);
void GSprimSetTextWidth(f32 width);
void GSprimSetTextSize(f32 width, f32 height);
void GSprimWriteStr(f32 x, f32 y, const char *format, ...);
void GSprimDraw2DTexQuad4V(GSvec2 *vertices, GSvec2 *texCoords, GStexture *texture, GSgfxColAu8 color);

GSgfxLayerID GSgfxLayerGetActive(void);
void GSprimSetLineWidth(f32 width);
void GSprimSetBlendMode(s32 mode);
void GSprimWriteChar(f32 x, f32 y, char c);
void GSprimDraw2DTexQuad4VCA(GSvec2 *vertices, GSvec2 *texCoords, GStexture *texture,
                             const GSgfxColAu8 *color, GSrectS *clipArea);
void GSprimDraw2DQuadWH(GSgfxColAu8 color, f32 x, f32 y, f32 width, f32 height);
void GSprimDraw2DPixelListC(GSivec2 *positions, u32 count, GSgfxColAu8 *colors);
void GSprimDraw2DLineListC(GSvec2 *positions, u32 count, GSgfxColAu8 *colors);
void GSprimDraw2DVLine(GSgfxColAu8 color, f32 x, f32 top, f32 bottom);
void GSprimDraw2DHLine(GSgfxColAu8 color, f32 left, f32 right, f32 y);
void GSprimDraw2DLine(const GSgfxColAu8 *startColor, const GSgfxColAu8 *endColor, f32 x1, f32 y1, f32 x2, f32 y2);


enum GSprimExtraPrimType { GSprimPrimLines = 0xA8, GSprimPrimPoints = 0xB8 };

#ifndef GSmax
#define GSmax(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef GSmin
#define GSmin(a, b) ((a) < (b) ? (a) : (b))
#endif

static inline void GSprimLayerLineWidth(GSgfxLayer *layer, u8 width)
{
    layer->lineWidth(width, GX_TO_ZERO);
}

static inline void GSprimLayerPointSize(GSgfxLayer *layer, u8 size)
{
    layer->pointSize(size, GX_TO_ZERO);
}

static inline void GSprimLayerClipRect(GSgfxLayer *layer, GSrectS rect)
{
    layer->mTunitRegColor[GSGFX_TUNIT_REG0].signedFormat = 1;
    layer->mTunitRegColor[GSGFX_TUNIT_REG0].value.signedColor.r = GSmin(GSmax(rect.left, -1024), 1023);
    layer->mTunitRegColor[GSGFX_TUNIT_REG0].value.signedColor.g = GSmin(GSmax(rect.top, -1024), 1023);
    layer->mTunitRegColor[GSGFX_TUNIT_REG0].value.signedColor.b = GSmin(GSmax(rect.right, -1024), 1023);
    layer->mTunitRegColor[GSGFX_TUNIT_REG0].value.signedColor.a = GSmin(GSmax(rect.bottom, -1024), 1023);
    layer->mPassState[0].mRegColorFlags |= 1 << GSGFX_TUNIT_REG0;
    layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    layer->mPassState[1].mRegColorFlags |= 1 << GSGFX_TUNIT_REG0;
    layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_TUNIT;
}

// Shared state of the immediate-mode primitive renderer.
struct GSprimData {
    GSgfxLayer *layer; // Layer the queued draws are pushed to.
    s32 linePushMode;  // Width currently pushed: 0 none, 1 line, 2 text.
    f32 lineWidth;
    f32 textWidth;
    s32 quadVertexOrder;
    GSgfxPrimType primitiveType;
};

extern GSprimData primData;
#ifdef __cplusplus
}
#endif

void GSprimSetup2DRender(GSgfxLayer *layer);

#ifdef __cplusplus
void GSprimSetRenderLayer(GSgfxLayer *layer);
#endif
#endif
