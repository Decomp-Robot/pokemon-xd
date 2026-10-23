#include <pause.h>
#include <sound.h>

#pragma peephole off

u8 pauseState;
u8 pauseInitialized;
u32 pauseBgmID;
s32 pauseAppID;

// Helpers the pause module drives; they live in the GS sound, movie and
// graphics libraries and in the game app module.
extern "C" {
extern void GSgfxRenderUnpause(void);
extern void GSsndPopVolumeALL(s32 a, s32 b, s32 c);
extern void GSgappPauseOff(void);
extern void GSsndContinue(s32 bgmID, s32 volume, s32 unk);
extern void GSmovieAllContinue(void);
extern void GSgfxRenderPause(void);
extern void GSsndPushVolumeALL(s32 a, s32 b, s32 c, s32 d);
extern void GSgappPauseOn(void);
extern void GSsndPause(s32 bgmID, s32 volume, s32 unk);
extern void GSmovieAllPause(void);
extern s32 GSgappCreate(s32 app, s32 a, s32 b, void (*callback)(u32, u32));
}

// Pause flag query; game systems call this to skip their updates.
u8 pauseIsPause(void)
{
    return pauseState;
}

void pauseOff(void)
{
    if (pauseState == 0) {
        return;
    }
    pauseState = 0;
    GSgfxRenderUnpause();
    GSsndPopVolumeALL(0, 0, 1);
    GSgappPauseOff();
    if (pauseBgmID != 0xFFFFFFFF) {
        GSsndContinue(pauseBgmID, 50, 0);
    }
    GSmovieAllContinue();
}

void pauseOn(void)
{
    if (pauseState != 0) {
        return;
    }
    if (pauseInitialized == 0) {
        return;
    }
    pauseState = 1;
    GSgfxRenderPause();
    GSsndPushVolumeALL(0, 0, 0, 1);
    GSgappPauseOn();
    pauseBgmID = soundGetBGMID();
    if (pauseBgmID != 0xFFFFFFFF) {
        GSsndPause(pauseBgmID, 50, 0);
    }
    GSmovieAllPause();
}

// GSgapp callback for the pause app; intentionally empty.
void _pauseDaemon(u32 appID, u32 parameter)
{
#pragma unused(appID, parameter)
}

void pauseInit(void)
{
    pauseState = 0;
    pauseInitialized = 1;
    pauseAppID = GSgappCreate(1, 0, 0, _pauseDaemon);
}

#pragma peephole reset
