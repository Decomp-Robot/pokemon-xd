#include <WorldMap.h>

extern const f32 worldMapFloatZero;
extern const f32 worldMapFloatOne;

/* WorldMap::IsBlending -- report whether the camera blend driven by
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

/* WorldMap::SetCameraNextWork -- stage the pending camera target. */
void WorldMap::SetCameraNextWork(GSvec* pos, f32 distance, f32 rotY, f32 rotX, f32 fov)
{
    WorldMapCamWork* work = mCamWork;

    if (pos != 0) {
        work->nextPos = *pos;
    }
    if (distance != worldMapFloatZero) {
        work->nextDist = distance;
    }
    work->nextRotY = rotY;
    work->nextRotX = rotX;
    work->nextFov = fov;
    work->nextBlendStart = worldMapFloatZero;
    work->nextBlendEnd = worldMapFloatOne;
    work->nextBlend = worldMapFloatZero;
}

/* WorldMap::SetCameraWork -- stage the current camera target.
 * The position is skipped when null; the distance is kept when it reads
 * as the zero constant, otherwise it is staged with the angle parameters.
 * The current blend range restarts from the tuned constants. */
void WorldMap::SetCameraWork(GSvec* pos, f32 distance, f32 rotY, f32 rotX, f32 fov)
{
    WorldMapCamWork* work = mCamWork;

    if (pos != 0) {
        work->curPos = *pos;
    }
    if (distance != worldMapFloatZero) {
        work->curDist = distance;
    }
    work->curRotY = rotY;
    work->curRotX = rotX;
    work->curFov = fov;
    work->curBlendStart = worldMapFloatZero;
    work->curBlendEnd = worldMapFloatOne;
    work->curBlend = worldMapFloatZero;
}
