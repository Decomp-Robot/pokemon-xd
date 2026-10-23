// The write exchange sits on the SI hardware as well, so the SDK headers that
// describe the transfers and the threads behind them come first.
#include <dolphin/os.h>
#include <dolphin/si.h>

#include <string.h>

#include <dolphin/gba.h>

#include "__gba.h"

/*
 * The write exchange of the link library.
 *
 * GBAWriteAsync hands the channel a write transfer: the command byte and the word
 * of the caller go out as one block, and the GBA answers with the status byte of
 * the word it took. WriteProc reports that status to the caller.
 *
 * GBAWrite is the blocking form of the same transfer: it takes the channel, waits
 * for the answer on the thread queue of the channel record and reports the result
 * of the transfer.
 */

// Report the status byte the GBA answered the word with.
void WriteProc(s32 chan) {
    GBAControl* gba = &__GBA[chan];

    if (gba->result == GBA_RESULT_OK) {
        *gba->status = gba->input[0] & GBA_LINK_STATUS_MASK;
    }
}

// Start the write exchange of a channel.
s32 GBAWriteAsync(s32 chan, void* src, u8* status, GBACallback callback) {
    GBAControl* gba = &__GBA[chan];
    s32 result;

    if (gba->callback != NULL) {
        result = GBA_RESULT_BUSY;
    } else {
        gba->command = GBA_COMMAND_WRITE;
        memcpy(gba->output, src, 4);
        gba->buffer = src;
        gba->status = status;
        gba->callback = callback;
        result = __GBATransfer(chan, GBA_WRITE_BYTES, GBA_COMMAND_BYTES, WriteProc);
    }
    return result;
}

// Run the write exchange of a channel and wait for its answer.
BOOL GBAWrite(s32 chan, void* src, u8* status) {
    s32 result = GBAWriteAsync(chan, src, status, __GBASyncCallback);

    switch (result) {
    case GBA_RESULT_OK:
        return __GBASync(chan);
    default:
        return result;
    }
}
