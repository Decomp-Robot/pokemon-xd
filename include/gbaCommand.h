#ifndef GBA_COMMAND_H
#define GBA_COMMAND_H

#include <types.h>

// Per-channel SI probe error counters (sbss 0x804EA768), one byte per
// channel.
extern u8 SIProbeErrorCnt[4];

// Command the key layer runs on a channel: gbaKeyState holds it and
// gbaCommandKeyRead answers it with the matching link call.
enum {
    GBA_KEY_COMMAND_NONE = 0,     // no command; the channel is not answered.
    GBA_KEY_COMMAND_READ = 1,     // reads the key word the GBA answers with.
    GBA_KEY_COMMAND_TRANSFER = 2, // a transfer owns the channel, no key is read.
    GBA_KEY_COMMAND_IDLE = 3,     // sends the idle word and reads its answer.
};

#ifdef __cplusplus
extern "C" {
#endif

void gba_SIProbe_ErrorCntClear(s32 chan);

// Makes the channel run the given key command and clears the state the
// previous one left; returns the command that was running before.
long gbaCommandSetKeyState(long chan, long state);

// Whether the port carries a controller that answers the link commands and
// has answered the last one.
u8 gbaCommandIsConnected(s32 port);

#ifdef __cplusplus
}
#endif

#endif
