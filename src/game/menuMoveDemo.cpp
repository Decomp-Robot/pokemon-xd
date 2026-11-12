#include <GSsnd.h>
#include <GSres.h>
#include <GSthread.h>
#include <GSflag.h>
#include <camera.h>
#include <app/camera/CameraType.h>
#include <fade.h>
#include <floor.h>
#include <glow.h>
#include <sound.h>
#include <libc/memory.h>
#include <WorldMap.h>
#include <floorManager.h>

/* Floor the move demo is played on. */
#define FLOOR_ID_MOVE_DEMO 937

/* Sound played while the move demo is running. */
#define SE_ID_MOVE_DEMO 1464

enum MoveDemoType {
    MOVE_DEMO_DEPARTURE = 0,
    MOVE_DEMO_RETURN = 1
};

enum MoveDemoResource {
    KYOGRE_MODEL_RESOURCE = 0x1B870400,
    KYOGRE_MODEL = 0x1B871000,
    RETURN_MAP_RESOURCE = 0x21200400,
    RETURN_MAP_MODEL = 0x21201000,
    RETURN_CAMERA = 0x211E1800,
    DEPARTURE_MAP_RESOURCE = 0x21210400,
    DEPARTURE_MAP_MODEL = 0x21211000,
    DEPARTURE_CAMERA = 0x211F1800,
    KYOGRE_ATTACHED_MODEL_RESOURCE = 0x21AC0400,
    KYOGRE_ATTACHED_MODEL = 0x21AC1000,
    RETURN_PARTICLE_PART5 = 0x21391400,
    RETURN_PARTICLE_PART19 = 0x213A1400,
    COMMON_PARTICLE_PART26 = 0x213B1400,
    COMMON_PARTICLE_PART12 = 0x213C1400,
    DEPARTURE_PARTICLE_PART5 = 0x21DE1400,
    DEPARTURE_PARTICLE_PART19 = 0x21DF1400
};

enum {
    MOVE_DEMO_RETURN_MAP_INDEX = 7,
    MOVE_DEMO_DESTINATION_MAP_INDEX = 13,
    MOVE_DEMO_DESTINATION_VISITED = 2,
    FLOOR_ID_MOVE_DEMO_FIRST_ARRIVAL = 86,
    MOVE_DEMO_RETURN_LINK = 10,
    MOVE_DEMO_FADE_SCRIPT = 0x05960008,
    MOVE_DEMO_BGM = 1341,
    MOVE_DEMO_LOOP_MOTION = 8
};

struct MoveDemoWork {
    u8 _00[4];
    s32 type;
    u32 group;
    u32 modelResource;
    u32 model;
    u32 mapResource;
    u32 mapModel;
    s32 attachPart;
    u32 cameraResource;
    f32 frameCount;
    _GScamera* camera;
    _GSparticleGen* particles[4];
};

class kyogreMoveDemo : public WorldMapModel {
public:
    kyogreMoveDemo();
    ~kyogreMoveDemo();
    void Main();
    void Release();
    u32 GetNextFloorID();
    void Init();
    _GScamera* cameraPlay(unsigned long resource, long frame, bool loop);
    void playBGM(unsigned short sound);

private:
    glowEffect* mGlow;
    MoveDemoWork* mWork;
};

kyogreMoveDemo* pMDemo;
s32 _movdemo_type;

static bool moveDemoLoop = true;
static bool moveDemoWaitForEnd = true;
static f32 _glowstrngth = 1.0f;
static u8 moveDemoGlowRadius = 20;
static u8 moveDemoGlowAlphaBlend = 1;
static u8 moveDemoGlowAlphaPass = 1;
static u8 moveDemoGlowSteps = 1;
static f32 _fadeFrame = 40.0f;
static f32 _fadeTime = 0.7f;
static u8 moveDemoGlowRenderMode;

kyogreMoveDemo::~kyogreMoveDemo()
{
    Release();
}

kyogreMoveDemo::kyogreMoveDemo()
{
    Init();
}

