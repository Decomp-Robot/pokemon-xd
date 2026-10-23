#include <WorldMap.h>

extern const f32 lbl_804EBEEC;
extern const f32 lbl_804EBEF8;

/* WorldMap::SetCameraNextWork -- stage the pending camera target.
 * The position is skipped when null; the distance is kept when it reads
 * as the zero constant, otherwise it is staged with the angle parameters.
 * The pending blend range restarts from the tuned constants. */
void WorldMap::SetCameraNextWork(GSvec* pos, f32 distance, f32 rotY, f32 rotX, f32 fov)
{
    WorldMapCamWork* work = mCamWork;
    f32 blendEnd;
    f32 blendStart;

    if (pos != 0) {
        work->nextPos.x = pos->x;
        work->nextPos.y = pos->y;
        work->nextPos.z = pos->z;
    }
    if (distance != lbl_804EBEEC) {
        work->_58 = distance;
    }
    work->_5C = rotY;
    blendStart = lbl_804EBEEC;
    work->_60 = rotX;
    blendEnd = lbl_804EBEF8;
    work->_64 = fov;
    work->_4C = blendStart;
    work->_50 = blendEnd;
    work->_54 = blendStart;
}
