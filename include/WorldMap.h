#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <GSgfx.h>
#include <hero.h>
#include <types.h>
#include <winSprite.h>

class glowEffect;

/* 3D/screen position vector (GSvec) comes from GSgfx.h. */

/* Partial view of the WorldMap work block addressed by the pointer at 0x08.
 * The block is 0x94 bytes and holds the per-map model state: the selected
 * cursor, the map/particle model ids the move demo drives, the resource
 * handles the move demo keeps alive and the screen-space icon state. */
struct WorldMapWork {
    /* 0x00: set while the world map was opened by a menu script (Init
     * copies getWorldMapScriptFlag() into it). Main takes the scripted
     * MainSub2 path while this is set; Cursor, readScooter and Release only
     * do their free-roam handling while it is clear. */
    u8 mScriptFlag;
    u8 _01[0x2B];                // 0x01-0x2B: unknown.
    /* 0x2C: index the map menu edits (MainSub hands it to menuOpenCustom and
     * restores it into the selected index slot). */
    s32 mMenuIndex;
    /* 0x30: selected cursor slot (GetNowCursor, ControlPad). */
    s32 cursor;
    /* 0x34: map resource object released by MainSub. */
    void* mMapResource;
    u8 _38[0x04];                // 0x38-0x3B: unknown.
    /* 0x3C: offscreen render alpha; moveDemo fades it out once the demo
     * animation is over. */
    f32 mOffscreenAlpha;
    /* 0x40: animation table entry the move demo picked for this map. */
    void* mAnime;
    /* 0x44: camera resource the move demo's camera animation plays on;
     * _cameraPlayAnime hands it back and RenderMoveDemoModel sets it on the
     * demo models' layers. */
    void* mAnimeCamera;
    /* 0x48: set while the map model is still loading. */
    u8 mModelLoading;
    u8 _49[0x03];                // 0x49-0x4B: unknown.
    /* 0x4C: map model id of the map the scooter is read for. */
    u32 mMapModelID;
    /* 0x50/0x54/0x58/0x5C: model ids used by the move demo (the map model
     * plus its three sub models) - they differ between the demo map and the
     * regular maps. */
    u32 mDemoModel0;
    u32 mDemoModel1;
    u32 mDemoModel2;
    u32 mDemoModel3;
    /* 0x60: scooter type index, used to pick the loaded scooter entry and
     * the recovery sound. */
    s32 mScooterIndex;
    /* 0x64: scooter motion resource handed to GSresFreeResourceAllGroup. */
    u32 mScooterMotion;
    /* 0x68: scooter model id detached again in Release. */
    u32 mScooterModel;
    /* 0x6C: model resource of the map setNullModel restores (the value it
     * hands to SetModel). */
    u32 mModelRes;
    /* 0x70: model id of the move-demo map; the scooter motion is played on
     * it and RenderMoveDemoModel puts it on its own layer. */
    u32 mDemoMapModel;
    /* 0x74: scooter table entry the map currently uses. */
    void* mScooter;
    /* 0x78: resource groups of the move-demo sub models; the matching
     * particle generator handles sit at 0x84. */
    u32 mParticleRes[0x03];
    u32 mParticleGen[0x03];
    /* 0x90: set when the move demo changed the map; byte only. */
    u8 mMapChanged;
    u8 _91[0x03];                // 0x91-0x93: unknown.
};

/* _EmitParams record (8 bytes). The map attests the class (its ctor is in
 * the map module's model code, outside the recovered units; the pe particle
 * node Start functions take it as their last argument). The map module uses
 * it as the per-ribbon link record: the distance to the neighbouring ribbon
 * and the angle it is seen under, relative to the icon that owns the array.
 * The user-provided constructor copies the TU's shared 0.0f literal into
 * both fields, so MWCC allocates its arrays through __construct_new_array
 * with a 0x10-byte header that the delete[] in ~WorldMapIconWork strips
 * again. */
struct _EmitParams {
    _EmitParams();
    f32 dist;                    // 0x00
    f32 angle;                   // 0x04
};

