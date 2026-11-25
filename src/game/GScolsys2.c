#include <GScolsys2.h>
#include <GScolsys2Internal.h>
#include <GSAPI/GScolsys/GScolsys.h>
#include <app/floor/floorModule.h>
#include <string.h>

GScolsys2State GScolsys2;

FloorModuleInterface _floorModuleColsys;
GSgfxVF _colsysVF;
unsigned long _hModuleField;

static const unsigned long GSCOLSYS2_MODULE_PRIORITY = 20;
static const unsigned long GSCOLSYS2_MODULE_UID = 20;
static const u16 GSCOLSYS2_SAVE_MAGIC = 0xAA55;

enum {
    GSCOLSYS2_FLOOR_TABLE_COUNT = sizeof(GScolsys2.mFloor) / sizeof(GScolsys2.mFloor[0])
};

struct GScolsys2SaveHead {
    u16 mMagic;
    u16 mPadding;
    u16 mObjectFlags[GSCOLSYS2_OBJECT_MAX];
};

f32 GScolsy2UtilGetSidePlanePoint(GSvec* normal, GSvec* tri, GSvec* pos)
{
    return normal->x * (pos->x - tri->x)
         + normal->y * (pos->y - tri->y)
         + normal->z * (pos->z - tri->z);
}

s32 GScolsys2GetObjEnable(s32 index, u8* enable)
{
    CCD_FILEHEAD* file;
    GSCOLSYS2_OBJ* objects;
    GSCOLSYS2_OBJ* object;
    s32 result;

    file = GScolsys2.mFile;
    if (file == 0) {
        result = GSCOLSYS2_NO_CURRENT_FLOOR;
    } else if (index < 0 || index >= file->mObjectCount) {
        result = GSCOLSYS2_INVALID_OBJECT;
    } else {
        result = GSCOLSYS2_SUCCESS;
        objects = GScolsys2.mFloor[GScolsys2.mTableIndex].mObject;
        objects += index;
        object = objects;
    }
    if (result != GSCOLSYS2_SUCCESS) {
        return result;
    }
    if (object->mFlags & GSCOLSYS2_OBJECT_DISABLED) {
        *enable = 0;
    } else {
        *enable = 1;
    }
    return GSCOLSYS2_SUCCESS;
}

s32 GScolsys2SetObjEnable(s32 index, u8 enable)
{
    CCD_FILEHEAD* file;
    GSCOLSYS2_OBJ* objects;
    GSCOLSYS2_OBJ* object;
    s32 result;

    file = GScolsys2.mFile;
    if (file == 0) {
        result = GSCOLSYS2_NO_CURRENT_FLOOR;
    } else if (index < 0 || index >= file->mObjectCount) {
        result = GSCOLSYS2_INVALID_OBJECT;
    } else {
        result = GSCOLSYS2_SUCCESS;
        objects = GScolsys2.mFloor[GScolsys2.mTableIndex].mObject;
        objects += index;
        object = objects;
    }
    if (result != GSCOLSYS2_SUCCESS) {
        return result;
    }
    if (enable != 0) {
        object->mFlags &= ~GSCOLSYS2_OBJECT_DISABLED;
    } else {
        object->mFlags |= GSCOLSYS2_OBJECT_DISABLED;
    }
    return GSCOLSYS2_SUCCESS;
}

s32 GScolsys2GetObjRotMatrix(GSmtx* matrix, s32 index)
{
    GSmtx rotX;
    GSmtx rotY;
    GSmtx rotZ;
    GSCOLSYS2_OBJ* object;

    if (GScolsys2.mFile == 0) {
        return 0;
    }
    if (index >= GScolsys2.mFile->mObjectCount) {
        return 0;
    }
    object = GScolsys2.mFloor[GScolsys2.mTableIndex].mObject;
    object += index;
    memcpy(matrix, mtxIdent, sizeof(GSmtx));
    if (object->mFlags & GSCOLSYS2_OBJECT_MOVING) {
        PSMTXRotRad(rotX.sdk(), 'X', object->mRotation.x);
        PSMTXRotRad(rotY.sdk(), 'Y', object->mRotation.y);
        PSMTXConcat(rotY.sdk(), rotX.sdk(), rotX.sdk());
        PSMTXRotRad(rotZ.sdk(), 'Z', object->mRotation.z);
        PSMTXConcat(rotZ.sdk(), rotX.sdk(), rotX.sdk());
        PSMTXConcat(rotX.sdk(), matrix->sdk(), matrix->sdk());
    }
    return 1;
}

