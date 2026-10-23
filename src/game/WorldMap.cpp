#include <GSmath.h>
#include <libc/memory.h>
#include <WorldMap.h>
#include <floorRoom.h>
#include <ProgressionData.h>
#include <GScamera.h>
#include <GSmem.h>
#include <GSmodel.h>
#include <GSres.h>
#include <saveData.h>
#include <window.h>

enum {
    worldMapTravelVariant1ModelResource = 0x20C30400,
    worldMapTravelVariant1MotionResource = 0x20C31000,
    worldMapTravelVariant1CameraResource = 0x20BF1800,
    worldMapTravelVariant2ModelResource = 0x20C40400,
    worldMapTravelVariant2MotionResource = 0x20C41000,
    worldMapTravelVariant2CameraResource = 0x20C01800,
    worldMapTravelVariant3ModelResource = 0x20C50400,
    worldMapTravelVariant3MotionResource = 0x20C51000,
    worldMapTravelVariant3CameraResource = 0x20C11800,
    worldMapTravelVariant4ModelResource = 0x20C60400,
    worldMapTravelVariant4MotionResource = 0x20C61000,
    worldMapTravelVariant4CameraResource = 0x20C21800,

    worldMapSpecialTravelVariant1ModelResource = 0x20CB0400,
    worldMapSpecialTravelVariant1MotionResource = 0x20CB1000,
    worldMapSpecialTravelVariant1CameraResource = 0x20C71800,
    worldMapSpecialTravelVariant2ModelResource = 0x20CC0400,
    worldMapSpecialTravelVariant2MotionResource = 0x20CC1000,
    worldMapSpecialTravelVariant2CameraResource = 0x20C81800,
    worldMapSpecialTravelVariant3ModelResource = 0x20CD0400,
    worldMapSpecialTravelVariant3MotionResource = 0x20CD1000,
    worldMapSpecialTravelVariant3CameraResource = 0x20C91800,
    worldMapSpecialTravelVariant4ModelResource = 0x20CE0400,
    worldMapSpecialTravelVariant4MotionResource = 0x20CE1000,
    worldMapSpecialTravelVariant4CameraResource = 0x20CA1800,

    worldMapTravelScooterAttachmentPart = 1,
    worldMapUnusedParticleAttachmentPart = 0,
};

enum {
    worldMapScooterVariant1ModelResource = 0x206D0400,
    worldMapScooterVariant1MotionResource = 0x206D1000,
    worldMapScooterVariant1PrimaryParticleAttachmentPart = 0x00000051,
    worldMapScooterVariant1SecondaryParticleAttachmentPart = 0x00000055,

    worldMapScooterVariant2ModelResource = 0x206E0400,
    worldMapScooterVariant2MotionResource = 0x206E1000,
    worldMapScooterVariant2PrimaryParticleAttachmentPart = 0x000000A6,
    worldMapScooterVariant2SecondaryParticleAttachmentPart = 0x000000AA,

    worldMapScooterVariant3ModelResource = 0x206F0400,
    worldMapScooterVariant3MotionResource = 0x206F1000,
    worldMapScooterVariant3PrimaryParticleAttachmentPart = 0x00000056,
    worldMapScooterVariant3SecondaryParticleAttachmentPart = 0x0000005A,

    worldMapScooterVariant4ModelResource = 0x20700400,
    worldMapScooterVariant4MotionResource = 0x20701000,
    worldMapScooterVariant4PrimaryParticleAttachmentPart = 0x000000AA,
    worldMapScooterVariant4SecondaryParticleAttachmentPart = 0x000000AE,

    worldMapSpecialScooterVariant1ModelResource = 0x20710400,
    worldMapSpecialScooterVariant1MotionResource = 0x20711000,
    worldMapSpecialScooterVariant1PrimaryParticleAttachmentPart = 0x00000050,
    worldMapSpecialScooterVariant1SecondaryParticleAttachmentPart = 0x00000051,
    worldMapSpecialScooterVariant1TertiaryParticleAttachmentPart = 0x00000052,

    worldMapSpecialScooterVariant2ModelResource = 0x20720400,
    worldMapSpecialScooterVariant2MotionResource = 0x20721000,
    worldMapSpecialScooterVariant2PrimaryParticleAttachmentPart = 0x00000054,
    worldMapSpecialScooterVariant2SecondaryParticleAttachmentPart = 0x00000055,
    worldMapSpecialScooterVariant2TertiaryParticleAttachmentPart = 0x00000056,

    worldMapSpecialScooterVariant3ModelResource = 0x20730400,
    worldMapSpecialScooterVariant3MotionResource = 0x20731000,
    worldMapSpecialScooterVariant3PrimaryParticleAttachmentPart = 0x00000057,
    worldMapSpecialScooterVariant3SecondaryParticleAttachmentPart = 0x00000058,
    worldMapSpecialScooterVariant3TertiaryParticleAttachmentPart = 0x00000059,
};

WorldMapAnimeRecord worldMapScooterTravelAnime[4] = {
    { worldMapTravelVariant1ModelResource, worldMapTravelVariant1MotionResource,
      {0, 0}, worldMapTravelScooterAttachmentPart, worldMapTravelVariant1CameraResource },
    { worldMapTravelVariant2ModelResource, worldMapTravelVariant2MotionResource,
      {0, 0}, worldMapTravelScooterAttachmentPart, worldMapTravelVariant2CameraResource },
    { worldMapTravelVariant3ModelResource, worldMapTravelVariant3MotionResource,
      {0, 0}, worldMapTravelScooterAttachmentPart, worldMapTravelVariant3CameraResource },
    { worldMapTravelVariant4ModelResource, worldMapTravelVariant4MotionResource,
      {0, 0}, worldMapTravelScooterAttachmentPart, worldMapTravelVariant4CameraResource },
};

WorldMapAnimeRecord worldMapSpecialScooterTravelAnime[4] = {
    { worldMapSpecialTravelVariant1ModelResource, worldMapSpecialTravelVariant1MotionResource,
      {0, 0}, worldMapTravelScooterAttachmentPart, worldMapSpecialTravelVariant1CameraResource },
    { worldMapSpecialTravelVariant2ModelResource, worldMapSpecialTravelVariant2MotionResource,
      {0, 0}, worldMapTravelScooterAttachmentPart, worldMapSpecialTravelVariant2CameraResource },
    { worldMapSpecialTravelVariant3ModelResource, worldMapSpecialTravelVariant3MotionResource,
      {0, 0}, worldMapTravelScooterAttachmentPart, worldMapSpecialTravelVariant3CameraResource },
    { worldMapSpecialTravelVariant4ModelResource, worldMapSpecialTravelVariant4MotionResource,
      {0, 0}, worldMapTravelScooterAttachmentPart, worldMapSpecialTravelVariant4CameraResource },
};

