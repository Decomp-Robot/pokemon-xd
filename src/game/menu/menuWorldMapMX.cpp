#include <camera.h>
#include <HSD_Rand.h>
#include <fade.h>
#include <msgctrl.h>
#include <glow.h>
#include <GSflag.h>
#include <sound.h>
#include <GSfsys.h>
#include <GSmem.h>
#include <menu.h>
#include <GSmsg.h>
#include <menuItemBios.h>
#include <savedata.h>
#include <GSsnd.h>
#include <floor.h>
#include <GScamera.h>
#include <GSmodel.h>
#include <GSparticle.h>
#include <GSres.h>
#include <GSmath.h>
#include <WorldMap.h>
#include <time.h>

extern u8 worldMapMenuOpen;

extern f32 _a0;
extern f32 _v0;

extern f32 CURSOR_DISTANCE_MAX;
extern f32 _checkAngle;
extern f32 _checkAngleStart;
extern f32 _checkAngleAdd;
extern f32 _testFadeTime;
extern f32 _glowstrngth;
extern bool worldMapHeroVisible;
extern u8 worldMapDemoEarlyEnd;
extern u8 worldMapGlowParam0;
extern u8 worldMapGlowParam2;
extern u8 worldMapGlowParam4;
extern u8 worldMapScooterFree;
extern bool worldMapDemoLoop;
extern u8 worldMapGlowParam1;
extern u8 worldMapGlowParam3;
extern s16 _cursor_index_num;
extern s16 _cursor_index[];
extern WorldMap* pWorldmap;
extern WorldMapAnimeRecord* worldMapAnimeTables[2];
extern WorldMapScooterData _mapScooterData[7];

struct WorldMapUnlockEntry { u8 _00[8]; u32 mValue; u8 _0C[20]; };
struct WorldMapUnlockData { WorldMapUnlockEntry mEntries[69]; };
extern WorldMapUnlockData* worldMapScooterUnlockData;

extern "C" void menuButtonNormal(tagWINDOW_WORK* window);
extern "C" void menuCursorNormal(tagWINDOW_WORK* window, long a);
extern "C" void menuScriptSetOpenWorldMapFlag(long a);
extern "C" u8 menuScriptGetOpenWorldMapFlag();
extern "C" void heroMovePoolCounter(long a, long b);

extern "C" void GSthreadSwitch();
extern "C" unsigned long roomDataGetFsysGidForLanguage(void* data);

extern "C" void GSgfxVideoSetLockFrameRate(long rate);

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

enum { worldMapFsysID = 0x9E6 };

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
    worldMapBaseModelID      = 0x18531000,
    worldMapCursorModelID    = 0x18531013,
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

enum { worldMapScooterUnlockFlag = 0x3C4 };

extern u8 worldMapTargetDebugEnabled;

extern "C" unsigned short menuScriptOpenWorldMapGetMapNumber();

extern u32* worldMapCamDataCnt;
extern WorldMapCamData* worldMapCamDataTable;

extern f32 _waittime;
extern "C" double sin(double angle);
extern "C" f32 GSlerpGetLinearInterpolationFloat(f32 start, f32 end, f32 blend);


extern "C" u32 charNameBiosGetNameID(u32 npcID);
extern u8 worldMapLabelsVisible;

struct WorldMapInfoData { s32 mUnlockIndex; u32 mMessages[18]; };
struct WorldMapInfoList { s32 mCount; };
extern WorldMapInfoData* worldMapInfoData;
extern WorldMapInfoList* worldMapInfoList;

extern "C" void cameraUpdate();
extern "C" void cameraSetPosition(GSvec* position);
extern "C" void cameraSetTargetPos(GSvec* target);
extern "C" void cameraSetRotation(GSvec* rotation);
extern "C" void cameraSetFov(f32 fov);

struct WorldMapLayerSelection { GSgfxLayerID layer; };

static const WorldMapLayerSelection worldMapCursorLayers = {static_cast<GSgfxLayerID>(2)};
static const WorldMapLayerSelection worldMapIconLayers = {static_cast<GSgfxLayerID>(2)};

extern WorldMapAnimeRecord _animeTable_A[];
extern WorldMapAnimeRecord _animeTable_B[];
WorldMapAnimeRecord* worldMapAnimeTables[2] = { _animeTable_A, _animeTable_B };
f32 CURSOR_DISTANCE_MAX = 700.0f;
f32 _waittime = 0.7f;
f32 _a0 = 0.0001f;
f32 _checkAngle = 1.3089969f;
f32 _checkAngleStart = 0.3926990926f;
f32 _checkAngleAdd = 0.08726646f;
bool worldMapHeroVisible = true;
f32 _testFadeTime = 1.5f;
u8 worldMapDemoEarlyEnd = 1;
f32 _glowstrngth = 0.5f;
u8 worldMapGlowParam0 = 20;
u8 worldMapGlowParam2 = 1;
u8 worldMapGlowParam4 = 1;
u8 worldMapScooterFree = 1;

