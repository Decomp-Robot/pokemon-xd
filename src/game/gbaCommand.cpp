#include <gbaCommand.h>

// Clear the probe error counters; chan -1 clears every channel.
void gba_SIProbe_ErrorCntClear(s32 chan)
{
    if (chan == -1) {
        SIProbeErrorCnt[0] = SIProbeErrorCnt[1] = SIProbeErrorCnt[2] = SIProbeErrorCnt[3] = 0;
    } else {
        SIProbeErrorCnt[chan] = 0;
    }
}
