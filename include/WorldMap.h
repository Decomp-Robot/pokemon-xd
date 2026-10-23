#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <GSvec.h>
#include <GSattach.h>
#include <GSgfx.h>
#include <GSeffect.h>
#include <hero.h>
#include <types.h>
#include <winSprite.h>

struct _GSmodel;
struct WorldMapArchiveHeader;

struct WorldMapWork {
    u8 mScriptFlag;
    u8 mPadding01[3];
    void* mSavedCamera;
    f32 mMoveBlend;
    f32 mMoveSpeed;
    f32 mMoveAccel;
    u8 mPadding14[0x18];
    s32 mMenuIndex;
    s32 cursor;
    u32 mMapID;
    f32 mCursorAngle;
    f32 mOffscreenAlpha;
    void* mAnime;
    void* mAnimeCamera;
    u8 mIconAnimationActive;
    u8 mPadding49[0x03];
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
    u32 mParticleGen[0x03];
    u8 mMapChanged;
    u8 mPadding91[0x03];
};

/* Distance and direction to a neighbouring icon. */
struct WorldMapIconDistance {
    WorldMapIconDistance() { dist = 0.0f; angle = 0.0f; }
    f32 dist;
    f32 angle;
};

/* Eight pad directions index the neighbouring icons. */
struct WorldMapIconWork {
    u32 mModelID;
    s16 mAnimationIndex;
    u8 mPadding06[2];
    GSvec pos;

    s8 mNext[0x08];
    GSvec screen;

    WorldMapIconDistance* mDist;
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

struct WorldMapCameraState {
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
    WorldMapCameraState current;
    WorldMapCameraState next;
};

struct WorldMapAnimeRecord {
    u32 mModelResource;
    u32 mMotionResource;
    u8 mPadding08[0x02];
    s16 mScooterAttachmentPart;
    u32 mCameraResource;
};

struct WorldMapScooterData {
    u32 mModelResource;
    u32 mMotionResource;
    u32 mPrimaryParticleAttachmentPart;
    u32 mSecondaryParticleAttachmentPart;
    u32 mTertiaryParticleAttachmentPart;
};

struct WorldMapMapData {
    s16 mCameraIndex;
    u16 mNpcID;
    u32 mFlagID;
    u32 mItemID;
    u32 mModelObjectID;
    u32 mTextureID;
    u32 mModelID;
    u32 mModelResource;
    u32 mFloorID;
    u32 mScriptMessage;
};


extern s32 *worldMapRibbonCount;
extern WorldMapMapData *worldMapData;

class WorldMapModel {
public:
    WorldMapModel() { mOffscreenTexture = 0; }
    ~WorldMapModel() { if (mOffscreenTexture) releaseOffscreenData(); }
    void releaseOffscreenData();
    void renderOffscreenData(f32 param);
    void initializeOffscreenData();
    WorldMapArchiveHeader* SetModel(unsigned long model, unsigned long resource, bool visible);
    void UnloadModel(unsigned long model, unsigned long resource);
    void ObjectVisibility(unsigned long model, bool visible);
    void Visibility(unsigned long model, bool visible);
    void ModelDetach(unsigned long model);
    bool ModelAttach(unsigned long model, unsigned long part, long mode, GSattachMode attachMode);
    unsigned long SetModelPartPaticle(unsigned long model, unsigned long part, long mode, GSattachMode attachMode, bool f1, bool f2, bool f3);
    void SetMotion(unsigned long motion, short startFrame, f32 rate, bool loop);
    bool CheckMotion(unsigned long model);
    void StopMotion(unsigned long model);
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
    void Init();
    void InitCamera(long cursor);
    void SetCamera();
    void ControlCamera(long cursor);
    bool Blendf(f32* output, f32 from, f32 to, f32 blend);
    void DrawIcon(long index, tagWINDOW_WORK* window);
    void SetInfo(long index, tagWINDOW_WORK* window, tagSPRITE_WORK* sprite);
    u16 GetNpcID(long index);
    u32 GetInfoMsgID(long index);
    s32 GetInfoLevel();
    u32 SearchFloorID(unsigned short index);
    s32 SearchDataFloorID(unsigned int floor);
    void ControlCursor(long cursor);
    void CheckIconAnime(long cursor);
    void CheckCursorAnime();
    void SetIconMotion(long icon,unsigned long model,short motion,f32 frame,bool loop);

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
    s32 GetModelID(long index);
    void SetCursorPosition(long index, bool flag);
    void InitModel2D(f32 param);
    void InitModel(long cursor);
    void InitCursor(long cursor, short animation);
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
    u8 mPadding16[2];
    s32 mCursorIndex;
    u8 mCursorMoving;

    u8 mPadding1D[3];
    GSvec mCursorPosition;
    GSvec mCursorTarget;
    f32 mCursorBlend;
    f32 mCursorBob;
    f32 mCursorTime;
    WorldMapCamWork* mCamWork;
};

void* _cameraPlayAnime(unsigned long cameraRes, long startFrame, bool loop);

extern "C" void worldMapInitDummy(WorldMap* map);
extern "C" u8 getWorldMapScriptFlag(WorldMap* map);

#endif
