// The read exchange sits on the SI hardware as well, so the SDK headers that
// describe the transfers and the threads behind them come first.
#include <dolphin/os.h>
#include <dolphin/si.h>

#include <string.h>

#include <dolphin/gba.h>

#include "__gba.h"

/*
 * The read exchange of the link library.
 *
 * GBAReadAsync hands the channel a read transfer: a single command byte goes
 * out, and the GBA answers with a four-byte packet followed by the link status
 * byte. ReadProc
 * copies that packet into the buffer of the caller and reports the status.
 *
 * GBARead is the blocking form of the same transfer: it takes the channel, waits
 * for the packet on the thread queue of the channel record and reports the result
 * of the transfer.
 */

// Read the packet the GBA answered with into the buffer of the caller.
void ReadProc(s32 chan) {
    GBAControl* gba = &__GBA[chan];

    if (gba->result == GBA_RESULT_OK) {
        memcpy(gba->buffer, gba->input, 4);
        *gba->status = gba->input[4] & GBA_LINK_STATUS_MASK;
    }
}

// Start the read exchange of a channel.
s32 GBAReadAsync(s32 chan, void* dst, u8* status, GBACallback callback) {
    GBAControl* gba = &__GBA[chan];
    s32 result;

    if (gba->callback != NULL) {
        result = GBA_RESULT_BUSY;
    } else {
        gba->command = GBA_COMMAND_READ;
        gba->buffer = dst;
        gba->status = status;
        gba->callback = callback;
        result = __GBATransfer(chan, GBA_COMMAND_BYTES, GBA_READ_BYTES, ReadProc);
    }
    return result;
}

// Run the read exchange of a channel and wait for its packet.
BOOL GBARead(s32 chan, void* dst, u8* status) {
    s32 result = GBAReadAsync(chan, dst, status, __GBASyncCallback);

    switch (result) {
    case GBA_RESULT_OK:
        return __GBASync(chan);
    default:
        return result;
    }
}
