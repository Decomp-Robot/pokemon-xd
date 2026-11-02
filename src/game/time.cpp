#include <GSthread.h>

#include "timeInline.h"

extern "C" {

// Blocks until the given number of seconds has elapsed, yielding to the
// scheduler once per frame.
void timeWait(f32 seconds)
{
    f32 elapsed = 0.0f;

    while (elapsed < seconds) {
        GSthreadSwitch();
        elapsed += timeGetLastFrameSec();
    }
}

// Number of vsync periods the last rendered frame took, rounded to the nearest
// whole frame. Reports 0 while the game is paused so that animations and timers
// do not advance.
u32 timeGetLastFrameTime(void)
{
    if (pauseIsPause()) {
        return 0;
    }
    return (u32)(GSgfxVideoGetLastRenderTime() + 0.5f);
}
}
