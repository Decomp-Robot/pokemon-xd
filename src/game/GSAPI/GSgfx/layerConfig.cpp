#include <GSgfx.h>

/* A layer reserves its buffers in whole 32-byte blocks, so the sizes a cached
 * layer was built with have to be compared the same way. */
#define GS_LAYER_BUFFER_ALIGNMENT(size) (((size) + 31) & ~31)

/* Slots handed out to the layers that do not pin an id of their own. */
enum GSgfxDynamicLayerIDRange { GSGFX_LAYER_DYNAMIC_ID_FIRST = 8, GSGFX_LAYER_DYNAMIC_ID_END = 16 };


/* The registered layer carrying the given id, or null when none does. */
static inline GSgfxLayer *_findLayer(GSgfxLayerID id)
{
    GSgfxLayer *layer;

    for (layer = GSgfxLayer::GetLayerList(); layer != NULL; layer = layer->mNext) {
        if (layer->mLayerID == id) {
            return layer;
        }
    }
    return NULL;
}

/* The next dynamic slot no layer occupies, or GSGFX_LAYER_INVALID when every
 * one of them is in use. */
static inline GSgfxLayerID _findFreeLayerID(void)
{
    u32 id;

    for (id = GSGFX_LAYER_DYNAMIC_ID_FIRST; id < GSGFX_LAYER_DYNAMIC_ID_END; id++) {
        GSgfxLayer *layer;
        bool taken;

        taken = false;
        for (layer = GSgfxLayer::GetLayerList(); layer != NULL; layer = layer->mNext) {
            if (id == layer->mLayerID) {
                taken = true;
                break;
            }
        }
        if (!taken) {
            return (GSgfxLayerID)id;
        }
    }
    return GSGFX_LAYER_INVALID;
}

void GSgfxLayerConfig::Move(GSgfxLayerID from, GSgfxLayerID to, GSgfxLayerOffset offset)
{
    GSgfxLayer *fromLayer = _findLayer(from);
    GSgfxLayer *toLayer;

    if (fromLayer == NULL) {
        return;
    }
    toLayer = _findLayer(to);
    if (toLayer == NULL) {
        /* Without an anchor the layer only takes a place in the order. */
        switch (offset) {
        case GS_LAYER_INSERT_BEFORE:
            offset = GS_LAYER_INSERT_FIRST;
            break;
        case GS_LAYER_INSERT_AFTER:
            offset = GS_LAYER_INSERT_LAST;
            break;
        }
    }
    fromLayer->SetPriority(offset, toLayer);
}

void GSgfxLayerConfig::Remove(GSgfxLayerID id)
{
    GSgfxLayer *layer = _findLayer(id);

    if (layer != NULL) {
        delete layer;
    }
}

