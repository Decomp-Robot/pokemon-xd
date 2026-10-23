#ifndef _DOLPHIN_GBA_INTERNAL_H_
#define _DOLPHIN_GBA_INTERNAL_H_

#include <dolphin/gba.h>
#include <dolphin/os.h>

#ifdef __cplusplus
extern "C" {
#endif

// Bytes of one exchange with a channel. The link library writes the command of
// the exchange into the channel and lets the SI hardware send it out together
// with the word behind it; the answer of the GBA lands behind the command again,
// one byte for the status exchanges and the packet of a transfer for the reads.
#define GBA_COMMAND_BYTES 1
#define GBA_WRITE_BYTES 5
#define GBA_STATUS_BYTES 3
#define GBA_READ_BYTES 5

// Commands the exchanges start with, which the GBA mirrors in its answer.
#define GBA_COMMAND_STATUS 0x00
#define GBA_COMMAND_READ 0x14
#define GBA_COMMAND_WRITE 0x15
#define GBA_COMMAND_RESET 0xFF

// Bits of the link status byte the GBA reports for a read: the low two bits are
// the transfer mode, the busy bit is set while the GBA has not taken the word.
#define GBA_LINK_STATUS_MASK 0x3A

// Error nibble of the SI status word SITransfer reports back for the channel:
// the four low bits carry the error the transfer ran into, 0 when it went
// through.
#define GBA_ERR_MASK 0x0000000F

// The status exchange answers with the type word of the channel: its low byte is
// clear and the word itself carries the code of a Game Boy Advance.
#define GBA_TYPE_STATUS_BYTE 0x00
#define GBA_TYPE_MASK 0xFFFF0000
#define GBA_TYPE_ANSWER 0x00040000

// One SI channel of the link library. The record holds the exchange that runs on
// the channel, the buffers the SI hardware reads and writes, and the state the
// GBA answered the last exchange with.
typedef struct GBAControl {
    /* 0x00 */ u8 command;    // command byte the exchange sends out first
    /* 0x01 */ u8 output[4];  // word the exchange sends behind the command
    /* 0x05 */ u8 input[5];   // bytes the GBA answers the exchange with
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ u32 outputBytes;   // bytes the exchange sends out
    /* 0x10 */ u32 inputBytes;    // bytes the exchange reads
    /* 0x14 */ u8* status;    // link status byte the exchange reports
    /* 0x18 */ u8* buffer;    // buffer the exchange reads the answer into
    /* 0x1C */ GBACallback callback; // callback of the running transfer, 0 idle
    /* 0x20 */ s32 result;    // result of the running transfer, 0 while it runs
    /* 0x24 */ OSThreadQueue threadQueue; // threads waiting for the transfer
    /* 0x2C */ u8 _2C[0x04];
    /* 0x30 */ OSTime delay;  // delay the transfer waits before the exchange
    /* 0x38 */ void (*proc)(s32); // exchange that runs when the type is in
    /* 0x3C */ u8 _3C[0xBC];
    /* 0xF8 */ u32* secParam;    // security parameters of the channel
} GBAControl;

// Security parameters of one channel, handed to the key exchange of the GBA.
typedef struct GBASecParam {
    u32 param[0x10];
} GBASecParam;

extern GBAControl __GBA[GBA_MAX_CHAN];
extern GBASecParam SecParams[GBA_MAX_CHAN];

// Set while the link library owes the hardware its initialization: a reset of
// the console sets it, GBAInit takes it down again, and the exchanges of the
// channels stand still while it is set.
extern s32 __GBAReset;

// Hands the processor back after the transfer callback wakes the thread
// waiting on the channel.
void __OSReschedule(void);

void ShortCommandProc(s32 chan);
void ReadProc(s32 chan);
void WriteProc(s32 chan);
void TypeAndStatusCallback(s32 chan, u32 type);
void __GBAHandler(s32 chan, u32 error, OSContext* context);
void __GBASyncCallback(s32 chan, s32 result);
s32 __GBASync(s32 chan);
s32 __GBATransfer(s32 chan, u32 outputBytes, u32 inputBytes, void (*proc)(s32));

#ifdef __cplusplus
}
#endif

#endif // _DOLPHIN_GBA_INTERNAL_H_
