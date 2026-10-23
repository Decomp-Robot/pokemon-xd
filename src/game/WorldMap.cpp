#include <WorldMap.h>

extern f32 worldMapCamAccel;
extern f32 worldMapCamSpeed;
/* Float constants of the map module's shared sdata2 pool. */
extern const f32 worldMapFloatZero;          /* 0.0f */
extern const f32 worldMapFloatOne;           /* 1.0f */
extern const f32 worldMapFloatHalf;          /* 0.5f */
extern const f32 worldMapPi;                 /* pi */
extern const f32 worldMapTwoPi;              /* 2*pi */
extern const f32 worldMapHalfPi;             /* pi/2 */
extern const f32 worldMapThreeHalfPi;        /* 3*pi/2 */
extern const f32 worldMapFloatQuarter;       /* 0.25f */
extern const f32 worldMapDegToRad;           /* pi/180 */
/* Move-demo camera projection: scale and screen centre of the offscreen pass. */
extern const f32 worldMapProjectionScale;    /* -240.0f */
extern const f32 worldMapScreenCenterX;      /* 320.0f */
extern const f32 worldMapScreenCenterY;      /* 240.0f */
/* The move demo ends this many frames before the map motion finishes. */
extern const f32 worldMapDemoEndLeadFrames;  /* 60.0f */
/* Maximum ribbon distance CheckPadIndex links a pad direction through. */
extern const f32 worldMapRibbonDistMax;
extern f32 worldMapPadAngleMax;
extern const f32 worldMapPadWindowAngle;
extern const f32 worldMapPadWindowStep;
extern f32 worldMapDemoSpeed;
extern const f32 worldMapDemoGlow;
extern bool worldMapHeroVisible;
extern u8 worldMapDemoEarlyEnd;
extern u8 worldMapGlowParam0;
extern u8 worldMapGlowParam2;
extern u8 worldMapGlowParam4;
extern const u8 worldMapScooterFree[0x05];
extern bool worldMapDemoLoop;
extern u8 worldMapGlowParam1;
extern u8 worldMapGlowParam3[0x06];
extern const s16 worldMapCursorCnt;
extern const u8 worldMapMenuOpen;
extern const s16 worldMapCursorTable[0x20];
extern u32 worldMapMoveDemoActive;
extern WorldMapAnimeRecord* worldMapAnimeTables[2];
extern WorldMapScooterData worldMapScooterData[7];
extern GSgfxLayerID worldMapModelLayer;
extern GSgfxLayerID worldMapScooterLayer;

/* Save block the scooter types are unlocked against; the flag value the last
 * scooters need sits at +0x888. The rest of the block is not recovered. */
