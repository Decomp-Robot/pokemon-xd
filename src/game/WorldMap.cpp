#include <WorldMap.h>

extern f32 lbl_804E7E90;
extern f32 lbl_804EA704;
extern const f32 lbl_804EBEEC;  /* 0.0f */
extern const f32 lbl_804EBEE8;  /* 2*pi */
extern const f32 lbl_804EBEF0;  /* pi/2 */
extern const f32 lbl_804EBEF4;  /* 3*pi/2 */
extern const f32 lbl_804EBEF8;  /* 1.0f */
extern const f32 lbl_804EBEFC;  /* 60.0f */
extern const f32 lbl_804EBF20;  /* pi */
extern const f32 lbl_804EBF24;  /* 0.25f */
extern const f32 lbl_804EBF00;  /* 0.5f */
extern const f32 lbl_804EBF10;  /* pi/180 */
extern const f32 lbl_804EBF14;
extern const f32 lbl_804EBF18;
extern const f32 lbl_804EBF1C;
extern const f32 lbl_804E7E88;
extern const f32 lbl_804E7E8C;
extern f32 lbl_804E7E94;
extern const f32 lbl_804E7E98;
extern const f32 lbl_804E7E9C;
extern f32 lbl_804E7EA4;
extern const f32 lbl_804E7EAC;
extern bool lbl_804E7EA0;
extern u8 lbl_804E7EA8;
extern u8 lbl_804E7EB0;
extern u8 lbl_804E7EB1;
extern u8 lbl_804E7EB2;
extern const u8 lbl_804E7EB3;
extern bool lbl_804EA708;
extern u8 lbl_804EA709;
extern u8 lbl_804EA70A;
extern const s16 lbl_804EA700;
extern const u8 lbl_804EA6FE;
extern const s16 lbl_80428080[0x20];
extern u32 lbl_804EA6F8;
extern WorldMapAnimeRecord* lbl_804E7E80[2];
extern WorldMapScooterData lbl_803122F8[7];
extern GSgfxLayerID lbl_804EFC90;
extern GSgfxLayerID lbl_804EFC94;
extern void* lbl_804E893C;

extern "C" long getRibbonCnt(WorldMap* map);

extern "C" void menuButtonNormal(tagWINDOW_WORK* window);
extern "C" void zz_010f6b0_(tagWINDOW_WORK* window, long a);
extern "C" u16 windowGetKeyRepeat();
extern "C" s8 fadeCheck(long a);
extern "C" void fadeSet(long a, f32 b);
extern "C" s32 windowGetActiveID();
extern "C" s32 menuOpenCustom(s32 menu, s32 window, void* work, ...);
extern "C" void menuClose(s32 menu);
extern "C" void menuCloseSync(s32 menu, s32 a);
extern "C" void zz_005ee78_(long a);
extern "C" u8 zz_005ee80_();
extern "C" void msgctrlSetValue(long win, long value);
extern "C" void floorPop();
extern "C" void heroMovePoolCounter(long a, long b);
extern "C" void _recoveryEventSetSE__FUl(unsigned long id);
extern "C" void GSthreadSwitch();
extern "C" void GSfsysFree(long id);
extern "C" void GSfsysPreRead(long id);
extern "C" void* zz_roomID_get_map_data(void* res);
extern "C" unsigned long zz_room_data_get_fsys_gid_for_language(void* data);
extern "C" Hero* zz_get_trainer_data_start_offset(long a, long b);
extern "C" void* GSresGetResource(unsigned long id, unsigned long res);
extern "C" void GSsndPlayNow(long id);
extern "C" f32 zz_00133ac_(WorldMap* map);
extern "C" void zz_01202e0_(void* p, long a);
extern "C" void floorSetFadeScript(long a, long b);
extern "C" long zz_current_map_get_group_id();
extern "C" void GSmemFree(void* p);
extern "C" void* __nw__FUl(unsigned long size);
extern "C" void __destroy_new_array(void* p, void* dtor);
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
extern "C" void zz_00ebff8_(void* cam);
extern "C" void zz_00f765c_(unsigned long layer, void* cam);
extern "C" unsigned long floorOpenModel(WorldMap* map, unsigned long id);
extern "C" void GSmodelSetShadowFlags(void* res, long flags);
extern "C" void GSmodelSetShadowLight(void* res, void* light);
extern "C" void GSmodelSetShadowSurface(void* res, long a, void* b);
extern "C" void GSmodelSetBoundCheck(void* res, long a);
extern "C" unsigned long GSflagGet(long flag);
extern "C" unsigned long HSD_Rand();
extern "C" u8 zz_019e000_(long id);
extern "C" void zz_009e3b0_(WorldMap* map, unsigned long a, long b);

