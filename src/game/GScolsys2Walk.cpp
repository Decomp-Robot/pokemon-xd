#include <GScolsys2Internal.h>
#include <GScolsys2Geometry.h>

extern "C" s32 GScolsys2WalkGetLayer(GSvec* pos, u8* color0, u8* color1)
{
    GSCOLSYS2_WALK_HEIGHT heights[GSCOLSYS2_WALK_MAX];
    f32 distance;
    f32 nearestDistance;
    s32 heightIndex;
    s32 heightCount;
    s32 nearestHeightIndex;

    heightCount = GScolsys2WalkGetHeight(pos, heights);
    if (heightCount <= 0) {
        return 0;
    }
    // Return colors from the walk surface closest to the query height.
    nearestDistance = (pos->y - heights[0].mHeight) > 0.0f ? (pos->y - heights[0].mHeight) : -(pos->y - heights[0].mHeight);
    nearestHeightIndex = 0;
    for (heightIndex = 1; heightIndex < heightCount; heightIndex++) {
        distance = pos->y - heights[heightIndex].mHeight;

        distance = distance > 0.0f ? distance : -distance;
        if (nearestDistance > distance) {
            nearestHeightIndex = heightIndex;
            nearestDistance = distance;
        }
    }
    *color0 = heights[nearestHeightIndex].mColor0;
    *color1 = heights[nearestHeightIndex].mColor1;
    return 1;
}

static inline f32 walkTriangleEdgeSide(const GSvec& point, const GSvec& vertex,
                                     const GSvec& nextVertex)
{
    f32 edgeDeltaX;
    f32 edgeDeltaZ;
    f32 pointDeltaX;
    f32 pointDeltaZ;

    GScolsys2SubtractXZ(edgeDeltaX, edgeDeltaZ, nextVertex, vertex);
    GScolsys2SubtractXZ(pointDeltaX, pointDeltaZ, point, vertex);
    return GScolsys2Cross2D(edgeDeltaX, edgeDeltaZ, pointDeltaX, pointDeltaZ);
}

bool getCpPolyVec(GSvec* out, GSvec* pos, GSvec* poly, GSvec* normal)
{
    GSvec minimum;
    GSvec maximum;
    const f32 initialMinimum = 1000000.0f;
    const f32 initialMaximum = -1000000.0f;
    GScolsys2FillXZ(minimum, initialMinimum);
    GScolsys2FillXZ(maximum, initialMaximum);
    s32 i;
    f32 heightOffset;

    for (i = 0; i < 3; i++) {
        if (minimum.x > poly[i].x) minimum.x = poly[i].x;
        if (minimum.z > poly[i].z) minimum.z = poly[i].z;
        if (maximum.x < poly[i].x) maximum.x = poly[i].x;
        if (maximum.z < poly[i].z) maximum.z = poly[i].z;
    }

    if (minimum.x > pos->x || minimum.z > pos->z || maximum.x < pos->x || maximum.z < pos->z) {
        return 0;
    }

/* Inside test: the query point has to sit behind all three edge planes. */
    for (i = 0; i < 3; i++) {
        s32 n = i + 1;

        if (n >= 3) {
            n = 0;
        }
        if (walkTriangleEdgeSide(*pos, poly[i], poly[n]) > 0.0f) {
            return 0;
        }
    }

    if (0.0f == normal->y) {
        return 0;
    }
    GSvec planeOffset;
    planeOffset.y = pos->y - poly[0].y;
    planeOffset.x = pos->x - poly[0].x;
    planeOffset.z = pos->z - poly[0].z;
    heightOffset = -(normal->x * planeOffset.x + normal->y * planeOffset.y
                        + normal->z * planeOffset.z) / normal->y;
    out->x = pos->x;
    out->y = pos->y + heightOffset;
    out->z = pos->z;
    return 1;
}


