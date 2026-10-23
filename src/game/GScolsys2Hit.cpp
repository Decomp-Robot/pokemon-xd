#include <GScolsys2Internal.h>

bool checkHitCollision(GSvec* pos, GSvec* cur, f32 radius, GSvec* out, bool checkMdl);

namespace {
extern const f32 collisionIntervalStart, collisionIntervalEnd;

inline void copyCollisionPoint(GSvec* destination, const GSvec* source)
{
    for (s32 component = 0; component < 3; component++) {
        ((f32*)destination)[component] = ((const f32*)source)[component];
    }
}

inline bool checkHitFace(GSvec* hit, GSvec* normal, GSvec* triangle, GSvec* pos, f32 radiusSquared)
{
    GSvec closest;
    if (GScolsy2UtilGetSidePlanePoint(normal, triangle, pos) < collisionIntervalStart) return false;
    GScolsy2UtilGetCpPlanePoint(&closest, normal, triangle, pos);
    if (PSVECSquareDistance((const Vec *)&closest, (const Vec *)pos) >= radiusSquared) return false;
    if (!GScolsy2UtilChkInTri((f32*)&closest, (f32*)triangle, normal)) return false;
    *hit = closest;
    return true;
}
inline bool checkHitEdge(GSvec* hit, GSvec* normal, GSvec* triangle, GSvec* pos, f32 radiusSquared, u16 flags)
{
    GSvec closest;
    u16 edgeMask[3] = {1, 2, 4};
    if (GScolsy2UtilGetSidePlanePoint(normal, triangle, pos) < collisionIntervalStart) return false;
    for (s32 edge = 0; edge < 3; edge++) {
        if (!(flags & edgeMask[edge])) continue;
        s32 next = edge + 1;
        if (next >= 3) next = 0;
        f32 parameter = GScolsys2UtilGetCpLinePoint(&closest, &triangle[edge], &triangle[next], pos);
        if (parameter < collisionIntervalStart || parameter > collisionIntervalEnd) continue;
        if (PSVECSquareDistance((const Vec *)&closest, (const Vec *)pos) < radiusSquared) {
            copyCollisionPoint(hit, &closest);
            return true;
        }
    }
    return false;
}
inline bool checkHitVertex(GSvec* hit, GSvec* normal, GSvec* triangle, GSvec* pos, f32 radiusSquared, u16 flags)
{
    u16 pointMask[3] = {1, 2, 4};
    if (GScolsy2UtilGetSidePlanePoint(normal, triangle, pos) < collisionIntervalStart) return false;
    for (s32 vertex = 0; vertex < 3; vertex++) {
        s32 previous = vertex + 2;
        if (previous >= 3) previous -= 3;
        if (!(flags & pointMask[vertex]) || !(flags & pointMask[previous])) continue;
        if (PSVECSquareDistance((const Vec *)&triangle[vertex], (const Vec *)pos) < radiusSquared) {
            copyCollisionPoint(hit, &triangle[vertex]);
            return true;
        }
    }
    return false;
}
/* Sweep a sphere along the query segment, testing successive intervals. */
inline s32 sweepHitCollision(GSvec* a, GSvec* b, f32 radius, GSvec* out, bool checkMdl)
{
    GSvec dir;
    GSvec step;
    GSvec at;
    f32 total;
    f32 travelled;
    f32 next;
    f32 stepLen;
    f32 limit;

    /* Without a loaded collision file there is nothing to march against. */
    if (GScolsys2GetCCDFileHead() == 0) {
        return 0;
    }
    total = PSVECDistance((const Vec *)a, (const Vec *)b);
    stepLen = collisionIntervalStart;
    if (total > stepLen) {
        stepLen = radius / (f32)total;
        if (stepLen > collisionIntervalEnd) {
            stepLen = collisionIntervalEnd;
        }
    }
    PSVECSubtract((const Vec *)b, (const Vec *)a, (Vec *)&dir);
    travelled = collisionIntervalStart;
    limit = collisionIntervalEnd;
    for (; travelled < limit; travelled = travelled + stepLen) {
        next = travelled + stepLen;

        if (next > limit) {
            next = limit;
        }
        PSVECScale((const Vec *)&dir, (Vec *)&step, travelled);
        PSVECAdd((const Vec *)&step, (const Vec *)a, (Vec *)&step);
        PSVECScale((const Vec *)&dir, (Vec *)&at, next);
        PSVECAdd((const Vec *)&at, (const Vec *)a, (Vec *)&at);
        if (checkHitCollision(&step, &at, radius, out, checkMdl)) {
            return 1;
        }
        if (stepLen <= collisionIntervalStart) {
            break;
        }
    }
    return 0;
}

const f32 collisionIntervalStart = 0.0f;
const f32 collisionIntervalEnd = 1.0f;

}

