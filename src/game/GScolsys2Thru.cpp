#include <GScolsys2Internal.h>

enum { GSCOLSYS2_THRU_EDGE_0_FLAG = 1, GSCOLSYS2_THRU_EDGE_1_FLAG = 2, GSCOLSYS2_THRU_EDGE_2_FLAG = 4 };

inline void getThruCellBounds(GSvec* start, f32 radius, CCD_THRUMDL_HEAD* head, s32* ix0, s32* iz0, s32* ix1, s32* iz1)
{
    GSvec minimum, maximum;
    minimum.x = start->x - radius;
    minimum.z = start->z - radius;
    maximum.x = start->x + radius;
    maximum.z = start->z + radius;
    (*ix0) = (s32)((minimum.x - head->mOriginX) / head->mCellSizeX);

    if ((*ix0) < 0) {
        (*ix0) = 0;
    }
    (*iz0) = (s32)((minimum.z - head->mOriginZ) / head->mCellSizeZ);

    if ((*iz0) < 0) {
        (*iz0) = 0;
    }
    (*ix1) = (s32)((maximum.x - head->mOriginX) / head->mCellSizeX);

    if ((*ix1) > head->mCountX - 1) {
        (*ix1) = head->mCountX - 1;
    }
    (*iz1) = (s32)((maximum.z - head->mOriginZ) / head->mCellSizeZ);

    if ((*iz1) > head->mCountZ - 1) {
        (*iz1) = head->mCountZ - 1;
    }
}

inline bool checkThruPass(GSvec* start, GSvec* end, GSvec* normal, GSvec* triangle)
{
    GSvec crossing;
    f32 parameter;
    if (!GScolsys2UtilGetCpPlaneLine(&crossing, &parameter, normal, triangle, start, end)) return false;
    if (parameter < 0.0f || parameter > 1.0f) return false;
    if (!GScolsy2UtilChkInTri((f32*)&crossing, (f32*)triangle, normal)) return false;
    return true;
}
inline bool checkThruFace(GSvec* normal, GSvec* triangle, GSvec* position, f32 radiusSquared)
{
    GSvec closest;
    if (GScolsy2UtilGetSidePlanePoint(normal, triangle, position) < 0.0f) {
        return false;
    }
    GScolsy2UtilGetCpPlanePoint(&closest, normal, triangle, position);
    if (PSVECSquareDistance((const Vec *)&closest, (const Vec *)position) >= radiusSquared) {
        return false;
    }
    if (!GScolsy2UtilChkInTri((f32*)&closest, (f32*)triangle, normal)) {
        return false;
    }
    return true;
}

inline bool checkThruEdge(GSvec* normal, GSvec* triangle, GSvec* position, f32 radiusSquared, u16 flags)
{
    GSvec closest;
    u16 edgeMask[3] = {GSCOLSYS2_THRU_EDGE_0_FLAG, GSCOLSYS2_THRU_EDGE_1_FLAG, GSCOLSYS2_THRU_EDGE_2_FLAG};
    if (GScolsy2UtilGetSidePlanePoint(normal, triangle, position) < 0.0f) {
        return false;
    }
    for (s32 edge = 0; edge < 3; edge++) {
        if (!(flags & edgeMask[edge])) {
            continue;
        }
        s32 next = edge + 1;
        if (next >= 3) {
            next = 0;
        }
        f32 parameter = GScolsys2UtilGetCpLinePoint(&closest, &triangle[edge], &triangle[next], position);
        if (parameter < 0.0f || parameter > 1.0f) {
            continue;
        }
        if (PSVECSquareDistance((const Vec *)&closest, (const Vec *)position) < radiusSquared) {
            return true;
        }
    }
    return false;
}

inline bool checkThruVertex(GSvec* normal, GSvec* triangle, GSvec* position, f32 radiusSquared, u16 flags)
{
    u16 pointMask[3] = {GSCOLSYS2_THRU_EDGE_0_FLAG, GSCOLSYS2_THRU_EDGE_1_FLAG, GSCOLSYS2_THRU_EDGE_2_FLAG};
    if (GScolsy2UtilGetSidePlanePoint(normal, triangle, position) < 0.0f) {
        return false;
    }
    for (s32 vertex = 0; vertex < 3; vertex++) {
        s32 previous = vertex + 2;
        if (previous >= 3) {
            previous -= 3;
        }
        if (!(flags & pointMask[vertex]) || !(flags & pointMask[previous])) {
            continue;
        }
        if (PSVECSquareDistance((const Vec *)&triangle[vertex], (const Vec *)position) < radiusSquared) {
            return true;
        }
    }
    return false;
}