WorldMapScooterData _mapScooterData[7] = {
    {0x206D0400, 0x206D1000, 81, 85, 0},
    {0x206E0400, 0x206E1000, 166, 170, 0},
    {0x206F0400, 0x206F1000, 86, 90, 0},
    {0x20700400, 0x20701000, 170, 174, 0},
    {0x20710400, 0x20711000, 80, 81, 82},
    {0x20720400, 0x20721000, 84, 85, 86},
    {0x20730400, 0x20731000, 87, 88, 89},
};
s16 _cursor_index[32];
WorldMap* pWorldmap;
u8 worldMapTargetDebugEnabled;
u8 worldMapLabelsVisible;
u8 worldMapMenuOpen;
s16 _cursor_index_num;
f32 _v0;
bool worldMapDemoLoop;
u8 worldMapGlowParam1;
u8 worldMapGlowParam3;

#pragma force_active on
extern "C" void menuWorldMapMXDebug()
{
    for (s32 i = 0; i < *worldmapXD_data_number; i++) {
        u32 flag = worldmapXD_data[i].mFlagID;
        if (flag) GSflagSet(flag, 2);
    }
}

extern "C" s32 menuWorldMapMXCtrl(tagWINDOW_WORK* window)
{
    if (pWorldmap) pWorldmap->Ctrl(window);
    return 0;
}

extern "C" void menuWorldMapMXExit()
{
    delete pWorldmap;
    pWorldmap = 0;
}

extern "C" void menuWorldMapMXMain()
{
    pWorldmap->Main();
}

inline WorldMapLayerSelection worldMapModelLayers()
{
    WorldMapLayerSelection selection = {static_cast<GSgfxLayerID>(0)};
    return selection;
}
inline WorldMapLayerSelection worldMapScooterLayers()
{
    WorldMapLayerSelection selection = {static_cast<GSgfxLayerID>(0)};
    return selection;
}

extern "C" void menuWorldMapMXInit()
{
    pWorldmap = new WorldMap;
    pWorldmap->Init();
}

extern "C" void menuWorldMapButton(tagWINDOW_WORK* window)
{
    if (pWorldmap) pWorldmap->Button(window);
}

extern "C" void menuWorldMapCursor(tagWINDOW_WORK* window)
{
    if (pWorldmap) pWorldmap->Cursor(window);
}

extern "C" s32 menuWorldMapMX(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    if (pWorldmap) pWorldmap->CallBackFunc(window, sprite);
    return 0;
}

#pragma force_active reset

void WorldMap::CallBackFunc(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    s32 itemID;
    s32 cursor = GetNowCursor(window);
    itemID = sprite->mItemID;
    switch (itemID) {
    case 917:
        SetSnap(cursor, window, sprite);
        break;
    case 932:
        SetInfo(cursor, window, sprite);
        break;
    case 911: case 912: case 913: case 914: case 915: case 916:
    case 933: case 934:
        if (mModelWork->mScriptFlag) winSpriteSetDisp(sprite, 0);
        else winSpriteSetDisp(sprite, 1);
        break;
    case 327: case 328: case 329: case 330: case 331: case 332: case 333:
    case 334: case 335: case 336: case 337: case 338: case 339: case 340:
    case 1505: case 908: case 909: case 910:
        if (worldMapLabelsVisible) {
            if (menuItemBiosGetSelectFlag(worldMapMenuID, itemID)) {
                u32 labelIDs[18] = {327,328,329,330,331,332,333,334,335,336,337,338,339,340,1505,908,909,910};
                s32 i;
                s32 labelCursor = -1;
                for (i = 0; i < 18; i++) {
                    if ((u32)sprite->mItemID == labelIDs[i]) {
                        for (labelCursor = 0; labelCursor < _cursor_index_num; labelCursor++) {
                            if (i == _cursor_index[labelCursor]) break;
                        }
                        break;
                    }
                }
                if (cursor == labelCursor) {
                    sprite->mRgba.mRed = 0x22;
                    sprite->mRgba.mGreen = 0x44;
                    sprite->mRgba.mBlue = 0xEE;
                } else {
                    sprite->mRgba.mRed = 0xFF;
                    sprite->mRgba.mGreen = 0xFF;
                    sprite->mRgba.mBlue = 0xFF;
                }
                winSpriteSetDisp(sprite, 1);
            } else winSpriteSetDisp(sprite, 0);
        } else winSpriteSetDisp(sprite, 0);
        break;
    case 1476:
        break;
    }
}

s32 WorldMap::Ctrl(tagWINDOW_WORK* window)
{
    s32 cursor = GetNowCursor(window);
    switch (window->mState) {
    case 0:
        if (!mModelWork->mScriptFlag) {
            mModelWork->mScooterIndex = getScooterType();
            mModelWork->mScooter = &_mapScooterData[mModelWork->mScooterIndex];
            GSfsysReadID(worldMapFsysID, ((WorldMapScooterData*)mModelWork->mScooter)->mModelRes);
        }
        break;
    case 2:
        MainControl(cursor);
        break;
    case 3:
        break;
    }
    return 0;
}

void WorldMap::MainControl(long index)
{
    ControlCamera(index);
    TargetDebug(index);
    SetCamera();
    CalcModel2D(mCamWork->current.fov, false);
    ControlCursor(index);
    CheckIconAnime(index);
    CheckCursorAnime();
}

