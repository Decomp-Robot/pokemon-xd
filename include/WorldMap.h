#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <GScamera.h>
#include <GSgfx.h>
#include <GSmodel.h>
#include <hero.h>
#include <types.h>
#include <winSprite.h>

struct _GSparticleGen;
struct WorldMapArchiveHeader;

class glowEffect;

struct WorldMapWork {

    u8 mScriptFlag;
    u8 _01[3];
    _GScamera* mBaseCamera;
    f32 _08;
    f32 _0C;
    f32 _10;
    u8 _14[0x18];

    s32 mMenuIndex;

    s32 cursor;

    s32 mFloorID;
    f32 mInitialAngle;

    f32 mOffscreenAlpha;

    void* mAnime;

    _GScamera* mAnimeCamera;

    u8 mModelLoading;
    u8 _49[0x03];

    u32 mMapModelID;

    u32 mDemoModel0;
    u32 mDemoModel1;
    u32 mDemoModel2;
    u32 mDemoModel3;

    s32 mScooterIndex;

    u32 mScooterMotion;

    u32 mScooterModel;

    u32 mModelRes;

    u32 mDemoMapModel;

    void* mScooter;

    u32 mParticleRes[0x03];
    _GSparticleGen* mParticleGen[0x03];

    u8 mMapChanged;
    u8 _91[0x03];
};

struct WorldMapIconSubWork {
    WorldMapIconSubWork(f32 distance = 0.0f, f32 bearing = 0.0f)
        : dist(distance), angle(bearing) {}
    f32 dist;
    f32 angle;
};

struct WorldMapIconWork {
    u32 mModel;
    s16 mMotion;
    u8 _06[2];
    GSvec pos;

    s8 mNext[0x08];

    GSvec screen;

    WorldMapIconSubWork* mDist;
    WorldMapIconWork();
    ~WorldMapIconWork();
};

struct WorldMapCamData {
    GSvec pos;
    f32 distance;
    f32 height;
    f32 yaw;
    f32 fov;
};

struct WorldMapCameraTarget {
    GSvec pos;
    GSvec up;
    f32 distance;
    f32 height;
    f32 yaw;
    f32 fov;
};

struct WorldMapCamWork {
    WorldMapCamWork() { mElapsed = 0.0f; blend = 0.0f; mAccel = 0.0f; }

    s32 mDebugTarget;

    s32 mPrevDebugTarget;

    f32 blend;

    f32 mAccel;

    f32 mSpeed;

    f32 mElapsed;

    WorldMapCameraTarget current;
    WorldMapCameraTarget next;
};

struct WorldMapAnimeRecord {

    u32 mModelRes;

    u32 mModelID;
    u8 _08[0x02];

    s16 mPart;

    u32 mCamera;
};

struct WorldMapScooterData {
    u32 mModelRes;
    u32 mMotionRes;

    u32 mPart0;
    u32 mPart1;
    u32 mPart2;
};

struct WorldMapMapData {
    s16 mCameraIndex;
    u16 mNpcID;

    u32 mFlagID;
    u32 mMenuItemID;
    u32 mIconModelID;
    u32 mSnapTexture;
    u32 mModelID;
    u8 _18[0x04];
    s32 mFloorID;
    u32 mNpcMsgID;
};

extern s32* worldmapXD_data_number;
extern WorldMapMapData* worldmapXD_data;

#include <GSattach.h>

class WorldMapModel {
public:
    WorldMapModel() { mOffscreenTexture = 0; }
    ~WorldMapModel() { if (mOffscreenTexture) releaseOffscreenData(); }
    void releaseOffscreenData();
    bool CheckMotion(unsigned long model);
    void StopMotion(unsigned long model);
    void renderOffscreenData(f32 param);
    void initializeOffscreenData();
    WorldMapArchiveHeader* SetModel(unsigned long model, unsigned long resource, bool visible);
    void UnloadModel(unsigned long model, unsigned long resource);
    void Visibility(unsigned long model, bool visible);
    void ObjectVisibility(unsigned long model, bool visible);
    void ModelDetach(unsigned long model);
    void ModelAttach(unsigned long model, unsigned long part, long mode, GSattachMode attachMode);
    _GSparticleGen* SetModelPartPaticle(unsigned long model, unsigned long particleResource, long part, GSattachMode attachMode, bool oneShot, bool resetTransform, bool linkChildren);
    void DetachModelPartPaticle(_GSparticleGen* generator, bool keepLink);
    _GSmodel* GetModelPtr(unsigned long modelID);
    void SetMotion(unsigned long motion, short startFrame, f32 rate, bool loop);
    void SetLayerID(unsigned long model, long part, GSgfxLayerID* layer);
    f32 GetAnimeFrameCount(unsigned long motion, unsigned long a);
    f32 GetAnimeFrame(unsigned long motion);

protected:

