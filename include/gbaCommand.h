#ifndef GBA_COMMAND_H
#define GBA_COMMAND_H

#include <types.h>

// Per-channel SI probe error counters (sbss 0x804EA768), one byte per
// channel.
extern u8 SIProbeErrorCnt[4];

#ifdef __cplusplus
extern "C" {
#endif

void gba_SIProbe_ErrorCntClear(s32 chan);

#ifdef __cplusplus
}
#endif

#endif