s32 getMdlEventListPass(GSvec* start, GSvec* end, CCD_THRUMDL_HEAD* head,
                        GSmtx* mtx, GSmtx* rotMtx, GScolsys2Thru_c* out)
{
    f32 parameter;
    GSvec worldNormal;
    GSvec travelDirection;
    GSvec crossing;
    GSvec worldPoints[3];
    CCD_THRUMDL* modelCursor;
    u32 modelIndex;
    s32 eventCount;

    eventCount = 0;
    PSVECSubtract((const Vec *)end, (const Vec *)start, (Vec *)&travelDirection);
    modelCursor = (CCD_THRUMDL*)head->mModel;
    for (modelIndex = 0; modelIndex < (u32)head->mCount && eventCount < GSCOLSYS2_THRU_MAX; modelIndex++, modelCursor++) {
        CCD_THRUMDL* model = modelCursor;
        s32 duplicateIndex;
        s32 vertexIndex;
        bool crossed;

        for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
            if (out[duplicateIndex].mEventId == model->mEventId) {
                break;
            }
        }
        if (duplicateIndex < eventCount) {
            continue;
        }
        PSMTXMultVec((MtxPtr)rotMtx, (const Vec *)&model->mTri.mNormal, (Vec *)&worldNormal);
        if (PSVECDotProduct((const Vec *)&worldNormal, (const Vec *)&travelDirection) >= 0.0f) {
            continue;
        }
        for (vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&model->mTri.mPoint[vertexIndex], (Vec *)&worldPoints[vertexIndex]);
        }
        if (!GScolsys2UtilGetCpPlaneLine(&crossing, &parameter, &worldNormal, &worldPoints[0], start, end)) {
            crossed = false;
        } else if (parameter < 0.0f || parameter > 1.0f) {
            crossed = false;
        } else if (!GScolsy2UtilChkInTri((f32*)&crossing, (f32*)worldPoints, &worldNormal)) {
            crossed = false;
        } else {
            crossed = true;
        }
        if (!crossed) {
            continue;
        }
        u16 eventId = model->mEventId;
        for (s32 point = 0; point < 3; point++) out[eventCount].mPoint[point] = worldPoints[point];
        out[eventCount].mNormal = worldNormal;
        out[eventCount].mEventId = eventId;
        eventCount++;
    }
    return eventCount;
}

inline void appendStaticThruPassTrigger(GSvec* start, GSvec* end, GSvec* direction,
                                         CCD_THRUMDL* model, GScolsys2Thru_c* events,
                                         s32& eventCount)
{
    s32 duplicate;
    for (duplicate = 0; duplicate < eventCount; duplicate++) {
        if (events[duplicate].mEventId == model->mEventId) break;
    }
    if (duplicate < eventCount) return;
    if (PSVECDotProduct((const Vec *)&model->mTri.mNormal, (const Vec *)direction) >= 0.0f) return;
    GSvec* normal = &model->mTri.mNormal;
    if (!checkThruPass(start, end, normal, model->mTri.mPoint)) return;
    u16 eventId = model->mEventId;
    for (s32 point = 0; point < 3; point++) {
        events[eventCount].mPoint[point] = model->mTri.mPoint[point];
    }
    events[eventCount].mNormal = model->mTri.mNormal;
    events[eventCount].mEventId = eventId;
    eventCount++;
}


inline s32 collectFixedThruPassEvents(GSvec* start, GSvec* end,
                                      CCD_THRUMDL_HEAD* head, GScolsys2Thru_c* out)
{
    CCD_THRUMDL* modelCursor;
    u32 modelIndex;
    s32 passEventCount;
    GSvec travelDirection;

    passEventCount = 0;
    PSVECSubtract((const Vec *)end, (const Vec *)start, (Vec *)&travelDirection);
    modelCursor = (CCD_THRUMDL*)head->mModel;
    for (modelIndex = 0; modelIndex < (u32)head->mCount && passEventCount < GSCOLSYS2_THRU_MAX; modelIndex++, modelCursor++) {
        appendStaticThruPassTrigger(start, end, &travelDirection, modelCursor, out, passEventCount);
    }
    return passEventCount;
}