s32 GScolsys2GetObjMatrix(GSmtx* matrix, s32 index)
{
    GSmtx rotX;
    GSmtx rotY;
    GSmtx rotZ;
    GSCOLSYS2_OBJ* object;

    if (GScolsys2.mFile == 0) {
        return 0;
    }
    if (index >= GScolsys2.mFile->mObjectCount) {
        return 0;
    }
    object = GScolsys2.mFloor[GScolsys2.mTableIndex].mObject;
    object += index;
    memcpy(matrix, mtxIdent, sizeof(GSmtx));
    if (object->mFlags & GSCOLSYS2_OBJECT_MOVING) {
        PSMTXScaleApply(matrix->sdk(), matrix->sdk(), object->mScale.x,
                        object->mScale.y, object->mScale.z);
        PSMTXRotRad(rotX.sdk(), 'X', object->mRotation.x);
        PSMTXRotRad(rotY.sdk(), 'Y', object->mRotation.y);
        PSMTXConcat(rotY.sdk(), rotX.sdk(), rotX.sdk());
        PSMTXRotRad(rotZ.sdk(), 'Z', object->mRotation.z);
        PSMTXConcat(rotZ.sdk(), rotX.sdk(), rotX.sdk());
        PSMTXConcat(rotX.sdk(), matrix->sdk(), matrix->sdk());
        PSMTXTransApply(matrix->sdk(), matrix->sdk(), object->mPosition.x,
                        object->mPosition.y, object->mPosition.z);
    }
    return 1;
}

CCD_FILEHEAD* GScolsys2GetCCDFileHead(void)
{
    return GScolsys2.mFile;
}

GSCOLSYS2_FLOOR* GScolsys2GetCurFloor(void)
{
    if (GScolsys2.mTableIndex < 0 ||
        GScolsys2.mTableIndex >= GSCOLSYS2_FLOOR_TABLE_COUNT) {
        return 0;
    }
    return &GScolsys2.mFloor[GScolsys2.mTableIndex];
}

s32 GScolsys2UnloadCCD(void)
{
    GScolsys2.mFile = 0;
    if (GScolsys2.mFixedDisplayList != 0) {
        GSgfxDLFree(GScolsys2.mFixedDisplayList);
        GScolsys2.mFixedDisplayList = 0;
    }
    return 1;
}

void _offsetCCD(CCD_FILEHEAD* file)
{
    CCD_OBJECT* object;
    CCD_GRIDHEAD* grid;
    u32 i;

    if (file == 0) {
        return;
    }
    file->mObject = (CCD_OBJECT*)((u32)file->mObject + (u32)file);
    object = file->mObject;
    for (i = 0; i < file->mObjectCount; i++, object++) {
        if (object->mWalkMdl != 0) {
            object->mWalkMdl = (CCD_WALKMDL_HEAD*)((u32)object->mWalkMdl + (u32)file);
            grid = object->mWalkMdl;
            if (grid->mModel != 0) {
                grid->mModel = (void*)((u32)grid->mModel + (u32)file);
            }
            if (grid->mCellStart != 0) {
                grid->mCellStart = (s32*)((u32)grid->mCellStart + (u32)file);
            }
            if (grid->mCellList != 0) {
                grid->mCellList = (s32*)((u32)grid->mCellList + (u32)file);
            }
        }
        if (object->mHitMdl != 0) {
            object->mHitMdl = (CCD_HITMDL_HEAD*)((u32)object->mHitMdl + (u32)file);
            grid = object->mHitMdl;
            if (grid->mModel != 0) {
                grid->mModel = (void*)((u32)grid->mModel + (u32)file);
            }
            if (grid->mCellStart != 0) {
                grid->mCellStart = (s32*)((u32)grid->mCellStart + (u32)file);
            }
            if (grid->mCellList != 0) {
                grid->mCellList = (s32*)((u32)grid->mCellList + (u32)file);
            }
        }
        if (object->mThruMdl != 0) {
            object->mThruMdl = (CCD_THRUMDL_HEAD*)((u32)object->mThruMdl + (u32)file);
            grid = object->mThruMdl;
            if (grid->mModel != 0) {
                grid->mModel = (void*)((u32)grid->mModel + (u32)file);
            }
            if (grid->mCellStart != 0) {
                grid->mCellStart = (s32*)((u32)grid->mCellStart + (u32)file);
            }
            if (grid->mCellList != 0) {
                grid->mCellList = (s32*)((u32)grid->mCellList + (u32)file);
            }
        }
        if (object->mCheckMdl != 0) {
            object->mCheckMdl = (CCD_CHECKMDL_HEAD*)((u32)object->mCheckMdl + (u32)file);
            if (object->mCheckMdl->mModel != 0) {
                object->mCheckMdl->mModel = (CCD_CHECKMDL*)((u32)object->mCheckMdl->mModel + (u32)file);
            }
        }
        if (object->mHitMdl2 != 0) {
            object->mHitMdl2 = (CCD_HITMDL_HEAD*)((u32)object->mHitMdl2 + (u32)file);
            grid = object->mHitMdl2;
            if (grid->mModel != 0) {
                grid->mModel = (void*)((u32)grid->mModel + (u32)file);
            }
            if (grid->mCellStart != 0) {
                grid->mCellStart = (s32*)((u32)grid->mCellStart + (u32)file);
            }
            if (grid->mCellList != 0) {
                grid->mCellList = (s32*)((u32)grid->mCellList + (u32)file);
            }
        }
        if (object->mTriMdl != 0) {
            object->mTriMdl = (CCD_TRIMDL_HEAD*)((u32)object->mTriMdl + (u32)file);
            if (object->mTriMdl->mModel != 0) {
                object->mTriMdl->mModel = (CCD_TRIMDL*)((u32)object->mTriMdl->mModel + (u32)file);
            }
        }
    }
}