WorldMapScooterData worldMapScooterData[7] = {
    { worldMapScooterVariant1ModelResource, worldMapScooterVariant1MotionResource,
      worldMapScooterVariant1PrimaryParticleAttachmentPart, worldMapScooterVariant1SecondaryParticleAttachmentPart,
      worldMapUnusedParticleAttachmentPart },
    { worldMapScooterVariant2ModelResource, worldMapScooterVariant2MotionResource,
      worldMapScooterVariant2PrimaryParticleAttachmentPart, worldMapScooterVariant2SecondaryParticleAttachmentPart,
      worldMapUnusedParticleAttachmentPart },
    { worldMapScooterVariant3ModelResource, worldMapScooterVariant3MotionResource,
      worldMapScooterVariant3PrimaryParticleAttachmentPart, worldMapScooterVariant3SecondaryParticleAttachmentPart,
      worldMapUnusedParticleAttachmentPart },
    { worldMapScooterVariant4ModelResource, worldMapScooterVariant4MotionResource,
      worldMapScooterVariant4PrimaryParticleAttachmentPart, worldMapScooterVariant4SecondaryParticleAttachmentPart,
      worldMapUnusedParticleAttachmentPart },
    { worldMapSpecialScooterVariant1ModelResource, worldMapSpecialScooterVariant1MotionResource,
      worldMapSpecialScooterVariant1PrimaryParticleAttachmentPart, worldMapSpecialScooterVariant1SecondaryParticleAttachmentPart,
      worldMapSpecialScooterVariant1TertiaryParticleAttachmentPart },
    { worldMapSpecialScooterVariant2ModelResource, worldMapSpecialScooterVariant2MotionResource,
      worldMapSpecialScooterVariant2PrimaryParticleAttachmentPart, worldMapSpecialScooterVariant2SecondaryParticleAttachmentPart,
      worldMapSpecialScooterVariant2TertiaryParticleAttachmentPart },
    { worldMapSpecialScooterVariant3ModelResource, worldMapSpecialScooterVariant3MotionResource,
      worldMapSpecialScooterVariant3PrimaryParticleAttachmentPart, worldMapSpecialScooterVariant3SecondaryParticleAttachmentPart,
      worldMapSpecialScooterVariant3TertiaryParticleAttachmentPart },
};


extern f32 worldMapCamAccel;
extern f32 worldMapCamSpeed;

extern const f32 worldMapRibbonDistMax;
extern f32 worldMapPadAngleMax;
extern const f32 worldMapPadWindowAngle;
extern const f32 worldMapPadWindowStep;
extern f32 worldMapDemoSpeed;
extern const f32 worldMapDemoGlow;
extern bool worldMapHeroVisible;
extern u8 worldMapDemoEarlyEnd;
extern u8 worldMapGlowSpread;
extern u8 worldMapGlowParam2;
extern u8 worldMapGlowSubdivisions;
extern const u8 worldMapScooterFree[0x05];
extern bool worldMapDemoLoop;
extern u8 worldMapGlowAlphaBlend;
extern u8 worldMapGlowRenderMode[0x06];
extern s16 worldMapCursorCnt;
extern const u8 worldMapMenuOpen;
extern s16 worldMapCursorTable[0x20];
extern u32 worldMapMoveDemoActive;
extern WorldMapAnimeRecord* worldMapAnimeTables[2];

extern GSgfxLayerID worldMapModelLayer;
extern GSgfxLayerID worldMapScooterLayer;

extern "C" long getRibbonCnt(WorldMap* map);

extern "C" void menuButtonNormal(tagWINDOW_WORK* window);
extern "C" void menuCursorNormal(tagWINDOW_WORK* window, long a);
extern "C" u16 windowGetKeyRepeat();
extern "C" s8 fadeCheck(long a);
extern "C" void fadeSet(long a, f32 b);
extern "C" s32 windowGetActiveID();
extern "C" int menuOpenCustom(int menu, int window, void* work, int a, int b, int c, ...);
extern "C" int menuClose(int menu);
extern "C" int menuCloseSync(int menu, int a);
extern "C" void menuScriptSetWorldMapFlag(long a);
extern "C" u8 menuScriptGetWorldMapFlag();
extern "C" void msgctrlSetValue(long win, long value);
extern "C" void floorPop();
extern "C" void heroMovePoolCounter(long a, long b);

void _recoveryEventSetSE(unsigned long id);
extern "C" void GSthreadSwitch();
extern "C" void GSfsysFree(long id);
extern "C" void GSfsysPreRead(long id);
extern "C" unsigned long roomDataGetFsysGidForLanguage(void* data);
extern "C" void GSsndPlayNow(long id);
extern "C" f32 worldMapGetFrameSec(WorldMap* map);
extern "C" void floorSetFadeScript(long a, long b);
extern "C" long getCurrentMapGroupId();
extern "C" void GSparticleGeneratorSetCamera(void* gen, void* cam);
extern "C" void GSgfxVideoSetLockFrameRate(long rate);
extern "C" _GSmodel* floorOpenModel(WorldMap* map, unsigned long id);
extern "C" unsigned long GSflagGet(long flag);
extern "C" unsigned long HSD_Rand();
extern "C" u8 GSfsysWait(long id);
extern "C" void ReleaseModelPartPaticle(WorldMap* map, unsigned long a, long b);

extern "C" double atan2(double y, double x);
extern "C" double tan(double x);

enum {
    worldMapKeyUp        = 0x01,
    worldMapKeyDown      = 0x02,
    worldMapKeyLeft      = 0x04,
    worldMapKeyRight     = 0x08,
    worldMapKeyUpLeft    = 0x05,
    worldMapKeyUpRight   = 0x09,
    worldMapKeyDownLeft  = 0x06,
    worldMapKeyDownRight = 0x0A,
};

enum { worldMapFsysID = 0x9E6, worldMapEnvFadeDuration = 1000 };

enum {
    worldMapDemoSetAMapModel = 0x1B4A0400,
    worldMapDemoSetAModel0   = 0x1B4A1000,
    worldMapDemoSetAModel1   = 0x1B4A1001,
    worldMapDemoSetAModel2   = 0x1B4A1002,
    worldMapDemoSetAModel3   = 0x1B4A1603,
    worldMapDemoSetBModel0   = 0x1FDA1000,
    worldMapDemoSetBModel1   = 0x1FDA1001,
    worldMapDemoSetBModel2   = 0x1FDA1002,
    worldMapDemoSetBModel3   = 0x1FDA1602,

    worldMapHeroModel        = 0x18530200,
};

enum {
    worldMapScooterParticle0        = 0x20D01400,
    worldMapScooterParticle1        = 0x20CF1400,
    worldMapSpecialScooterParticle0 = 0x213E1400,
    worldMapSpecialScooterParticle1 = 0x213D1400,
};

enum {
    worldMapMenuID             = 0x54,
    worldMapMsgWindowID        = 0x59,
    worldMapDemoEndMsgID       = 0x1772,
    worldMapScooterSeID        = 0x5B2,
    worldMapSpecialScooterSeID = 0x5DC,
    worldMapMoveFadeScriptID   = 0x5960008,
};

enum {
    worldMapScooterUnlockFlag = 0x3C4,
    worldMapSpecialScooterUnlockEntry = 68,
};

extern u8 worldMapTargetDebugEnabled;

extern const GSgfxLayerID worldMapCursorLayer = GSgfxLayerID(2);

enum {
    worldMapHeroIconModel = 0x18531000,
    worldMapIconFlagHidden = 0,
    worldMapIconFlagUnvisited = 1,
    worldMapIconFlagVisited = 2,
    worldMapIconAnimationHidden = 0,
    worldMapIconAnimationVisitedAppear = 1,
    worldMapIconAnimationVisitedIdle = 2,
    worldMapIconAnimationVisitedScripted = 3,
    worldMapIconAnimationVisitedSelected = 4,
    worldMapIconAnimationVisitedConfirm = 5,
    worldMapIconAnimationUnvisitedAppear = 6,
    worldMapIconAnimationUnvisitedIdle = 7,
    worldMapIconAnimationUnvisitedSelected = 8,
    worldMapIconAnimationUnvisitedConfirm = 9,
    worldMapCursorModel = 0x18531013,
    worldMapCursorMotionIdle = 1,
    worldMapCursorMotionConfirm = 2,
    worldMapUnvisitedIconTexture = 0x33F,
    worldMapNoNpcNameMessage = 6002,
    worldMapFallbackFloorIcon = 6,
    worldMapCameraType = 3,
    worldMapOverviewCamera = 18,
    worldMapExpandedOverviewCamera = 19,
    worldMapExpandedOverviewIcon = 5,
    worldMapInfoTextRGB = 0xFFFFFF00
};

extern u32* worldMapCamDataCnt;
extern WorldMapCamData* worldMapCamDataTable;

