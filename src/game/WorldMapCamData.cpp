#include <WorldMap.h>

extern u32* worldMapCamDataCnt;
extern WorldMapCamData* worldMapCamDataTable;

/* Copy one camera table entry into the caller's buffers. Out-of-range
 * indices are ignored. */
void WorldMap::GetCameraData(long index, GSvec* pos, f32* distance, f32* rotY, f32* rotX, f32* fov)
{
    WorldMapCamData* entry;
    f32 x;
    f32 y;
    f32 z;

    if ((u32)index >= *worldMapCamDataCnt) {
        return;
    }
    entry = &worldMapCamDataTable[index];
    z = entry->pos.z;
    y = entry->pos.y;
    x = entry->pos.x;
    pos->x = x;
    pos->y = y;
    pos->z = z;
    *distance = entry->distance;
    *rotY = entry->rotY;
    *rotX = entry->rotX;
    *fov = entry->fov;
}