void kyogreMoveDemo::Release()
{
    if (pMDemo) {
        if (mWork->model) {
            for (s32 i = 0; i < 4; i++) {
                if (mWork->particles[i])
                    DetachModelPartPaticle(mWork->particles[i], true);
            }
            ModelDetach(mWork->model);
            ModelDetach(KYOGRE_ATTACHED_MODEL);
        }
        if (mGlow)
            delete mGlow;
        if (mWork) {
            delete mWork;
            mWork = 0;
        }
    }
}

void kyogreMoveDemo::Main()
{
    if (pMDemo) {
        for (;;) {
            if (moveDemoWaitForEnd) {
                if (mWork->frameCount - _fadeFrame < GetAnimeFrame(mWork->mapModel))
                    break;
            }
            GSthreadSwitch();
        }
        fadeSet(FADE_OUT_BLACK, _fadeTime);
        do {
            GSthreadSwitch();
        } while (fadeCheck(0));
        u32 floor = GetNextFloorID();
        if (floor) {
            if (mWork->type == MOVE_DEMO_RETURN)
                floorLink(floor, MOVE_DEMO_RETURN_LINK);
            else
                floorLink(floor, 0);
            floorSetFadeScript(0, MOVE_DEMO_FADE_SCRIPT);
        }
    }
}

u32 kyogreMoveDemo::GetNextFloorID()
{
    u32 floor = 0;
    switch (mWork->type) {
    case MOVE_DEMO_RETURN:
        floor = worldmapXD_data[MOVE_DEMO_RETURN_MAP_INDEX].mFloorID;
        break;
    case MOVE_DEMO_DEPARTURE:
        u32 flag = worldmapXD_data[MOVE_DEMO_DESTINATION_MAP_INDEX].mFlagID;
        if (GSflagGet(flag) < MOVE_DEMO_DESTINATION_VISITED) {
            GSflagSet(flag, MOVE_DEMO_DESTINATION_VISITED);
            floor = FLOOR_ID_MOVE_DEMO_FIRST_ARRIVAL;
        } else {
            floor = worldmapXD_data[MOVE_DEMO_DESTINATION_MAP_INDEX].mFloorID;
        }
        break;
    }
    return floor;
}