extern s32* worldMapRibbonCount;
extern WorldMapMapData* worldMapData;
extern s32* worldMapInfoCount;
struct WorldMapInfoData { s32 mUnlockIndex; u32 mMessage[18]; };
extern WorldMapInfoData* worldMapInfoTable;
extern const GSgfxLayerID worldMapIconLayer = GSgfxLayerID(2);
extern f32 worldMapCursorBlendEnd;
extern "C" void soundStopEnv(s32 fadeTime);
extern "C" void cameraSetType(long type);
extern "C" void cameraUpdate();
extern "C" void cameraSetPosition(GSvec* pos);
extern "C" void cameraSetTargetPos(GSvec* pos);
extern "C" void cameraSetRotation(GSvec* rot);
extern "C" void cameraSetFov(f32 fov);
extern "C" f32 timeGetLastFrameSec();
extern "C" f32 GSlerpGetLinearInterpolationFloat(f32,f32,f32);
extern "C" void GSflagSet(long flag,long value);
extern "C" void menuItemBiosSetSelectFlag(long menu,long item,bool enable);
extern "C" u16 menuScriptGetWorldMapMode();
extern "C" u32 charNameBiosGetNameID(u16);
extern "C" void GSmsgPrint2(GSgfxLayerID,s32,s32,u32,s32);
extern "C" double sin(double);

s32 WorldMap::GetNowCursor(tagWINDOW_WORK* window) {
    if (worldMapMenuOpen != 0) {
        return window->cursor;
    }
    return mModelWork->cursor;
}

extern "C" void worldMapInitDummy(WorldMap* map)
{
}

void WorldMap::Button(tagWINDOW_WORK* window)
{
    if (mCursorMoving != 1) {
        menuButtonNormal(window);
    }
}

void WorldMap::Cursor(tagWINDOW_WORK* window)
{
    if (worldMapMenuOpen != 0) {
        menuCursorNormal(window, 1);
    } else if (mModelWork->mScriptFlag == 0) {
        ControlPad(window);
    }
}

void WorldMap::ControlPad(tagWINDOW_WORK* window)
{
    u16 key;
    s32 dir;
    s32 iconID;
    s8 next;

    windowGetKeyState();
    key = windowGetKeyRepeat();
    dir = -1;
    iconID = worldMapCursorTable[mModelWork->cursor];

    if (fadeCheck(0) != 0) {
        return;
    }
    if (mCursorMoving == 1) {
        return;
    }
    if (mModelWork->mIconAnimationActive == 1) {
        return;
    }
    GetModelID(iconID);
    if (key == worldMapKeyRight) {
        dir = 0;
    } else if (key == worldMapKeyDownRight) {
        dir = 1;
    } else if (key == worldMapKeyDown) {
        dir = 2;
    } else if (key == worldMapKeyDownLeft) {
        dir = 3;
    } else if (key == worldMapKeyLeft) {
        dir = 4;
    } else if (key == worldMapKeyUpLeft) {
        dir = 5;
    } else if (key == worldMapKeyUp) {
        dir = 6;
    } else if (key == worldMapKeyUpRight) {
        dir = 7;
    }
    WorldMapIconWork* icon = &mIcons[iconID];

    if (dir < 0) {
        return;
    }
    next = icon->mNext[dir];
    if (next < 0) {
        return;
    }
    for (s16 i = 0; i < worldMapCursorCnt; i++) {
        if (worldMapCursorTable[i] == next) {
            mModelWork->cursor = i;
            window->cursor = i;
            SetCursorPosition(i, 1);
            break;
        }
    }
}

void WorldMap::Release()
{
    if (mModelWork->mScooterModel != 0) {
        for (s32 i = 0; i < 3; i++) {
            if (mModelWork->mParticleGen[i] != 0) {
                ReleaseModelPartPaticle(this, mModelWork->mParticleGen[i], 1);
            }
        }
        ModelDetach(mModelWork->mScooterModel);
        if (worldMapScooterFree[0] != 0 && GSresGetResourceAllGroup(mModelWork->mScooterModel) != 0) {
            UnloadModel(worldMapFsysID, ((WorldMapScooterData*)mModelWork->mScooter)->mModelResource);
            GSresFreeResourceAllGroup(mModelWork->mScooterMotion);
        }
    } else if (mModelWork->mScriptFlag == 0) {
        while (GSfsysWait(worldMapFsysID) == 0) {
            GSthreadSwitch();
        }
        GSresFreeResource(worldMapFsysID, ((WorldMapScooterData*)mModelWork->mScooter)->mModelResource);
    }
    if (mModelWork != 0) {
        GSmemFree(mModelWork);
        mModelWork = 0;
    }
    if (mIcons != 0) {
        delete[] mIcons;
    }
    if (mCamWork != 0) {
        delete mCamWork;
    }
}

WorldMapIconWork::~WorldMapIconWork()
{
    delete[] mDist;
}

void WorldMap::RenderMoveDemoModel()
{
    if (worldMapMoveDemoActive != 0) {
        u32 model = mModelWork->mDemoModel1;
        void* cam = mModelWork->mAnimeCamera;

        if (model == worldMapDemoSetAModel1 && mDemoEffect != 0) {
            mDemoEffect->mIntensity = worldMapDemoGlow;
        }
        renderOffscreenData(mModelWork->mOffscreenAlpha);
        GSgfxLayerFindByID(0);
        GScameraIsAnimationRunning(cam);
        GSmodelSetRenderCamera(floorOpenModel(this, mModelWork->mDemoModel1), cam);
        GSmodelSetRenderCamera(floorOpenModel(this, mModelWork->mDemoModel2), cam);
        GSmodelSetRenderCamera(floorOpenModel(this, mModelWork->mScooterModel), cam);
        GSmodelSetRenderCamera(floorOpenModel(this, mModelWork->mDemoMapModel), cam);
        for (s32 i = 0; i < 3; i++) {
            if (mModelWork->mParticleGen[i] != 0) {
                GSresGetResourceAllGroup(mModelWork->mParticleRes[i]);
                GSparticleGeneratorSetCamera((void*)mModelWork->mParticleGen[i], cam);
            }
        }
        GScameraGetAnimFrame(cam);
    }
}

