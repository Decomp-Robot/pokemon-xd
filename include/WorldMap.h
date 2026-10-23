#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <hero.h>
#include <types.h>
#include <winSprite.h>

class glowEffect;

/* 3D/screen position vector (GSvec). */
struct GSvec {
    f32 x;
    f32 y;
    f32 z;
};

/* Partial view of the WorldMap work block addressed by the pointer at 0x08.
 * The block is 0x94 bytes and holds the per-map model state: the selected
 * cursor, the map/particle model ids the move demo drives, the resource
 * handles the move demo keeps alive and the screen-space icon state. */
struct WorldMapWork {
    /* 0x00: set once the map model is ready; Cursor, Main, readScooter and
     * MainSub2 only run their body while this is clear. */
    u8 _00;
    u8 _01[0x2B];                // 0x01-0x2B: unknown.
    /* 0x2C: index the map menu edits (MainSub hands it to menuOpenCustom and
     * restores it into the selected index slot). */
    s32 _2C;
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
    /* 0x44: animation frame count handed back by _cameraPlayAnime. */
    s32 mAnimeFrames;
    /* 0x48: set while the map model is still loading. */
    u8 _48;
    u8 _49[0x03];                // 0x49-0x4B: unknown.
    /* 0x4C: map model id of the map the scooter is read for. */
    u32 mMapModelID;
    /* 0x50/0x54/0x58/0x5C: model ids used by the move demo (the map model
     * plus its three sub models) - they differ between the demo map and the
     * regular maps. */
    u32 _50;
    u32 _54;
    u32 _58;
    u32 _5C;
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
    /* 0x70: model id the scooter motion is played on. */
    u32 _70;
    /* 0x74: scooter table entry the map currently uses. */
    void* mScooter;
    /* 0x78: resource groups of the move-demo sub models; the matching
     * particle generator handles sit at 0x84. */
    u32 _78[0x03];
    u32 _84[0x03];
    /* 0x90: set when the move demo changed the map; byte only. */
    u8 _90;
    u8 _91[0x03];                // 0x91-0x93: unknown.
};

/* Per-ribbon emit parameters (8 bytes): the distance to the ribbon and the
 * angle it is seen under, both relative to the icon that owns the array.
 * WorldMapIconWork's constructor builds the arrays with __construct_new_array
 * and passes __ct__11_EmitParamsFv as the element constructor, so the element
 * type is a class with a non-trivial constructor: that is what makes MWCC
 * subtract the 0x10 array header at the delete[] in the destructor. */
struct _EmitParams {
    _EmitParams();               // defined in WorldMapIcon.cpp, external here
    f32 dist;                    // 0x00
    f32 angle;                   // 0x04
};

/* Icon work entry (0x2C bytes). SelectIcon and TargetDebug address the
 * camera position at +0x08, moveDemo the screen position at +0x1C and
 * CheckPadIndex the per-ribbon distance array at +0x28. */
struct WorldMapIconWork {
    u8 _00[0x08];                // 0x00-0x07: unknown.
    GSvec pos;                   // 0x08: camera target of this icon.
    /* 0x14: walked-direction flags, one byte per direction index. */
    s8 _14[0x08];
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
    /* 0x00: debug target slot; TargetDebug shifts this to +0x04 and
     * reloads it from the debug table. */
    s32 _00;
    /* 0x04: previous debug target slot. */
    s32 _04;
    /* 0x08: camera blend parameter; 0 = camera at the old position,
     * 1 = fully blended to the new one. */
    f32 blend;
    /* 0x0C: rate feeding the per-frame increment; ControlCamera adds
     * rate * dt into +0x10 every frame. */
    f32 _0C;
    /* 0x10: per-frame blend increment added into the blend parameter. */
    f32 _10;
    /* 0x14: elapsed camera-move time; ControlCamera accumulates dt here. */
    f32 _14;
    /* 0x18: current camera target position. */
    GSvec curPos;
    /* 0x24/0x28/0x2C: current blend range (start/end/current). */
    f32 _24;
    f32 _28;
    f32 _2C;
    /* 0x30/0x34/0x38/0x3C: current distance and angle parameters. */
    f32 _30;
    f32 _34;
    f32 _38;
    f32 _3C;
    /* 0x40: pending camera target position (ControlCamera blends to it). */
    GSvec nextPos;
    /* 0x4C/0x50/0x54: pending blend range (start/end/current). */
    f32 _4C;
    f32 _50;
    f32 _54;
    /* 0x58/0x5C/0x60/0x64: pending distance and angle parameters. */
    f32 _58;
    f32 _5C;
    f32 _60;
    f32 _64;
};

/* Move-demo animation table entry (0x10 bytes). readScooter reads the model
 * id at +0x0A; moveDemo indexes a table of them. */
struct WorldMapAnimeRecord {
    /* 0x00: model resource setNullModel loads for this map. */
    u32 _00;
    /* 0x04: model id setNullModel puts in the work block (SetLayerID and
     * GSresGetResourceAllGroup use it). */
    u32 _04;
    u8 _08[0x02];                // 0x08-0x09: unknown.
    /* 0x0A: model part readScooter attaches the scooter model to. */
    s16 _0A;
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
    u32 _08;
    u32 _0C;
    u32 _10;
};

/* Map data entry returned by SearchData; the move demo reads the model id at
 * +0x14 and MainSub releases the object at +0x1C. */
struct WorldMapMapData {
    u8 _00[0x14];
    u32 mModelID;
    u8 _18[0x04];
    void* mExtra;
    u8 _20[0x04];
};

/* Attachment modes of WorldMapModel::ModelAttach. */
enum GSattachMode {
    GS_ATTACH_MODE_0 = 0,
    GS_ATTACH_MODE_4 = 4,
    GS_ATTACH_MODE_7 = 7,
};

/* Layer id used by WorldMapModel::SetLayerID; the two layer ids the map
 * hands over (lbl_804EFC90/94) are 4-byte objects. */
class GSgfxLayerID {
public:
    u32 mID;                     // 0x00: 4-byte layer id handle.
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
    u8 _10;                      // 0x10-0x14: setup bytes written by moveDemo.
    u8 _11;
    u8 _12;
    u8 _13;
    u8 _14;
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
     * `new WorldMapIconWork[lbl_804E87D8->?]` (0x2C bytes per entry); the
     * 0x10-byte block header __construct_new_array installs in front of the
     * entries is where the per-icon distance arrays start (see
     * WorldMapIconWork::mDist). InitModel, SelectIcon and SetCursorPosition
     * index the entries. */
    WorldMapIconWork* mIcons;
    /* 0x10..0x1B: unrecovered map state (cursor position, camera targets). */
    u8 _10[0x0C];
    /* 0x1C: menu mode; Button and ControlPad only forward pad input while
     * this does not read 1. */
    u8 _1C;
    /* 0x1D..0x43: unrecovered map state. */
    u8 _1D[0x27];
    /* 0x44: Camera work block (0x68 bytes). Init allocates it and zeroes
     * the blend fields; IsBlending reads the blend parameter at +0x08 and
     * ControlCamera drives it toward the targets set by SetCameraNextWork
     * (see WorldMapCamWork). */
    WorldMapCamWork* mCamWork;
};

/* Plays the camera animation of the map (the frame count is returned). */
long _cameraPlayAnime(unsigned long model, long loop, bool flag);

/* Map setup helpers shared with the WorldMap constructor unit. */
extern "C" void zz_0010558_(WorldMap* map);
extern "C" u8 zz_0011408_(WorldMap* map);

#endif
