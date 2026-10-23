#include <GSthread.h>
#include <WorldMap.h>
#include <floor.h>
#include <camera.h>
#include <hsdRandom.h>

static const unsigned long kMoveDemoSceneResource = 0x1B4A0400;
static const unsigned long kMoveDemoAnimatedModelResource = 0x1B4A1000;

WorldMapMoveDemo* pMoveDemo;
WorldMapMoveDemoAnime _animeTable[0x0F] = {
    { 0, 0, 0x1B491800 },
    { 1, 0, 0x1B781800 },
    { 2, 0, 0x1B791800 },
    { 3, 0, 0x1B7A1800 },
    { 4, 0, 0x1B7B1800 },
    { 5, 0, 0x1B7C1800 },
    { 6, 0, 0x1B7D1800 },
    { 7, 0, 0x1B7E1800 },
    { 8, 0, 0x1B7F1800 },
    { 9, 0, 0x1B801800 },
    { 10, 0, 0x1B811800 },
    { 11, 0, 0x1B821800 },
    { 12, 0, 0x1B831800 },
    { 13, 0, 0x1B841800 },
    { 14, 0, 0x1B851800 },
};

/* Menu entry points. */
#pragma force_active on

/* Release the move demo and clear the singleton. */
extern "C" void menuWorldMapMoveDemoExit(void)
{
    WorldMapMoveDemo* const demo = pMoveDemo;
    if (demo != 0) {
        delete demo;
        pMoveDemo = 0;
    }
}

/* Drive the move demo while its singleton exists. */
extern "C" void menuWorldMapMoveDemoMain(void)
{
    WorldMapMoveDemo* demo = pMoveDemo;
    if (demo != 0) {
        demo->Main();
    }
}

/* Hand out a fresh demo object; its texture handle starts cleared. */
extern "C" void menuWorldMapMoveDemoInit(void)
{
    pMoveDemo = new WorldMapMoveDemo();
}

#pragma force_active reset

/* Load the demo floor and play a random animation with its camera. */
void WorldMapMoveDemo::Main(void)
{
    unsigned long group = floorGetCurrentGroupID();
    SetModel(group, kMoveDemoSceneResource, false);
    u16 variant = (u16)HSD_Rand() % 0x0F;
    WorldMapMoveDemoAnime* anime = &_animeTable[variant];
    cameraPlayAnime(group, anime->cameraResourceId, 0, 0.0f);
    SetMotion(kMoveDemoAnimatedModelResource, anime->animationIndex, 0.0f, false);
    f32 count = GetAnimeFrameCount(kMoveDemoAnimatedModelResource, anime->animationIndex);
    for (;;) {
        f32 endFrame = count - 30.0f;
        if (endFrame < GetAnimeFrame(kMoveDemoAnimatedModelResource)) {
            break;
        }
        GSthreadSwitch();
    }
    GSfloorPop();
}
