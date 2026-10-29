#include <new.h>
#include <types.h>
#include <GSvec.h>
#include <GSgfx.h>

// Header of the asset a lens flare is generated from; its flare elements and
// textures are described by the tables that follow.
struct GSlensFlareData {
    u32 magic;
    u16 textureCount;
    u16 elementCount;
    u32 flags;
};

// One lens flare effect: whether it is drawn, the sun it is generated around
// and the asset it was loaded from.
struct GSlensFlare {
    u8 visible;
    u8 _0001[3];
    GSvec sunPosition;
    f32 _0010;
    GSlensFlareData *data;
};

enum {
    // Set when the flares are drawn through a graphics layer, so that the
    // layer is hidden and shown along with them.
    LENS_FLARE_SYNC_LAYER = 0x8,
    // Graphics layer whose visibility follows the flares.
    LENS_FLARE_LAYER_ID = 5
};

// Set once the flare module has registered with the renderer; the controls
// below do nothing before that.
extern u8 _lfInitialized;

// Loaded lens flares, indexed by handle - 1.
extern GSlensFlare *_lfLensFlarePtrs[];

// Shows or hides one flare, and keeps the flare layer in step with it.
extern "C" void GSlensFlareSetVisible(u32 handle, bool visible)
{
    if (_lfInitialized == 0) {
        return;
    }
    if (handle == 0) {
        return;
    }
    u32 index = handle - 1;
    if (_lfLensFlarePtrs[index] == 0) {
        return;
    }
    if (visible == _lfLensFlarePtrs[index]->visible) {
        return;
    }
    _lfLensFlarePtrs[index]->visible = visible;
    if (_lfLensFlarePtrs[index]->data->flags & LENS_FLARE_SYNC_LAYER) {
        GSgfxLayer *layer = GSgfxLayerFindByID((GSgfxLayerID)LENS_FLARE_LAYER_ID);
        if (layer != 0) {
            layer->SetLayerVisibility(visible);
        }
    }
}

// Places the sun the flare is generated around.
extern "C" void GSlensFlareSetSunPosition(u32 handle, GSvec *position)
{
    if (_lfInitialized == 0) {
        return;
    }
    if (handle == 0) {
        return;
    }
    u32 index = handle - 1;
    GSlensFlare *flare = _lfLensFlarePtrs[index];
    if (flare == 0) {
        return;
    }
    flare->sunPosition.x = position->x;
    flare->sunPosition.y = position->y;
    flare->sunPosition.z = position->z;
}

// Releases one flare and drops it from the instance table.
extern "C" void GSlensFlareFree(u32 handle)
{
    if (_lfInitialized == 0) {
        return;
    }
    if (handle == 0) {
        return;
    }
    u32 index = handle - 1;
    if (_lfLensFlarePtrs[index] == 0) {
        return;
    }
    delete _lfLensFlarePtrs[index];
    _lfLensFlarePtrs[index] = 0;
}
