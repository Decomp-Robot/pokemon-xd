#ifndef TIME_INLINE_H
#define TIME_INLINE_H

#include <time.h>
#include <pause.h>

extern "C" {
f32 GSgfxVideoGetLastRenderTime(void);
u8 GSgfxVideoGetVsyncRate(void);

inline f32 timeGetLastFrameSec(void)
{
    u32 paused = pauseIsPause();

    if (paused != 0) {
        return 0.0f;
    }
    return GSgfxVideoGetLastRenderTime() / (f32)GSgfxVideoGetVsyncRate();
}
}

#endif