void WorldMap::moveDemo(long from, long to)
{
    WorldMapMapData* data;
    _GSmodel* scooterModel;
    _GSmodel* shadowSurfaceModel;
    void* shadowLight;
    _GSmodel* shadowSurfaces[1];
    f32 frames;
    f32 angle;
    u16 anime = (u16)HSD_Rand() % 2;

    mDemoEffect = new glowEffect();
    GSgfxVideoSetLockFrameRate(1);
    WorldMapIconWork* iconTo = &mIcons[worldMapCursorTable[to]];
    WorldMapIconWork* iconFrom = &mIcons[worldMapCursorTable[from]];
    angle = (f32)atan2(iconTo->screen.y - iconFrom->screen.y, iconTo->screen.x - iconFrom->screen.x);
    if (angle < 0.0f) {
        angle = 6.2831854820251465f + angle;
    }
    if (!(angle >= 1.5707963705062866f && angle < 4.71238899230957f)) {
        anime += 2;
    }
    data = SearchData(worldMapCursorTable[from]);
    mModelWork->mMapModelID = data->mModelID;
    if (data->mModelID == worldMapDemoSetAMapModel) {
        mModelWork->mDemoModel0 = worldMapDemoSetAModel0;
        mModelWork->mDemoModel1 = worldMapDemoSetAModel1;
        mModelWork->mDemoModel2 = worldMapDemoSetAModel2;
        mModelWork->mDemoModel3 = worldMapDemoSetAModel3;
        mModelWork->mAnime = &worldMapAnimeTables[0][anime];
    } else {
        mModelWork->mDemoModel0 = worldMapDemoSetBModel0;
        mModelWork->mDemoModel1 = worldMapDemoSetBModel1;
        mModelWork->mDemoModel2 = worldMapDemoSetBModel2;
        mModelWork->mDemoModel3 = worldMapDemoSetBModel3;
        mModelWork->mAnime = &worldMapAnimeTables[1][anime];
    }
    setNullModel();
    readScooter();
    GSthreadSwitch();
    SetModel((unsigned long)getCurrentMapGroupId(), mModelWork->mMapModelID, true);
    GSthreadSwitch();
    ObjectVisibility(mModelWork->mMapModelID, true);
    mModelWork->mOffscreenAlpha = 1.0f;
    initializeOffscreenData();
    if (mModelWork->mDemoModel1 == worldMapDemoSetAModel1 && mDemoEffect != 0) {
        mDemoEffect->mIntensity = 0.0f;
        mDemoEffect->mSpread = worldMapGlowSpread;
        mDemoEffect->mAlphaBlend = worldMapGlowAlphaBlend;
        mDemoEffect->mParam2 = worldMapGlowParam2;
        mDemoEffect->mRenderMode = worldMapGlowRenderMode[0];
        mDemoEffect->mSubdivisions = worldMapGlowSubdivisions;
        mDemoEffect->Start();
    }
    scooterModel = (_GSmodel*)GSresGetResourceAllGroup(((WorldMapScooterData*)mModelWork->mScooter)->mMotionResource);
    shadowSurfaceModel = (_GSmodel*)GSresGetResourceAllGroup(mModelWork->mDemoModel2);
    shadowLight = GSresGetResourceAllGroup(mModelWork->mDemoModel3);
    shadowSurfaces[0] = shadowSurfaceModel;
    GSmodelSetShadowFlags(scooterModel, 1);
    GSmodelSetShadowLight(scooterModel, shadowLight);
    GSmodelSetShadowSurface(scooterModel, 1, shadowSurfaces);
    SetMotion(mModelWork->mDemoMapModel, 0, 0.0f, worldMapDemoLoop);
    frames = GetAnimeFrameCount(mModelWork->mDemoMapModel, 0);
    mModelWork->mAnimeCamera = _cameraPlayAnime(((WorldMapAnimeRecord*)mModelWork->mAnime)->mCameraResource, 0, worldMapDemoLoop);
    if (mModelWork->mScooterIndex >= 4) {
        GSsndPlayNow(worldMapSpecialScooterSeID);
    } else {
        GSsndPlayNow(worldMapScooterSeID);
    }
    for (;;) {
        f32 dt = worldMapGetFrameSec(this) / worldMapDemoSpeed;

        MainControl(to);
        RenderMoveDemoModel();
        if (0.0f != mModelWork->mOffscreenAlpha) {
            mModelWork->mOffscreenAlpha -= dt;
            if (mModelWork->mOffscreenAlpha < 0.0f) {
                mModelWork->mOffscreenAlpha = 0.0f;
                ObjectVisibility(worldMapHeroModel, worldMapHeroVisible);
            }
        }
        if (worldMapDemoEarlyEnd && frames - 60.0f < GetAnimeFrame(mModelWork->mDemoMapModel)) {
            break;
        }
        GSthreadSwitch();
    }
}

unsigned long WorldMap::readScooter()
{
    if (mModelWork->mScriptFlag != 0) {
        return 0;
    }
    mModelWork->mScooter = &worldMapScooterData[mModelWork->mScooterIndex];
    WorldMapScooterData* scooter = (WorldMapScooterData*)mModelWork->mScooter;
    u32 motionRes = scooter->mMotionResource;
    u32 modelRes = scooter->mModelResource;

    while (GSfsysWait(worldMapFsysID) == 0) {
        GSthreadSwitch();
    }
    GSfsysFree(worldMapFsysID);
    SetModel(worldMapFsysID, modelRes, false);
    ObjectVisibility(modelRes, true);
    mModelWork->mScooterMotion = modelRes;
    SetMotion(mModelWork->mScooterModel = motionRes, 8, 0.0f, true);
    if (mModelWork->mDemoMapModel != 0 && mModelWork->mScooterModel != 0) {
        ModelAttach(mModelWork->mScooterModel, mModelWork->mDemoMapModel,
                    ((WorldMapAnimeRecord*)mModelWork->mAnime)->mScooterAttachmentPart, GS_ATTACH_TRANSFORM);
    }
    GSgfxLayerID layer = worldMapScooterLayer;
    SetLayerID(mModelWork->mScooterModel, 1, &layer);
    if (mModelWork->mScooterIndex >= 4) {
        mModelWork->mParticleRes[0] = worldMapSpecialScooterParticle0;
        mModelWork->mParticleRes[1] = worldMapSpecialScooterParticle1;
        mModelWork->mParticleRes[2] = worldMapSpecialScooterParticle1;
        mModelWork->mParticleGen[0] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[0],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPrimaryParticleAttachmentPart,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
        mModelWork->mParticleGen[1] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[1],
            ((WorldMapScooterData*)mModelWork->mScooter)->mSecondaryParticleAttachmentPart,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
        mModelWork->mParticleGen[2] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[2],
            ((WorldMapScooterData*)mModelWork->mScooter)->mTertiaryParticleAttachmentPart,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
    } else {
        mModelWork->mParticleRes[0] = worldMapScooterParticle0;
        mModelWork->mParticleRes[1] = worldMapScooterParticle1;
        mModelWork->mParticleRes[2] = worldMapScooterParticle1;
        mModelWork->mParticleGen[0] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[0],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPrimaryParticleAttachmentPart,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
        mModelWork->mParticleGen[1] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[1],
            ((WorldMapScooterData*)mModelWork->mScooter)->mSecondaryParticleAttachmentPart,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
        mModelWork->mParticleGen[2] = 0;
    }
    GSmodelSetBoundCheck((_GSmodel*)GSresGetResourceAllGroup(motionRes), 0);
    return motionRes;
}

s32 WorldMap::getScooterType()
{
    Hero* hero;
    s32 type;

    type = 0;
    hero = (Hero*)getTrainerDataStartOffset(0, TRAINER_DATA_HERO);

    if (hero != 0) {
        u32 unlocked;
        u8 follower;

        if (GSflagGet(worldMapScooterUnlockFlag) < worldMapScooterUnlockData[worldMapSpecialScooterUnlockEntry].mRequiredProgress) {
            unlocked = 0;
        } else {
            unlocked = 1;
        }
        if (hero->getFollowerID() != 1) {
            follower = 0;
        } else {
            follower = 1;
        }

        switch (hero->getHeroStyle()) {
        case 0:
            if (unlocked) {
                type = 4;
                break;
            }
            if (follower) {
                type = 1;
                break;
            }
            type = 0;
            break;
        case 2:
            if (unlocked) {
                type = 5;
                break;
            }
            if (follower) {
                type = 3;
                break;
            }
            type = 2;
            break;
        case 1:
            if (unlocked) {
                type = 6;
            }
            break;
        }
    }
    return type;
}

void WorldMap::setNullModel()
{
    WorldMapAnimeRecord* anime;
    u32 model;
    u32 res;
    long group;

    anime = (WorldMapAnimeRecord*)mModelWork->mAnime;
    model = anime->mMotionResource;
    res = anime->mModelResource;
    group = getCurrentMapGroupId();
    mModelWork->mModelRes = res;
    mModelWork->mDemoMapModel = model;
    SetModel(group, res, false);
    GSgfxLayerID layer = worldMapModelLayer;
    SetLayerID(mModelWork->mDemoMapModel, 1, &layer);
    GSmodelSetBoundCheck((_GSmodel*)GSresGetResourceAllGroup(mModelWork->mDemoMapModel), 0);
}

