#include <types.h>
#include <Pokemon.h>
#include <darkPokemon.h>
#include <GSgfx.h>
#include <time.h>
#include <GSprim.h>
#include <math.h>

f32 meterCount;

void _menuDrawReliveMeterSub(DarkPokemon *pokemon, long x, long y, long width, long height, u8 alpha,
                            GSgfxLayer *layer);
void _menuDrawReliveMeterSubDpValue(long initPoint, long darkPoint, long x, long y, long width, long height,
                                   u8 alpha, GSgfxLayer *layer);
extern "C" void menuDrawReliveMeter3(Pokemon *pokemon, long x, long y, long width, long height, u8 alpha,
                                    GSgfxLayerID layerID)
{
    GSgfxLayer *layer = GSgfxLayerFindByID(layerID);
    if (layer == 0) {
        return;
    }
    _menuDrawReliveMeterSub(pokemon->getDarkPokemon(), x, y, width, height, alpha, layer);
}

extern "C" void menuDrawReliveMeterByDarkPokemon(DarkPokemon *pokemon, long x, long y, long width, long height,
                                                 u8 alpha, GSgfxLayerID layerID)
{
    GSgfxLayer *layer = GSgfxLayerFindByID(layerID);
    if (layer == 0) {
        return;
    }
    _menuDrawReliveMeterSub(pokemon, x, y, width, height, alpha, layer);
}

extern "C" void menuDrawReliveMeter(Pokemon *pokemon, long width, long height, GSgfxLayerID layerID)
{
    if (!pokemon->isDarkPokemon()) {
        return;
    }
    GSgfxLayer *layer = GSgfxLayerFindByID(layerID);
    if (layer == 0) {
        return;
    }
    _menuDrawReliveMeterSub(pokemon->getDarkPokemon(), 0, 0, width, height, 255, layer);
}

extern "C" void menuDrawReliveUpdate(void)
{
    meterCount += timeGetLastFrameSec();
}

void _menuDrawReliveMeterSub(DarkPokemon *pokemon, long x, long y, long width, long height, u8 alpha,
                            GSgfxLayer *layer)
{
    long initPoint = pokemon->getInitDarkPointDirect();
    long darkPoint = pokemon->getDarkPointDirect();
    _menuDrawReliveMeterSubDpValue(initPoint, darkPoint, x, y, width, height, alpha, layer);
}

enum {
    METER_VERTEX_FORMAT_XY_S16_RGBA8 = 3,
    METER_LINE_WIDTH_ONE_PIXEL = 6
};

static f32 max_alpha = 255.0f;

static inline void meterRect(long left, long top, long width, long height, GSgfxColAu8 color,
                             GSgfxLayer *layer)
{
    layer->mPrimitiveType = GSGFX_QUADS;
    GSprimLayerLineWidth(layer, METER_LINE_WIDTH_ONE_PIXEL);
    layer->setRenderStateAndSelect(GSgfxVFGetPredef(METER_VERTEX_FORMAT_XY_S16_RGBA8));
    layer->beginSurface(4);
    GXWGFifo.s16 = left;
    GXWGFifo.s16 = top;
    GXWGFifo.u8 = color.r;
    GXWGFifo.u8 = color.g;
    GXWGFifo.u8 = color.b;
    GXWGFifo.u8 = color.a;
    GXWGFifo.s16 = (left + width);
    GXWGFifo.s16 = top;
    GXWGFifo.u8 = color.r;
    GXWGFifo.u8 = color.g;
    GXWGFifo.u8 = color.b;
    GXWGFifo.u8 = color.a;
    GXWGFifo.s16 = (left + width);
    GXWGFifo.s16 = (top + height);
    GXWGFifo.u8 = color.r;
    GXWGFifo.u8 = color.g;
    GXWGFifo.u8 = color.b;
    GXWGFifo.u8 = color.a;
    GXWGFifo.s16 = left;
    GXWGFifo.s16 = (top + height);
    GXWGFifo.u8 = color.r;
    GXWGFifo.u8 = color.g;
    GXWGFifo.u8 = color.b;
    GXWGFifo.u8 = color.a;
    layer->endSurface();
}

