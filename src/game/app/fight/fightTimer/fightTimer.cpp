#include <app/fight/fightTimer/fightTimer.h>
#include <GSgfx.h>
#include <GSthread.h>
#include <floorManager.h>
#include <menuCBRule.h>

// A time limit is given in seconds while the elapsed time accumulates in
// 60 Hz render frames.
static const f32 sTimerZero[] = { 0.0f };
static const f32 sTimerFramesPerSecond[] = { 60.0f };

// One countdown: the completion and disable flags, the elapsed and limit times
// in seconds, and the worker that accumulates the elapsed time.
struct FightTimer {
    u8 mIsOver;
    u8 mIsDisable;
    u8 mUnknown[2];
    f32 mElapsed;
    f32 mLimit;
    GSthread *mThread;
};

FightTimer gCommandTimer;
FightTimer gBattleTimer;

extern "C" {
void fightTimerCommandTerminate(void);
f32 fightTimerCommandGetNokoriTime(void);
u8 fightTimerCommandBlock(void);
void fightTimerCommandStart(void);
void fightTimerCommandInit(void);
u8 fightTimerAllIsOver(void);
f32 fightTimerAllGetNokoriTime(void);
u8 fightTimerAllBlock(void);
void fightTimerAllStart(void);
void fightTimerAllInit(void);
void fightTimerAllTerminate(void);

u8 fightTimerIsOver(FightTimer *timer);
f32 fightTimerGetNokoriTime(FightTimer *timer);
u8 fightTimerBlock(FightTimer *timer);
void fightTimerStart(FightTimer *timer);
void fightTimerTerminate(FightTimer *timer);
void fightTimerInit(FightTimer *timer, f32 limit);
void fightTimerThreadFunc(FightTimer *timer);
}

void fightTimerCommandTerminate(void)
{
    fightTimerTerminate(&gCommandTimer);
}

u8 fightTimerCommandIsOver(void)
{
    return fightTimerIsOver(&gCommandTimer);
}

f32 fightTimerCommandGetNokoriTime(void)
{
    return fightTimerGetNokoriTime(&gCommandTimer);
}

u8 fightTimerCommandBlock(void)
{
    return fightTimerBlock(&gCommandTimer);
}

void fightTimerCommandStart(void)
{
    fightTimerStart(&gCommandTimer);
}

void fightTimerCommandInit(void)
{
    fightTimerInit(&gCommandTimer, (f32)menuCBRule_GetCommandTimeLimit());
}

u8 fightTimerAllIsOver(void)
{
    return fightTimerIsOver(&gBattleTimer);
}

f32 fightTimerAllGetNokoriTime(void)
{
    return fightTimerGetNokoriTime(&gBattleTimer);
}

u8 fightTimerAllBlock(void)
{
    return fightTimerBlock(&gBattleTimer);
}

void fightTimerAllStart(void)
{
    fightTimerStart(&gBattleTimer);
}

void fightTimerAllInit(void)
{
    fightTimerInit(&gBattleTimer, (f32)menuCBRule_GetBattleTimeLimit());
}

void fightTimerAllTerminate(void)
{
    fightTimerTerminate(&gBattleTimer);
}

u8 fightTimerIsOver(FightTimer *timer)
{
    if (timer->mIsDisable == 1) {
        return 0;
    }
    if (timer->mThread == NULL) {
        return 1;
    }
    return timer->mIsOver;
}

f32 fightTimerGetNokoriTime(FightTimer *timer)
{
    if (timer->mIsDisable == 1) {
        return -3600.0f;
    }
    if (timer->mThread == NULL) {
        return sTimerZero[0];
    }
    return timer->mLimit - timer->mElapsed / GSgfxVideoGetVsyncRate();
}

u8 fightTimerBlock(FightTimer *timer)
{
    if (timer->mIsDisable == 1) {
        return 0;
    }
    if (timer->mThread == NULL) {
        return 0;
    }
    GSthreadBlock(timer->mThread);
    return 1;
}

void fightTimerStart(FightTimer *timer)
{
    GSthread *thread;

    if (timer->mIsDisable == 1) {
        return;
    }
    thread = GSthreadCreate(1, GSfloorGetCurrentFloorGroupID(), 0x4000, 1, 0,
                            (GSthreadEntry)fightTimerThreadFunc);
    if (thread != NULL) {
        timer->mThread = thread;
        GSthreadSetArgs(thread, 1, timer);
    }
}

void fightTimerTerminate(FightTimer *timer)
{
    if (timer->mThread != NULL) {
        GSthreadTerminate(timer->mThread);
    }
    timer->mIsOver = 0;
    timer->mElapsed = sTimerZero[0];
    timer->mLimit = sTimerZero[0];
    timer->mThread = NULL;
    timer->mIsDisable = 0;
}

void fightTimerInit(FightTimer *timer, f32 limit)
{
    if (timer->mThread != NULL) {
        fightTimerTerminate(timer);
    }
    timer->mIsOver = 0;
    timer->mElapsed = sTimerZero[0];
    if (limit < sTimerZero[0]) {
        timer->mIsDisable = 1;
        timer->mLimit = -3600.0f;
    }
    else {
        timer->mIsDisable = 0;
        timer->mLimit = limit;
    }
    timer->mThread = NULL;
}

void fightTimerThreadFunc(FightTimer *timer)
{
    timer->mElapsed = sTimerZero[0];
    while (timer->mElapsed < sTimerFramesPerSecond[0] * timer->mLimit) {
        GSthreadSwitch();
        timer->mElapsed += GSgfxVideoGetLastRenderTime();
    }
    timer->mIsOver = 1;
    while (1) {
        GSthreadSwitch();
    }
}