void* _cameraPlayAnime(unsigned long cameraRes, long startFrame, bool loop)
{
    void* camera = GSresGetResourceAllGroup(cameraRes);

    GScameraSetAnimIndex(camera, 0);
    if (loop) {
        GScameraSetAnimType(camera, 1);
    } else {
        GScameraSetAnimType(camera, 0);
    }
    GScameraSetAnimRate(camera, 0.5f);
    GScameraSetAnimFrame(camera, startFrame);
    GScameraStartAnimation(camera);
    return camera;
}

void WorldMap::Main()
{
    if (mModelWork->mScriptFlag != 0) {
        MainSub2();
    } else {
        MainSub();
    }
}

extern "C" u8 getWorldMapScriptFlag(WorldMap* map)
{
    return menuScriptGetWorldMapFlag();
}

void WorldMap::MainSub2()
{
    WorldMapWork* work = mModelWork;

    menuOpenCustom(worldMapMenuID, windowGetActiveID(), &work->mMenuIndex, 0, 1, 0);
    if (worldMapMoveDemoActive != 0) {
        PreRead(0);
        menuClose(worldMapMenuID);
        menuCloseSync(worldMapMenuID, 1);
        menuScriptSetWorldMapFlag(0);
        msgctrlSetValue(worldMapMsgWindowID, worldMapDemoEndMsgID);
        floorPop();
    }
}

void WorldMap::MainSub()
{
    int old = mModelWork->mMenuIndex;
    int sel = menuOpenCustom(worldMapMenuID, windowGetActiveID(), &mModelWork->mMenuIndex, 0, 1, 0);
    WorldMapMapData* data;

    if (worldMapMoveDemoActive == 0) {
        return;
    }
    if (sel < 0) {
        sel = old;
    }
    PreRead(sel);
    SelectIcon(sel);
    data = SearchData(worldMapCursorTable[sel]);
    menuClose(worldMapMenuID);
    menuCloseSync(worldMapMenuID, 1);
    if (sel != old) {
        mModelWork->mMapChanged = 1;
        heroMovePoolCounter(2, 0x28);
        moveDemo(old, sel);
    }
    if (sel != old) {
        fadeSet(3, 1.0f);
        do {
            RenderMoveDemoModel();
            GSthreadSwitch();
        } while (fadeCheck(0) != 0);
        if (mModelWork->mScooterIndex >= 4) {
            _recoveryEventSetSE(worldMapSpecialScooterSeID);
        } else {
            _recoveryEventSetSE(worldMapScooterSeID);
        }
        releaseOffscreenData();
    } else {
        do {
            MainControl(sel);
            GSthreadSwitch();
        } while (!IsBlending());
    }
    GSthreadSwitch();
    if (mDemoEffect != 0) {
        delete mDemoEffect;
        mDemoEffect = 0;
    }
    if (data != 0) {
        floorUnlinkWithSE(data->mFloorID, 0);
        if (mModelWork->mMapChanged != 0) {
            floorSetFadeScript(0, worldMapMoveFadeScriptID);
        }
        SetFlagData(worldMapCursorTable[sel]);
    } else {
        floorUnlinkWithSE(mModelWork->mMapID, 0);
    }
}

void WorldMap::PreRead(long index)
{
    WorldMapMapData* data = SearchData(worldMapCursorTable[index]);
    void* mapData;

    if (mModelWork->mScriptFlag != 0) {
        mapData = roomIDGetMapData(mModelWork->mMapID);
    } else if (data != 0) {
        mapData = roomIDGetMapData(data->mFloorID);
    } else {
        mapData = roomIDGetMapData(mModelWork->mMapID);
    }
    GSfsysPreRead(roomDataGetFsysGidForLanguage(mapData));
}

void WorldMap::CheckPadIndex(bool flag)
{
    WorldMapIconWork* icon;
    s32 i;
    s32 j;
    s32 k;
    s8 dirs[8];
    s32 dists[8];
    f32 dist;

    f32 angle = worldMapPadWindowAngle;
    f32 step = worldMapPadWindowStep;
    s8 pad;

    for (i = 0; i < getRibbonCnt(this); i++) {
        icon = &mIcons[i];
        for (k = 0; k < 8; k++) {
            icon->mNext[k] = -1;
        }
    }

    do {
        for (i = 0; i < getRibbonCnt(this); i++) {
            memset(dists, 0, sizeof(dists));
            for (k = 0; k < 8; k++) {
                dirs[k] = -1;
            }
            icon = &mIcons[i];
            for (j = 0; j < getRibbonCnt(this); j++) {
                if (i == j) {
                    continue;
                }
                if (GetFlagData(i) == 0) {
                    continue;
                }
                if (GetFlagData(j) == 0) {
                    continue;
                }
                for (k = 0; k < 8; k++) {
                    pad = GetPadIndex3(icon->mDist[j].angle, k, angle);
                    if (pad >= 0 && icon->mNext[pad] < 0) {
                        dist = icon->mDist[j].dist;
                        if (dist < worldMapRibbonDistMax) {
                            if (dists[pad] == 0) {
                                dirs[pad] = (s8)j;
                                dists[pad] = (s32)(f32)dist;
                            } else if (dists[pad] > (s32)dist) {
                                dirs[pad] = (s8)j;
                                dists[pad] = (s32)(f32)dist;
                            }
                        }
                    }
                }
            }
            for (k = 0; k < 8; k++) {
                if (dists[k] != 0.0f) {
                    icon->mNext[k] = dirs[k];
                }
            }
        }
        angle += step;
    } while (angle <= worldMapPadAngleMax);
}

void WorldMap::CalcModel2D(f32 angle, bool flag)
{
    WorldMapIconWork* icon;
    void* cam;
    GSmtx* mtx;
    GSmtx proj;
    GSmtx mvp;
    f32 xscale;

    f32 dist;
    s32 i;
    s32 j;

    cam = (void*)GSresGetResource(0, 0);
    mtx = (GSmtx*)GScameraGetMatrixPtr(cam);
    const GSmtx* cameraProjection = (const GSmtx*)GScameraGetProjMatrixPtr(cam);
    proj.m[0][0] = cameraProjection->m[0][0];
    proj.m[0][1] = cameraProjection->m[0][1];
    proj.m[0][2] = cameraProjection->m[0][2];
    proj.m[0][3] = cameraProjection->m[0][3];
    proj.m[1][0] = cameraProjection->m[1][0];
    proj.m[1][1] = cameraProjection->m[1][1];
    proj.m[1][2] = cameraProjection->m[1][2];
    proj.m[1][3] = cameraProjection->m[1][3];
    proj.m[2][0] = cameraProjection->m[2][0];
    proj.m[2][1] = cameraProjection->m[2][1];
    proj.m[2][2] = cameraProjection->m[2][2];
    proj.m[2][3] = cameraProjection->m[2][3];
    PSMTXConcat(mtx->sdk(), proj.sdk(), mvp.sdk());

    f32 a = 0.017453292f;
    a *= angle;

    f32 halfAngle = 0.5f;
    halfAngle = a * halfAngle;
    f32 tangent = (f32)tan(halfAngle);
    xscale = -240.0f / tangent;
    for (i = 0; i < getRibbonCnt(this); i++) {
        GSvec vec;
        f32 sx;

        icon = &mIcons[i];

        PSMTXMultVec(mtx->sdk(), (Vec *)&icon->pos, (Vec *)&vec);
        sx = xscale * vec.x / vec.z;
        dist = -xscale * vec.y / vec.z;
        icon->screen.x = 320.0f + sx;
        icon->screen.y = 240.0f + dist;
        icon->screen.z = 0.0f;
    }
    for (i = 0; i < getRibbonCnt(this); i++) {
        icon = &mIcons[i];
        for (j = 0; j < getRibbonCnt(this); j++) {
            WorldMapIconWork* other = &mIcons[j];

            if (i != j) {
                f32 ang;

                if (GetFlagData(i) == 0 || GetFlagData(j) == 0) {
                    dist = 0.0f;
                    ang = dist;
                } else {
                    f32 d = PSVECDistance((Vec *)&icon->screen, (Vec *)&other->screen);
                    dist = d;
                    ang = (f32)atan2(other->screen.y - icon->screen.y,
                                     other->screen.x - icon->screen.x);
                    if (ang < 0.0f) {
                        ang = 6.2831854820251465f + ang;
                    }
                }
                icon->mDist[j].dist = dist;
                icon->mDist[j].angle = ang;
            } else {
                icon->mDist[j].dist = 0.0f;
                icon->mDist[j].angle = 0.0f;
            }
        }
    }
    CheckPadIndex(flag);
}

