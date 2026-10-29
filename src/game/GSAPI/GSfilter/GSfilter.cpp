#include <dolphin/os/OSCache.h>
#include <GSgfx.h>
#include <GSmtx.h>
#include <GSmem.h>
#include <libc/memory.h>

/* Full-screen colour overlay. The module owns a pool of filter slots and one
 * four-vertex quad per slot, which the render pass blends over the finished
 * frame. The game creates the slots, recolours them while they live and frees
 * them again; the overlay itself is drawn on the translucent pass of its own
 * layer. */

struct GSfilter;

/* Interface the game drives: one slot pool per process, the slot it may paint
 * without creating one and the calls that create, recolour and free a slot. */
extern "C" {
void GSfilterInitialise(u8 maxFilters);
GSfilter *GSfilterCreate(GScolor *color);
void GSfilterSetColor(GSfilter *filter, GScolor *color);
void GSfilterFree(GSfilter *filter);
extern GSfilter *gsDefaultFilterPtr;
}

/* The layer the overlay draws on. The layer set is configured with the id the
 * game reserves for the full-screen overlays. */
enum GSfilterLayerID {
    GS_FILTER_LAYER = 6,
};

/* Screen the corner quad is laid over and the corners it spans: the quad is
 * built from the four corners of the 640x480 projection, so a slot only needs
 * a colour to be drawn. */
enum {
    GS_FILTER_SCREEN_WIDTH = 640,
    GS_FILTER_SCREEN_HEIGHT = 480,
    GS_FILTER_CORNER_COUNT = 4,
};

/* Priority the overlay takes in the render module list: it has to run after
 * the world has been drawn and stays on top of the other overlays. */
enum { GS_FILTER_RENDER_MODULE_ID = 156 };

/* Vertex format of the quad: one position index that selects a corner of the
 * shared corner array and one packed colour. */
enum {
    GS_FILTER_VERTEX_POSITION = 9,
    GS_FILTER_VERTEX_COLOR = 11,
};

/* One filter slot the game holds a pointer to. The colour lives in the colour
 * table next to the slots, so a slot is only the index, whether it is handed
 * out and whether it takes part in the pass. */
struct GSfilter {
    u8 mColorIndex;
    u8 mActive;
    u8 mVisible;
};

/* Slots, their colours, the corner array and the vertex format every slot is
 * drawn with, plus the counters the pass loops over. */
struct GSfilterData {
    GSfilter *mFilters;
    GScolor *mColors;
    u16 mCorners[GS_FILTER_CORNER_COUNT][2];
    GSgfxVF mVertexFormat;
    u8 mMaxFilters;
    u8 mFilterCount;
    u8 mVisibleCount;
};

static GSfilterData filtersData;

/* Slot the game may use without creating one. */
GSfilter *gsDefaultFilterPtr;

/* Whether the overlay has been added to the render module list of this
 * process. */
static u8 filtersRegistered;

static void _filterRender(GSgfxLayer *layer, GSgfxLayerPass pass);

/* Takes the first free slot, paints it with the colour and counts it in. */
static inline GSfilter *filterCreateSlot(GScolor *color)
{
    GSfilter *filter;
    GScolor *dest;
    u8 maxFilters = filtersData.mMaxFilters;
    u8 i;

    if (filtersData.mFilterCount < maxFilters) {
        filter = filtersData.mFilters;
        for (i = 0; i < maxFilters; ++i, ++filter) {
            if (filter->mActive) {
                continue;
            }
            dest = &filtersData.mColors[i];
            dest->channels.r = color->channels.r;
            dest->channels.g = color->channels.g;
            dest->channels.b = color->channels.b;
            dest->channels.a = color->channels.a;
            if (color->channels.a != 0 && !filter->mVisible) {
                filter->mVisible = 1;
                filtersData.mVisibleCount++;
            }
            filter->mActive = 1;
            filtersData.mFilterCount++;
            return filter;
        }
    }
    return 0;
}

/* Builds the slot pool, the corner array and the vertex format for maxFilters
 * slots, takes the default slot and adds the overlay to the render module list
 * once. Without memory the overlay stays unusable and the game gets no slot. */
