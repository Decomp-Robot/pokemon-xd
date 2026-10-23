#include <WorldMap.h>

extern f32 worldMapFloatOne;

/* WorldMap::IsBlending - report whether the camera blend driven by
 * ControlCamera has reached full fade-in. MainSub spins on this until the
 * camera has settled on the requested target. */
bool WorldMap::IsBlending()
{
    bool blending = false;
    if (mCamWork->blend >= worldMapFloatOne) {
        blending = true;
    }
    return blending;
}