inline s32 collectFixedWalkHeights(GSvec* pos, CCD_WALKMDL_HEAD* head, GSCOLSYS2_WALK_HEIGHT* heights)
{
    CCD_WALKMDL* model;
    s32* cell;
    s32* entry;
    s32 triangleIndex;
    s32 foundCount;

    foundCount = 0;
    s32 cellZ, cellX;
    cellX = (s32)((pos->x - head->mOriginX) / head->mCellSizeX);
    if (cellX < 0 || cellX >= head->mCountX) return 0;
    cellZ = (s32)((pos->z - head->mOriginZ) / head->mCellSizeZ);
    if (cellZ < 0 || cellZ >= head->mCountZ) return 0;
    cell = head->mCellStart;
    cell += 2 * (cellX + cellZ * head->mCountX);
    entry = head->mCellList;
    entry += cell[0];
    for (triangleIndex = foundCount; triangleIndex < (u32)cell[1] && foundCount < GSCOLSYS2_WALK_MAX; triangleIndex++, entry++) {
        model = (CCD_WALKMDL*)head->mModel;
        model += *entry;
        GSvec closest;
        if (!getCpPolyVec(&closest, pos, (GSvec*)model, &model->mTri.mNormal)) continue;
        s32 duplicate;
        for (duplicate = 0; duplicate < foundCount; duplicate++) {
            if (closest.y == heights[duplicate].mHeight) break;
        }
        u8 added;
        if (duplicate < foundCount) {
            added = false;
        } else {
            heights[foundCount].mHeight = closest.y;
            u32 layer = ((u32)model->mColor0 >> 4) & 0xf;
            if (layer == 0xf) heights[foundCount].mLayer = 0xffff;
            else heights[foundCount].mLayer = layer;
            layer = model->mColor0 & 0xf;
            if (layer == 0xf) heights[foundCount].mLayer2 = 0xffff;
            else heights[foundCount].mLayer2 = layer;
            heights[foundCount].mColor0 = ((u32)model->mColor1 >> 4) & 0xf;
            heights[foundCount].mColor1 = model->mColor1 & 0xf;
            added = true;
        }
        if (added) foundCount++;
    }
    return foundCount;
}
inline s32 collectMovingWalkHeights(GSvec* pos, CCD_WALKMDL_HEAD* head, GSCOLSYS2_WALK_HEIGHT* heights, GSmtx* transform, GSmtx* rotation)
{
            CCD_WALKMDL* model;
            s32 triangleIndex;
            s32 foundCount;
            model = (CCD_WALKMDL*)head->mModel;
            foundCount = 0;
            triangleIndex = 0;
            for (; triangleIndex < (u32)head->mCount && foundCount < GSCOLSYS2_WALK_MAX; triangleIndex++, model++) {
                GSvec triangle[3];
                GSvec normal;
                GSvec closest;
                for (s32 vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
                    PSMTXMultVec((MtxPtr)transform, (const Vec *)&model->mTri.mPoint[vertexIndex], (Vec *)&triangle[vertexIndex]);
                }
                PSMTXMultVec((MtxPtr)rotation, (const Vec *)&model->mTri.mNormal, (Vec *)&normal);
                if (!getCpPolyVec(&closest, pos, triangle, &normal)) continue;
                s32 duplicate;
                for (duplicate = 0; duplicate < foundCount; duplicate++) {
                    if (closest.y == heights[duplicate].mHeight) break;
                }
                u8 added;
                if (duplicate < foundCount) {
                    added = false;
                } else {
                    heights[foundCount].mHeight = closest.y;
                    u32 layer = ((u32)model->mColor0 >> 4) & 0xf;
                    if (layer == 0xf) heights[foundCount].mLayer = 0xffff;
                    else heights[foundCount].mLayer = layer;
                    layer = model->mColor0 & 0xf;
                    if (layer == 0xf) heights[foundCount].mLayer2 = 0xffff;
                    else heights[foundCount].mLayer2 = layer;
                    heights[foundCount].mColor0 = ((u32)model->mColor1 >> 4) & 0xf;
                    heights[foundCount].mColor1 = model->mColor1 & 0xf;
                    added = true;
                }
                if (added) foundCount++;
            }
    return foundCount;

}
extern "C" s32 GScolsys2WalkGetHeight(GSvec* pos, GSCOLSYS2_WALK_HEIGHT* out)
{
    CCD_FILEHEAD* file;
    GSCOLSYS2_WALK_HEIGHT heights[GSCOLSYS2_WALK_MAX];
    CCD_OBJECT* object;
    CCD_WALKMDL_HEAD* head;
    u32 objectIndex;
    s32 total = 0;
    s32 heightIndex;
    file = GScolsys2GetCCDFileHead();
    if (file == 0) return 0;
    object = file->mObject;
    for (objectIndex = 0; objectIndex < (u32)file->mObjectCount && total < GSCOLSYS2_WALK_MAX; objectIndex++, object++) {
        u8 enabled;
        s32 count;
        GScolsys2GetObjEnable(objectIndex, &enabled);
        if (!enabled) continue;
        head = object->mWalkMdl;
        if (!head) continue;
        if (object->mFlags & GSCOLSYS2_OBJECT_MOVING) {
            GSmtx matrix;
            GSmtx rotation;
            GScolsys2GetObjMatrix(&matrix, objectIndex);
            GScolsys2GetObjRotMatrix(&rotation, objectIndex);
            count = collectMovingWalkHeights(pos, head, heights, &matrix, &rotation);
} else {
            count = collectFixedWalkHeights(pos, head, heights);
}
        for (heightIndex = 0; heightIndex < count && total < GSCOLSYS2_WALK_MAX; heightIndex++, total++) {
            GSCOLSYS2_WALK_HEIGHT* sample = &heights[heightIndex];
            out[total].mHeight = sample->mHeight;
            out[total].mLayer = sample->mLayer;
            out[total].mLayer2 = sample->mLayer2;
            out[total].mColor0 = sample->mColor0;
            out[total].mColor1 = sample->mColor1;
        }
    }
    return total;
}