struct WorldMapUnlockData {
    u8 _000[0x888];              // 0x000-0x887: unknown.
    u32 scooterUnlockFlagValue;  // 0x888: flag value the special scooters need.
};
extern WorldMapUnlockData* worldMapScooterUnlockData;

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
/* Recovery event sound helper of the camera play-back system. */
void _recoveryEventSetSE(unsigned long id);
extern "C" void GSthreadSwitch();
extern "C" void GSfsysFree(long id);
extern "C" void GSfsysPreRead(long id);
extern "C" void* roomIDGetMapData(void* res);
extern "C" unsigned long roomDataGetFsysGidForLanguage(void* data);
extern "C" Hero* getTrainerDataStartOffset(long a, long b);
extern "C" void* GSresGetResource(unsigned long id, unsigned long res);
extern "C" void GSsndPlayNow(long id);
extern "C" f32 worldMapGetFrameSec(WorldMap* map);
extern "C" void floorUnlinkWithSE(void* p, long a);
extern "C" void floorSetFadeScript(long a, long b);
extern "C" long getCurrentMapGroupId();
extern "C" void GSmemFree(void* p);
extern "C" void* GSresGetResourceAllGroup(void* res);
extern "C" void GSresFreeResourceAllGroup(void* res);
extern "C" void GSresFreeResource(long id, unsigned long res);
extern "C" void GScameraGetAnimFrame(void* cam);
extern "C" void GScameraSetAnimIndex(void* cam, unsigned long index);
extern "C" void GScameraSetAnimType(void* cam, long type);
extern "C" void GScameraSetAnimRate(void* cam, f32 rate);
extern "C" void GScameraSetAnimFrame(void* cam, f32 frame);
extern "C" void GScameraStartAnimation(void* cam);
extern "C" void* GScameraGetMatrixPtr(void* cam);
extern "C" void* GScameraGetProjMatrixPtr(void* cam);
extern "C" void GSparticleGeneratorSetCamera(void* gen, void* cam);
extern "C" void GSgfxVideoSetLockFrameRate(long rate);
extern "C" void GSgfxLayerFindByID(long id);
extern "C" void GScameraIsAnimationRunning(void* cam);
extern "C" void GSgfxLayerSetCamera(unsigned long layer, void* cam);
extern "C" unsigned long floorOpenModel(WorldMap* map, unsigned long id);
extern "C" void GSmodelSetShadowFlags(void* res, long flags);
extern "C" void GSmodelSetShadowLight(void* res, void* light);
extern "C" void GSmodelSetShadowSurface(void* res, long a, void* b);
extern "C" void GSmodelSetBoundCheck(void* res, long a);
extern "C" unsigned long GSflagGet(long flag);
extern "C" unsigned long HSD_Rand();
extern "C" u8 GSfsysWait(long id);
extern "C" void ReleaseModelPartPaticle(WorldMap* map, unsigned long a, long b);

extern "C" double atan2(double y, double x);
extern "C" double tan(double x);
extern "C" void PSMTXConcat(void* a, void* b, void* dst);
extern "C" void PSMTXMultVec(void* mtx, GSvec* src, GSvec* dst);
extern "C" f32 PSVECDistance(GSvec* a, GSvec* b);
extern "C" void* memset(void* dst, long c, unsigned long n);

/* Window key bits (tagWINDOW_KEY::mKeyRepeat) the map cursor walks with. */
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

/* fsys group the map loads and frees its resources through. */
enum { worldMapFsysID = 0x9E6 };

/* Model ids of the move demo. Moving from the map whose model is
 * worldMapDemoSetAMapModel plays the 0x1B4A set, every other map the
 * 0x1FDA set; each set is the demo map model plus its three sub models. */
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
    /* Hero model hidden once the demo fade-out finished. */
    worldMapHeroModel        = 0x18530200,
};

/* Particle resource groups the scooters emit from their three parts. */
enum {
    worldMapScooterParticle0        = 0x20D01400,
    worldMapScooterParticle1        = 0x20CF1400,
    worldMapSpecialScooterParticle0 = 0x213E1400,
    worldMapSpecialScooterParticle1 = 0x213D1400,
};

/* The map menu window and the message shown once its move demo finished,
 * the scooter recovery sounds (special scooters get their own) and the
 * floor fade script started after the demo changed the map. */
enum {
    worldMapMenuID             = 0x54,
    worldMapMsgWindowID        = 0x59,
    worldMapDemoEndMsgID       = 0x1772,
    worldMapScooterSeID        = 0x5B2,
    worldMapSpecialScooterSeID = 0x5DC,
    worldMapMoveFadeScriptID   = 0x5960008,
};

/* Save flag the scooter types are unlocked against (see getScooterType). */
enum { worldMapScooterUnlockFlag = 0x3C4 };

/* Empty map setup hook; Init calls it before the map data is read. */
extern "C" void worldMapInitDummy(WorldMap* map)
{
}

/* Forward pad input to the active window unless the map owns the menu. */
void WorldMap::Button(tagWINDOW_WORK* window)
{
    if (mMenuMode != 1) {
        menuButtonNormal(window);
    }
}

/* The map drives its own pad handling, or hands it to the window while the
 * move demo is running. */
void WorldMap::Cursor(tagWINDOW_WORK* window)
{
    if (worldMapMenuOpen != 0) {
        menuCursorNormal(window, 1);
    } else if (mModelWork->mScriptFlag == 0) {
        ControlPad(window);
    }
}