/* Resolve sphere overlap against faces, flagged edges, then vertices. */
bool checkHitFixedMdl(GSvec* pos, f32 radius, CCD_HITMDL_HEAD* head, GSvec* out)
{
    GSvec hit;
    f32 radiusSquared;
    const u16 anyEdgeMask = 7;
    s32 minX;
    s32 minZ;
    s32 maxX;
    s32 maxZ;
    CCD_HITMDL* model;
    s32* cell;
    s32* triangleEntry;
    s32 cellTriangleIndex;
    s32 cellX;
    s32 cellZ;
    GSvec minimum;
    GSvec maximum;

    /* Cell bounds: the query circle's bounding box, snapped to the grid and
     * clamped to the head's own extent. */
    minimum.x = pos->x - radius;
    minimum.z = pos->z - radius;
    maximum.x = pos->x + radius;
    maximum.z = pos->z + radius;

    minX = (s32)((minimum.x - head->mOriginX) / head->mCellSizeX);
    if (minX < 0) {
        minX = 0;
    }
    minZ = (s32)((minimum.z - head->mOriginZ) / head->mCellSizeZ);
    if (minZ < 0) {
        minZ = 0;
    }
    maxX = (s32)((maximum.x - head->mOriginX) / head->mCellSizeX);
    if (maxX > head->mCountX - 1) {
        maxX = head->mCountX - 1;
    }
    maxZ = (s32)((maximum.z - head->mOriginZ) / head->mCellSizeZ);
    if (maxZ > head->mCountZ - 1) {
        maxZ = head->mCountZ - 1;
    }
    /* Faces first: the closest point on the triangle plane has to lie inside
     * the triangle and inside the query radius. */
    radiusSquared = radius * radius;
    bool found = false;
    for (cellZ = minZ; cellZ <= maxZ && !found; cellZ++) {
        cell = head->mCellStart;
        cell += 2 * (minX + cellZ * head->mCountX);
        for (cellX = minX; cellX <= maxX && !found; cellX++, cell += 2) {
            triangleEntry = head->mCellList;
            triangleEntry += cell[0];

            for (cellTriangleIndex = 0; cellTriangleIndex < (u32)cell[1] && !found; cellTriangleIndex++, triangleEntry++) {
                model = (CCD_HITMDL*)head->mModel;
                model += *triangleEntry;
                found = checkHitFace(&hit, &model->mTri.mNormal, (GSvec*)model, pos, radiusSquared);
            }
        }
    }
    if (found) {
        if (out == 0) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(out, &hit,
                                          0.0001f + radius, pos);
        return 1;
    }

    /* Then the masked edges of the edge-flagged meshes. */
    found = false;
    for (cellZ = minZ; cellZ <= maxZ && !found; cellZ++) {
        cell = head->mCellStart;
        cell += 2 * (minX + cellZ * head->mCountX);
        for (cellX = minX; cellX <= maxX && !found; cellX++, cell += 2) {
            triangleEntry = head->mCellList;
            triangleEntry += cell[0];

            for (cellTriangleIndex = 0; cellTriangleIndex < (u32)cell[1] && !found; cellTriangleIndex++, triangleEntry++) {
                model = (CCD_HITMDL*)head->mModel;
                model += *triangleEntry;
                u16 flags = model->mEdgeMask[0];

                if (!(flags & anyEdgeMask)) {
                    continue;
                }

                found = checkHitEdge(&hit, &model->mTri.mNormal, (GSvec*)model, pos, radiusSquared, flags);
            }
        }
    }
    if (found) {
        if (out == 0) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(out, &hit,
                                          0.0001f + radius, pos);
        return 1;
    }

    /* Finally the masked vertices of the same meshes. A vertex only counts
     * when both edges meeting there are masked. */
    found = false;
    for (cellZ = minZ; cellZ <= maxZ && !found; cellZ++) {
        cell = head->mCellStart;
        cell += 2 * (minX + cellZ * head->mCountX);
        for (cellX = minX; cellX <= maxX && !found; cellX++, cell += 2) {
            triangleEntry = head->mCellList;
            triangleEntry += cell[0];

            for (cellTriangleIndex = 0; cellTriangleIndex < (u32)cell[1] && !found; cellTriangleIndex++, triangleEntry++) {
                model = (CCD_HITMDL*)head->mModel;
                model += *triangleEntry;
                u16 flags = model->mEdgeMask[0];

                if (!(flags & anyEdgeMask)) {
                    continue;
                }

                found = checkHitVertex(&hit, &model->mTri.mNormal, (GSvec*)model, pos, radiusSquared, flags);
            }
        }
    }

    if (found) {
        if (out == 0) return 1;
        GScolsy2UtilGetPointExtentionLine(out, &hit, 0.0001f + radius, pos);
        return 1;
    }
    return 0;
}