/* Icon work entry (0x2C bytes). SelectIcon and TargetDebug address the
 * camera position at +0x08, moveDemo the screen position at +0x1C and
 * CheckPadIndex the per-ribbon distance array at +0x28. */
struct WorldMapIconWork {
    u8 _00[0x08];                // 0x00-0x07: unknown.
    GSvec pos;                   // 0x08: camera target of this icon.
    /* 0x14: neighbour icon index per pad direction (-1 = none); filled by
     * CheckPadIndex, read by ControlPad. */
    s8 mNext[0x08];
    /* 0x1C: screen position; CalcModel2D keeps z at 0 so the distance and
     * angle between two icons can be taken over this vector. */
    GSvec screen;
    /* 0x28: one _EmitParams per ribbon, allocated with new[] in the
     * constructor (0x10 array header in front of the entries). */
    _EmitParams* mDist;
    ~WorldMapIconWork();
};

/* 3x4 matrix (48 bytes) as the GS matrix helpers take it. */
struct GSmtx {
    f32 m00, m01, m02, m03;
    f32 m10, m11, m12, m13;
    f32 m20, m21, m22, m23;
};

/* Camera data table entry (0x1C bytes). GetCameraData copies the position
 * plus the four camera parameters staged by SetCameraNextWork. */
struct WorldMapCamData {
    GSvec pos;
    f32 distance;
    f32 rotY;
    f32 rotX;
    f32 fov;
};

/* Camera-blend work block (0x68 bytes, allocated by Init). IsBlending only
 * reads the blend parameter at +0x08, which ControlCamera drives from 0 to
 * 1 (its per-frame increment +0x10 is clamped so the blend never passes 1);
 * SetCameraWork targets the current slot at +0x18 while SetCameraNextWork
 * targets the pending slot at +0x40. */
struct WorldMapCamWork {
    /* 0x00: debug target slot; TargetDebug stages the picked icon here and
     * keeps the previous pick at +0x04. */
    s32 mDebugTarget;
    /* 0x04: previous debug target slot. */
    s32 mPrevDebugTarget;
    /* 0x08: camera blend parameter; 0 = camera at the old position,
     * 1 = fully blended to the new one. */
    f32 blend;
    /* 0x0C: rate feeding the per-frame increment; ControlCamera adds
     * mAccel * dt into mSpeed every frame. */
    f32 mAccel;
    /* 0x10: per-frame blend increment added into the blend parameter. */
    f32 mSpeed;
    /* 0x14: elapsed camera-move time; ControlCamera accumulates dt here. */
    f32 mElapsed;
    /* 0x18: current camera target position. */
    GSvec curPos;
    /* 0x24/0x28/0x2C: current blend range (start/end/current). */
    f32 curBlendStart;
    f32 curBlendEnd;
    f32 curBlend;
    /* 0x30/0x34/0x38/0x3C: current distance and angle parameters. */
    f32 curDist;
    f32 curRotY;
    f32 curRotX;
    f32 curFov;
    /* 0x40: pending camera target position (ControlCamera blends to it). */
    GSvec nextPos;
    /* 0x4C/0x50/0x54: pending blend range (start/end/current). */
    f32 nextBlendStart;
    f32 nextBlendEnd;
    f32 nextBlend;
    /* 0x58/0x5C/0x60/0x64: pending distance and angle parameters. */
    f32 nextDist;
    f32 nextRotY;
    f32 nextRotX;
    f32 nextFov;
};

/* Move-demo animation table entry (0x10 bytes). readScooter reads the model
 * id at +0x0A; moveDemo indexes a table of them. */
struct WorldMapAnimeRecord {
    /* 0x00: model resource setNullModel loads for this map. */
    u32 mModelRes;
    /* 0x04: model id setNullModel puts in the work block (SetLayerID and
     * GSresGetResourceAllGroup use it). */
    u32 mModelID;
    u8 _08[0x02];                // 0x08-0x09: unknown.
    /* 0x0A: model part readScooter attaches the scooter model to. */
    s16 mPart;
    /* 0x0C: camera animation the move demo plays for this entry. */
    u32 mCamera;
};