/* Translate the repeated pad key into a direction and move the cursor to the
 * neighbour the walked direction of the selected icon points at. */
void WorldMap::ControlPad(tagWINDOW_WORK* window)
{
    u16 key;
    s32 dir;
    s32 iconID;
    s8 next;

    /* Reads the shared key state (the result stays unused), then walks its
     * repeat bits. */
    windowGetKeyState();
    key = windowGetKeyRepeat();
    dir = -1;
    iconID = worldMapCursorTable[mModelWork->cursor];

    if (fadeCheck(0) != 0) {
        return;
    }
    if (mMenuMode == 1) {
        return;
    }
    if (mModelWork->mModelLoading == 1) {
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

/* Tear down the map model: release the particle generators, detach the map
 * model and free the resource groups and the work block itself. */
void WorldMap::Release()
{
    if (mModelWork->mScooterModel != 0) {
        for (s32 i = 0; i < 3; i++) {
            if (mModelWork->mParticleGen[i] != 0) {
                ReleaseModelPartPaticle(this, mModelWork->mParticleGen[i], 1);
            }
        }
        ModelDetach(mModelWork->mScooterModel);
        if (worldMapScooterFree[0] != 0 && GSresGetResourceAllGroup((void*)mModelWork->mScooterModel) != 0) {
            UnloadModel(worldMapFsysID, ((WorldMapScooterData*)mModelWork->mScooter)->mModelRes);
            GSresFreeResourceAllGroup((void*)mModelWork->mScooterMotion);
        }
    } else if (mModelWork->mScriptFlag == 0) {
        while (GSfsysWait(worldMapFsysID) == 0) {
            GSthreadSwitch();
        }
        GSresFreeResource(worldMapFsysID, ((WorldMapScooterData*)mModelWork->mScooter)->mModelRes);
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

/* Free the per-icon distance array (allocated 0x10 bytes into the block). */
WorldMapIconWork::~WorldMapIconWork()
{
    delete[] mDist;
}

/* Draw the move-demo map: the offscreen model plus the four floor sub models
 * and their particle generators. Kept out of line so the per-frame move-demo
 * loops stay small (see dbgMenuItemCreate.c for the same pragma). */
#pragma dont_inline on
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
        GSgfxLayerSetCamera(floorOpenModel(this, mModelWork->mDemoModel1), cam);
        GSgfxLayerSetCamera(floorOpenModel(this, mModelWork->mDemoModel2), cam);
        GSgfxLayerSetCamera(floorOpenModel(this, mModelWork->mScooterModel), cam);
        GSgfxLayerSetCamera(floorOpenModel(this, mModelWork->mDemoMapModel), cam);
        for (s32 i = 0; i < 3; i++) {
            if (mModelWork->mParticleGen[i] != 0) {
                GSresGetResourceAllGroup((void*)mModelWork->mParticleRes[i]);
                GSparticleGeneratorSetCamera((void*)mModelWork->mParticleGen[i], cam);
            }
        }
        GScameraGetAnimFrame(cam);
    }
}
#pragma dont_inline reset

/* Play the map-to-map move demo until its animation finished. */
void WorldMap::moveDemo(long from, long to)
{
    WorldMapMapData* data;
    void* resA;
    void* resB;
    void* resC;
    void* res[1];
    f32 frames;
    f32 angle;
    u16 anime = (u16)HSD_Rand() % 2;

    mDemoEffect = new glowEffect();
    GSgfxVideoSetLockFrameRate(1);
    WorldMapIconWork* iconTo = &mIcons[worldMapCursorTable[to]];
    WorldMapIconWork* iconFrom = &mIcons[worldMapCursorTable[from]];
    angle = (f32)atan2(iconTo->screen.y - iconFrom->screen.y, iconTo->screen.x - iconFrom->screen.x);
    if (angle < worldMapFloatZero) {
        angle = worldMapTwoPi + angle;
    }
    if (!(angle >= worldMapHalfPi && angle < worldMapThreeHalfPi)) {
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
    mModelWork->mOffscreenAlpha = worldMapFloatOne;
    initializeOffscreenData();
    if (mModelWork->mDemoModel1 == worldMapDemoSetAModel1 && mDemoEffect != 0) {
        mDemoEffect->mIntensity = worldMapFloatZero;
        mDemoEffect->mParam0 = worldMapGlowParam0;
        mDemoEffect->mParam1 = worldMapGlowParam1;
        mDemoEffect->mParam2 = worldMapGlowParam2;
        mDemoEffect->mParam3 = worldMapGlowParam3[0];
        mDemoEffect->mParam4 = worldMapGlowParam4;
        mDemoEffect->Start();
    }
    resA = GSresGetResourceAllGroup((void*)((WorldMapScooterData*)mModelWork->mScooter)->mMotionRes);
    resB = GSresGetResourceAllGroup((void*)mModelWork->mDemoModel2);
    resC = GSresGetResourceAllGroup((void*)mModelWork->mDemoModel3);
    res[0] = resB;
    GSmodelSetShadowFlags(resA, 1);
    GSmodelSetShadowLight(resA, resC);
    GSmodelSetShadowSurface(resA, 1, res);
    SetMotion(mModelWork->mDemoMapModel, 0, worldMapFloatZero, worldMapDemoLoop);
    frames = GetAnimeFrameCount(mModelWork->mDemoMapModel, 0);
    mModelWork->mAnimeCamera = _cameraPlayAnime(((WorldMapAnimeRecord*)mModelWork->mAnime)->mCamera, 0, worldMapDemoLoop);
    if (mModelWork->mScooterIndex >= 4) {
        GSsndPlayNow(worldMapSpecialScooterSeID);
    } else {
        GSsndPlayNow(worldMapScooterSeID);
    }
    for (;;) {
        f32 dt = worldMapGetFrameSec(this) / worldMapDemoSpeed;

        MainControl(to);
        RenderMoveDemoModel();
        if (worldMapFloatZero != mModelWork->mOffscreenAlpha) {
            mModelWork->mOffscreenAlpha -= dt;
            if (mModelWork->mOffscreenAlpha < worldMapFloatZero) {
                mModelWork->mOffscreenAlpha = worldMapFloatZero;
                ObjectVisibility(worldMapHeroModel, worldMapHeroVisible);
            }
        }
        if (worldMapDemoEarlyEnd && frames - worldMapDemoEndLeadFrames < GetAnimeFrame(mModelWork->mDemoMapModel)) {
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
    u32 motionRes = scooter->mMotionRes;
    u32 modelRes = scooter->mModelRes;

    while (GSfsysWait(worldMapFsysID) == 0) {
        GSthreadSwitch();
    }
    GSfsysFree(worldMapFsysID);
    SetModel(worldMapFsysID, modelRes, false);
    ObjectVisibility(modelRes, true);
    mModelWork->mScooterMotion = modelRes;
    SetMotion(mModelWork->mScooterModel = motionRes, 8, worldMapFloatZero, true);
    if (mModelWork->mDemoMapModel != 0 && mModelWork->mScooterModel != 0) {
        ModelAttach(mModelWork->mScooterModel, mModelWork->mDemoMapModel,
                    ((WorldMapAnimeRecord*)mModelWork->mAnime)->mPart, GS_ATTACH_MODE_7);
    }
    GSgfxLayerID layer = worldMapScooterLayer;
    SetLayerID(mModelWork->mScooterModel, 1, &layer);
    if (mModelWork->mScooterIndex >= 4) {
        mModelWork->mParticleRes[0] = worldMapSpecialScooterParticle0;
        mModelWork->mParticleRes[1] = worldMapSpecialScooterParticle1;
        mModelWork->mParticleRes[2] = worldMapSpecialScooterParticle1;
        mModelWork->mParticleGen[0] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[0],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart0,
            GS_ATTACH_MODE_4, false, true, false);
        mModelWork->mParticleGen[1] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[1],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart1,
            GS_ATTACH_MODE_4, false, true, false);
        mModelWork->mParticleGen[2] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[2],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart2,
            GS_ATTACH_MODE_4, false, true, false);
    } else {
        mModelWork->mParticleRes[0] = worldMapScooterParticle0;
        mModelWork->mParticleRes[1] = worldMapScooterParticle1;
        mModelWork->mParticleRes[2] = worldMapScooterParticle1;
        mModelWork->mParticleGen[0] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[0],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart0,
            GS_ATTACH_MODE_4, false, true, false);
        mModelWork->mParticleGen[1] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[1],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart1,
            GS_ATTACH_MODE_4, false, true, false);
        mModelWork->mParticleGen[2] = 0;
    }
    GSmodelSetBoundCheck(GSresGetResourceAllGroup((void*)motionRes), 0);
    return motionRes;
}

