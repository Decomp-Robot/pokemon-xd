#include <GScolsys2Internal.h>
#include <GSinput.h>
extern "C" {
#include <dolphin/gx/GXEnum.h>
}

extern u8 gsColSysFixedObjDisplayList[];

extern "C" {
void* memset(void* dst, int value, unsigned int size);
void GSgfxDLBegin(void* list, GSgfxVF* vf, s32 size, s32 offset);
void GSgfxDLEnd(void);
void floorMapSetDisp(s32 flag);
void GSgfxDLEndSurface(void);
}

union CollisionDebugColor { u8 channels[4]; u32 packed; };

inline void GXColor4u8(u8 red, u8 green, u8 blue, u8 alpha)
{
    GXWGFifo.u8 = red;
    GXWGFifo.u8 = green;
    GXWGFifo.u8 = blue;
    GXWGFifo.u8 = alpha;
}

inline void writePackedCollisionDebugColor(u32* packed)
{
    u8* color = (u8*)packed;
    GXColor4u8(color[0], color[1], color[2], color[3]);
}

template <class Model, class Head> inline void drawDebugMesh(Head* head, GSmtx* mtx, const CollisionDebugColor& meshColor)
{

    u32 t;
    s32 k;
    Model* mdl;

    mdl = (Model*)head->mModel;
    GSgfxDLPrimitiveType(GX_TRIANGLES);
    for (t = 0; t < (u32)head->mCount; t++, mdl++) {
        GSgfxDLBeginSurface(3);
        for (k = 0; k < 3; k++) {
            GSvec v;
            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&mdl->mTri.mPoint[k], (Vec *)&v);
            GXWGFifo.f32 = v.x;
            GXWGFifo.f32 = v.y;
            GXWGFifo.f32 = v.z;
            GXWGFifo.u8 = meshColor.channels[0];
            GXWGFifo.u8 = meshColor.channels[1];
            GXWGFifo.u8 = meshColor.channels[2];
            GXWGFifo.u8 = meshColor.channels[3];
        }
        GSgfxDLEndSurface();
    }
}

void drawHitMdl(CCD_HITMDL_HEAD* head, GSmtx* mtx, GSmtx* rotMtx)
{
    GSvec tri[3];
    CollisionDebugColor color = {{255,255,255,255}};
    u32 t;
    CCD_HITMDL* mdl;
    s32 i;

    mdl = (CCD_HITMDL*)head->mModel;
    for (t = 0; t < (u32)head->mCount; t++, mdl++) {
        for (i = 0; i < 3; i++) {
            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&mdl->mTri.mPoint[i], (Vec *)&tri[i]);
        }
        GSgfxDLPrimitiveType(GX_LINES);
        for (i = 0; i < 3; i++) {
            s32 j = i + 1;

            if (j >= 3) {
                j = 0;
            }
            GSgfxDLBeginSurface(2);
            GXWGFifo.f32 = tri[i].x;
            GXWGFifo.f32 = tri[i].y;
            GXWGFifo.f32 = tri[i].z;
            writePackedCollisionDebugColor((u32*)&color);
            GXWGFifo.f32 = tri[j].x;
            GXWGFifo.f32 = tri[j].y;
            GXWGFifo.f32 = tri[j].z;
            writePackedCollisionDebugColor((u32*)&color);
            GSgfxDLEndSurface();
        }
    }
}

inline void drawThruMdl(CCD_THRUMDL_HEAD* head, GSmtx* mtx) { CollisionDebugColor color = {{255,0,255,192}}; drawDebugMesh<CCD_THRUMDL>(head, mtx, color); }

inline void drawSecondaryThruMdl(CCD_THRUMDL_HEAD* head, GSmtx* mtx) { CollisionDebugColor color = {{255,255,0,192}}; drawDebugMesh<CCD_THRUMDL>(head, mtx, color); }

inline void drawCheckMdl(CCD_CHECKMDL_HEAD* head, GSmtx* mtx) { CollisionDebugColor color = {{0,255,255,192}}; drawDebugMesh<CCD_CHECKMDL>(head, mtx, color); }

void drawWalkMdl(CCD_WALKMDL_HEAD* head, GSmtx* mtx, GSmtx* rotMtx)
{
    u8 packed[4];
    u32 t;
    CCD_WALKMDL* mdl;
    s32 k;

    GSgfxDLPrimitiveType(GX_TRIANGLES);
    mdl = (CCD_WALKMDL*)head->mModel;
    for (t = 0; t < (u32)head->mCount; t++, mdl++) {
        s32 lo;
        s32 hi;

        memset(packed, 0, 4);
        packed[3] = 0xC0;
        lo = mdl->mColor0 & 0xF;
        hi = (mdl->mColor0 >> 4) & 0xF;

        lo++;
        packed[1] = (u8)(s32)((127.0f) * ((f32)hi / (15.0f)) + (128.0f));
        if (lo >= 16) {
            lo = 0;
        }
        packed[2] = (u8)(s32)((255.0f) * ((f32)lo / (15.0f)));
        /* The high colour nibble tints the red channel. */
        if (((mdl->mColor1 >> 4) & 0xF) > 0) {
            packed[0] = (u8)((((mdl->mColor1 >> 4) & 0xF) << 2) + 0xC0);
        }
        GSgfxDLBeginSurface(3);
        for (k = 0; k < 3; k++) {
            GSvec v;

            PSMTXMultVec((MtxPtr)mtx, (const Vec *)&mdl->mTri.mPoint[k], (Vec *)&v);
            GXWGFifo.f32 = v.x;
            GXWGFifo.f32 = (0.1f) + v.y;
            GXWGFifo.f32 = v.z;
            GXWGFifo.u8 = packed[0];
            GXWGFifo.u8 = packed[1];
            GXWGFifo.u8 = packed[2];
            GXWGFifo.u8 = packed[3];
        }
        GSgfxDLEndSurface();
    }
}