s8 WorldMap::GetPadIndex3(f32 angle, long index, f32 width)
{
    bool inWindow = false;
    s8 padIndex = -1;
    f32 wrapEnd = 6.2831854820251465f - width;
    f32 slotAngle = 3.1415927f * index;
    f32 slotCenter = 0.25f;
    slotCenter = slotAngle * slotCenter;

    if (0.0f == index) {
        if (angle >= wrapEnd || angle < width) {
            inWindow = true;
        }
    } else if (angle >= slotCenter - width && angle < slotCenter + width) {
        inWindow = true;
    }
    if (inWindow) {
        padIndex = index;
    }
    return padIndex;
}

void WorldMap::InitModel2D(f32 param)
{
    long i;

    CalcModel2D(param, true);
    for (i = 0; i < getRibbonCnt(this); i++) {
    }
}

void WorldMap::SetCameraSpeed(f32 speed)
{
    mCamWork->blend = 0.0f;
    mCamWork->mSpeed = worldMapCamSpeed;
    mCamWork->mAccel = worldMapCamAccel;
    mCamWork->mElapsed = 0.0f;
}

void WorldMap::InitCamera(long cursor)
{
    GSvec position;
    f32 fov, distance, height, yaw;
    f32 aspect, nearZ, farZ;
    cameraSetType(worldMapCameraType);
    mCamWork->blend = 0.0f;
    mCamWork->mSpeed = 0.0f;
    if (worldMapTargetDebugEnabled) {
        GetCameraData(worldMapExpandedOverviewCamera,&position,&distance,&height,&yaw,&fov);
    } else if (GetFlagData(worldMapExpandedOverviewIcon)) {
        GetCameraData(worldMapExpandedOverviewCamera,&position,&distance,&height,&yaw,&fov);
    } else {
        GetCameraData(worldMapOverviewCamera,&position,&distance,&height,&yaw,&fov);
    }
    SetCameraNextWork(&position,distance,height,yaw,fov);
    memcpy(&mCamWork->current.pos,&mCamWork->next.pos,0x28);
    SetCamera();
    cameraUpdate();
    f32 initialFov = fov;
    void* camera = GSresGetResource(0,0);
    GScameraGetMatrixPtr(camera);
    GScameraGetPerspective(camera,&fov,&aspect,&nearZ,&farZ);
    GScameraSetPerspective(camera,initialFov,aspect,nearZ,farZ);
    InitModel2D(fov);
    if (!mModelWork->mScriptFlag) {
        long icon = worldMapCursorTable[mModelWork->mMenuIndex];
        WorldMapMapData* data = SearchData(icon);
        GetCameraData(data->mCameraIndex,&position,&distance,&height,&yaw,&fov);
        SetCameraWork(&mIcons[icon].pos,distance,height,yaw,fov);
        SetCameraSpeed(1.0f);
        SetCamera();
    }
}

void WorldMap::Init()
{
    soundStopEnv(worldMapEnvFadeDuration);
    mModelWork = (WorldMapWork*)GSmemAllocNoClr(sizeof(WorldMapWork));
    memset(mModelWork,0,sizeof(WorldMapWork));
    mModelWork->mSavedCamera = GScameraGetActiveCamera();
    mIcons = new WorldMapIconWork[*worldMapRibbonCount];
    mCamWork = new WorldMapCamWork;
    mModelWork->mScriptFlag = getWorldMapScriptFlag(this);
    mModelWork->mIconAnimationActive = 1;
    mModelWork->mMoveBlend = 0.0f;
    mModelWork->mMoveSpeed = 1.0f;
    mModelWork->mMoveAccel = 0.0f;
    s16 count = 0;
    for (s16 i = 0; i < getRibbonCnt(this); i++) {
        WorldMapMapData* data = SearchData(i);
        if (data->mFlagID == 0) {
            menuItemBiosSetSelectFlag(worldMapMenuID,data->mItemID,true);
            worldMapCursorTable[count++] = i;
        } else if (GSflagGet(data->mFlagID)) {
            menuItemBiosSetSelectFlag(worldMapMenuID,data->mItemID,true);
            worldMapCursorTable[count++] = i;
        } else {
            menuItemBiosSetSelectFlag(worldMapMenuID,data->mItemID,false);
        }
    }
    worldMapCursorCnt = count;
    if (mModelWork->mScriptFlag) {
        u16 mode = menuScriptGetWorldMapMode();
        mModelWork->mMapID = SearchFloorID(mode);
        mModelWork->mMenuIndex = SearchDataFloorID(mModelWork->mMapID);
        mModelWork->cursor = mModelWork->mMenuIndex;
        mModelWork->mCursorAngle = GetAngle(mModelWork->mMenuIndex);
        mModelWork->mMapID = floorGetPreviousMap();
    } else {
        mModelWork->mMapID = floorGetCurrentMap();
        mModelWork->mMenuIndex = SearchDataFloorID(mModelWork->mMapID);
        mModelWork->cursor = mModelWork->mMenuIndex;
        mModelWork->mCursorAngle = GetAngle(mModelWork->mMenuIndex);
    }
    InitModel(mModelWork->mMenuIndex);
    InitCamera(mModelWork->mMenuIndex);
    worldMapInitDummy(this);
}

WorldMapIconWork::WorldMapIconWork()
{
    mDist = new WorldMapIconDistance[*worldMapRibbonCount];
    mNext[0] = -1; mNext[1] = -1; mNext[2] = -1; mNext[3] = -1;
    mNext[4] = -1; mNext[5] = -1; mNext[6] = -1; mNext[7] = -1;
}

void WorldMap::SetCamera()
{
    GSmtx matrix;
    GSvec rotation;
    rotation.x = 0.0f; rotation.y = 0.0f; rotation.z = 0.0f;
    GSvec offset, position;
    f32 aspect, nearZ, farZ, fov;
    void* camera;
    timeGetLastFrameSec();
    WorldMapCameraState* current = &mCamWork->current;
    camera = GSresGetResource(0,0);
    GScameraGetMatrixPtr(camera);
    GScameraGetPerspective(camera,&fov,&aspect,&nearZ,&farZ);
    GScameraSetPerspective(camera,current->fov,aspect,nearZ,farZ);
    f32 distance = current->distance;
    f32 height = current->height;
    offset.y = height;
    offset.x = 0.0f;
    offset.z = distance;
    PSMTXRotRad(matrix.sdk(),'Y',current->yaw);
    PSMTXMultVec(matrix.sdk(),(Vec *)&offset,(Vec *)&offset);
    PSVECAdd((Vec *)&current->pos,(Vec *)&offset,(Vec *)&position);
    rotation.y = current->yaw;
    rotation.x = -(f32)atan2(current->height,current->distance);
    rotation.z = 0.0f;
    GScameraLookAt(camera,&current->up,&current->pos);
    cameraSetPosition(&position);
    cameraSetTargetPos(&current->pos);
    cameraSetRotation(&rotation);
    cameraSetFov(current->fov);
}