/* Hit test against a moving collision mesh.
 *
 * The same three passes checkHitFixedMdl runs over the static grid are applied
 * to a mesh that moves, so every triangle is transformed by mtx and its normal
 * by rotMtx before it is tested. A hit point is reported through out when the
 * caller asked for one. */
bool checkHitMdl(GSvec* pos, f32 radius, CCD_HITMDL_HEAD* head, GSmtx* mtx,
                GSmtx* rotMtx, GSvec* out)
{
    GSvec normal;
    GSvec hit;
    GSvec triangle[3];
    CCD_HITMDL* mdl;
    f32 radiusSquared = radius * radius;
    s32 triangleIndex;
    bool found;
    const u16 anyEdgeMask = 7;

    mdl = (CCD_HITMDL*)head->mModel;
    triangleIndex = 0;
    found = false;
    for (; triangleIndex < (u32)head->mCount && !found; triangleIndex++, mdl++) {
        for (s32 pointIndex = 0; pointIndex < 3; pointIndex++) {
            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&mdl->mTri.mPoint[pointIndex], (Vec *)&triangle[pointIndex]);
        }
        PSMTXMultVec((MtxPtr)rotMtx, (const Vec *)&mdl->mTri.mNormal, (Vec *)&normal);
        found = checkHitFace(&hit, &normal, triangle, pos, radiusSquared);
    }
    if (found) {
        if (out == 0) return true;
        GScolsy2UtilGetPointExtentionLine(out, &hit, 0.0001f + radius, pos);
        return true;
    }

    triangleIndex = 0;
    found = false;
    mdl = (CCD_HITMDL*)head->mModel;
    for (; triangleIndex < (u32)head->mCount && !found; triangleIndex++, mdl++) {
        if (!(mdl->mEdgeMask[0] & anyEdgeMask)) continue;
        for (s32 pointIndex = 0; pointIndex < 3; pointIndex++) {
            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&mdl->mTri.mPoint[pointIndex], (Vec *)&triangle[pointIndex]);
        }
        PSMTXMultVec((MtxPtr)rotMtx, (const Vec *)&mdl->mTri.mNormal, (Vec *)&normal);
        found = checkHitEdge(&hit, &normal, triangle, pos, radiusSquared, mdl->mEdgeMask[0]);
    }
    if (found) {
        if (out == 0) return true;
        GScolsy2UtilGetPointExtentionLine(out, &hit, 0.0001f + radius, pos);
        return true;
    }

    triangleIndex = 0;
    found = false;
    mdl = (CCD_HITMDL*)head->mModel;
    for (; triangleIndex < (u32)head->mCount && !found; triangleIndex++, mdl++) {
        if (!(mdl->mEdgeMask[0] & anyEdgeMask)) continue;
        for (s32 pointIndex = 0; pointIndex < 3; pointIndex++) {
            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&mdl->mTri.mPoint[pointIndex], (Vec *)&triangle[pointIndex]);
        }
        PSMTXMultVec((MtxPtr)rotMtx, (const Vec *)&mdl->mTri.mNormal, (Vec *)&normal);
        found = checkHitVertex(&hit, &normal, triangle, pos, radiusSquared, mdl->mEdgeMask[0]);
    }
    if (found) {
        if (out == 0) return true;
        GScolsy2UtilGetPointExtentionLine(out, &hit, 0.0001f + radius, pos);
        return true;
    }
    return false;
}