void GSfilterInitialise(u8 maxFilters)
{
    GScolor color;
    GSfilter *filter;
    u8 *data;
    u8 i;

    memset(&filtersData, 0, sizeof(filtersData));
    data = (u8 *)GSmemAlloc(maxFilters * (sizeof(GSfilter) + sizeof(GScolor)));
    if (data == 0) {
        return;
    }
    memset(data, 0, maxFilters * (sizeof(GSfilter) + sizeof(GScolor)));
    filtersData.mFilters = (GSfilter *)data;
    filtersData.mColors = (GScolor *)(data + maxFilters * sizeof(GSfilter));
    filtersData.mCorners[0][0] = 0;
    filtersData.mCorners[0][1] = 0;
    filtersData.mCorners[1][0] = 0;
    filtersData.mCorners[1][1] = GS_FILTER_SCREEN_HEIGHT;
    filtersData.mCorners[2][0] = GS_FILTER_SCREEN_WIDTH;
    filtersData.mCorners[2][1] = GS_FILTER_SCREEN_HEIGHT;
    filtersData.mCorners[3][0] = GS_FILTER_SCREEN_WIDTH;
    filtersData.mCorners[3][1] = 0;
    filtersData.mMaxFilters = maxFilters;
    GSgfxVFInit(&filtersData.mVertexFormat);
    GSgfxVFCreate(&filtersData.mVertexFormat);
    GSgfxVFSetElement(&filtersData.mVertexFormat, GS_FILTER_VERTEX_POSITION, 0, 2, 0);
    GSgfxVFSetElement(&filtersData.mVertexFormat, GS_FILTER_VERTEX_COLOR, 1, 5, 0);
    GSgfxVFSetArray(&filtersData.mVertexFormat, GS_FILTER_VERTEX_POSITION, 2, filtersData.mCorners,
                    4, sizeof(filtersData.mCorners));
    filter = filtersData.mFilters;
    for (i = 0; i < maxFilters; ++i, ++filter) {
        filter->mColorIndex = i;
    }
    color.channels.r = 0;
    color.channels.g = 0;
    color.channels.b = 0;
    color.channels.a = 0;
    gsDefaultFilterPtr = filterCreateSlot(&color);
    if (!filtersRegistered) {
        GSgfxRenderModule module = {
            0,
            0,
            _filterRender,
            0,
            0,
            0,
            GSGFX_RENDER_MODULE_SIGNATURE,
        };

        GSgfxRenderModuleRegister(GS_FILTER_RENDER_MODULE_ID, &module, 0);
        filtersRegistered = 1;
    }
}

/* Render hook of the overlay: sets the 640x480 projection and the blend state
 * up and submits one quad per slot that has something to show. */
static void _filterRender(GSgfxLayer *layer, GSgfxLayerPass pass)
{
    u8 maxFilters;
    GSfilter *filter;
    u8 i;
    GScolor *color;
    f32 matrix[4][4];

    if (layer->mLayerID != GS_FILTER_LAYER || pass != GSGFX_LAYER_PASS_XLU) {
        return;
    }
    if (filtersData.mFilterCount == 0) {
        return;
    }
    if (filtersData.mVisibleCount == 0) {
        return;
    }
    maxFilters = filtersData.mMaxFilters;
    filter = filtersData.mFilters;
    GSmtx44Ortho(matrix, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    layer->orthographicProjection(matrix);
    layer->mMatrixStack->identity();
    layer->projectionDepthRange(0.0f, 30000.0f);
    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, true);
    layer->pixelMode(true, true, false);
    layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS,
                        0);
    layer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    layer->cullMode(GSGFX_CULL_NONE);
    layer->setRenderStateAndSelect(&filtersData.mVertexFormat);
    layer->mPrimitiveType = GSGFX_QUADS;
    layer->beginSurface((u16)(filtersData.mVisibleCount * GS_FILTER_CORNER_COUNT));

    color = filtersData.mColors;
    for (i = 0; i < maxFilters; ++i, ++filter, ++color) {
        if (!filter->mVisible) {
            continue;
        }
        GSgfxFifo.u8 = 0;
        GSgfxFifo.u32 = color->rgba;
        GSgfxFifo.u8 = 1;
        GSgfxFifo.u32 = color->rgba;
        GSgfxFifo.u8 = 2;
        GSgfxFifo.u32 = color->rgba;
        GSgfxFifo.u8 = 3;
        GSgfxFifo.u32 = color->rgba;
    }
    layer->endSurface();
}

/* Recolours a live slot. The colour is flushed to main memory because the
 * renderer reads it; a slot only takes part in the pass while its colour has
 * alpha. */
void GSfilterSetColor(GSfilter *filter, GScolor *color)
{
    GScolor *dest;

    if (!filter->mActive) {
        return;
    }
    dest = &filtersData.mColors[filter->mColorIndex];
    dest->channels.r = color->channels.r;
    dest->channels.g = color->channels.g;
    dest->channels.b = color->channels.b;
    dest->channels.a = color->channels.a;
    DCFlushRange(dest, sizeof(GScolor));
    if (color->channels.a != 0) {
        if (!filter->mVisible) {
            filter->mVisible = 1;
            filtersData.mVisibleCount++;
        }
    } else if (filter->mVisible) {
        filter->mVisible = 0;
        filtersData.mVisibleCount--;
    }
}

/* Hands a slot back to the pool. The colour stays where it is and is
 * overwritten by the next slot that gets it. */
void GSfilterFree(GSfilter *filter)
{
    if (!filter->mActive) {
        return;
    }
    filter->mActive = 0;
    if (filter->mVisible) {
        filter->mVisible = 0;
        filtersData.mVisibleCount--;
    }
    filtersData.mFilterCount--;
}

/* Hands out a slot painted with the colour, or null when the pool is full. */
GSfilter *GSfilterCreate(GScolor *color)
{
    GSfilter *filter;
    GScolor *dest;
    u8 maxFilters = filtersData.mMaxFilters;
    u8 i;

    if (filtersData.mFilterCount < maxFilters) {
        filter = filtersData.mFilters;
        for (i = 0; i < maxFilters; ++i, ++filter) {
            if (filter->mActive) {
                continue;
            }
            dest = &filtersData.mColors[i];
            dest->channels.r = color->channels.r;
            dest->channels.g = color->channels.g;
            dest->channels.b = color->channels.b;
            dest->channels.a = color->channels.a;
            if (color->channels.a != 0 && !filter->mVisible) {
                filter->mVisible = 1;
                filtersData.mVisibleCount++;
            }
            filter->mActive = 1;
            filtersData.mFilterCount++;
            return filter;
        }
    }
    return 0;
}
