#ifndef _DOLPHIN_GBA_H_
#define _DOLPHIN_GBA_H_

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Channels the link library drives: one Game Boy Advance per SI channel.
#define GBA_MAX_CHAN 4

// Result a link call reports. A call that started a transfer reports 0, and so
// does a blocking call whose transfer ran through; 1 reports that the transfer ran
// into an error (the channel was not answered by a Game Boy Advance), and 2
// reports that the channel is taken by another transfer, which the caller has to
// wait out.
#define GBA_RESULT_OK 0
#define GBA_RESULT_ERROR 1
#define GBA_RESULT_BUSY 2

// Callback of an asynchronous link call: it is handed the channel and the result
// of the transfer it was started for.
typedef void (*GBACallback)(s32 chan, s32 result);

// Bring up the link library. The transfers themselves are started by the
// asynchronous calls below, which is what the game's link layer drives.
void GBAInit(void);

// Start one exchange with a channel and report its code, or run it through and
// report whether it went through.
s32 GBAGetStatusAsync(s32 chan, u8* status, GBACallback callback);
BOOL GBAGetStatus(s32 chan, u8* status);

// Exchange the word or packet the caller hands over: the read copies the packet
// the GBA answers with into dst, the write sends the word src points at out.
s32 GBAReadAsync(s32 chan, void* dst, u8* status, GBACallback callback);
BOOL GBARead(s32 chan, void* dst, u8* status);

s32 GBAWriteAsync(s32 chan, void* src, u8* status, GBACallback callback);
BOOL GBAWrite(s32 chan, void* src, u8* status);

// Bring the Game Boy Advance back to the state the link library hands its code
// to it in.
s32 GBAResetAsync(s32 chan, u8* status, GBACallback callback);
BOOL GBAReset(s32 chan, u8* status);

#ifdef __cplusplus
}
#endif

#endif // _DOLPHIN_GBA_H_
