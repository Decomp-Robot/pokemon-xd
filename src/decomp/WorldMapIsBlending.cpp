#include <WorldMap.h>

extern f32 lbl_804EBEF8;

/* WorldMap::IsBlending - report whether the camera blend driven by
 * ControlCamera has reached full fade-in. MainSub spins on this until the
 * camera has settled on the requested target. */
bool WorldMap::IsBlending()
{
    bool blending = false;
    if (mCamWork->blend >= lbl_804EBEF8) {
        blending = true;
    }
    return blending;
}