/* Scooter table entry (0x14 bytes, indexed by WorldMapWork::mScooterIndex).
 * readScooter loads the model resource at +0x00 and the motion/resource
 * group at +0x04. */
struct WorldMapScooterData {
    u32 mModelRes;
    u32 mMotionRes;
    /* 0x08/0x0C/0x10: the three scooter parts readScooter attaches. */
    u32 mPart0;
    u32 mPart1;
    u32 mPart2;
};

/* Map data entry returned by SearchData; the move demo reads the model id at
 * +0x14 and MainSub releases the object at +0x1C. */
struct WorldMapMapData {
    u8 _00[0x04];                // 0x00-0x03: unknown.
    /* 0x04: save flag the map keeps its state in, 0 while it has none.
     * SearchDataFloorID reads it (and skips maps without one),
     * _getWorldMapFlagData/_setWorldMapFlagData store the map's state in it. */
    u32 mFlagID;
    u8 _08[0x0C];                // 0x08-0x13: unknown.
    u32 mModelID;
    u8 _18[0x04];
    void* mExtra;
    u8 _20[0x04];
};

/* The map list the world map loads: the number of maps the map is made of at
 * +0. getRibbonCnt hands it out and Init sizes the per-map works with it; no
 * other field of the block is read by the recovered units. */
struct WorldMapMapList {
    u32 mMapCnt;                 // 0x00: number of maps in the world map.
};

/* The map list and the map records it counts, one WorldMapMapData per map.
 * Both are set by the code loading the world map data and are shared by the
 * map module, the PDA's map search and the debug menu's world map entries. */
extern WorldMapMapList* worldMapMapList;
extern WorldMapMapData* worldMapMapData;

/* Attachment modes of WorldMapModel::ModelAttach. */
enum GSattachMode {
    GS_ATTACH_MODE_0 = 0,
    GS_ATTACH_MODE_4 = 4,
    GS_ATTACH_MODE_7 = 7,
};

/* Model container the map renders its offscreen texture with. WorldMap
 * derives from it, so its member functions are reachable unqualified. */
class WorldMapModel {
public:
    void releaseOffscreenData();
    void renderOffscreenData(f32 param);
    void initializeOffscreenData();
    void SetModel(unsigned long model, unsigned long resource, bool visible);
    void UnloadModel(unsigned long model, unsigned long resource);
    void ObjectVisibility(unsigned long model, bool visible);
    void ModelDetach(unsigned long model);
    void ModelAttach(unsigned long model, unsigned long part, long mode, GSattachMode attachMode);
    unsigned long SetModelPartPaticle(unsigned long model, unsigned long part, long mode, GSattachMode attachMode, bool f1, bool f2, bool f3);
    void SetMotion(unsigned long motion, short startFrame, f32 rate, bool loop);
    void SetLayerID(unsigned long model, long part, GSgfxLayerID* layer);
    f32 GetAnimeFrameCount(unsigned long motion, unsigned long a);
    f32 GetAnimeFrame(unsigned long motion);

protected:
    /* 0x00: offscreen texture handle; cleared by the constructor and
     * non-null while the offscreen pass is set up. */
    u32 mOffscreenTexture;
};

/* Effect base class. The vtable pointer sits behind the eight bytes of base
 * state, and the five virtuals declared before the destructor occupy the
 * first five slots of its table. */
class GSeffectBase {
protected:
    /* 0x00/0x01: started/successful flags maintained by Start. */
    u8 mActive;
    u8 mVisible;
    u8 _02[0x02];                // 0x02-0x03: unknown.
    /* 0x04: next entry of the global effect list. */
    GSeffectBase* mNext;
};

class GSeffect : public GSeffectBase {
public:
    GSeffect();
    virtual int _start();
    virtual void _stop();
    virtual int _isActive();
    virtual void _reset();
    virtual void _dummy();
    virtual ~GSeffect();
    void Start();
    void Stop();
};