extern "C" s32 GScolsys2ThruPassEventID(GSvec* start, GSvec* end, GScolsys2Thru_c* out)
{
    CCD_FILEHEAD* collisionFile;
    CCD_OBJECT* objectCursor;
    CCD_THRUMDL_HEAD* head;
    u32 objectIndex;
    s32 eventCount = 0;
    GScolsys2Thru_c objectEvents[GSCOLSYS2_THRU_MAX];
    GSmtx objectMatrix;
    GSmtx rotationMatrix;

    collisionFile = GScolsys2GetCCDFileHead();
    if (collisionFile == 0) {
        return 0;
    }
    objectCursor = collisionFile->mObject;
    for (objectIndex = 0; objectIndex < (u32)collisionFile->mObjectCount && eventCount < GSCOLSYS2_THRU_MAX; objectIndex++, objectCursor++) {
        u8 enabled;
        s32 objectEventCount;
        s32 eventIndex;

        GScolsys2GetObjEnable(objectIndex, &enabled);
        if (enabled == 0) continue;
        head = objectCursor->mThruMdl;
        if (head == 0) continue;
        if (objectCursor->mFlags & GSCOLSYS2_OBJECT_MOVING) {
            GScolsys2GetObjMatrix(&objectMatrix, objectIndex);
            GScolsys2GetObjRotMatrix(&rotationMatrix, objectIndex);
            objectEventCount = getMdlEventListPass(start, end, head, &objectMatrix, &rotationMatrix, objectEvents);
        } else {
            objectEventCount = collectFixedThruPassEvents(start, end, head, objectEvents);
        }
        for (eventIndex = 0; eventIndex < objectEventCount && eventCount < GSCOLSYS2_THRU_MAX; eventIndex++) {
            s32 duplicateIndex;

            for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
                if (out[duplicateIndex].mEventId == objectEvents[eventIndex].mEventId) {
                    break;
                }
            }
            if (duplicateIndex < eventCount) {
                continue;
            }
            s32 outputIndex = eventCount++;
            for (s32 point = 0; point < 3; point++) {
                out[outputIndex].mPoint[point] = objectEvents[eventIndex].mPoint[point];
            }
            out[outputIndex].mNormal = objectEvents[eventIndex].mNormal;
            out[outputIndex].mEventId = objectEvents[eventIndex].mEventId;

        }
    }
    return eventCount;
}