static inline void meterLine(long left, long top, long deltaX, long deltaY, GSgfxColAu8 color,
                             GSgfxLayer *layer)
{
    layer->mPrimitiveType = GSGFX_PRIM_LINES;
    GSprimLayerLineWidth(layer, METER_LINE_WIDTH_ONE_PIXEL);
    layer->setRenderStateAndSelect(GSgfxVFGetPredef(METER_VERTEX_FORMAT_XY_S16_RGBA8));
    layer->beginSurface(2);
    GXWGFifo.s16 = left;
    GXWGFifo.s16 = top;
    GXWGFifo.u8 = color.r;
    GXWGFifo.u8 = color.g;
    GXWGFifo.u8 = color.b;
    GXWGFifo.u8 = color.a;
    GXWGFifo.s16 = (left + deltaX);
    GXWGFifo.s16 = (top + deltaY);
    GXWGFifo.u8 = color.r;
    GXWGFifo.u8 = color.g;
    GXWGFifo.u8 = color.b;
    GXWGFifo.u8 = color.a;
    layer->endSurface();
}

void _menuDrawReliveMeterSubDpValue(long initPoint, long darkPoint, long x, long y, long width, long height,
                                   u8 alpha, GSgfxLayer *layer)
{
    if (initPoint == 0) {
        return;
    }
    long widthPercent = width * 100 / 210;
    long segmentWidth = widthPercent * 40 / 100;
    long segmentHeight = (height * 100 / 16) * 14 / 100;
    long gap = 2;
    if (widthPercent < 50) {
        gap /= 2;
    }
    long segmentIndex;
    GSgfxColAu8 color;
    color.r = color.g = color.b = 0;
    color.a = alpha;
    meterRect(x, y, segmentWidth * 5 + gap * 4 + 2, segmentHeight + 2, color, layer);
    color.r = color.g = color.b = 117;
    color.a = alpha;
    for (segmentIndex = 0; segmentIndex < 5; segmentIndex++) {
        long backgroundOffset = segmentIndex * (segmentWidth + gap) + 1;
        meterRect(backgroundOffset + x, y + 1, segmentWidth, segmentHeight, color, layer);
    }
    if (darkPoint != 0) {
        long percent = darkPoint * 100 / initPoint;
        long count = percent / 20 + 1;
        if (count > 5) {
            count = 5;
        }
        color.r = 134;
        color.g = 0;
        color.b = 225;
        color.a = alpha;
        for (segmentIndex = 0; segmentIndex < count; segmentIndex++) {
            long bottom = segmentHeight + y;
            long segmentOffset = segmentIndex * (segmentWidth + gap) + 1;
            color.r = 134;
            color.g = 0;
            color.b = 225;
            color.a = alpha;
            if (count == segmentIndex + 1) {
                percent -= segmentIndex * 20;
                meterRect(segmentOffset + x, y + 1, segmentWidth * percent / 20,
                          segmentHeight, color, layer);
                color.r = 174;
                color.g = 54;
                color.b = 225;
                color.a = alpha;
                meterLine(segmentOffset + x, y + 2, segmentWidth * percent / 20, 0, color, layer);
                color.r = 108;
                color.g = 0;
                color.b = 181;
                color.a = alpha;
                meterLine(segmentOffset + x, bottom + 1, segmentWidth * percent / 20, 0, color, layer);
            } else {
                meterRect(segmentOffset + x, y + 1, segmentWidth,
                          segmentHeight, color, layer);
                color.r = 174;
                color.g = 54;
                color.b = 225;
                color.a = alpha;
                meterLine(segmentOffset + x, y + 2, segmentWidth, 0, color, layer);
                color.r = 108;
                color.g = 0;
                color.b = 181;
                color.a = alpha;
                meterLine(segmentOffset + x, bottom + 1, segmentWidth, 0, color, layer);
            }
        }
    } else {
        color.r = color.g = color.b = 255;
        f32 pulse = 6.2831855f * meterCount;
        pulse = 1.0f + (f32)sin(pulse);
        pulse = 150.0f * pulse;
        if (pulse >= max_alpha) {
            pulse = max_alpha;
        }
        u16 pulseAlpha = pulse;
        pulseAlpha = pulseAlpha * alpha / 255;
        for (segmentIndex = 0; segmentIndex < 5; segmentIndex++) {
            long bottom = segmentHeight + y;
            long segmentOffset = segmentIndex * (segmentWidth + gap) + 1;
            color.a = pulseAlpha;
            meterRect(segmentOffset + x, y + 1, segmentWidth, segmentHeight, color, layer);
            color.a = pulseAlpha * 75 / 100;
            meterLine(segmentOffset + x, y + 1, segmentWidth, 0, color, layer);
            meterLine(x + (segmentOffset + 1), y + 1, 0, segmentHeight, color, layer);
            meterLine(x + (segmentOffset + segmentWidth), y + 1, 0, segmentHeight, color, layer);
            meterLine(segmentOffset + x, bottom + 1, segmentWidth, 0, color, layer);
        }
    }
}
