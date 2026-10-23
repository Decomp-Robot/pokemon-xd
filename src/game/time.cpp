#include <time.h>
#include <pause.h>

#pragma peephole off

// Half a timer tick, added before the render time is truncated to a count.
extern const f32 renderTimeRoundingBias;

extern "C" {
f32 GSgfxVideoGetLastRenderTime(void);
u8 GSgfxVideoGetVsyncRate(void);
}

// Last frame render time in timer units; frozen at 0 while paused.
u32 timeGetLastFrameTime(void)
{
    u32 paused = pauseCheck();

    if (paused != 0) {
        return 0;
    }
    return (u32)(GSgfxVideoGetLastRenderTime() + renderTimeRoundingBias);
}

// Last frame render time in seconds; frozen at 0.0f while paused.
f32 timeGetLastFrameSec(void)
{
    u32 paused = pauseCheck();

    if (paused != 0) {
        return 0.0f;
    }
    return GSgfxVideoGetLastRenderTime() / (f32)GSgfxVideoGetVsyncRate();
}

#pragma peephole reset
