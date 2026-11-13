#include <GScolsys2Internal.h>

/* Collect the triggers of one check mesh the segment runs into.
 * The face is used when the segment crosses its plane inside the triangle and
 * the face points against the travel direction. Returns whether anything was
 * reported; the triggers themselves are written to out. */
bool getMdlEventList(GSvec* start, GSvec* end, GSvec* direction, CCD_CHECKMDL_HEAD* head,
                     GSmtx* mtx, GSmtx* rotMtx, GScolsys2Check_c* out)
{
    GSvec worldPoints[3];
    GSvec worldNormal;
    GSvec crossing;
    f32 parameter;
    CCD_CHECKMDL* modelCursor;
    u32 modelIndex;
    s32 eventCount;

    eventCount = 0;
    modelCursor = head->mModel;
    for (modelIndex = 0; modelIndex < (u32)head->mCount && eventCount < GSCOLSYS2_CHECK_MAX; modelIndex++, modelCursor++) {
        CCD_CHECKMDL* model = modelCursor;
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
        if (PSVECDotProduct((const Vec *)&worldNormal, (const Vec *)direction) >= 0.0f) {
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
    return eventCount != 0;
}

/* Walk every enabled object of the loaded CCD file and merge the triggers its
 * check mesh reports, one per event id. */
s32 getEventList(GSvec* start, GSvec* end, GScolsys2Check_c* out)
{
    CCD_FILEHEAD* collisionFile;
    CCD_OBJECT* objectCursor;
    CCD_CHECKMDL_HEAD* head;
    s32 eventIndex;
    s32 duplicateIndex;
    u32 objectIndex;
    u8 objectEventCount;
    s32 eventCount;

    eventCount = 0;
    GScolsys2Check_c objectEvents[GSCOLSYS2_CHECK_MAX];
    GSmtx objectMatrix;
    GSmtx rotationMatrix;
    GSvec direction;

    collisionFile = GScolsys2GetCCDFileHead();
    PSVECSubtract((const Vec *)end, (const Vec *)start, (Vec *)&direction);
    objectCursor = collisionFile->mObject;
    for (objectIndex = 0; objectIndex < collisionFile->mObjectCount && eventCount < GSCOLSYS2_CHECK_MAX; objectIndex++, objectCursor++) {
        u8 enabled;

        GScolsys2GetObjEnable(objectIndex, &enabled);
        if (enabled == 0) {
            continue;
        }
        head = objectCursor->mCheckMdl;
        if (head == 0) {
            continue;
        }
        GScolsys2GetObjMatrix(&objectMatrix, objectIndex);
        GScolsys2GetObjRotMatrix(&rotationMatrix, objectIndex);
        objectEventCount = getMdlEventList(start, end, &direction, head, &objectMatrix, &rotationMatrix, objectEvents);
        for (eventIndex = 0; eventIndex < objectEventCount && eventCount < GSCOLSYS2_CHECK_MAX; eventIndex++) {
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

/* Return the floor collision event IDs along the segment from start to end. */
extern "C" s32 GScolsys2CheckGetEventID(GSvec* start, GSvec* end, GScolsys2Check_c* out)
{
    if (GScolsys2GetCCDFileHead() == 0) {
        return 0;
    }
    if (PSVECDistance((const Vec *)end, (const Vec *)start) <= 0.0f) {
        return 0;
    }
    return getEventList(start, end, out);
}
