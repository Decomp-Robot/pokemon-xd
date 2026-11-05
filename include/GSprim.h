#ifndef GSPRIM_H
#define GSPRIM_H
#include <GSgfx.h>
#include <GSmathTypes.h>
#include <GStexture.h>

#include <GSgfxTypes.h>

typedef struct GSrectS { s16 left, top, right, bottom; } GSrectS;

#ifdef __cplusplus
// TEV register-zero inputs used by clipped textured quads: color C0 and alpha A0.
static const GSgfxTunitCArg GSGFX_TUNIT_C_C0 = (GSgfxTunitCArg)2;
static const GSgfxTunitAArg GSGFX_TUNIT_A_A0 = (GSgfxTunitAArg)1;
#endif

#ifdef __cplusplus
extern "C" {
#endif
enum { GS_PRIM_VERTEX_ORDER_PERIMETER_QUADS = 1 };
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
void GSprimDraw2DQuadWH(const GSgfxColAu8 *color, f32 x, f32 y, f32 width, f32 height);
void GSprimDraw2DPixelListC(GSivec2 *positions, u32 count, GSgfxColAu8 *colors);
void GSprimDraw2DLineListC(GSvec2 *positions, u32 count, GSgfxColAu8 *colors);
void GSprimDraw2DVLine(const GSgfxColAu8 *color, f32 x, f32 top, f32 bottom);
void GSprimDraw2DHLine(const GSgfxColAu8 *color, f32 left, f32 right, f32 y);
void GSprimDraw2DLine(const GSgfxColAu8 *startColor, const GSgfxColAu8 *endColor, f32 x1, f32 y1, f32 x2, f32 y2);


// Deferred draw state the 2D primitive renderer pushes to the layer:
// line width, point size and clip rectangle, with their dirty bits.
// The fields below occupy the layer's deferred state area at offsets
// 0xD34 and 0x13FC.
struct GSgfxLayerDeferredState {
    u8 _0000[0xD34];
    u8 mLineWidth;
    u8 _0D35[3];
    u32 mLineWidthTexOffset;
    u8 mPointSize;
    u8 _0D3D[3];
    u32 mPointSizeTexOffset;
    u8 _0D44[0x13FC - 0xD44];
    u32 mClipRectEnabled;
    GSrectS mClipRect;
};

struct GSgfxPassStateClipView {
    u8 _0000[0x3C];
    u32 mClipRectFlags;
};

enum GSprimExtraPrimType { GSprimPrimLines = 0xA8, GSprimPrimPoints = 0xB8 };

#ifndef GSmax
#define GSmax(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef GSmin
#define GSmin(a, b) ((a) < (b) ? (a) : (b))
#endif

static inline void GSprimLayerLineWidth(GSgfxLayer *layer, u8 width)
{
    GSgfxLayerDeferredState *state = (GSgfxLayerDeferredState *)layer;
    state->mLineWidth = width;
    state->mLineWidthTexOffset = 0;
    layer->mPassState[0].mGeometryFlags |= 0x4;
    layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
    layer->mPassState[1].mGeometryFlags |= 0x4;
    layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
}

static inline void GSprimLayerPointSize(GSgfxLayer *layer, u8 size)
{
    GSgfxLayerDeferredState *state = (GSgfxLayerDeferredState *)layer;
    state->mPointSize = size;
    state->mPointSizeTexOffset = 0;
    layer->mPassState[0].mGeometryFlags |= 0x8;
    layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
    layer->mPassState[1].mGeometryFlags |= 0x8;
    layer->mPassState[1].mFlags |= GSGFX_PASS_FLAG_GROUP_A;
}

static inline void GSprimLayerClipRect(GSgfxLayer *layer, GSrectS rect)
{
    GSgfxLayerDeferredState *state = (GSgfxLayerDeferredState *)layer;
    GSgfxPassStateClipView *pass0 = (GSgfxPassStateClipView *)&layer->mPassState[0];
    GSgfxPassStateClipView *pass1 = (GSgfxPassStateClipView *)&layer->mPassState[1];
    state->mClipRectEnabled = 1;
    state->mClipRect.left = GSmin(GSmax(rect.left, -1024), 1023);
    state->mClipRect.top = GSmin(GSmax(rect.top, -1024), 1023);
    state->mClipRect.right = GSmin(GSmax(rect.right, -1024), 1023);
    state->mClipRect.bottom = GSmin(GSmax(rect.bottom, -1024), 1023);
    pass0->mClipRectFlags |= 0x2;
    layer->mPassState[0].mFlags |= GSGFX_PASS_FLAG_TUNIT;
    pass1->mClipRectFlags |= 0x2;
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

#endif
