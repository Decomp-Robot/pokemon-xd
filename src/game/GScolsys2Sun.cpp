#include <GScolsys2Internal.h>
#include <string.h>

enum {
    GSCOLSYS2_SUN_ALL_HIT = -1,
    GSCOLSYS2_SUN_NO_HIT = 0,
    GSCOLSYS2_SUN_HIT = 1
};

/* Keep earlier hits and report when every segment has been occluded. */

s32 getCpSegPolyArray(bool* hits, GSvec* start, GSvec* ends, u32 count, GSvec* points, GSvec* normal)
{
    GSvec crossing;
    f32 parameter;
    u32 ray;
    bool anyHit;
    bool allHit;
    GSvec* endCursor;

    anyHit = false;
    allHit = true;
    endCursor = ends;
    for (ray = 0; ray < count; ray++, endCursor++, hits++) {
        if (*hits) {
            continue;
        }
        if (PSVECSquareDistance((const Vec*)start, (const Vec*)endCursor) < 0.00001f) {
            allHit = false;
            continue;
        }
        if (!GScolsys2UtilGetCpPlaneLine(&crossing, &parameter, normal, points, start, endCursor)) {
            allHit = false;
            continue;
        }
        if (parameter < 0.0f || parameter > 1.0f) {
            allHit = false;
            continue;
        }
        if (!(u8)GScolsy2UtilChkInTri((f32*)&crossing, (f32*)points, normal)) {
            allHit = false;
            continue;
        }
        anyHit = true;
        *hits = true;
    }
    if (allHit) {
        return GSCOLSYS2_SUN_ALL_HIT;
    }
    return anyHit != false;
}

inline s32 getMdlSunArray(bool* hits, GSvec* start, GSvec* ends, u32 count,
                          CCD_TRIMDL_HEAD* head, GSmtx* matrix, GSmtx* rotation)
{
    GSvec worldPoints[3];
    GSvec worldNormal;
    CCD_TRIMDL* model;
    u32 triangle;
    s32 vertex;
    s32 result;

    result = GSCOLSYS2_SUN_NO_HIT;
    model = head->mModel;
    for (triangle = 0; triangle < (u32)head->mCount; triangle++, model++) {
        PSMTXMultVec((MtxPtr)rotation, (const Vec*)&model->mTri.mNormal, (Vec*)&worldNormal);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec((MtxPtr)matrix, (const Vec*)&model->mTri.mPoint[vertex], (Vec*)&worldPoints[vertex]);
        }
        s32 hit = getCpSegPolyArray(hits, start, ends, count, worldPoints, &worldNormal);
        if (hit < 0) {
            result = GSCOLSYS2_SUN_ALL_HIT;
            break;
        } else if (hit > 0) {
            result = GSCOLSYS2_SUN_HIT;
        }
    }
    return result;
}

inline bool getSunArrayList(bool* hits, GSvec* start, GSvec* ends, u32 count)
{
    CCD_OBJECT* object;
    CCD_TRIMDL_HEAD* head;
    u32 objectIndex;
    bool result;
    CCD_FILEHEAD* file;
    GSmtx matrix;
    GSmtx rotation;
    u8 enabled;

    result = false;
    file = GScolsys2GetCCDFileHead();
    object = file->mObject;
    for (objectIndex = 0; objectIndex < (u32)file->mObjectCount; objectIndex++, object++) {
        GScolsys2GetObjEnable(objectIndex, &enabled);
        if (!enabled) {
            continue;
        }
        head = object->mTriMdl;
        if (head == NULL) {
            continue;
        }
        GScolsys2GetObjMatrix(&matrix, objectIndex);
        GScolsys2GetObjRotMatrix(&rotation, objectIndex);
        s32 hit = getMdlSunArray(hits, start, ends, count, head, &matrix, &rotation);
        if (hit < 0) {
            result = true;
            break;
        } else if (hit > 0) {
            result = true;
        }
    }
    return result;
}

/* Each endpoint forms a segment from the common start point. */
extern "C" bool GScolsys2SunArray(bool* hits, GSvec* start, GSvec* ends, u32 count)
{
    if (GScolsys2GetCCDFileHead() == NULL) {
        return false;
    }
    memset(hits, 0, count);
    return getSunArrayList(hits, start, ends, count);
}

inline bool getCpSegPoly(GSvec* start, GSvec* end, GSvec* points, GSvec* normal)
{
    GSvec crossing;
    f32 parameter;
    if (!GScolsys2UtilGetCpPlaneLine(&crossing, &parameter, normal, points, start, end)) {
        return false;
    } else if (parameter < 0.0f || parameter > 1.0f) {
        return false;
    } else if (!(u8)GScolsy2UtilChkInTri((f32*)&crossing, (f32*)points, normal)) {
        return false;
    }
    return true;
}

inline bool getMdlSun(GSvec* start, GSvec* end, GSvec* direction, CCD_TRIMDL_HEAD* head,
                       GSmtx* matrix, GSmtx* rotation)
{
    GSvec worldPoints[3];
    GSvec worldNormal;
    CCD_TRIMDL* model = head->mModel;
    for (u32 triangle = 0; triangle < (u32)head->mCount; triangle++, model++) {
        PSMTXMultVec((MtxPtr)rotation, (const Vec*)&model->mTri.mNormal, (Vec*)&worldNormal);
        for (s32 vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec((MtxPtr)matrix, (const Vec*)&model->mTri.mPoint[vertex], (Vec*)&worldPoints[vertex]);
        }
        if (getCpSegPoly(start, end, worldPoints, &worldNormal)) {
            return true;
        }
    }
    return false;
}

inline bool getSunList(GSvec* start, GSvec* end)
{
    CCD_OBJECT* object;
    CCD_TRIMDL_HEAD* head;
    u32 objectIndex;
    CCD_FILEHEAD* file;
    GSmtx matrix;
    GSmtx rotation;
    GSvec direction;
    u8 enabled;

    file = GScolsys2GetCCDFileHead();
    PSVECSubtract((const Vec*)end, (const Vec*)start, (Vec*)&direction);
    object = file->mObject;
    for (objectIndex = 0; objectIndex < (u32)file->mObjectCount; objectIndex++, object++) {
        GScolsys2GetObjEnable(objectIndex, &enabled);
        if (!enabled) {
            continue;
        }
        head = object->mTriMdl;
        if (head == NULL) {
            continue;
        }
        GScolsys2GetObjMatrix(&matrix, objectIndex);
        GScolsys2GetObjRotMatrix(&rotation, objectIndex);
        if (getMdlSun(start, end, &direction, head, &matrix, &rotation)) {
            return true;
        }
    }
    return false;
}

extern "C" bool GScolsys2Sun(GSvec* start, GSvec* end)
{
    if (GScolsys2GetCCDFileHead() == NULL) {
        return false;
    }
    if (PSVECDistance((const Vec*)end, (const Vec*)start) <= 0.0f) {
        return false;
    }
    return getSunList(start, end);
}