void WorldMap::SelectIcon(long index)
{
    GSvec offset, position;
    f32 distance, fov, yaw, height;
    long icon = worldMapCursorTable[index];
    WorldMapMapData* data = SearchData(icon);
    GetCameraData(data->mCameraIndex,&offset,&distance,&height,&yaw,&fov);
    PSVECAdd((Vec *)&mIcons[icon].pos,(Vec *)&offset,(Vec *)&position);
    SetCameraNextWork(&position,distance,height,yaw,fov);
    SetCameraSpeed(1.0f);
    u32 model = mIcons[icon].mModelID;
    if (GetFlagData(icon) == worldMapIconFlagUnvisited) SetIconMotion(icon,model,worldMapIconAnimationUnvisitedConfirm,0.0f,false);
    else SetIconMotion(icon,model,worldMapIconAnimationVisitedConfirm,0.0f,false);
    InitCursor(index,worldMapCursorMotionConfirm);
}

extern const f32 worldMapIconAngleStep = (3.1415927f / 8.0f);

void WorldMap::TargetDebug(long index)
{
    s32 prev = mCamWork->mDebugTarget;
    long id = worldMapCursorTable[index];
    mCamWork->mPrevDebugTarget = prev;
    mCamWork->mDebugTarget = id;
    if (worldMapTargetDebugEnabled == 0) return;
    if (mCamWork->mPrevDebugTarget == mCamWork->mDebugTarget) return;
    SetCameraNextWork(&mIcons[id].pos, 51.75f, 35.31f, 0.0f, 50.0f);
    SetCameraSpeed(1.0f);
}

void WorldMap::ControlCamera(long index)
{
    f32 dt = timeGetLastFrameSec();
    mCamWork->mElapsed += dt;
    mCamWork->mSpeed += mCamWork->mAccel * dt;
    if (mCamWork->mSpeed <= 0.01f) mCamWork->mSpeed = 0.01f;
    mCamWork->blend += mCamWork->mSpeed;
    if (mCamWork->blend > 1.0f) mCamWork->blend = 1.0f;
    WorldMapCamWork* work = mCamWork;
    WorldMapCameraState* current = &work->current;
    Blendf(&current->pos.x,current->pos.x,work->next.pos.x,mCamWork->blend);
    Blendf(&current->pos.y,current->pos.y,work->next.pos.y,mCamWork->blend);
    Blendf(&current->pos.z,current->pos.z,work->next.pos.z,mCamWork->blend);
    Blendf(&current->distance,current->distance,work->next.distance,mCamWork->blend);
    Blendf(&current->height,current->height,work->next.height,mCamWork->blend);
    Blendf(&current->fov,current->fov,work->next.fov,mCamWork->blend);
}

bool WorldMap::IsBlending()
{
    bool blending = false;
    if (mCamWork->blend >= 1.0f) {
        blending = true;
    }
    return blending;
}

void WorldMap::SetCameraNextWork(GSvec* pos, f32 distance, f32 height, f32 yaw, f32 fov)
{
    WorldMapCamWork* work = mCamWork;

    if (pos != 0) {
        work->next.pos = *pos;
    }
    if (distance) {
        work->next.distance = distance;
    }
    work->next.height = height;
    work->next.yaw = yaw;
    work->next.fov = fov;
    work->next.up.x = 0.0f;
    work->next.up.y = 1.0f;
    work->next.up.z = 0.0f;
}

void WorldMap::SetCameraWork(GSvec* pos, f32 distance, f32 height, f32 yaw, f32 fov)
{
    WorldMapCamWork* work = mCamWork;

    if (pos != 0) {
        work->current.pos = *pos;
    }
    if (distance) {
        work->current.distance = distance;
    }
    work->current.height = height;
    work->current.yaw = yaw;
    work->current.fov = fov;
    work->current.up.x = 0.0f;
    work->current.up.y = 1.0f;
    work->current.up.z = 0.0f;
}

bool WorldMap::Blendf(f32* output,f32 from,f32 to,f32 blend)
{
    *output = GSlerpGetLinearInterpolationFloat(from,to,blend);
    return true;
}

void WorldMap::InitModel(long cursor)
{
    _GSmodel* model;
    long icon;
    unsigned long modelID;
    _GSmodel* hero;
    u32 flag;
    const GSgfxLayerID& iconLayer = worldMapIconLayer;
    GSgfxLayerID layer = iconLayer;
    hero = (_GSmodel*)GSresGetResourceAllGroup(worldMapHeroIconModel);
    GSmodelSetAnimIndex(hero, 0);
    GSmodelSetAnimFrame(hero, 0.0f);
    GSmodelSetAnimRate(hero, 0.5f);
    GSmodelStartAnimation(hero);
    SetLayerID(worldMapHeroIconModel, 1, &layer);

    for (icon = 0; icon < getRibbonCnt(this); ++icon) {
        modelID = GetModelID(icon);
        flag = GetFlagData(icon);
        model = GetModel(icon);
        mIcons[icon].mModelID = modelID;
        if (mModelWork->mScriptFlag != 0) {
            if (icon == worldMapCursorTable[cursor]) {
                if (flag == worldMapIconFlagVisited) mIcons[icon].mAnimationIndex = worldMapIconAnimationVisitedScripted;
                else mIcons[icon].mAnimationIndex = worldMapIconAnimationUnvisitedIdle;
            } else {
                mIcons[icon].mAnimationIndex = GetStartIconAnimIndex(flag);
            }
        } else {
            if (icon == worldMapCursorTable[cursor]) {
                mIcons[icon].mAnimationIndex = worldMapIconAnimationVisitedSelected;
            } else {
                mIcons[icon].mAnimationIndex = GetStartIconAnimIndex(flag);
            }
        }
        SetLayerID(modelID, 1, &layer);
        switch (mIcons[icon].mAnimationIndex) {
        case worldMapIconAnimationHidden:
            Visibility(modelID, false);
            break;
        case worldMapIconAnimationVisitedScripted:
        case worldMapIconAnimationVisitedSelected:
            SetMotion(modelID, mIcons[icon].mAnimationIndex, 0.0f, true);
            break;
        default:
            SetMotion(modelID, mIcons[icon].mAnimationIndex, 0.0f, false);
            break;
        }
        GSmodelGetPosition(model, &mIcons[icon].pos);
    }
    InitCursor(cursor, worldMapCursorMotionIdle);
}

void WorldMap::SetCursorPosition(long index,bool move)
{
    SearchData(worldMapCursorTable[index]);
    WorldMapIconWork* icon = &mIcons[worldMapCursorTable[index]];
    mCursorTarget = icon->pos;
    mCursorIndex = index;
    if (move) {
        mCursorMoving = 1;
        mCursorBlend = 0.0f;
    }
}

void WorldMap::InitCursor(long index,short motion)
{
    const GSgfxLayerID& cursorLayer = worldMapCursorLayer;
    GSgfxLayerID layer = cursorLayer;
    mCursorTime = 0.0f; mCursorBob = 0.0f;
    mCursorModel = worldMapCursorModel;
    mCursorMotion = motion;
    mCursorBlend = 0.0f;
    mCursorMoving = 1;
    switch (motion) {
    case worldMapCursorMotionConfirm: SetMotion(mCursorModel,mCursorMotion,0.0f,false); break;
    default: SetMotion(mCursorModel,mCursorMotion,0.0f,true); break;
    }
    SetLayerID(mCursorModel,1,&layer);
    SetCursorPosition(index,false);
    mCursorPosition = mCursorTarget;
}

s32 WorldMap::GetModelID(long index)
{
    return SearchData(index)->mModelObjectID;
}

void WorldMap::DrawIcon(long index,tagWINDOW_WORK* window)
{
    WorldMapMapData* data = SearchData(worldMapCursorTable[index]);
    switch (GetFlagData(worldMapCursorTable[index])) {
    case worldMapIconFlagHidden: break;
    case worldMapIconFlagUnvisited: windowDrawTextureId(0,0,0,worldMapUnvisitedIconTexture,0,(GSgfxLayerID)window->mLayerID); break;
    case worldMapIconFlagVisited: windowDrawTextureId(0,0,0,(u16)data->mTextureID,0,(GSgfxLayerID)window->mLayerID); break;
    }
}