s32 WorldMap::getScooterType()
{
    Hero* hero;
    s32 type;

    type = 0;
    hero = getTrainerDataStartOffset(0, 2);

    if (hero != 0) {
        u32 unlocked;
        u8 follower;

        if (GSflagGet(worldMapScooterUnlockFlag) < worldMapScooterUnlockData->scooterUnlockFlagValue) {
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
    model = anime->mModelID;
    res = anime->mModelRes;
    group = getCurrentMapGroupId();
    mModelWork->mModelRes = res;
    mModelWork->mDemoMapModel = model;
    SetModel(group, res, false);
    GSgfxLayerID layer = worldMapModelLayer;
    SetLayerID(mModelWork->mDemoMapModel, 1, &layer);
    GSmodelSetBoundCheck(GSresGetResourceAllGroup((void*)mModelWork->mDemoMapModel), 0);
}

void* _cameraPlayAnime(unsigned long cameraRes, long startFrame, bool loop)
{
    void* camera = GSresGetResourceAllGroup((void*)cameraRes);

    GScameraSetAnimIndex(camera, 0);
    if (loop) {
        GScameraSetAnimType(camera, 1);
    } else {
        GScameraSetAnimType(camera, 0);
    }
    GScameraSetAnimRate(camera, worldMapFloatHalf);
    GScameraSetAnimFrame(camera, startFrame);
    GScameraStartAnimation(camera);
    return camera;
}

/* Map loop entry: the move-demo variant while the demo map is loaded, the
 * regular one otherwise. */
void WorldMap::Main()
{
    if (mModelWork->mScriptFlag != 0) {
        MainSub2();
    } else {
        MainSub();
    }
}

/* Map helper without recovered callers; it forwards menuScriptGetWorldMapFlag(). */
extern "C" u8 getWorldMapScriptFlag(WorldMap* map)
{
    return menuScriptGetWorldMapFlag();
}

/* Close the map menu again (the move demo already ran). */
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

/* Open the map menu, move the demo to the newly picked map and hand the map
 * back to the regular update. */
void WorldMap::MainSub()
{
    s32 old = mModelWork->mMenuIndex;
    s32 sel = menuOpenCustom(worldMapMenuID, windowGetActiveID(), &mModelWork->mMenuIndex, 0, 1, 0);
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
        fadeSet(3, worldMapFloatOne);
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
        floorUnlinkWithSE(data->mExtra, 0);
        if (mModelWork->mMapChanged != 0) {
            floorSetFadeScript(0, worldMapMoveFadeScriptID);
        }
        SetFlagData(worldMapCursorTable[sel]);
    } else {
        floorUnlinkWithSE(mModelWork->mMapResource, 0);
    }
}

void WorldMap::PreRead(long index)
{
    WorldMapMapData* data = SearchData(worldMapCursorTable[index]);
    void* mapData;

    if (mModelWork->mScriptFlag != 0) {
        mapData = roomIDGetMapData(mModelWork->mMapResource);
    } else if (data != 0) {
        mapData = roomIDGetMapData(data->mExtra);
    } else {
        mapData = roomIDGetMapData(mModelWork->mMapResource);
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
    /* Window angle handed to GetPadIndex3; widened step by step so the
     * direction search gets less strict for the ribbons that are left. */
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
                if (dists[k] != worldMapFloatZero) {
                    icon->mNext[k] = dirs[k];
                }
            }
        }
        angle += step;
    } while (angle <= worldMapPadAngleMax);
}