s32 GScolsys2LoadCCD(CCD_FILEHEAD* resource)
{
    CCD_FILEHEAD* file;
    CCD_FILEHEAD* ccdFile;
    GSCOLSYS2_FLOOR* floor;
    GSCOLSYS2_OBJ* target;
    CCD_OBJECT* object;
    u32 i;

    file = (CCD_FILEHEAD*)resource;
    ccdFile = (CCD_FILEHEAD*)resource;
    if (GScolsys2.mTableIndex < 0) {
        return 0;
    }
    floor = &GScolsys2.mFloor[GScolsys2.mTableIndex];
    for (i = 0; i < GSCOLSYS2_HUMAN_MAX; i++) {
        floor->mHuman[i].mFlags &= ~GSCOLSYS2_HUMAN_IN_USE;
    }
    _offsetCCD(ccdFile);
    GScolsys2.mFile = file;
    if (file != 0) {
        object = file->mObject;
        target = GScolsys2.mFloor[GScolsys2.mTableIndex].mObject;
        for (i = 0; i < file->mObjectCount; i++) {
            target[i].mPosition = object[i].mPosition;
            target[i].mRotation = object[i].mRotation;
            target[i].mScale = object[i].mScale;
            target[i].mFlags = 0;
        }
    }
    return 1;
}

static const char* _gscolsysGetModuleName(void)
{
    static const char name[] = "GScolsys";

    return name;
}

static unsigned long _gscolsysGetUID(void)
{
    return GSCOLSYS2_MODULE_UID;
}

static unsigned long _gscolsysGetStateSize(FloorStateMode mode)
{
    if (mode == FLOOR_STATE_MODE_FIELD || mode == FLOOR_STATE_MODE_FIGHT) {
        return GSCOLSYS2_FLOOR_SIZE;
    }
    if (mode == FLOOR_STATE_MODE_SAVE) {
        return sizeof(GScolsys2SaveHead);
    }
    return 0;
}

static void _gscolsysRestoreStateData(FloorStateMode mode, void* data, unsigned long size)
{
    GScolsys2SaveHead* save;
    u16* flags;
    u32 i;

    if (mode == FLOOR_STATE_MODE_FIELD || mode == FLOOR_STATE_MODE_FIGHT) {
        if (size == GSCOLSYS2_FLOOR_SIZE) {
            memcpy(&GScolsys2.mFloor[0], data, GSCOLSYS2_FLOOR_SIZE);
        }
    } else if (mode == FLOOR_STATE_MODE_SAVE) {
        save = (GScolsys2SaveHead*)data;
        flags = save->mObjectFlags;
        for (i = 0; i < GSCOLSYS2_OBJECT_MAX; i++) {
            GScolsys2.mFloor[0].mObject[i].mFlags = *flags++;
        }
    }
}

static void _gscolsysMakeStateData(FloorStateMode mode, void* data, unsigned long size)
{
    GScolsys2SaveHead* save;
    u16* flags;
    u32 i;

    if (mode == FLOOR_STATE_MODE_FIELD || mode == FLOOR_STATE_MODE_FIGHT) {
        if (size == GSCOLSYS2_FLOOR_SIZE) {
            memcpy(data, &GScolsys2.mFloor[0], GSCOLSYS2_FLOOR_SIZE);
        }
    } else if (mode == FLOOR_STATE_MODE_SAVE) {
        save = (GScolsys2SaveHead*)data;
        save->mMagic = GSCOLSYS2_SAVE_MAGIC;
        save->mPadding = 0;
        flags = save->mObjectFlags;
        for (i = 0; i < GSCOLSYS2_OBJECT_MAX; i++) {
            *flags++ = GScolsys2.mFloor[0].mObject[i].mFlags;
        }
    }
}

extern "C" void GScolsys2Init(void)
{
    GScolsys2.mTableIndex = 0;
    GScolsys2.mFixedDisplayList = 0;
    GSgfxVFInit(&_colsysVF);
    GSgfxVFCreate(&_colsysVF);
    GScolsys2.mVf = &_colsysVF;
    GSgfxVFSetElement(&_colsysVF, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GSgfxVFSetElement(GScolsys2.mVf, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    FloorModuleInterface_clear(&_floorModuleColsys);
    _floorModuleColsys.makeStateData = _gscolsysMakeStateData;
    _floorModuleColsys.restoreStateData = _gscolsysRestoreStateData;
    _floorModuleColsys.getStateSize = _gscolsysGetStateSize;
    _floorModuleColsys.getUID = _gscolsysGetUID;
    _floorModuleColsys.getModuleName = _gscolsysGetModuleName;
    _hModuleField = floorRegisterModule(GSCOLSYS2_MODULE_PRIORITY, 0, &_floorModuleColsys);
}