/* Collect sphere-overlap events from faces, flagged edges, then vertices. */
s32 getFixedMdlEventList(GSvec* start, GSvec* end, f32 radius,
                         CCD_THRUMDL_HEAD* head, GScolsys2Thru_c* out)
{
    CCD_THRUMDL* model;
    u32 cellEntryIndex;
    s32* cell;
    s32* triangleIndices;
    s32 cellX;
    enum { triggerEdgeMask = GSCOLSYS2_THRU_EDGE_0_FLAG | GSCOLSYS2_THRU_EDGE_1_FLAG | GSCOLSYS2_THRU_EDGE_2_FLAG };
    s32 minCellX;
    s32 minCellZ;
    s32 maxCellX;
    s32 maxCellZ;
    s32 eventCount;
    s32 cellZ;
    f32 radiusSquared;

    eventCount = 0;
    getThruCellBounds(start,radius,head,&minCellX,&minCellZ,&maxCellX,&maxCellZ);
    radiusSquared = radius * radius;
    for (cellZ = minCellZ; cellZ <= maxCellZ && eventCount < GSCOLSYS2_THRU_MAX; cellZ++) {
        cell = head->mCellStart;
        cell += 2 * (minCellX + cellZ * head->mCountX);

        for (cellX = minCellX; cellX <= maxCellX && eventCount < GSCOLSYS2_THRU_MAX; cellX++, cell += 2) {
            triangleIndices = head->mCellList;
            triangleIndices += cell[0];

            for (cellEntryIndex = 0; cellEntryIndex < (u32)cell[1] && eventCount < GSCOLSYS2_THRU_MAX; cellEntryIndex++, triangleIndices++) {
                model = (CCD_THRUMDL*)head->mModel;
                model += *triangleIndices;

                {
                    s32 duplicateIndex;

                    for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
                        if (out[duplicateIndex].mEventId == model->mEventId) {
                            break;
                        }
                    }
                    if (duplicateIndex < eventCount) {
                        continue;
                    }
                }
                if (PSVECDotProduct((const Vec *)&model->mTri.mNormal, (const Vec *)end)
                    >= 0.0f) {
                    continue;
                }
                if (!checkThruFace(&model->mTri.mNormal, model->mTri.mPoint, start, radiusSquared)) continue;
                u16 eventId = model->mEventId;
                out[eventCount].mPoint[0] = model->mTri.mPoint[0];
                out[eventCount].mPoint[1] = model->mTri.mPoint[1];
                out[eventCount].mPoint[2] = model->mTri.mPoint[2];
                out[eventCount].mNormal = model->mTri.mNormal;
                out[eventCount].mEventId = eventId;
                eventCount++;
            }
        }
    }
    for (cellZ = minCellZ; cellZ <= maxCellZ && eventCount < GSCOLSYS2_THRU_MAX; cellZ++) {
        cell = head->mCellStart;
        cell += 2 * (minCellX + cellZ * head->mCountX);

        for (cellX = minCellX; cellX <= maxCellX && eventCount < GSCOLSYS2_THRU_MAX; cellX++, cell += 2) {
            triangleIndices = head->mCellList;
            triangleIndices += cell[0];

            for (cellEntryIndex = 0; cellEntryIndex < (u32)cell[1] && eventCount < GSCOLSYS2_THRU_MAX; cellEntryIndex++, triangleIndices++) {
                model = (CCD_THRUMDL*)head->mModel;
                model += *triangleIndices;
                u16 flags = model->mFlags;

                if (!(flags & triggerEdgeMask)) {
                    continue;
                }
                {
                    s32 duplicateIndex;

                    for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
                        if (out[duplicateIndex].mEventId == model->mEventId) {
                            break;
                        }
                    }
                    if (duplicateIndex < eventCount) {
                        continue;
                    }
                }
                if (PSVECDotProduct((const Vec *)&model->mTri.mNormal, (const Vec *)end)
                    >= 0.0f) {
                    continue;
                }
                if (!checkThruEdge(&model->mTri.mNormal, model->mTri.mPoint, start, radiusSquared, model->mFlags)) continue;
                u16 eventId = model->mEventId;
                out[eventCount].mPoint[0] = model->mTri.mPoint[0];
                out[eventCount].mPoint[1] = model->mTri.mPoint[1];
                out[eventCount].mPoint[2] = model->mTri.mPoint[2];
                out[eventCount].mNormal = model->mTri.mNormal;
                out[eventCount].mEventId = eventId;
                eventCount++;
            }
        }
    }
    for (cellZ = minCellZ; cellZ <= maxCellZ && eventCount < GSCOLSYS2_THRU_MAX; cellZ++) {
        cell = head->mCellStart;
        cell += 2 * (minCellX + cellZ * head->mCountX);

        for (cellX = minCellX; cellX <= maxCellX && eventCount < GSCOLSYS2_THRU_MAX; cellX++, cell += 2) {
            triangleIndices = head->mCellList;
            triangleIndices += cell[0];

            for (cellEntryIndex = 0; cellEntryIndex < (u32)cell[1] && eventCount < GSCOLSYS2_THRU_MAX; cellEntryIndex++, triangleIndices++) {
                model = (CCD_THRUMDL*)head->mModel;
                model += *triangleIndices;
                u16 flags = model->mFlags;

                if (!(flags & triggerEdgeMask)) {
                    continue;
                }
                {
                    s32 duplicateIndex;

                    for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
                        if (out[duplicateIndex].mEventId == model->mEventId) {
                            break;
                        }
                    }
                    if (duplicateIndex < eventCount) {
                        continue;
                    }
                }
                if (PSVECDotProduct((const Vec *)&model->mTri.mNormal, (const Vec *)end)
                    >= 0.0f) {
                    continue;
                }
                if (!checkThruVertex(&model->mTri.mNormal, model->mTri.mPoint, start, radiusSquared, model->mFlags)) continue;
                u16 eventId = model->mEventId;
                out[eventCount].mPoint[0] = model->mTri.mPoint[0];
                out[eventCount].mPoint[1] = model->mTri.mPoint[1];
                out[eventCount].mPoint[2] = model->mTri.mPoint[2];
                out[eventCount].mNormal = model->mTri.mNormal;
                out[eventCount].mEventId = eventId;
                eventCount++;
            }
        }
    }

    return eventCount;
}

/* The same three trigger passes as getFixedMdlEventList, run against a mesh
 * that moves: every triangle is transformed into world space by mtx before it
 * is tested, and the normal by rotMtx. */
