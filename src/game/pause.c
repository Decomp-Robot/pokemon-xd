#include <pause.h>

#pragma peephole off

// Pause state; the rest of the game reads it through pauseCheck. MWCC lays
// .sbss out in reverse declaration order, so the declarations below run back
// to front over the block pauseState, pauseInitialized, pauseBgmID, pauseAppID.
s32 pauseAppID;
u32 pauseBgmID;
u8 pauseInitialized;
u8 pauseState;

// Helpers the pause module drives; they live in the GS sound, movie and
// graphics libraries and in the game app module.
extern void GSgfxRenderUnpause(void);
extern void GSsndPopVolumeALL(s32 a, s32 b, s32 c);
extern void zz_010bda8_(void);
extern void GSsndContinue(s32 bgmID, s32 volume, s32 unk);
extern void GSmovieAllContinue(void);
extern void zz_02ab388_(void);
extern void GSsndPushVolumeALL(s32 a, s32 b, s32 c, s32 d);
extern void zz_010bdb4_(void);
extern s32 soundGetBGMID(void);
extern void GSsndPause(s32 bgmID, s32 volume, s32 unk);
extern void GSmovieAllPause(void);
extern s32 GSgappCreate(s32 app, s32 a, s32 b, void (*callback)(void));

// Pause flag query; game systems call this to skip their updates.
u8 pauseCheck(void)
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
    zz_010bda8_();
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
    zz_02ab388_();
    GSsndPushVolumeALL(0, 0, 0, 1);
    zz_010bdb4_();
    pauseBgmID = soundGetBGMID();
    if (pauseBgmID != 0xFFFFFFFF) {
        GSsndPause(pauseBgmID, 50, 0);
    }
    GSmovieAllPause();
}

// GSgapp callback for the pause app; intentionally empty.
void pauseAppCallback(void)
{
}

void pauseInit(void)
{
    pauseState = 0;
    pauseInitialized = 1;
    pauseAppID = GSgappCreate(1, 0, 0, pauseAppCallback);
}

#pragma peephole reset