extern "C" double atan2(double y, double x);
extern "C" double tan(double x);
extern "C" void PSMTXConcat(void* a, void* b, void* dst);
extern "C" void PSMTXMultVec(void* mtx, GSvec* src, GSvec* dst);
extern "C" f32 PSVECDistance(GSvec* a, GSvec* b);
extern "C" void* memset(void* dst, long c, unsigned long n);

/* Empty map helper; the original build left a single blr here. */
extern "C" void zz_0010558_(WorldMap* map)
{
}

/* Forward pad input to the active window unless the map owns the menu. */
void WorldMap::Button(tagWINDOW_WORK* window)
{
    if (_1C != 1) {
        menuButtonNormal(window);
    }
}

/* The map drives its own pad handling, or hands it to the window while the
 * move demo is running. */
void WorldMap::Cursor(tagWINDOW_WORK* window)
{
    if (lbl_804EA6FE != 0) {
        zz_010f6b0_(window, 1);
    } else if (mModelWork->_00 == 0) {
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

    windowGetKeyState();
    key = windowGetKeyRepeat();
    dir = -1;
    iconID = lbl_80428080[mModelWork->cursor];

    if (fadeCheck(0) != 0) {
        return;
    }
    if (_1C == 1) {
        return;
    }
    if (mModelWork->_48 == 1) {
        return;
    }
    GetModelID(iconID);
    if (key == 8) {
        dir = 0;
    } else if (key == 10) {
        dir = 1;
    } else if (key == 2) {
        dir = 2;
    } else if (key == 6) {
        dir = 3;
    } else if (key == 4) {
        dir = 4;
    } else if (key == 5) {
        dir = 5;
    } else if (key == 1) {
        dir = 6;
    } else if (key == 9) {
        dir = 7;
    }
    WorldMapIconWork* icon = &mIcons[iconID];

    if (dir < 0) {
        return;
    }
    next = icon->_14[dir];
    if (next < 0) {
        return;
    }
    for (s16 i = 0; i < lbl_804EA700; i++) {
        if (lbl_80428080[i] == next) {
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
            if (mModelWork->_84[i] != 0) {
                zz_009e3b0_(this, mModelWork->_84[i], 1);
            }
        }
        ModelDetach(mModelWork->mScooterModel);
        if (lbl_804E7EB3 != 0 && GSresGetResourceAllGroup((void*)mModelWork->mScooterModel) != 0) {
            UnloadModel(0x9E6, ((WorldMapScooterData*)mModelWork->mScooter)->mModelRes);
            GSresFreeResourceAllGroup((void*)mModelWork->mScooterMotion);
        }
    } else if (mModelWork->_00 == 0) {
        while (zz_019e000_(0x9E6) == 0) {
            GSthreadSwitch();
        }
        GSresFreeResource(0x9E6, ((WorldMapScooterData*)mModelWork->mScooter)->mModelRes);
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
 * and their particle generators. */
#pragma dont_inline on
void WorldMap::RenderMoveDemoModel()
{
    if (lbl_804EA6F8 != 0) {
        u32 model = mModelWork->_54;
        void* cam = (void*)mModelWork->mAnimeFrames;

        if (model == 0x1B4A1001 && mDemoEffect != 0) {
            mDemoEffect->mIntensity = lbl_804E7EAC;
        }
        renderOffscreenData(mModelWork->mOffscreenAlpha);
        GSgfxLayerFindByID(0);
        zz_00ebff8_(cam);
        zz_00f765c_(floorOpenModel(this, mModelWork->_54), cam);
        zz_00f765c_(floorOpenModel(this, mModelWork->_58), cam);
        zz_00f765c_(floorOpenModel(this, mModelWork->mScooterModel), cam);
        zz_00f765c_(floorOpenModel(this, mModelWork->_70), cam);
        for (s32 i = 0; i < 3; i++) {
            if (mModelWork->_84[i] != 0) {
                GSresGetResourceAllGroup((void*)mModelWork->_78[i]);
                GSparticleGeneratorSetCamera((void*)mModelWork->_84[i], cam);
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
    WorldMapIconWork* iconTo = &mIcons[lbl_80428080[to]];
    WorldMapIconWork* iconFrom = &mIcons[lbl_80428080[from]];
    angle = (f32)atan2(iconTo->screen.y - iconFrom->screen.y, iconTo->screen.x - iconFrom->screen.x);
    if (angle < lbl_804EBEEC) {
        angle = lbl_804EBEE8 + angle;
    }
    if (!(angle >= lbl_804EBEF0 && angle < lbl_804EBEF4)) {
        anime += 2;
    }
    data = SearchData(lbl_80428080[from]);
    mModelWork->mMapModelID = data->mModelID;
    if (data->mModelID == 0x1B4A0400) {
        mModelWork->_50 = 0x1B4A1000;
        mModelWork->_54 = 0x1B4A1001;
        mModelWork->_58 = 0x1B4A1002;
        mModelWork->_5C = 0x1B4A1603;
        mModelWork->mAnime = (u8*)lbl_804E7E80[0] + anime * 0x10;
    } else {
        mModelWork->_50 = 0x1FDA1000;
        mModelWork->_54 = 0x1FDA1001;
        mModelWork->_58 = 0x1FDA1002;
        mModelWork->_5C = 0x1FDA1602;
        mModelWork->mAnime = (u8*)lbl_804E7E80[1] + anime * 0x10;
    }
    setNullModel();
    readScooter();
    GSthreadSwitch();
    SetModel((unsigned long)zz_current_map_get_group_id(), mModelWork->mMapModelID, true);
    GSthreadSwitch();
    ObjectVisibility(mModelWork->mMapModelID, true);
    mModelWork->mOffscreenAlpha = lbl_804EBEF8;
    initializeOffscreenData();
    if (mModelWork->_54 == 0x1B4A1001 && mDemoEffect != 0) {
        mDemoEffect->mIntensity = lbl_804EBEEC;
        mDemoEffect->_10 = lbl_804E7EB0;
        mDemoEffect->_11 = lbl_804EA709;
        mDemoEffect->_12 = lbl_804E7EB1;
        mDemoEffect->_13 = lbl_804EA70A;
        mDemoEffect->_14 = lbl_804E7EB2;
        mDemoEffect->Start();
    }
    resA = GSresGetResourceAllGroup((void*)((WorldMapScooterData*)mModelWork->mScooter)->mMotionRes);
    resB = GSresGetResourceAllGroup((void*)mModelWork->_58);
    resC = GSresGetResourceAllGroup((void*)mModelWork->_5C);
    res[0] = resB;
    GSmodelSetShadowFlags(resA, 1);
    GSmodelSetShadowLight(resA, resC);
    GSmodelSetShadowSurface(resA, 1, res);
    SetMotion(mModelWork->_70, 0, lbl_804EBEEC, lbl_804EA708);
    frames = GetAnimeFrameCount(mModelWork->_70, 0);
    mModelWork->mAnimeFrames = _cameraPlayAnime(((WorldMapAnimeRecord*)mModelWork->mAnime)->mCamera, 0, lbl_804EA708);
    if (mModelWork->mScooterIndex >= 4) {
        GSsndPlayNow(0x5DC);
    } else {
        GSsndPlayNow(0x5B2);
    }
    for (;;) {
        f32 dt = zz_00133ac_(this) / lbl_804E7EA4;

        MainControl(to);
        RenderMoveDemoModel();
        if (lbl_804EBEEC != mModelWork->mOffscreenAlpha) {
            mModelWork->mOffscreenAlpha -= dt;
            if (mModelWork->mOffscreenAlpha < lbl_804EBEEC) {
                mModelWork->mOffscreenAlpha = lbl_804EBEEC;
                ObjectVisibility(0x18530200, lbl_804E7EA0);
            }
        }
        if (lbl_804E7EA8 && frames - lbl_804EBEFC < GetAnimeFrame(mModelWork->_70)) {
            break;
        }
        GSthreadSwitch();
    }
}

unsigned long WorldMap::readScooter()
{
    if (mModelWork->_00 != 0) {
        return 0;
    }
    mModelWork->mScooter = &lbl_803122F8[mModelWork->mScooterIndex];
    WorldMapScooterData* scooter = (WorldMapScooterData*)mModelWork->mScooter;
    u32 motionRes = scooter->mMotionRes;
    u32 modelRes = scooter->mModelRes;

    while (zz_019e000_(0x9E6) == 0) {
        GSthreadSwitch();
    }
    GSfsysFree(0x9E6);
    SetModel(0x9E6, modelRes, false);
    ObjectVisibility(modelRes, true);
    mModelWork->mScooterMotion = modelRes;
    SetMotion(mModelWork->mScooterModel = motionRes, 8, lbl_804EBEEC, true);
    if (mModelWork->_70 != 0 && mModelWork->mScooterModel != 0) {
        ModelAttach(mModelWork->mScooterModel, mModelWork->_70,
                    ((WorldMapAnimeRecord*)mModelWork->mAnime)->_0A, GS_ATTACH_MODE_7);
    }
    GSgfxLayerID layer = lbl_804EFC94;
    SetLayerID(mModelWork->mScooterModel, 1, &layer);
    if (mModelWork->mScooterIndex >= 4) {
        mModelWork->_78[0] = 0x213E1400;
        mModelWork->_78[1] = 0x213D1400;
        mModelWork->_78[2] = 0x213D1400;
        mModelWork->_84[0] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->_78[0],
            ((WorldMapScooterData*)mModelWork->mScooter)->_08,
            GS_ATTACH_MODE_4, false, true, false);
        mModelWork->_84[1] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->_78[1],
            ((WorldMapScooterData*)mModelWork->mScooter)->_0C,
            GS_ATTACH_MODE_4, false, true, false);
        mModelWork->_84[2] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->_78[2],
            ((WorldMapScooterData*)mModelWork->mScooter)->_10,
            GS_ATTACH_MODE_4, false, true, false);
    } else {
        mModelWork->_78[0] = 0x20D01400;
        mModelWork->_78[1] = 0x20CF1400;
        mModelWork->_78[2] = 0x20CF1400;
        mModelWork->_84[0] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->_78[0],
            ((WorldMapScooterData*)mModelWork->mScooter)->_08,
            GS_ATTACH_MODE_4, false, true, false);
        mModelWork->_84[1] = (u32)SetModelPartPaticle(
            mModelWork->mScooterModel, mModelWork->_78[1],
            ((WorldMapScooterData*)mModelWork->mScooter)->_0C,
            GS_ATTACH_MODE_4, false, true, false);
        mModelWork->_84[2] = 0;
    }
    GSmodelSetBoundCheck(GSresGetResourceAllGroup((void*)motionRes), 0);
    return motionRes;
}

s32 WorldMap::getScooterType()
{
    Hero* hero;
    s32 type;

    type = 0;
    hero = zz_get_trainer_data_start_offset(0, 2);

    if (hero != 0) {
        u32 unlocked;
        u8 follower;

        if (GSflagGet(0x3C4) < *(u32*)((u8*)lbl_804E893C + 0x888)) {
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
    model = anime->_04;
    res = anime->_00;
    group = zz_current_map_get_group_id();
    mModelWork->mModelRes = res;
    mModelWork->_70 = model;
    SetModel(group, res, false);
    GSgfxLayerID layer = lbl_804EFC90;
    SetLayerID(mModelWork->_70, 1, &layer);
    GSmodelSetBoundCheck(GSresGetResourceAllGroup((void*)mModelWork->_70), 0);
}

long _cameraPlayAnime(unsigned long model, long loop, bool flag)
{
    void* camera = GSresGetResourceAllGroup((void*)model);

    GScameraSetAnimIndex(camera, 0);
    if (flag) {
        GScameraSetAnimType(camera, 1);
    } else {
        GScameraSetAnimType(camera, 0);
    }
    GScameraSetAnimRate(camera, lbl_804EBF00);
    GScameraSetAnimFrame(camera, (f32)loop);
    GScameraStartAnimation(camera);
    return (long)camera;
}

/* Map loop entry: the move-demo variant while the demo map is loaded, the
 * regular one otherwise. */
void WorldMap::Main()
{
    if (mModelWork->_00 != 0) {
        MainSub2();
    } else {
        MainSub();
    }
}

/* Map helper without recovered callers; it forwards zz_005ee80_(). */
extern "C" u8 zz_0011408_(WorldMap* map)
{
    return zz_005ee80_();
}

/* Close the map menu again (the move demo already ran). */
void WorldMap::MainSub2()
{
    WorldMapWork* work = mModelWork;

    menuOpenCustom(0x54, windowGetActiveID(), (u8*)work + 0x2C, 0, 1, 0);
    if (lbl_804EA6F8 != 0) {
        PreRead(0);
        menuClose(0x54);
        menuCloseSync(0x54, 1);
        zz_005ee78_(0);
        msgctrlSetValue(0x59, 0x1772);
        floorPop();
    }
}

/* Open the map menu, move the demo to the newly picked map and hand the map
 * back to the regular update. */
void WorldMap::MainSub()
{
    WorldMapWork* work = mModelWork;
    s32 old = *(s32*)((u8*)work + 0x2C);
    s32 sel = menuOpenCustom(0x54, windowGetActiveID(), (u8*)work + 0x2C, 0, 1, 0);
    WorldMapMapData* data;

    if (lbl_804EA6F8 == 0) {
        return;
    }
    if (sel < 0) {
        sel = old;
    }
    PreRead(sel);
    SelectIcon(sel);
    data = SearchData(lbl_80428080[sel]);
    menuClose(0x54);
    menuCloseSync(0x54, 1);
    if (sel != old) {
        mModelWork->_90 = 1;
        heroMovePoolCounter(2, 0x28);
        moveDemo(old, sel);
    }
    if (sel != old) {
        fadeSet(3, lbl_804EBEF8);
        do {
            RenderMoveDemoModel();
            GSthreadSwitch();
        } while (fadeCheck(0) != 0);
        if (mModelWork->mScooterIndex >= 4) {
            _recoveryEventSetSE__FUl(0x5DC);
        } else {
            _recoveryEventSetSE__FUl(0x5B2);
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
        zz_01202e0_(data->mExtra, 0);
        if (mModelWork->_90 != 0) {
            floorSetFadeScript(0, 0x5960008);
        }
        SetFlagData(lbl_80428080[sel]);
    } else {
        zz_01202e0_(mModelWork->mMapResource, 0);
    }
}

void WorldMap::PreRead(long index)
{
    WorldMapMapData* data = SearchData(lbl_80428080[index]);
    void* mapData;

    if (mModelWork->_00 != 0) {
        mapData = zz_roomID_get_map_data(mModelWork->mMapResource);
    } else if (data != 0) {
        mapData = zz_roomID_get_map_data(data->mExtra);
    } else {
        mapData = zz_roomID_get_map_data(mModelWork->mMapResource);
    }
    GSfsysPreRead(zz_room_data_get_fsys_gid_for_language(mapData));
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
    f32 angle = lbl_804E7E98;
    f32 step = lbl_804E7E9C;
    s8 pad;

    for (i = 0; i < getRibbonCnt(this); i++) {
        icon = &mIcons[i];
        for (k = 0; k < 8; k++) {
            icon->_14[k] = -1;
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
                    if (pad >= 0 && icon->_14[pad] < 0) {
                        dist = icon->mDist[j].dist;
                        if (dist < lbl_804E7E88) {
                            if (dists[pad] == 0) {
                                dirs[pad] = (s8)j;
                                dists[pad] = (s32)dist;
                            } else if (dists[pad] > (s32)dist) {
                                dirs[pad] = (s8)j;
                                dists[pad] = (s32)(f32)dist;
                            }
                        }
                    }
                }
            }
            for (k = 0; k < 8; k++) {
                if (dists[k] != lbl_804EBEEC) {
                    icon->_14[k] = dirs[k];
                }
            }
        }
        angle += step;
    } while (angle <= lbl_804E7E94);
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
    /* Distance to the neighbour ribbon, stored into mDist[j].dist. The
     * projection loop that runs first reuses the same variable for the
     * projected screen y offset, so both loops share this one scratch value. */
    f32 dist;
    s32 i;
    s32 j;

    cam = (void*)GSresGetResource(0, 0);
    mtx = (GSmtx*)GScameraGetMatrixPtr(cam);
    proj = *(GSmtx*)GScameraGetProjMatrixPtr(cam);
    PSMTXConcat(mtx, &proj, &mvp);

    f32 a = lbl_804EBF10 * angle;

    xscale = lbl_804EBF14 / (f32)tan(a * lbl_804EBF00);
    for (i = 0; i < getRibbonCnt(this); i++) {
        GSvec vec;
        f32 sx;

        icon = &mIcons[i];

        PSMTXMultVec(mtx, &icon->pos, &vec);
        sx = xscale * vec.x / vec.z;
        dist = -xscale * vec.y / vec.z;
        icon->screen.x = lbl_804EBF18 + sx;
        icon->screen.y = lbl_804EBF1C + dist;
        icon->screen.z = lbl_804EBEEC;
    }
    for (i = 0; i < getRibbonCnt(this); i++) {
        icon = &mIcons[i];
        for (j = 0; j < getRibbonCnt(this); j++) {
            WorldMapIconWork* other = &mIcons[j];

            if (i != j) {
                f32 ang;

                if (GetFlagData(i) == 0 || GetFlagData(j) == 0) {
                    dist = lbl_804EBEEC;
                    ang = dist;
                } else {
                    f32 d = PSVECDistance(&icon->screen, &other->screen);
                    dist = d;
                    ang = (f32)atan2(other->screen.y - icon->screen.y,
                                     other->screen.x - icon->screen.x);
                    if (ang < lbl_804EBEEC) {
                        ang = lbl_804EBEE8 + ang;
                    }
                }
                icon->mDist[j].dist = dist;
                icon->mDist[j].angle = ang;
            } else {
                icon->mDist[j].dist = lbl_804EBEEC;
                icon->mDist[j].angle = lbl_804EBEEC;
            }
        }
    }
    CheckPadIndex(flag);
}

/* Index of the pad slot whose window contains angle, or -1 when the angle

 * falls outside every slot. The eight slots are spread evenly over the
 * turn (pi/4 apart), so the slot at angle 0 wraps around the end. */
s8 WorldMap::GetPadIndex3(f32 angle, long index, f32 width)
{
    bool inWindow = false;
    s8 padIndex = -1;
    f32 wrapEnd = lbl_804EBEE8 - width;
    f32 slotAngle = lbl_804EBF20 * (f32)index;
    f32 slotCenter = slotAngle * lbl_804EBF24;

    if (lbl_804EBEEC == (f32)index) {
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
    mCamWork->blend = lbl_804EBEEC;
    mCamWork->_10 = lbl_804EA704;
    mCamWork->_0C = lbl_804E7E90;
    mCamWork->_14 = lbl_804EBEEC;
}