s32 getMdlEventList(GSvec* start, GSvec* end, f32 radius, CCD_THRUMDL_HEAD* head,
                    GSmtx* mtx, GSmtx* rotMtx, GScolsys2Thru_c* out)
{
    enum { triggerEdgeMask = GSCOLSYS2_THRU_EDGE_0_FLAG | GSCOLSYS2_THRU_EDGE_1_FLAG | GSCOLSYS2_THRU_EDGE_2_FLAG };
    GSvec worldNormal;
    GSvec worldPoints[3];
    CCD_THRUMDL* modelCursor;
    f32 radiusSquared = radius * radius;
    s32 eventCount;
    u32 modelIndex;

    eventCount = 0;
    modelCursor = (CCD_THRUMDL*)head->mModel;
    for (modelIndex = 0; modelIndex < (u32)head->mCount && eventCount < GSCOLSYS2_THRU_MAX; modelIndex++, modelCursor++) {
        CCD_THRUMDL* model = modelCursor;
        s32 vertexIndex;

        {
            s32 duplicateIndex;

            for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
                if (out[duplicateIndex].mEventId == model->mEventId) {
                    break;
                }
            }
            if (duplicateIndex < eventCount) {
                continue;
            }
        }
        PSMTXMultVec((MtxPtr)rotMtx, (const Vec *)&model->mTri.mNormal, (Vec *)&worldNormal);
        if (PSVECDotProduct((const Vec *)&worldNormal, (const Vec *)end) >= 0.0f) {
            continue;
        }
        for (vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&model->mTri.mPoint[vertexIndex], (Vec *)&worldPoints[vertexIndex]);
        }
        if (!checkThruFace(&worldNormal, worldPoints, start, radiusSquared)) continue;
        u16 eventId = model->mEventId;
        for (s32 point = 0; point < 3; point++) {
            out[eventCount].mPoint[point] = worldPoints[point];
        }
        out[eventCount].mNormal = worldNormal;
        out[eventCount].mEventId = eventId;
        eventCount++;
    }
    /* Edges and vertices use the enabled edge flags. */
    for (modelIndex = 0, modelCursor = (CCD_THRUMDL*)head->mModel;
         modelIndex < (u32)head->mCount && eventCount < GSCOLSYS2_THRU_MAX;
         modelIndex++, modelCursor++) {
        CCD_THRUMDL* model = modelCursor;
        u16 flags = model->mFlags;
        s32 vertexIndex;

        if (!(flags & triggerEdgeMask)) {
            continue;
        }
        {
            s32 duplicateIndex;

            for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
                if (out[duplicateIndex].mEventId == model->mEventId) {
                    break;
                }
            }
            if (duplicateIndex < eventCount) {
                continue;
            }
        }
        PSMTXMultVec((MtxPtr)rotMtx, (const Vec *)&model->mTri.mNormal, (Vec *)&worldNormal);
        if (PSVECDotProduct((const Vec *)&worldNormal, (const Vec *)end) >= 0.0f) {
            continue;
        }
        for (vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&model->mTri.mPoint[vertexIndex], (Vec *)&worldPoints[vertexIndex]);
        }
        if (!checkThruEdge(&worldNormal, worldPoints, start, radiusSquared, model->mFlags)) continue;
        u16 eventId = model->mEventId;
        for (s32 point = 0; point < 3; point++) {
            out[eventCount].mPoint[point] = worldPoints[point];
        }
        out[eventCount].mNormal = worldNormal;
        out[eventCount].mEventId = eventId;
        eventCount++;
    }
    for (modelIndex = 0, modelCursor = (CCD_THRUMDL*)head->mModel;
         modelIndex < (u32)head->mCount && eventCount < GSCOLSYS2_THRU_MAX;
         modelIndex++, modelCursor++) {
        CCD_THRUMDL* model = modelCursor;
        u16 flags = model->mFlags;
        s32 vertexIndex;

        if (!(flags & triggerEdgeMask)) continue;
        {
            s32 duplicateIndex;

            for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
                if (out[duplicateIndex].mEventId == model->mEventId) {
                    break;
                }
            }
            if (duplicateIndex < eventCount) {
                continue;
            }
        }
        PSMTXMultVec((MtxPtr)rotMtx, (const Vec *)&model->mTri.mNormal, (Vec *)&worldNormal);
        if (PSVECDotProduct((const Vec *)&worldNormal, (const Vec *)end) >= 0.0f) {
            continue;
        }
        for (vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&model->mTri.mPoint[vertexIndex], (Vec *)&worldPoints[vertexIndex]);
        }
        if (!checkThruVertex(&worldNormal, worldPoints, start, radiusSquared, model->mFlags)) continue;
        u16 eventId = model->mEventId;
        for (s32 point = 0; point < 3; point++) {
            out[eventCount].mPoint[point] = worldPoints[point];
        }
        out[eventCount].mNormal = worldNormal;
        out[eventCount].mEventId = eventId;
        eventCount++;
    }
    return eventCount;
}
s32 getEventList(GSvec* position, GSvec* direction, f32 radius,
                 GScolsys2Thru_c* out)
{
    CCD_FILEHEAD* collisionFile;
    CCD_OBJECT* objectCursor;
    u32 objectIndex;
    s32 eventCount = 0;
    GScolsys2Thru_c objectEvents[GSCOLSYS2_THRU_MAX];
    GSmtx objectMatrix;
    GSmtx rotationMatrix;

    collisionFile = GScolsys2GetCCDFileHead();
    objectCursor = collisionFile->mObject;
    for (objectIndex = 0; objectIndex < (u32)collisionFile->mObjectCount && eventCount < GSCOLSYS2_THRU_MAX; objectIndex++, objectCursor++) {
        CCD_THRUMDL_HEAD* head;
        u8 enabled;
        s32 objectEventCount;
        s32 eventIndex;

        GScolsys2GetObjEnable(objectIndex, &enabled);
        if (enabled == 0) continue;
        head = objectCursor->mThruMdl;
        if (head == 0) continue;
        if (objectCursor->mFlags & GSCOLSYS2_OBJECT_MOVING) {
            GScolsys2GetObjMatrix(&objectMatrix, objectIndex);
            GScolsys2GetObjRotMatrix(&rotationMatrix, objectIndex);
            objectEventCount = getMdlEventList(position, direction, radius, head, &objectMatrix, &rotationMatrix, objectEvents);
        } else {
            objectEventCount = getFixedMdlEventList(position, direction, radius, head, objectEvents);
        }
        for (eventIndex = 0; eventIndex < objectEventCount && eventCount < GSCOLSYS2_THRU_MAX; eventIndex++) {
            s32 duplicateIndex;
            for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
                if (out[duplicateIndex].mEventId == objectEvents[eventIndex].mEventId) {
                    break;
                }
            }
            if (duplicateIndex < eventCount) continue;
            s32 outputIndex = eventCount++;
            for (s32 point = 0; point < 3; point++) {
                out[outputIndex].mPoint[point] = objectEvents[eventIndex].mPoint[point];
            }
            out[outputIndex].mNormal = objectEvents[eventIndex].mNormal;
            out[outputIndex].mEventId = objectEvents[eventIndex].mEventId;
        }
    }
    return eventCount;
}