bool checkHitCollision(GSvec* pos, GSvec* cur, f32 radius, GSvec* out,
                       bool checkMdl)
{
    GSvec here;
    GSvec probe;
    GSmtx obj;
    GSmtx rot;
    u32 objectOffset;
    CCD_HITMDL_HEAD* head;
    CCD_FILEHEAD* file;
    s32 passes = 0;
    s32 hits;
    u32 objIndex;
    CCD_OBJECT* objects;
    GSvec* dst;

    here.x = cur->x;
    here.y = cur->y;
    here.z = cur->z;
    file = GScolsys2GetCCDFileHead();
    /* The models are handed a scratch point to report into; without a caller
     * result there is nowhere to write, so the sweep still runs. */
    if (out == 0) {
        dst = 0;
    } else {
        dst = &probe;
    }
    for (passes = 0; passes < 10; passes++) {
        hits = 0;
        objects = file->mObject;
        objectOffset = 0;
        for (objIndex = 0; objIndex < (u32)file->mObjectCount; objIndex++, objectOffset += sizeof(CCD_OBJECT)) {
            CCD_OBJECT* object = (CCD_OBJECT*)((u8*)objects + objectOffset);
            u8 enabled;
            bool hit;

            GScolsys2GetObjEnable(objIndex, &enabled);
            if (enabled == 0) {
                continue;
            }
            if (checkMdl) {
                head = object->mHitMdl2;
                if (head == 0) continue;
            } else {
                head = object->mHitMdl;
                if (head == 0) continue;
            }
            if (object->mFlags & GSCOLSYS2_OBJECT_MOVING) {
                GScolsys2GetObjMatrix(&obj, objIndex);
                GScolsys2GetObjRotMatrix(&rot, objIndex);
                hit = checkHitMdl(&here, radius, head, &obj, &rot, dst);
            } else {
                hit = checkHitFixedMdl(&here, radius, head, dst);
            }
            if (!hit) {
                continue;
            }
            if (out == 0) {
                return 1;
            }
            hits++;
            if (dst == 0) {
                continue;
            }
            here.x = dst->x;
            here.y = dst->y;
            here.z = dst->z;
        }
        /* Another hit means the sample point moved, so the sweep is worth
         * repeating; ten rounds is the cap. */
        if (hits <= 0) {
            break;
        }
    }
    if (passes <= 0) {
        return 0;
    }
    out->x = here.x;
    out->y = here.y;
    out->z = here.z;
    return 1;
}

extern "C" s32 GScolsys2HitNpcCollision(GSvec* a, GSvec* b, f32 radius, GSvec* out)
{
    return sweepHitCollision(a, b, radius, out, true);
}

extern "C" s32 GScolsys2HitCollision(GSvec* a, GSvec* b, f32 radius, GSvec* out)
{
    return sweepHitCollision(a, b, radius, out, false);
}
