#ifndef GSCOLSYS2_H
#define GSCOLSYS2_H

#include <GSgfx.h>
#include <GSvec.h>
#include <types.h>

/* Moving objects use transformed collision meshes. */
#define GSCOLSYS2_OBJECT_MOVING 0x0001

/* Disables an object in the current floor collision table. */
#define GSCOLSYS2_OBJECT_DISABLED 0x0001

/* Bytes in one floor record. */
#define GSCOLSYS2_FLOOR_SIZE 0xDC0

/* Objects and registered humans per floor block. */
#define GSCOLSYS2_OBJECT_MAX 0x40
#define GSCOLSYS2_HUMAN_MAX 0x30
#define GSCOLSYS2_THRU_MAX 4
#define GSCOLSYS2_WALK_MAX 8

/* Passes the push solve makes before it gives up. */
#define GSCOLSYS2_HUMAN_PASSES 10

/* One triangle of a collision mesh: three corner points (0x00-0x23) and the
 * face normal (0x24-0x2F). Every CCD_*MDL entry starts with this block; the
 * per-kind records append their own fields after it. */
struct CCD_TRI {
    GSvec mPoint[3];
    GSvec mNormal;
};

/* Hit-mesh entry (0x34 bytes). */
struct CCD_HITMDL {
    CCD_TRI mTri;
    /* 0x30: triangle edge/vertex selection masks. */
    u16 mEdgeMask[2];
};

/* Walk-mesh entry (0x34 bytes). Same triangle block; 0x30 and 0x31 hold the
 * two texture-coordinate bytes drawWalkMdl packs into the vertex colour. */
struct CCD_WALKMDL {
    CCD_TRI mTri;
    u8 mColor0;  // 0x30
    u8 mColor1;  // 0x31
    u8 _32[2];
};

/* Thru-mesh entry (0x34 bytes). Carries the event id the getMdlEventList
 * walkers report in 0x32. */
struct CCD_THRUMDL {
    CCD_TRI mTri;
    u16 mFlags;   // 0x30
    u16 mEventId; // 0x32
};

/* Check-mesh entry (0x30 bytes): the triangle block only. */
struct CCD_CHECKMDL {
    CCD_TRI mTri;
};

/* Collision-mesh head. mModel points at an array of mCount entries; the
 * GScolsys2 draw and check entry points walk it in 0x34 (or 0x30) strides. */
struct CCD_MDLHEAD {
    void* mModel;
    s32 mCount;
};

/* Common head of a bucketed mesh: the model array plus the uniform grid the
 * mesh is bucketed into. Every GScolsys2 head but the check mesh carries one. */
struct CCD_GRIDHEAD {
    void* mModel;      // 0x00
    s32 mCount;        // 0x04
    s32* mCellStart;   // 0x08 {first, count} per cell
    s32* mCellList;    // 0x0C flat triangle indices
    u16 mCountX;       // 0x10 cells along x
    u16 mCountZ;       // 0x12 cells along z
    f32 mCellSizeX;    // 0x14
    f32 mCellSizeZ;    // 0x18
    f32 mOriginX;      // 0x1C
    f32 mOriginZ;      // 0x20
};

struct CCD_HITMDL_HEAD : public CCD_GRIDHEAD {
};

struct CCD_WALKMDL_HEAD : public CCD_GRIDHEAD {
};

/* A walk-surface head is only bucketed when the object record sets
 * GSCOLSYS2_OBJECT_WALK_GRID; otherwise every triangle is tested. */
#define GSCOLSYS2_OBJECT_WALK_GRID 0x8000

struct CCD_THRUMDL_HEAD : public CCD_GRIDHEAD {
};

struct CCD_CHECKMDL_HEAD {
    CCD_CHECKMDL* mModel;
    s32 mCount;
};

/* One CCD object record (0x40 bytes). */
struct CCD_OBJECT {
    u8 _00[0x24];
    CCD_WALKMDL_HEAD* mWalkMdl;  // 0x24
    CCD_HITMDL_HEAD* mHitMdl;    // 0x28
    CCD_THRUMDL_HEAD* mThruMdl;  // 0x2C
    CCD_THRUMDL_HEAD* mThruMdl2; // 0x30
    CCD_HITMDL_HEAD* mHitMdl2;   // 0x34
    CCD_CHECKMDL_HEAD* mCheckMdl; // 0x38
    u16 mFlags;                  // 0x3C
};

/* Per-floor object transform and collision flags (0x28 bytes). */
class GSCOLSYS2_OBJECT_STATE {
public:
    GSvec mPosition; // 0x00
    GSvec mRotation; // 0x0C
    GSvec mScale;    // 0x18
    u16 mFlags;     // 0x24
    u16 mPadding;
    GSCOLSYS2_OBJECT_STATE();
};

/* The whole collision-file header the GScolsys2 entry points read through
 * (GScolsys2LoadCCD stores the loaded file here). */
struct CCD_FILEHEAD {
    CCD_OBJECT* mObject; // 0x00
    s32 mObjectCount;    // 0x04
};