GSgfxLayerID GSgfxLayerConfig::Add(char *name, unsigned long pass0FifoSize, unsigned long pass1FifoSize,
                                   unsigned long matrixStackCapacity, GSgfxLayerType type, bool clearColor,
                                   bool clearAlpha, bool clearDepth, GSgfxLayerID relativeLayerID,
                                   GSgfxLayerOffset offset)
{
    GSgfxLayer *layer;
    GSgfxLayer *relativeLayer;
    static _GSgfxLayerData data;

    layer = GSgfxLayerFindByName(name);

    if (layer != NULL) {
        /* A cached layer is only worth keeping when it was built for these
         * reservations, otherwise its buffers are the wrong size. */
        if (GS_LAYER_BUFFER_ALIGNMENT(pass0FifoSize) != layer->mDrawPasses[0].bufferSize
            || GS_LAYER_BUFFER_ALIGNMENT(pass1FifoSize) != layer->mDrawPasses[1].bufferSize
            || GS_LAYER_BUFFER_ALIGNMENT(matrixStackCapacity)
                   != ((u8 *)layer->mMatrixStack->mLastEntry - (u8 *)layer->mMatrixStack->mEntries)
                          / sizeof(GSmtx)
                          + 1) {
            delete layer;
            layer = NULL;
        }
    }
    if (layer == NULL) {
        GSgfxLayerID id = _findFreeLayerID();

        if (id != GSGFX_LAYER_INVALID) {
            layer = new GSgfxLayer(id, name, pass0FifoSize, pass1FifoSize, matrixStackCapacity,
                                   GS_GFX_LAYER_NORMAL);
        }
        if (layer == NULL) {
            return GSGFX_LAYER_INVALID;
        }
    }

    data.id = layer->mLayerID;
    data.name = name;
    data.type = type;
    data.clearColor = clearColor;
    data.clearAlpha = clearAlpha;
    data.clearDepth = clearDepth;
    data.visible = true;
    data.pass0FifoSize = pass0FifoSize;
    data.pass1FifoSize = pass1FifoSize;
    data.matrixStackCapacity = matrixStackCapacity;

    relativeLayer = _findLayer(relativeLayerID);
    layer->SetLayerType(data.type);
    layer->SetWantClearFlags(data.clearColor, data.clearAlpha, data.clearDepth);
    if (relativeLayer != NULL) {
        layer->SetPriority(offset, relativeLayer);
    } else {
        /* Without an anchor the insertion offset only tells the layer which
         * end of the order it belongs to. */
        switch (offset) {
        case GS_LAYER_INSERT_BEFORE:
        case GS_LAYER_INSERT_FIRST:
            layer->SetPriority(GS_LAYER_INSERT_FIRST, NULL);
            break;
        case GS_LAYER_INSERT_AFTER:
        case GS_LAYER_INSERT_LAST:
            layer->SetPriority(GS_LAYER_INSERT_LAST, NULL);
            break;
        default:
            layer->SetPriority(GS_LAYER_INSERT_NONE, NULL);
            break;
        }
    }
    layer->SetPassVisibility(true, true);
    layer->mVisibleHolds = 0;
    layer->SetLayerVisibility(data.visible);
    return layer->mLayerID;
}

void GSgfxLayerConfig::Setup(_GSgfxLayerData *config)
{
    GSgfxLayer *layer;
    GSgfxLayer *previous;
    _GSgfxLayerData *entry;

    for (layer = GSgfxLayer::GetLayerList(); layer != NULL;) {
        GSgfxLayer *next = layer->mNext;

        delete layer;
        layer = next;
    }
    /* Build the registered layers in the order given by the configuration table. */
    entry = config;
    previous = NULL;
    while (entry->id != GSGFX_LAYER_INVALID) {
        layer = _findLayer(entry->id);
        if (layer == NULL) {
            if (entry->id == GSGFX_LAYER_AUTO) {
                GSgfxLayerID id = _findFreeLayerID();

                if (id != GSGFX_LAYER_INVALID) {
                    layer = new GSgfxLayer(id, entry->name, entry->pass0FifoSize,
                                           entry->pass1FifoSize, entry->matrixStackCapacity,
                                           GS_GFX_LAYER_NORMAL);
                }
            } else {
                layer = new GSgfxLayer(entry->id, entry->name, entry->pass0FifoSize,
                                       entry->pass1FifoSize, entry->matrixStackCapacity,
                                       GS_GFX_LAYER_NORMAL);
            }
            if (layer == NULL) {
                entry++;
                continue;
            }
        }
        layer->SetLayerType(entry->type);
        layer->SetWantClearFlags(entry->clearColor, entry->clearAlpha, entry->clearDepth);
        if (previous != NULL) {
            layer->SetPriority(GS_LAYER_INSERT_AFTER, previous);
        } else {
            layer->SetPriority(GS_LAYER_INSERT_LAST, NULL);
        }
        layer->SetPassVisibility(true, true);
        layer->mVisibleHolds = 0;
        layer->SetLayerVisibility(entry->visible);
        previous = layer;
        entry++;
    }
    GSgfxLayerSetActive(GSgfxLayer::GetLayerList()->mLayerID);
}