void* makeDisplayListFixedObj(void)
{
    u32 i;
    CCD_OBJECT* object;
    CCD_FILEHEAD* file;

    file = GScolsys2GetCCDFileHead();
    if (file == 0) {
        return 0;
    }
    memset(gsColSysFixedObjDisplayList, 0, 0x10);
    GSgfxDLBegin(gsColSysFixedObjDisplayList, gsColSysState.mVf, 0x400, 0x8000);
    object = file->mObject;
    for (i = 0; i < (u32)file->mObjectCount; i++, object++) {
        GSmtx obj;
        GSmtx rot;

        if (object->mFlags & GSCOLSYS2_OBJECT_MOVING) {
            continue;
        }
        GScolsys2GetObjMatrix(&obj, i);
        GScolsys2GetObjRotMatrix(&rot, i);
        if (object->mWalkMdl != 0) {
            drawWalkMdl(object->mWalkMdl, &obj, &rot);
        }
        if (object->mHitMdl != 0) {
            drawHitMdl(object->mHitMdl, &obj, &rot);
        }
        if (object->mThruMdl != 0) {
            drawThruMdl(object->mThruMdl, &obj);
        }
        if (object->mThruMdl2 != 0) {
            drawSecondaryThruMdl(object->mThruMdl2, &obj);
        }
        if (object->mHitMdl2 != 0) {
            drawHitMdl(object->mHitMdl2, &obj, &rot);
        }
        if (object->mCheckMdl != 0) {
            drawCheckMdl(object->mCheckMdl, &obj);
        }
    }
    GSgfxDLEnd();
    return gsColSysFixedObjDisplayList;
}

extern "C" void GScolsys2Draw(void)
{
    u32 i;
    CCD_OBJECT* object;
    CCD_FILEHEAD* file;
    GSCOLSYS2_OBJECT_STATE* draw;
    GSmtx obj;
    GSmtx rot;
    void* list;

    file = GScolsys2GetCCDFileHead();
    if (file == 0) {
        return;
    }
    if ((GSinputButtonsPressed(GS_INPUT_PORT_1) & GSCOLSYS2_DRAW_BUTTON_Z) != 0) {
        floorMapSetDisp(0);
    } else if ((GSinputButtonsPressed(GS_INPUT_PORT_1) & GSCOLSYS2_DRAW_BUTTON_X) != 0) {
        floorMapSetDisp(1);
    }
    GSgfxLayerSetActive((GSgfxLayerID)2);
    GScameraLoadActiveCameraMatrix(GSGFX_LAYER_DEFAULT);
    GSgfxCullMode(GX_CULL_FRONT);
    GSgfxSelectVertexFormat(gsColSysState.mVf);
    GSgfxBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA);
    GSgfxZCompare(GX_TRUE, GX_LEQUAL);
    GSgfxPixelMode(GX_TRUE, GX_TRUE, GX_TRUE);
    object = file->mObject;
    draw = gsColSysState.mFloor[gsColSysState.mTableIndex].mObject;
    for (i = 0;
         i < (u32)file->mObjectCount; i++, object++, draw++) {
        /* Draw only moving objects whose floor record is enabled. */
        if (draw->mFlags & GSCOLSYS2_OBJECT_DISABLED) {
            continue;
        }
        if (!(object->mFlags & GSCOLSYS2_OBJECT_MOVING)) {
            continue;
        }
        GScolsys2GetObjMatrix(&obj, i);
        GScolsys2GetObjRotMatrix(&rot, i);
        if (object->mWalkMdl != 0) {
            drawWalkMdl(object->mWalkMdl, &obj, &rot);
        }
        if (object->mHitMdl != 0) {
            drawHitMdl(object->mHitMdl, &obj, &rot);
        }
        if (object->mThruMdl != 0) {
            drawThruMdl(object->mThruMdl, &obj);
        }
        if (object->mThruMdl2 != 0) {
            drawSecondaryThruMdl(object->mThruMdl2, &obj);
        }
        if (object->mHitMdl2 != 0) {
            drawHitMdl(object->mHitMdl2, &obj, &rot);
        }
        if (object->mCheckMdl != 0) {
            drawCheckMdl(object->mCheckMdl, &obj);
        }

    }
    /* The fixed-object display list is built once and cached; the human
     * collision overlay is drawn whether or not the list was ready. */
    if (gsColSysState.mFixedDisplayList == 0) {
        gsColSysState.mFixedDisplayList = makeDisplayListFixedObj();
    }
    list = gsColSysState.mFixedDisplayList;
    if (list != 0) {
        GSgfxBeginSurface(list);
    }
    GScolsys2HumanDispColision();
}
