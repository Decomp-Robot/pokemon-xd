#ifndef TIME_INLINE_H
#define TIME_INLINE_H

#include <time.h>
#include <pause.h>
#include <GSgfx.h>

extern "C" {
// Length of the last rendered frame in seconds. Reports 0 while the game is
// paused so that animations and timers do not advance.
inline f32 timeGetLastFrameSec(void)
{
    if (pauseIsPause()) {
        return 0.0f;
    }
    return GSgfxVideoGetLastRenderTime() / (f32)GSgfxVideoGetVsyncRate();
}
}

#endif
