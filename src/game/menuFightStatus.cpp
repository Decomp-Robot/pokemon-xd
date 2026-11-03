#include <menuFightStatus.h>
#include <time.h>

// Elapsed fight time shown on the status screen and the frame time the last
// update saw; shared with the rest of the status module.
extern f32 menuStatusTime;
extern f32 menuStatusOldTime;

// Accumulates the elapsed fight time shown on the status screen.
void menuUpdateStatusTime(void)
{
    f32 time = timeGetLastFrameSec();
    if (time != menuStatusOldTime) {
        menuStatusOldTime = time;
        menuStatusTime += time;
    }
}