    u32 mOffscreenTexture;
};

class WorldMap : public WorldMapModel {
public:
    WorldMap();
    ~WorldMap();
    void InitRenderModule();
    s32 Ctrl(tagWINDOW_WORK* window);
    void CallBackFunc(tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);
    void SetCamera();
    void InitCursor(long index, short motion);
    void SetSnap(long index, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);
    void SetInfo(long index, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);
    u32 GetNpcID(long index);
    u32 GetInfoMsgID(long index);
    _GSmodel* GetModelPointer(long index);
    s32 GetInfoLevel();
    void CheckIconAnime(long index);
    void CheckCursorAnime();
    void SetIconMotion(long index, unsigned long model, short motion, f32 rate, bool loop);
    void Init();
    void InitModel(long index);
    void InitCamera(long index);
    u32 SearchFloorID(unsigned short index);
    s32 SearchDataFloorID(unsigned int floorID);
    u8 IsFloorPush();
    long GetDataNum();
    f32 GetLastTime();
    void ControlCamera(long index);
    bool Blendf(f32* result, f32 start, f32 end, f32 blend);
    void ControlCursor(long index);

    void Button(tagWINDOW_WORK* window);
    void Cursor(tagWINDOW_WORK* window);
    void ControlPad(tagWINDOW_WORK* window);
    void Release();
    void RenderMoveDemoModel();
    void moveDemo(long from, long to);
    unsigned long readScooter();
    s32 getScooterType();
    void setNullModel();
    void PreRead(long index);
    void Main();
    void MainSub();
    void MainSub2();
    void CheckPadIndex(bool flag);
    void CalcModel2D(f32 angle, bool flag);

    s32 GetNowCursor(tagWINDOW_WORK* window);
    s16 GetStartIconAnimIndex(unsigned long index);

    bool IsBlending();
    f32 GetAngle(long work);

private:
    static const f32 iconAngleStep;
    static f32 CalculateIconAngle(long index)
    {
        return iconAngleStep * index;
    }

public:
    s32 GetModelID(long index);
    void SetCursorPosition(long index, bool flag);
    void InitModel2D(f32 param);
    _GSmodel* GetModel(long index);
    void SetCameraSpeed(f32 speed);
    void TargetDebug(long index);
    void SetCameraNextWork(GSvec* pos, f32 distance, f32 height, f32 yaw, f32 fov);
    void SetCameraWork(GSvec* pos, f32 distance, f32 height, f32 yaw, f32 fov);
    void GetCameraData(long index, GSvec* pos, f32* distance, f32* height, f32* yaw, f32* fov);
    WorldMapMapData* SearchData(long index);
    void SelectIcon(long index);
    void MainControl(long index);
    void SetFlagData(long index);

    u32 GetFlagData(long index);
    s8 GetPadIndex3(f32 angle, long index, f32 width);

private:

    glowEffect* mDemoEffect;

    WorldMapWork* mModelWork;

    WorldMapIconWork* mIcons;

    u32 mCursorModel;
    s16 mCursorMotion;
    u8 _16[2];
    s32 mCursorIndex;

    u8 mMenuMode;

    u8 _1D[3];
    GSvec mCursorPosition;
    GSvec mCursorNextPosition;
    f32 mCursorBlend;
    f32 mCursorHeight;
    f32 mCursorElapsed;

    WorldMapCamWork* mCamWork;
};

_GScamera* _cameraPlayAnime(unsigned long cameraRes, long startFrame, bool loop);


#endif