extern "C" s32 GScolsys2ThruGetEventID(GSvec* start, GSvec* end, f32 radius,
                                      GScolsys2Thru_c* out)
{
    GScolsys2Thru_c sampleEvents[GSCOLSYS2_THRU_MAX];
    s32 eventCount = 0;
    GSvec direction;
    GSvec position;
    f32 progress, step, distance;

    if (GScolsys2GetCCDFileHead() == 0) return 0;
    PSVECSubtract((const Vec *)end, (const Vec *)start, (Vec *)&direction);
    distance = PSVECMag((const Vec *)&direction);
    if (distance <= 0.0f) return 0;
    step = radius / distance;
    if (step > 1.0f) step = 1.0f;
    for (progress = 0.0f; progress < 1.0f && eventCount < GSCOLSYS2_THRU_MAX; progress += step) {
        f32 sample = progress + step;
        if (sample > 1.0f) sample = 1.0f;
        PSVECScale((const Vec *)&direction, (Vec *)&position, sample);
        PSVECAdd((const Vec *)&position, (const Vec *)start, (Vec *)&position);
        s32 sampleCount = getEventList(&position, &direction, radius, sampleEvents);
        for (s32 sampleIndex = 0; sampleIndex < sampleCount && eventCount < GSCOLSYS2_THRU_MAX; sampleIndex++) {
            s32 duplicateIndex;
            for (duplicateIndex = 0; duplicateIndex < eventCount; duplicateIndex++) {
                if (sampleEvents[sampleIndex].mEventId == out[duplicateIndex].mEventId) break;
            }
            if (duplicateIndex < eventCount) continue;
            s32 outputIndex = eventCount++;
            for (s32 point = 0; point < 3; point++) {
                out[outputIndex].mPoint[point] = sampleEvents[sampleIndex].mPoint[point];
            }
            out[outputIndex].mNormal = sampleEvents[sampleIndex].mNormal;
            out[outputIndex].mEventId = sampleEvents[sampleIndex].mEventId;
        }
        if (step <= 0.0f) break;
    }
    return eventCount;
}