/* Glow effect the move demo allocates (0x18 bytes). */
class glowEffect : public GSeffect {
public:
    glowEffect();
    virtual ~glowEffect();

public:
    /* 0x0C: intensity; RenderMoveDemoModel boosts it for the demo model. */
    f32 mIntensity;
    /* 0x10-0x14: setup bytes written by moveDemo (worldMapGlowParam0-4). */
    u8 mParam0;
    u8 mParam1;
    u8 mParam2;
    u8 mParam3;
    u8 mParam4;
};

/* Partial WorldMap declaration (0x48 bytes; menuWorldMapMXInit allocates
 * 0x48 and runs the constructor over it).
 *
 * The constructor only initializes the base texture handle and the two work
 * pointers; the icon array at 0x0C, the cursor state and the camera work at
 * 0x44 are set up by Init and the other member functions, so only the
 * members those functions have exposed so far are declared here. */
class WorldMap : public WorldMapModel {
public:
    WorldMap();

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
    s32 GetStartIconAnimIndex(unsigned long index);
    /* True once the camera blend started by SetCameraNextWork has fully
     * faded in (blend parameter reached 1). */
    bool IsBlending();
    f32 GetAngle(long work);
    s32 GetModelID(long index);
    void SetCursorPosition(long index, bool flag);
    void InitModel2D(f32 param);
    void SetCameraSpeed(f32 speed);
    void TargetDebug(long index);
    void SetCameraNextWork(GSvec* pos, f32 distance, f32 rotY, f32 rotX, f32 fov);
    void SetCameraWork(GSvec* pos, f32 distance, f32 rotY, f32 rotX, f32 fov);
    void GetCameraData(long index, GSvec* pos, f32* distance, f32* rotY, f32* rotX, f32* fov);
    WorldMapMapData* SearchData(long index);
    void SelectIcon(long index);
    void MainControl(long index);
    void SetFlagData(long index);
    /* 0x00: 0 while the ribbon has no flag data, 2 while it has none at all. */
    u32 GetFlagData(long index);
    s8 GetPadIndex3(f32 angle, long index, f32 width);

private:
    /* 0x04: Move-demo effect. moveDemo allocates a glowEffect here;
     * RenderMoveDemoModel uses it, and MainSub invokes it
     * and releases it back to NULL. */
    glowEffect* mDemoEffect;
    /* 0x08: Model work block (0x94 bytes). Init allocates and clears it;
     * Release frees it and nulls the pointer. GetNowCursor reads the
     * selected cursor at +0x30 of this same block (see WorldMapWork). */
    WorldMapWork* mModelWork;
    /* 0x0C: Icon works. Init allocates the array with
     * `new WorldMapIconWork[getRibbonCnt(this)]` (0x2C bytes per entry); the
     * 0x10-byte block header __construct_new_array installs in front of the
     * entries is where the per-icon distance arrays start (see
     * WorldMapIconWork::mDist). InitModel, SelectIcon and SetCursorPosition
     * index the entries. */
    WorldMapIconWork* mIcons;
    /* 0x10..0x1B: unrecovered map state (cursor position, camera targets). */
    u8 _10[0x0C];
    /* 0x1C: menu mode; Button and ControlPad only forward pad input while
     * this does not read 1. */
    u8 mMenuMode;
    /* 0x1D..0x43: unrecovered map state. */
    u8 _1D[0x27];
    /* 0x44: Camera work block (0x68 bytes). Init allocates it and zeroes
     * the blend fields; IsBlending reads the blend parameter at +0x08 and
     * ControlCamera drives it toward the targets set by SetCameraNextWork
     * (see WorldMapCamWork). */
    WorldMapCamWork* mCamWork;
};

/* Loads the camera resource, plays its animation from startFrame (looping
 * when loop is set) and hands the camera back for the move-demo render. */
void* _cameraPlayAnime(unsigned long cameraRes, long startFrame, bool loop);

/* Map setup helpers shared with the WorldMap constructor unit. */
extern "C" void worldMapInitDummy(WorldMap* map);
extern "C" u8 getWorldMapScriptFlag(WorldMap* map);

#endif
