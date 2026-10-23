#include "timeInline.h"

extern "C" {
void GSthreadSwitch(void);

void timeWait(f32 seconds)
{
    f32 elapsed = 0.0f;

    while (elapsed < seconds) {
        GSthreadSwitch();
        elapsed += timeGetLastFrameSec();
    }
}

u32 timeGetLastFrameTime(void)
{
    u32 paused = pauseIsPause();

    if (paused != 0) {
        return 0;
    }
    return (u32)(GSgfxVideoGetLastRenderTime() + 0.5f);
}
}