s32 WorldMap::GetNowCursor(tagWINDOW_WORK* window) {
    if (worldMapMenuOpen != 0) {
        return window->mCursor.y;
    }
    return mModelWork->cursor;
}

void WorldMap::InitRenderModule()
{
}

void WorldMap::Button(tagWINDOW_WORK* window)
{
    if (mMenuMode != 1) {
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

    windowGetKeyInfo();
    key = windowGetKeyRepeat();
    dir = -1;
    iconID = _cursor_index[mModelWork->cursor];

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
    for (s16 i = 0; i < _cursor_index_num; i++) {
        if (_cursor_index[i] == next) {
            mModelWork->cursor = i;
            window->mCursor.y = i;
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
                DetachModelPartPaticle(mModelWork->mParticleGen[i], true);
            }
        }
        ModelDetach(mModelWork->mScooterModel);
        if (worldMapScooterFree != 0 && GSresGetResourceAllGroup(mModelWork->mScooterModel) != 0) {
            UnloadModel(worldMapFsysID, ((WorldMapScooterData*)mModelWork->mScooter)->mModelRes);
            GSresFreeResourceAllGroup(mModelWork->mScooterMotion);
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

WorldMapIconWork::~WorldMapIconWork()
{
    delete[] mDist;
}

void WorldMap::RenderMoveDemoModel()
{
    if (pWorldmap != 0) {
        u32 model = mModelWork->mDemoModel1;
        _GScamera* cam = mModelWork->mAnimeCamera;

        if (model == worldMapDemoSetAModel1 && mDemoEffect != 0) {
            mDemoEffect->mIntensity = _glowstrngth;
        }
        renderOffscreenData(mModelWork->mOffscreenAlpha);
        GSgfxLayerFindByID(static_cast<GSgfxLayerID>(0));
        GScameraIsAnimating(cam);
        GSmodelSetRenderCamera(GetModelPtr(mModelWork->mDemoModel1), cam);
        GSmodelSetRenderCamera(GetModelPtr(mModelWork->mDemoModel2), cam);
        GSmodelSetRenderCamera(GetModelPtr(mModelWork->mScooterModel), cam);
        GSmodelSetRenderCamera(GetModelPtr(mModelWork->mDemoMapModel), cam);
        for (s32 i = 0; i < 3; i++) {
            if (mModelWork->mParticleGen[i] != 0) {
                GSresGetResourceAllGroup(mModelWork->mParticleRes[i]);
                GSparticleGeneratorSetCamera(mModelWork->mParticleGen[i], cam);
            }
        }
        GScameraGetAnimFrame(cam);
    }
}

void WorldMap::moveDemo(long from, long to)
{
    WorldMapMapData* data;
    _GSmodel* shadowModel;
    _GSmodel* shadowSurface;
    _GSlight* shadowLight;
    _GSmodel* surfaces[1];
    f32 frames;
    f32 angle;
    u16 anime = (u16)HSD_Rand() % 2;

    mDemoEffect = new glowEffect();
    GSgfxVideoSetLockFrameRate(1);
    WorldMapIconWork* iconTo = &mIcons[_cursor_index[to]];
    WorldMapIconWork* iconFrom = &mIcons[_cursor_index[from]];
    angle = (f32)atan2(iconTo->screen.y - iconFrom->screen.y, iconTo->screen.x - iconFrom->screen.x);
    if (angle < 0.0f) {
        angle = 6.2831855f + angle;
    }
    if (!(angle >= 1.5707964f && angle < 4.712389f)) {
        anime += 2;
    }
    data = SearchData(_cursor_index[from]);
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
    SetModel((unsigned long)floorGetCurrentGroupID(), mModelWork->mMapModelID, true);
    GSthreadSwitch();
    ObjectVisibility(mModelWork->mMapModelID, true);
    mModelWork->mOffscreenAlpha = 1.0f;
    initializeOffscreenData();
    if (mModelWork->mDemoModel1 == worldMapDemoSetAModel1 && mDemoEffect != 0) {
        mDemoEffect->mIntensity = 0.0f;
        mDemoEffect->mParam0 = worldMapGlowParam0;
        mDemoEffect->mParam1 = worldMapGlowParam1;
        mDemoEffect->mParam2 = worldMapGlowParam2;
        mDemoEffect->mParam3 = worldMapGlowParam3;
        mDemoEffect->mParam4 = worldMapGlowParam4;
        mDemoEffect->Start();
    }
    shadowModel = static_cast<_GSmodel*>(GSresGetResourceAllGroup(((WorldMapScooterData*)mModelWork->mScooter)->mMotionRes));
    shadowSurface = static_cast<_GSmodel*>(GSresGetResourceAllGroup(mModelWork->mDemoModel2));
    shadowLight = static_cast<_GSlight*>(GSresGetResourceAllGroup(mModelWork->mDemoModel3));
    surfaces[0] = shadowSurface;
    GSmodelSetShadowFlags(shadowModel, 1);
    GSmodelSetShadowLight(shadowModel, shadowLight);
    GSmodelSetShadowSurface(shadowModel, 1, surfaces);
    SetMotion(mModelWork->mDemoMapModel, 0, 0.0f, worldMapDemoLoop);
    frames = GetAnimeFrameCount(mModelWork->mDemoMapModel, 0);
    mModelWork->mAnimeCamera = _cameraPlayAnime(((WorldMapAnimeRecord*)mModelWork->mAnime)->mCamera, 0, worldMapDemoLoop);
    if (mModelWork->mScooterIndex >= 4) {
        GSsndPlayNow(worldMapSpecialScooterSeID);
    } else {
        GSsndPlayNow(worldMapScooterSeID);
    }
    for (;;) {
        f32 dt = GetLastTime() / _testFadeTime;

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
    mModelWork->mScooter = &_mapScooterData[mModelWork->mScooterIndex];
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
    SetMotion(mModelWork->mScooterModel = motionRes, 8, 0.0f, true);
    if (mModelWork->mDemoMapModel != 0 && mModelWork->mScooterModel != 0) {
        ModelAttach(mModelWork->mScooterModel, mModelWork->mDemoMapModel,
                    ((WorldMapAnimeRecord*)mModelWork->mAnime)->mPart, GS_ATTACH_ALL);
    }
    WorldMapLayerSelection selection = worldMapScooterLayers();
    SetLayerID(mModelWork->mScooterModel, 1, &selection.layer);
    if (mModelWork->mScooterIndex >= 4) {
        mModelWork->mParticleRes[0] = worldMapSpecialScooterParticle0;
        mModelWork->mParticleRes[1] = worldMapSpecialScooterParticle1;
        mModelWork->mParticleRes[2] = worldMapSpecialScooterParticle1;
        mModelWork->mParticleGen[0] = SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[0],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart0,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
        mModelWork->mParticleGen[1] = SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[1],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart1,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
        mModelWork->mParticleGen[2] = SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[2],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart2,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
    } else {
        mModelWork->mParticleRes[0] = worldMapScooterParticle0;
        mModelWork->mParticleRes[1] = worldMapScooterParticle1;
        mModelWork->mParticleRes[2] = worldMapScooterParticle1;
        mModelWork->mParticleGen[0] = SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[0],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart0,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
        mModelWork->mParticleGen[1] = SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->mParticleRes[1],
            ((WorldMapScooterData*)mModelWork->mScooter)->mPart1,
            GS_ATTACH_POSITION_ROTATION, false, true, false);
        mModelWork->mParticleGen[2] = 0;
    }
    GSmodelSetBoundCheck(static_cast<_GSmodel*>(GSresGetResourceAllGroup(motionRes)), false);
    return motionRes;
}

s32 WorldMap::getScooterType()
{
    Hero* hero;
    s32 type;

    type = 0;
    hero = (Hero*)savedataGetStatus(0, 2);

    if (hero != 0) {
        u32 unlocked;
        u8 follower;

        if (GSflagGet(worldMapScooterUnlockFlag) < worldMapScooterUnlockData->mEntries[68].mValue) {
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
    group = floorGetCurrentGroupID();
    mModelWork->mModelRes = res;
    mModelWork->mDemoMapModel = model;
    SetModel(group, res, false);
    WorldMapLayerSelection selection = worldMapModelLayers();
    SetLayerID(mModelWork->mDemoMapModel, 1, &selection.layer);
    GSmodelSetBoundCheck(static_cast<_GSmodel*>(GSresGetResourceAllGroup(mModelWork->mDemoMapModel)), false);
}

_GScamera* _cameraPlayAnime(unsigned long cameraRes, long startFrame, bool loop)
{
    _GScamera* camera = static_cast<_GScamera*>(GSresGetResourceAllGroup(cameraRes));

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

u8 WorldMap::IsFloorPush()
{
    return menuScriptGetOpenWorldMapFlag();
}

void WorldMap::MainSub2()
{
    WorldMapWork* work = mModelWork;

    menuOpenCustom(worldMapMenuID, windowGetActiveID(), &work->mMenuIndex, 0, 1, 0);
    if (pWorldmap != 0) {
        PreRead(0);
        menuClose(worldMapMenuID);
        menuCloseSync(worldMapMenuID, 1);
        menuScriptSetOpenWorldMapFlag(0);
        msgctrlSetValue(worldMapMsgWindowID, worldMapDemoEndMsgID);
        GSfloorPop();
    }
}

void WorldMap::MainSub()
{
    s32 old = mModelWork->mMenuIndex;
    int sel = menuOpenCustom(worldMapMenuID, windowGetActiveID(), &mModelWork->mMenuIndex, 0, 1, 0);
    WorldMapMapData* data;

    if (pWorldmap == 0) {
        return;
    }
    if (sel < 0) {
        sel = old;
    }
    PreRead(sel);
    SelectIcon(sel);
    data = SearchData(_cursor_index[sel]);
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
            GSsndStopNow(worldMapSpecialScooterSeID);
        } else {
            GSsndStopNow(worldMapScooterSeID);
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
        floorLink(data->mFloorID, 0);
        if (mModelWork->mMapChanged != 0) {
            floorSetFadeScript(0, worldMapMoveFadeScriptID);
        }
        SetFlagData(_cursor_index[sel]);
    } else {
        floorLink(mModelWork->mFloorID, 0);
    }
}

void WorldMap::PreRead(long index)
{
    WorldMapMapData* data = SearchData(_cursor_index[index]);
    void* mapData;

    if (mModelWork->mScriptFlag != 0) {
        mapData = floorDataBiosGetPtr(mModelWork->mFloorID);
    } else if (data != 0) {
        mapData = floorDataBiosGetPtr(data->mFloorID);
    } else {
        mapData = floorDataBiosGetPtr(mModelWork->mFloorID);
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
    
    f32 angle = _checkAngleStart;
    f32 step = _checkAngleAdd;
    s8 pad;

    for (i = 0; i < GetDataNum(); i++) {
        icon = &mIcons[i];
        for (k = 0; k < 8; k++) {
            icon->mNext[k] = -1;
        }
    }

    do {
        for (i = 0; i < GetDataNum(); i++) {
            memset(dists, 0, sizeof(dists));
            for (k = 0; k < 8; k++) {
                dirs[k] = -1;
            }
            icon = &mIcons[i];
            for (j = 0; j < GetDataNum(); j++) {
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
                        if (dist < CURSOR_DISTANCE_MAX) {
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
    } while (angle <= _checkAngle);
}

void WorldMap::CalcModel2D(f32 fieldOfView, bool flag)
{
    WorldMapIconWork* icon;
    _GScamera* cam;
    GSmtx* mtx;
    GSmtx proj;
    GSmtx mvp;
    f32 xscale;
    
    f32 dist;
    s32 i;
    s32 j;

    cam = static_cast<_GScamera*>(GSresGetResource(0, 0));
    mtx = GScameraGetMatrixPtr(cam);
    proj = *(GSmtx*)GScameraGetProjMatrixPtr(cam);
    PSMTXConcat((MtxPtr)mtx, (MtxPtr)&proj, (MtxPtr)&mvp);

    f32 a = 0.017453292f * fieldOfView;

    f32 tangent = (f32)tan(a / 2.0f);
    xscale = -240.0f / tangent;
    for (i = 0; i < GetDataNum(); i++) {
        GSvec vec;
        f32 sx;

        icon = &mIcons[i];

        PSMTXMultVec((MtxPtr)mtx, (const Vec *)&icon->pos, (Vec *)&vec);
        sx = xscale * vec.x / vec.z;
        dist = -xscale * vec.y / vec.z;
        icon->screen.x = 320.0f + sx;
        icon->screen.y = 240.0f + dist;
        icon->screen.z = 0.0f;
    }
    for (i = 0; i < GetDataNum(); i++) {
        icon = &mIcons[i];
        for (j = 0; j < GetDataNum(); j++) {
            WorldMapIconWork* other = &mIcons[j];

            if (i != j) {
                f32 ang;

                if (GetFlagData(i) == 0 || GetFlagData(j) == 0) {
                    dist = 0.0f;
                    ang = dist;
                } else {
                    f32 d = PSVECDistance((const Vec *)&icon->screen, (const Vec *)&other->screen);
                    dist = d;
                    ang = (f32)atan2(other->screen.y - icon->screen.y,
                                     other->screen.x - icon->screen.x);
                    if (ang < 0.0f) {
                        ang = 6.2831855f + ang;
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
    f32 wrapEnd = 6.2831855f - width;
    f32 slotAngle = 3.1415927f * index;
    f32 slotCenter = slotAngle / 4.0f;

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

void WorldMap::InitModel2D(f32 fieldOfView)
{
    long i;

    CalcModel2D(fieldOfView, true);
    for (i = 0; i < GetDataNum(); i++) {
    }
}

void WorldMap::SetCameraSpeed(f32 speed)
{
    mCamWork->blend = 0.0f;
    mCamWork->mSpeed = _v0;
    mCamWork->mAccel = _a0;
    mCamWork->mElapsed = 0.0f;
}

void WorldMap::InitCamera(long index)
{
#pragma unused(index)
    GSvec pos;
    f32 fov, distance, height, yaw;
    f32 aspect, nearZ, farZ;
    cameraSetType(3);
    mCamWork->blend = 0.0f;
    mCamWork->mSpeed = 0.0f;
    if (worldMapTargetDebugEnabled) {
        GetCameraData(19, &pos, &distance, &height, &yaw, &fov);
    } else if (GetFlagData(5)) {
        GetCameraData(19, &pos, &distance, &height, &yaw, &fov);
    } else {
        GetCameraData(18, &pos, &distance, &height, &yaw, &fov);
    }
    SetCameraNextWork(&pos, distance, height, yaw, fov);
    memcpy(&mCamWork->current, &mCamWork->next, sizeof(WorldMapCameraTarget));
    SetCamera();
    cameraUpdate();
    f32 savedFov = fov;
    _GScamera* camera = static_cast<_GScamera*>(GSresGetResource(0, 0));
    GScameraGetMatrixPtr(camera);
    GScameraGetPerspective(camera, &fov, &aspect, &nearZ, &farZ);
    GScameraSetPerspective(camera, savedFov, aspect, nearZ, farZ);
    InitModel2D(fov);
    if (!mModelWork->mScriptFlag) {
        s32 icon = _cursor_index[mModelWork->mMenuIndex];
        WorldMapMapData* data = SearchData(icon);
        GetCameraData(data->mCameraIndex, &pos, &distance, &height, &yaw, &fov);
        SetCameraWork(&mIcons[icon].pos, distance, height, yaw, fov);
        SetCameraSpeed(1.0f);
        SetCamera();
    }
}

void WorldMap::Init()
{
    soundStopEnv(1000);
    mModelWork = (WorldMapWork*)GSmemAlloc(sizeof(WorldMapWork));
    memset(mModelWork, 0, sizeof(WorldMapWork));
    mModelWork->mBaseCamera = GScameraGetActiveCamera();
    mIcons = new WorldMapIconWork[*worldmapXD_data_number];
    mCamWork = new WorldMapCamWork;
    mModelWork->mScriptFlag = IsFloorPush();
    mModelWork->mModelLoading = 1;
    mModelWork->_08 = 0.0f;
    mModelWork->_0C = 1.0f;
    mModelWork->_10 = 0.0f;
    s16 count = 0;
    s16 i;
    for (i = 0; i < GetDataNum(); i++) {
        WorldMapMapData* data = SearchData(i);
        if (data->mFlagID == 0) {
            menuItemBiosSetSelectFlag(0x54, data->mMenuItemID, 1);
            _cursor_index[count++] = i;
        } else if (GSflagGet(data->mFlagID) != 0) {
            menuItemBiosSetSelectFlag(0x54, data->mMenuItemID, 1);
            _cursor_index[count++] = i;
        } else {
            menuItemBiosSetSelectFlag(0x54, data->mMenuItemID, 0);
        }
    }
    _cursor_index_num = count;
    if (mModelWork->mScriptFlag != 0) {
        unsigned short floor = menuScriptOpenWorldMapGetMapNumber();
        mModelWork->mFloorID = SearchFloorID(floor);
        mModelWork->mMenuIndex = SearchDataFloorID(mModelWork->mFloorID);
        mModelWork->cursor = mModelWork->mMenuIndex;
        mModelWork->mInitialAngle = GetAngle(mModelWork->mMenuIndex);
        mModelWork->mFloorID = floorGetNextFloorID();
    } else {
        mModelWork->mFloorID = floorGetPrevFloorID();
        mModelWork->mMenuIndex = SearchDataFloorID(mModelWork->mFloorID);
        mModelWork->cursor = mModelWork->mMenuIndex;
        mModelWork->mInitialAngle = GetAngle(mModelWork->mMenuIndex);
    }
    InitModel(mModelWork->mMenuIndex);
    InitCamera(mModelWork->mMenuIndex);
    InitRenderModule();
}

WorldMapIconWork::WorldMapIconWork()
{
    mDist = new WorldMapIconSubWork[*worldmapXD_data_number];
    for (long i = 0; i < 8; i++) mNext[i] = -1;
}

void WorldMap::SetCamera()
{
    _GScamera* camera;
    Mtx rotation;
    GSvec angles;
    angles.x = 0.0f;
    angles.y = 0.0f;
    angles.z = 0.0f;
    GSvec offset, position;
    f32 aspect, nearZ, farZ, fov;
    timeGetLastFrameSec();
    WorldMapCameraTarget* work = &mCamWork->current;
    camera = static_cast<_GScamera*>(GSresGetResource(0, 0));
    GScameraGetMatrixPtr(camera);
    GScameraGetPerspective(camera, &fov, &aspect, &nearZ, &farZ);
    GScameraSetPerspective(camera, work->fov, aspect, nearZ, farZ);
    f32 distance = work->distance;
    f32 height = work->height;
    offset.y = height;
    offset.x = 0.0f;
    offset.z = distance;
    PSMTXRotRad(rotation, 'Y', work->yaw);
    PSMTXMultVec(rotation, (Vec*)&offset, (Vec*)&offset);
    PSVECAdd((Vec*)&work->pos, (Vec*)&offset, (Vec*)&position);
    angles.y = work->yaw;
    angles.x = -(f32)atan2(work->height, work->distance);
    angles.z = 0.0f;
    GScameraLookAt(camera, &work->up, &work->pos);
    cameraSetPosition(&position);
    cameraSetTargetPos(&work->pos);
    cameraSetRotation(&angles);
    cameraSetFov(work->fov);
}

void WorldMap::SelectIcon(long index)
{
    GSvec pos, target;
    f32 distance, fov, yaw, height;
    s32 icon = _cursor_index[index];
    WorldMapMapData* data = SearchData(icon);
    GetCameraData(data->mCameraIndex, &pos, &distance, &height, &yaw, &fov);
    PSVECAdd((Vec*)&mIcons[icon].pos, (Vec*)&pos, (Vec*)&target);
    SetCameraNextWork(&target, distance, height, yaw, fov);
    SetCameraSpeed(1.0f);
    u32 model = mIcons[icon].mModel;
    if (GetFlagData(icon) == 1) SetIconMotion(icon, model, 9, 0.0f, false);
    else SetIconMotion(icon, model, 5, 0.0f, false);
    InitCursor(index, 2);
}

const f32 WorldMap::iconAngleStep = 3.1415927f / 8.0f;

void WorldMap::TargetDebug(long index)
{
    s32 prev = mCamWork->mDebugTarget;
    s16 id = _cursor_index[index];
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
    WorldMapCameraTarget* current = &work->current;
    Blendf(&current->pos.x, current->pos.x, work->next.pos.x, mCamWork->blend);
    Blendf(&current->pos.y, current->pos.y, work->next.pos.y, mCamWork->blend);
    Blendf(&current->pos.z, current->pos.z, work->next.pos.z, mCamWork->blend);
    Blendf(&current->distance, current->distance, work->next.distance, mCamWork->blend);
    Blendf(&current->height, current->height, work->next.height, mCamWork->blend);
    Blendf(&current->fov, current->fov, work->next.fov, mCamWork->blend);
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

bool WorldMap::Blendf(f32* result, f32 start, f32 end, f32 blend)
{
    *result = GSlerpGetLinearInterpolationFloat(start, end, blend);
    return true;
}

void WorldMap::InitModel(long index)
{
    WorldMapLayerSelection selection = worldMapIconLayers;
    _GSmodel* base;
    _GSmodel* iconModel;
    s32 i;
    u32 model;
    base = static_cast<_GSmodel*>(GSresGetResourceAllGroup(worldMapBaseModelID));
    GSmodelSetAnimIndex(base, 0);
    GSmodelSetAnimFrame(base, 0.0f);
    GSmodelSetAnimRate(base, 0.5f);
    GSmodelStartAnimation(base);
    SetLayerID(worldMapBaseModelID, 1, &selection.layer);
    for (i = 0; i < GetDataNum(); i++) {
        model = GetModelID(i);
        u32 flag = GetFlagData(i);
        iconModel = GetModelPointer(i);
        mIcons[i].mModel = model;
        if (mModelWork->mScriptFlag) {
            if (i == _cursor_index[index]) {
                if (flag == 2) mIcons[i].mMotion = 3;
                else mIcons[i].mMotion = 7;
            } else mIcons[i].mMotion = GetStartIconAnimIndex(flag);
        } else {
            if (i == _cursor_index[index]) mIcons[i].mMotion = 4;
            else mIcons[i].mMotion = GetStartIconAnimIndex(flag);
        }
        SetLayerID(model, 1, &selection.layer);
        switch (mIcons[i].mMotion) {
        case 0: Visibility(model, false); break;
        case 3: case 4: SetMotion(model, mIcons[i].mMotion, 0.0f, true); break;
        default: SetMotion(model, mIcons[i].mMotion, 0.0f, false); break;
        }
        GSmodelGetPosition(iconModel, &mIcons[i].pos);
    }
    InitCursor(index, 1);
}

void WorldMap::SetCursorPosition(long index, bool blend)
{
    SearchData(_cursor_index[index]);
    WorldMapIconWork* icon = &mIcons[_cursor_index[index]];
    mCursorNextPosition.x = icon->pos.x;
    mCursorNextPosition.y = icon->pos.y;
    mCursorNextPosition.z = icon->pos.z;
    mCursorIndex = index;
    if (blend) {
        mMenuMode = 1;
        mCursorBlend = 0.0f;
    }
}

void WorldMap::InitCursor(long index, short motion)
{
    WorldMapLayerSelection selection = worldMapCursorLayers;
    mCursorElapsed = 0.0f;
    mCursorHeight = 0.0f;
    mCursorModel = worldMapCursorModelID;
    mCursorMotion = motion;
    mCursorBlend = 0.0f;
    mMenuMode = 1;
    switch (motion) {
    case 2: SetMotion(mCursorModel, mCursorMotion, 0.0f, false); break;
    default: SetMotion(mCursorModel, mCursorMotion, 0.0f, true); break;
    }
    SetLayerID(mCursorModel, 1, &selection.layer);
    SetCursorPosition(index, false);
    mCursorPosition = mCursorNextPosition;
}

s32 WorldMap::GetModelID(long index)
{
    return SearchData(index)->mIconModelID;
}

void WorldMap::SetSnap(long index, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
#pragma unused(sprite)
    WorldMapMapData* data = SearchData(_cursor_index[index]);
    switch (GetFlagData(_cursor_index[index])) {
    case 0: break;
    case 1: windowDrawSprite(0, 0, 0, 831, 0, window->mLayerID); break;
    case 2: windowDrawSprite(0, 0, 0, data->mSnapTexture, 0, window->mLayerID); break;
    }
}

void WorldMap::SetInfo(long index, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite)
{
    SearchData(_cursor_index[index]);
    if (mModelWork->mScriptFlag) {
        u32 name = charNameBiosGetNameID(GetNpcID(index));
        if (!name) name = 6002;
        msgctrlSetValue(worldMapMsgWindowID, name);
    }
    GSmsgPrint2(window->mLayerID, 0, -4, 0xFFFFFF00 | sprite->mRgba.mAlpha, GetInfoMsgID(index));
}

u32 WorldMap::GetNpcID(long index)
{
    return SearchData(_cursor_index[index])->mNpcID;
}

u32 WorldMap::GetInfoMsgID(long index)
{
    s32 icon = _cursor_index[index];
    if (mModelWork->mScriptFlag) return SearchData(icon)->mNpcMsgID;
    return worldMapInfoData[GetInfoLevel()].mMessages[icon];
}

_GSmodel* WorldMap::GetModelPointer(long index)
{
    return static_cast<_GSmodel*>(GSresGetResourceAllGroup(GetModelID(index)));
}

s32 WorldMap::GetInfoLevel()
{
    WorldMapInfoData* data = worldMapInfoData;
    s32 count = 0;
    for (s16 i = 0; i < worldMapInfoList->mCount; data++, i++) {
        if (data->mUnlockIndex != 0) {
            u32 unlockValue = worldMapScooterUnlockData->mEntries[data->mUnlockIndex].mValue;
            if (GSflagGet(worldMapScooterUnlockFlag) >= unlockValue) count++;
        }
    }
    return count - 1;
}

f32 WorldMap::GetAngle(long work)
{
    return CalculateIconAngle(work);
}

void WorldMap::SetFlagData(long index)
{
    u32 flag = SearchData(index)->mFlagID;
    if (flag != 0) GSflagSet(flag, 2);
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
    return SearchData(6)->mFloorID;
}

s32 WorldMap::SearchDataFloorID(unsigned int floorID)
{
    s32 i;
    s32 count = 0;
    for (i = 0; i < GetDataNum(); i++) {
        WorldMapMapData* data = SearchData(i);
        u32 flag = GSflagGet(data->mFlagID);
        if (data->mFlagID == 0 || flag != 0) {
            count++;
            if (floorID == (u32)worldmapXD_data[i].mFloorID) return count - 1;
        }
    }
    return 0;
}

WorldMapMapData* WorldMap::SearchData(long index)
{
    if (index >= GetDataNum()) return 0;
    return &worldmapXD_data[index];
}

long WorldMap::GetDataNum()
{
    return *worldmapXD_data_number;
}

WorldMap::~WorldMap()
{
    Release();
}

WorldMap::WorldMap()
{
    mModelWork = 0;
    mDemoEffect = 0;
}

s16 WorldMap::GetStartIconAnimIndex(unsigned long index)
{
    switch (index) {
    case 0:
        return 0;
    case 1:
        return 6;
    case 2:
        return 1;
    default:
        return 0;
    }
}

void WorldMap::ControlCursor(long index)
{
    _GSmodel* model = static_cast<_GSmodel*>(GSresGetResourceAllGroup(mCursorModel));
    f32 dt = GetLastTime();
    if (mMenuMode != 0) {
        mCursorBlend += 2.0f * dt;
        if (mCursorBlend >= _waittime) {
            mCursorBlend = 1.0f;
            mMenuMode = 0;
        }
        Blendf(&mCursorPosition.x, mCursorPosition.x, mCursorNextPosition.x, mCursorBlend);
        Blendf(&mCursorPosition.y, mCursorPosition.y, mCursorNextPosition.y, mCursorBlend);
        Blendf(&mCursorPosition.z, mCursorPosition.z, mCursorNextPosition.z, mCursorBlend);
    }
    if (mModelWork->mScriptFlag != 0) {
        mCursorElapsed += dt;
        mCursorHeight = 5.0f * (f32)sin(6.2831855f * mCursorElapsed) + 5.0f;
        mCursorPosition.y = mCursorHeight;
    }
    GSmodelSetPosition(model, &mCursorPosition);
}

f32 WorldMap::GetLastTime()
{
    return timeGetLastFrameSec();
}

void WorldMap::CheckIconAnime(long index)
{
    s32 selected = _cursor_index[index];
    mModelWork->mModelLoading = 0;
    for (s32 i = 0; i < GetDataNum(); i++) {
        u32 model = mIcons[i].mModel;
        switch (mIcons[i].mMotion) {
        case 1:
            if (!CheckMotion(model)) SetIconMotion(i, model, 2, 0.0f, true);
            else mModelWork->mModelLoading = 1;
            break;
        case 6:
            if (!CheckMotion(model)) SetIconMotion(i, model, 7, 0.0f, true);
            break;
        case 2:
            if (selected == i) SetIconMotion(i, model, 4, 0.0f, true);
            break;
        case 7:
            if (selected == i) SetIconMotion(i, model, 8, 0.0f, true);
            break;
        case 4:
            if (selected != i) SetIconMotion(i, model, 2, 0.0f, true);
            break;
        case 8:
            if (selected != i) SetIconMotion(i, model, 7, 0.0f, true);
            break;
        case 5:
            if (!CheckMotion(model)) StopMotion(model);
            break;
        case 9:
            if (!CheckMotion(model)) StopMotion(model);
            break;
        }
    }
}

void WorldMap::CheckCursorAnime()
{
    u32 model = mCursorModel;
    switch (mCursorMotion) {
    case 2:
        if (!CheckMotion(model)) StopMotion(model);
        break;
    }
}

void WorldMap::SetIconMotion(long index, unsigned long model, short motion, f32 rate, bool loop)
{
    mIcons[index].mMotion = motion;
    SetMotion(model, motion, rate, loop);
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