void WorldMap::SetInfo(long index,tagWINDOW_WORK* window,tagSPRITE_WORK* sprite)
{
    SearchData(worldMapCursorTable[index]);
    if (mModelWork->mScriptFlag) {
        u32 name = charNameBiosGetNameID(GetNpcID(index));
        if (name == 0) name = worldMapNoNpcNameMessage;
        msgctrlSetValue(worldMapMsgWindowID,name);
    }
    GSmsgPrint2((GSgfxLayerID)window->mLayerID,0,-4,
                worldMapInfoTextRGB | sprite->mRgba.mAlpha,GetInfoMsgID(index));
}

u16 WorldMap::GetNpcID(long index)
{
    return SearchData(worldMapCursorTable[index])->mNpcID;
}

u32 WorldMap::GetInfoMsgID(long index)
{
    long icon = worldMapCursorTable[index];
    if (mModelWork->mScriptFlag) return SearchData(icon)->mScriptMessage;
    return worldMapInfoTable[GetInfoLevel()].mMessage[icon];
}

_GSmodel* WorldMap::GetModel(long index)
{
    return (_GSmodel*)GSresGetResourceAllGroup(GetModelID(index));
}

s32 WorldMap::GetInfoLevel()
{
    WorldMapInfoData* data = worldMapInfoTable;
    long level = 0;
    for (s16 i = 0; i < *worldMapInfoCount; data++, i++) {
        if (data->mUnlockIndex != 0) {
            u32 requirement = worldMapScooterUnlockData[data->mUnlockIndex].mRequiredProgress;
            if (GSflagGet(worldMapScooterUnlockFlag) >= requirement) level++;
        }
    }
    return level - 1;
}

f32 WorldMap::GetAngle(long direction)
{
    const f32& step = worldMapIconAngleStep;
    return step * direction;
}

void WorldMap::SetFlagData(long index)
{
    u32 flag = SearchData(index)->mFlagID;
    if (flag != 0) GSflagSet(flag,2);
}

u32 WorldMap::GetFlagData(long index)
{
    u32 flag = SearchData(index)->mFlagID;
    if (flag == 0) return 2;
    return GSflagGet(flag);
}

u32 WorldMap::SearchFloorID(unsigned short index)
{
    WorldMapMapData* data = SearchData(index);
    u32 flag = GSflagGet(data->mFlagID);
    if (data->mFlagID == 0 || flag != 0) return data->mFloorID;
    return SearchData(worldMapFallbackFloorIcon)->mFloorID;
}

s32 WorldMap::SearchDataFloorID(unsigned int floor)
{
    long i;
    long count = 0;
    for (i = 0; i < getRibbonCnt(this); i++) {
        WorldMapMapData* data = SearchData(i);
        u32 flag = GSflagGet(data->mFlagID);
        if (data->mFlagID == 0 || flag != 0) {
            count++;
            if (floor == worldMapData[i].mFloorID) return count - 1;
        }
    }
    return 0;
}

WorldMapMapData* WorldMap::SearchData(long index)
{
    if (index >= getRibbonCnt(this)) return 0;
    return &worldMapData[index];
}

extern "C" long getRibbonCnt(WorldMap* map)
{
    return *worldMapRibbonCount;
}

WorldMap::~WorldMap() { Release(); }

WorldMap::WorldMap()
{
    mModelWork = 0;
    mDemoEffect = 0;
}

s16 WorldMap::GetStartIconAnimIndex(unsigned long index)
{
    switch (index) {
    case worldMapIconFlagHidden:
        return worldMapIconAnimationHidden;
    case worldMapIconFlagUnvisited:
        return worldMapIconAnimationUnvisitedAppear;
    case worldMapIconFlagVisited:
        return worldMapIconAnimationVisitedAppear;
    default:
        return worldMapIconAnimationHidden;
    }
}

void WorldMap::ControlCursor(long index)
{
    _GSmodel* model = (_GSmodel*)GSresGetResourceAllGroup(mCursorModel);
    f32 dt = worldMapGetFrameSec(this);
    if (mCursorMoving) {
        f32 speed = 2.0f;
        mCursorBlend += speed * dt;
        if (mCursorBlend >= worldMapCursorBlendEnd) {
            mCursorBlend = 1.0f;
            mCursorMoving = 0;
        }
        Blendf(&mCursorPosition.x,mCursorPosition.x,mCursorTarget.x,mCursorBlend);
        Blendf(&mCursorPosition.y,mCursorPosition.y,mCursorTarget.y,mCursorBlend);
        Blendf(&mCursorPosition.z,mCursorPosition.z,mCursorTarget.z,mCursorBlend);
    }
    if (mModelWork->mScriptFlag) {
        mCursorTime += dt;
        mCursorBob = 5.0f + 5.0f * (f32)sin(6.2831854820251465f * mCursorTime);
        mCursorPosition.y = mCursorBob;
    }
    GSmodelSetPosition(model,&mCursorPosition);
}

extern "C" f32 worldMapGetFrameSec(WorldMap* map)
{
    return timeGetLastFrameSec();
}

void WorldMap::CheckIconAnime(long cursor)
{
    long selected = worldMapCursorTable[cursor];
    mModelWork->mIconAnimationActive = 0;
    for (long i = 0; i < getRibbonCnt(this); i++) {
        unsigned long model = mIcons[i].mModelID;
        switch (mIcons[i].mAnimationIndex) {
        case worldMapIconAnimationVisitedAppear:
            if (!CheckMotion(model)) SetIconMotion(i,model,worldMapIconAnimationVisitedIdle,0.0f,true);
            else mModelWork->mIconAnimationActive = 1;
            break;
        case worldMapIconAnimationUnvisitedAppear:
            if (!CheckMotion(model)) SetIconMotion(i,model,worldMapIconAnimationUnvisitedIdle,0.0f,true);
            break;
        case worldMapIconAnimationVisitedIdle:
            if (selected == i) SetIconMotion(i,model,worldMapIconAnimationVisitedSelected,0.0f,true);
            break;
        case worldMapIconAnimationUnvisitedIdle:
            if (selected == i) SetIconMotion(i,model,worldMapIconAnimationUnvisitedSelected,0.0f,true);
            break;
        case worldMapIconAnimationVisitedSelected:
            if (selected != i) SetIconMotion(i,model,worldMapIconAnimationVisitedIdle,0.0f,true);
            break;
        case worldMapIconAnimationUnvisitedSelected:
            if (selected != i) SetIconMotion(i,model,worldMapIconAnimationUnvisitedIdle,0.0f,true);
            break;
        case worldMapIconAnimationVisitedConfirm:
            if (!CheckMotion(model)) StopMotion(model);
            break;
        case worldMapIconAnimationUnvisitedConfirm:
            if (!CheckMotion(model)) StopMotion(model);
            break;
        }
    }
}

void WorldMap::CheckCursorAnime()
{
    short motion = mCursorMotion;
    unsigned long model = mCursorModel;
    switch (motion) {
    case worldMapCursorMotionConfirm: if (!CheckMotion(model)) StopMotion(model); break;
    }
}

void WorldMap::SetIconMotion(long icon,unsigned long model,short motion,f32 frame,bool loop)
{
    mIcons[icon].mAnimationIndex = motion;
    SetMotion(model,motion,frame,loop);
}

void WorldMap::GetCameraData(long index, GSvec* pos, f32* distance, f32* height, f32* yaw, f32* fov)
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
    *height = entry->height;
    *yaw = entry->yaw;
    *fov = entry->fov;
}