/* Human-table registration and collision-enable flags. */
#define GSCOLSYS2_HUMAN_IN_USE 0x0001
#define GSCOLSYS2_HUMAN_DISABLED 0x0002

/* Debug buttons that toggle the floor map while the collision view draws. */
#define GSCOLSYS2_DRAW_BUTTON_Z 0x00000040
#define GSCOLSYS2_DRAW_BUTTON_X 0x00000020

enum GSCOLSYS2_RESULT {
    GSCOLSYS2_SUCCESS = 0,
    GSCOLSYS2_NO_CURRENT_FLOOR = 1,
    GSCOLSYS2_INVALID_HUMAN = 4,
    GSCOLSYS2_HUMAN_TABLE_FULL = 5,
    GSCOLSYS2_COLLISION = 6,
    GSCOLSYS2_NO_COLLISION = 7
};

/* One human registered in a floor's collision table (0x14 bytes). */
struct GSCOLSYS2_HUMAN {
    s32 mGroupId;    // 0x00
    s32 mResourceId; // 0x04
    f32 mRadius;     // 0x08
    f32 mHeight;     // 0x0C
    u16 mFlags;      // 0x10: in-use and disabled flags
    u16 _12;
};

/* Per-floor object states and human tables (0xDC0 bytes). */
struct GSCOLSYS2_FLOOR {
    GSCOLSYS2_OBJECT_STATE mObject[GSCOLSYS2_OBJECT_MAX]; /* 0x000-0x9FF */
    GSCOLSYS2_HUMAN mHuman[GSCOLSYS2_HUMAN_MAX]; /* 0xA00-0xDBF */
    GSCOLSYS2_FLOOR();
};

/* One hit result: a triangle reference plus its face normal. */
class GScolsys2Thru_c {
public:
    GSvec mPoint[3];
    GSvec mNormal;
    u16 mEventId; // 0x30
    u16 _32;

    /* The caller fills the result fields. */
    inline GScolsys2Thru_c() {}
};

struct GScolsys2Check_c {
    GSvec mPoint[3];
    GSvec mNormal;
    u16 mFlags;
    u16 _32;
};

/* One walk-height sample GScolsys2WalkGetHeight fills in (0x0C bytes). */
struct GSCOLSYS2_WALK_HEIGHT {
    f32 mHeight; // 0x00
    u16 mLayer;  // 0x04
    u16 mLayer2; // 0x06
    u8 mColor0;  // 0x08
    u8 mColor1;  // 0x09
    u8 _0A[2];
};


extern "C" {
s32 GScolsys2GetObjEnable(s32 index, u8* enable);
s32 GScolsys2SetObjEnable(s32 index, s32 enable);
s32 GScolsys2GetObjRotMatrix(GSmtx* out, s32 index);
s32 GScolsys2GetObjMatrix(GSmtx* out, s32 index);
GSCOLSYS2_FLOOR* GScolsys2GetCurFloor(void);
s32 GScolsys2UnloadCCD(void);
/* Takes the collision file the loader of a stage read in. */
s32 GScolsys2LoadCCD(void* resource);

void GScolsys2Draw(void);
s32 GScolsys2WalkGetLayer(GSvec* pos, u8* color0, u8* color1);
s32 GScolsys2WalkGetHeight(GSvec* pos, GSCOLSYS2_WALK_HEIGHT* out);
s32 GScolsys2HitNpcCollision(GSvec* a, GSvec* b, f32 radius, GSvec* out);
s32 GScolsys2HitCollision(GSvec* a, GSvec* b, f32 radius, GSvec* out);

bool GScolsys2UtilGetCpPlaneLine(GSvec* out, f32* t, GSvec* normal,
                               GSvec* org, GSvec* start, GSvec* end);
f32 GScolsys2UtilGetCpLinePoint(GSvec* out, GSvec* a, GSvec* b, GSvec* p);
void GScolsy2UtilGetPointExtentionLine(GSvec* out, GSvec* start, f32 len, GSvec* end);
void GScolsy2UtilGetCpPlanePoint(GSvec* cp, GSvec* normal, GSvec* tri, GSvec* pos);
u8 GScolsy2UtilChkInTri(f32* p, f32* tri, GSvec* normal);
f32 GScolsy2UtilGetSidePlanePoint(GSvec* normal, void* tri, GSvec* pos);

f32 GScolsys2HumanGetHumanRadius(s32 index);
void GScolsys2HumanDispColision(void);
s32 GScolsys2HumanCollision(int index, GSvec* a, GSvec* b, GSvec* out);
s32 GScolsys2HumanEnable(s32 index, s32 enable);
s32 GScolsys2HumanAddCurrentFloor(s32* slotIndex, void* character);
f32 GScolsys2HumanGetWalkHeight(GSvec* pos);
f32 GScolsys2HumanGetWalkHighest(GSvec* pos);
s32 GScolsys2ThruPassEventID(GSvec* start, GSvec* end, GScolsys2Thru_c* out);
s32 GScolsys2ThruGetEventID(GSvec* start, GSvec* end, f32 radius, GScolsys2Thru_c* out);
}

#endif