/* Project every ribbon to screen space, then record the distance and the
 * angle of every icon pair for CheckPadIndex to pick pad slots from. */
void WorldMap::CalcModel2D(f32 angle, bool flag)
{
    WorldMapIconWork* icon;
    void* cam;
    GSmtx* mtx;
    GSmtx proj;
    GSmtx mvp;
    f32 xscale;
    /* Screen-space scratch: the projection loop and the distance loop below
     * both work through this one value. */
    f32 dist;
    s32 i;
    s32 j;

    cam = (void*)GSresGetResource(0, 0);
    mtx = (GSmtx*)GScameraGetMatrixPtr(cam);
    proj = *(GSmtx*)GScameraGetProjMatrixPtr(cam);
    PSMTXConcat(mtx, &proj, &mvp);

    f32 a = worldMapDegToRad * angle;

    xscale = worldMapProjectionScale / (f32)tan(a * worldMapFloatHalf);
    for (i = 0; i < getRibbonCnt(this); i++) {
        GSvec vec;
        f32 sx;

        icon = &mIcons[i];

        PSMTXMultVec(mtx, &icon->pos, &vec);
        sx = xscale * vec.x / vec.z;
        dist = -xscale * vec.y / vec.z;
        icon->screen.x = worldMapScreenCenterX + sx;
        icon->screen.y = worldMapScreenCenterY + dist;
        icon->screen.z = worldMapFloatZero;
    }
    for (i = 0; i < getRibbonCnt(this); i++) {
        icon = &mIcons[i];
        for (j = 0; j < getRibbonCnt(this); j++) {
            WorldMapIconWork* other = &mIcons[j];

            if (i != j) {
                f32 ang;

                if (GetFlagData(i) == 0 || GetFlagData(j) == 0) {
                    dist = worldMapFloatZero;
                    ang = dist;
                } else {
                    f32 d = PSVECDistance(&icon->screen, &other->screen);
                    dist = d;
                    ang = (f32)atan2(other->screen.y - icon->screen.y,
                                     other->screen.x - icon->screen.x);
                    if (ang < worldMapFloatZero) {
                        ang = worldMapTwoPi + ang;
                    }
                }
                icon->mDist[j].dist = dist;
                icon->mDist[j].angle = ang;
            } else {
                icon->mDist[j].dist = worldMapFloatZero;
                icon->mDist[j].angle = worldMapFloatZero;
            }
        }
    }
    CheckPadIndex(flag);
}

/* Index of the pad slot whose window contains angle, or -1 when the angle
 * falls outside every slot. The eight slot centres sit a quarter turn apart
 * (pi * index * 0.25), so the slot at angle 0 wraps around the end. */
s8 WorldMap::GetPadIndex3(f32 angle, long index, f32 width)
{
    bool inWindow = false;
    s8 padIndex = -1;
    f32 wrapEnd = worldMapTwoPi - width;
    f32 slotAngle = worldMapPi * index;
    f32 slotCenter = slotAngle * worldMapFloatQuarter;

    if (worldMapFloatZero == index) {
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

/* The loop re-reads the ribbon count each pass; the body left no code. */
void WorldMap::InitModel2D(f32 param)
{
    long i;

    CalcModel2D(param, true);
    for (i = 0; i < getRibbonCnt(this); i++) {
    }
}

/* Prime the camera blend from the default rate. */
void WorldMap::SetCameraSpeed(f32 speed)
{
    mCamWork->blend = worldMapFloatZero;
    mCamWork->mSpeed = worldMapCamSpeed;
    mCamWork->mAccel = worldMapCamAccel;
    mCamWork->mElapsed = worldMapFloatZero;
}