void kyogreMoveDemo::Init()
{
    mWork = 0;
    mWork = new MoveDemoWork;
    memset(mWork, 0, sizeof(MoveDemoWork));
    mGlow = 0;
    mGlow = new glowEffect;
    mWork->type = _movdemo_type;
    soundStopEnv(1000);
    if (mWork->type == MOVE_DEMO_DEPARTURE)
        playBGM(MOVE_DEMO_BGM);
    GSsndPlayNow(SE_ID_MOVE_DEMO);
    mWork->modelResource = KYOGRE_MODEL_RESOURCE;
    mWork->model = KYOGRE_MODEL;
    mWork->group = floorGetCurrentGroupID();
    GSgfxVideoSetLockFrameRate(1);
    switch (mWork->type) {
    case MOVE_DEMO_RETURN:
        mWork->mapResource = RETURN_MAP_RESOURCE;
        mWork->mapModel = RETURN_MAP_MODEL;
        mWork->attachPart = 1;
        mWork->cameraResource = RETURN_CAMERA;
        break;
    case MOVE_DEMO_DEPARTURE:
        mWork->mapResource = DEPARTURE_MAP_RESOURCE;
        mWork->mapModel = DEPARTURE_MAP_MODEL;
        mWork->attachPart = 1;
        mWork->cameraResource = DEPARTURE_CAMERA;
        break;
    }
    if (mGlow) {
        mGlow->mIntensity = _glowstrngth;
        mGlow->mBlurRadius = moveDemoGlowRadius;
        mGlow->mAlphaBlend = moveDemoGlowAlphaBlend;
        mGlow->mUseAlphaPass = moveDemoGlowAlphaPass;
        mGlow->mRenderMode = moveDemoGlowRenderMode;
        mGlow->mBlurSteps = moveDemoGlowSteps;
        mGlow->Start();
    }
    SetModel(mWork->group, mWork->mapResource, false);
    GSmodelSetBoundCheck((_GSmodel*)GSresGetResourceAllGroup(mWork->mapModel), 0);
    SetModel(mWork->group, KYOGRE_ATTACHED_MODEL_RESOURCE, false);
    GSmodelSetBoundCheck((_GSmodel*)GSresGetResourceAllGroup(KYOGRE_ATTACHED_MODEL), 0);
    SetMotion(KYOGRE_ATTACHED_MODEL, MOVE_DEMO_LOOP_MOTION, 0.0f, true);
    SetModel(mWork->group, mWork->modelResource, false);
    GSmodelSetBoundCheck((_GSmodel*)GSresGetResourceAllGroup(mWork->model), 0);
    ModelAttach(mWork->model, mWork->mapModel, mWork->attachPart, GS_ATTACH_TRANSFORM);
    SetMotion(mWork->model, MOVE_DEMO_LOOP_MOTION, 0.0f, true);
    ModelAttach(KYOGRE_ATTACHED_MODEL, KYOGRE_MODEL, 1, GS_ATTACH_POSITION_ROTATION);
    switch (mWork->type) {
    case MOVE_DEMO_RETURN:
        mWork->particles[0] = SetModelPartPaticle(mWork->model, RETURN_PARTICLE_PART5, 5, GS_ATTACH_POSITION, false, true, false);
        mWork->particles[1] = SetModelPartPaticle(mWork->model, RETURN_PARTICLE_PART19, 19, GS_ATTACH_POSITION, false, true, false);
        mWork->particles[2] = SetModelPartPaticle(mWork->model, COMMON_PARTICLE_PART26, 26, GS_ATTACH_POSITION, false, true, false);
        mWork->particles[3] = SetModelPartPaticle(mWork->model, COMMON_PARTICLE_PART12, 12, GS_ATTACH_POSITION, false, true, false);
        break;
    case MOVE_DEMO_DEPARTURE:
        mWork->particles[0] = SetModelPartPaticle(mWork->model, DEPARTURE_PARTICLE_PART5, 5, GS_ATTACH_POSITION, false, true, false);
        mWork->particles[1] = SetModelPartPaticle(mWork->model, DEPARTURE_PARTICLE_PART19, 19, GS_ATTACH_POSITION, false, true, false);
        mWork->particles[2] = SetModelPartPaticle(mWork->model, COMMON_PARTICLE_PART26, 26, GS_ATTACH_POSITION, false, true, false);
        mWork->particles[3] = SetModelPartPaticle(mWork->model, COMMON_PARTICLE_PART12, 12, GS_ATTACH_POSITION, false, true, false);
        break;
    }
    SetMotion(mWork->mapModel, 0, 0.0f, moveDemoLoop);
    mWork->frameCount = GetAnimeFrameCount(mWork->mapModel, 0);
    mWork->camera = cameraPlay(mWork->cameraResource, 0, moveDemoLoop);
    f32 fov, aspect, nearPlane, farPlane;
    GScameraGetPerspective(mWork->camera, &fov, &aspect, &nearPlane, &farPlane);
    farPlane = 50000.0f;
    GScameraSetPerspective(mWork->camera, fov, aspect, nearPlane, farPlane);
}

_GScamera* kyogreMoveDemo::cameraPlay(unsigned long resource, long frame, bool loop)
{
    cameraSetType(CAMERA_ANIMATION);
    cameraPlayAnime(mWork->group, resource, frame, loop);
    return (_GScamera*)GSresGetResourceAllGroup(resource);
}

void kyogreMoveDemo::playBGM(unsigned short sound)
{
    soundOpenPlay(sound, 0, 127);
}

/* Move demo menu entry points. */
#pragma force_active on

/* Stop the demo sound, release the model and clear the singleton. */
extern "C" void menuMoveDemoExit(void)
{
    GSsndStopNow(SE_ID_MOVE_DEMO);
    delete pMDemo;
    pMDemo = 0;
}

/* Advance the move demo by one frame. */
extern "C" void menuMoveDemoMain(void)
{
    pMDemo->Main();
}

/* Create the move demo model. */
extern "C" void menuMoveDemoInit(void)
{
    pMDemo = new kyogreMoveDemo();
}

#pragma force_active reset

/* Remember the requested demo and change to the floor it is played on. */
extern "C" void menuMoveDemo(s32 demo)
{
    _movdemo_type = demo;
    floorChange(FLOOR_ID_MOVE_DEMO);
}
