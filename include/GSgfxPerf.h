#ifndef GS_GFX_PERF_H
#define GS_GFX_PERF_H

#include <GSgfxTypes.h>

// Converts a perfbar 0xRRGGBB colour to an opaque GSgfx colour.
extern "C" inline GSgfxColAu8 GSgfxPerfColorToGSgfxCol(u32 color)
{
    static GSgfxColAu8 c;
    c.b = color & 0xFF;
    c.g = (color >> 8) & 0xFF;
    c.r = (color >> 16) & 0xFF;
    c.a = 0xFF;
    return c;
}

#endif
