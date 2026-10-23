// The link library sits on the SI hardware, so the SDK's OS and SI headers come
// first: they bring in the timers, threads and SI transfers the library drives,
// and the scalar types and the public link API on top of them.
#include <dolphin/dsp.h>
#include <dolphin/os.h>
#include <dolphin/si.h>

#include <dolphin/gba.h>

#include "__gba.h"

/*
 * The link library of the GBA connection.
 *
 * A library session with one Game Boy Advance runs as a sequence of exchanges on
 * an SI channel. Every exchange sends one command byte out (GBA_COMMAND_*) and
 * reads the answer of the GBA back: the three status commands answer with the
 * type word of the channel, a read answers with the packet it holds and a write
 * answers with the status byte of the packet it was handed.
 *
 * Because the SI hardware only starts one transfer at a time, the library keeps
 * one record per channel (__GBA). A transfer claims the channel by storing its
 * callback in the record, and the record reports the transfer as done again once
 * the callback has run.
 *
 * The exchanges are built in three layers:
 *
 *   GBAGetStatusAsync, GBAReadAsync, GBAWriteAsync and GBAResetAsync  hand the
 *       channel to a transfer: they fill the record of the channel in with the
 *       buffers of the exchange and start it.
 *   __GBATransfer  looks up the type of the channel first, so that the transfer
 *       only runs when a Game Boy Advance answers on it.
 *   TypeAndStatusCallback and __GBAHandler  run the SI transfer and hand the
 *       channel back to the caller's callback.
 *
 * The blocking forms of the calls (GBAGetStatus, GBARead, GBAWrite, GBAReset)
 * wait on the thread queue of the record until the callback has run.
 */

// Set while the link library owes the hardware its initialization.
s32 __GBAReset;

// The reset function of the library: after a reset the channels are not
// initialized, and the channel exchanges stand still until GBAInit has run
// again. The SDK runs the reset functions it was given a priority for; this
// library registers itself at priority 127 like the other game libraries.
static BOOL OnReset(BOOL final);

static OSResetFunctionInfo ResetFunctionInfo = {OnReset, 127};

// The status exchange: the GBA answers the command of the channel with its type
// word, whose low bytes carry the link status of the channel. A command that
// came back with anything but a Game Boy Advance, or one that was answered while
// the GBA is not ready, is reported as an error.
void ShortCommandProc(s32 chan) {
    GBAControl* gba = &__GBA[chan];

    if (gba->result == GBA_RESULT_OK) {
        if ((gba->input[0] != GBA_TYPE_STATUS_BYTE) ||
            (gba->input[1] != (GBA_TYPE_ANSWER >> 16))) {
            gba->result = GBA_RESULT_ERROR;
        } else {
            *gba->status = gba->input[2] & GBA_LINK_STATUS_MASK;
        }
    }
}

// Bring up the link library: the delay every exchange waits for the GBA to
// answer with, and the record of every channel.
void GBAInit(void) {
    s32 i;
    GBAControl* gba;

    for (i = 0; i < GBA_MAX_CHAN; i++) {
        gba = &__GBA[i];
        gba->delay = OSMicrosecondsToTicks(60);
        OSInitThreadQueue(&gba->threadQueue);
        gba->secParam = SecParams[i].param;
    }
    OSInitAlarm();
    DSPInit();
    __GBAReset = FALSE;
    OSRegisterResetFunction(&ResetFunctionInfo);
}

// Start the status exchange of a channel.
s32 GBAGetStatusAsync(s32 chan, u8* status, GBACallback callback) {
    GBAControl* gba = &__GBA[chan];
    s32 result;

    if (gba->callback != NULL) {
        result = GBA_RESULT_BUSY;
    } else {
        gba->command = GBA_COMMAND_STATUS;
        gba->status = status;
        gba->callback = callback;
        result = __GBATransfer(chan, GBA_COMMAND_BYTES, GBA_STATUS_BYTES, ShortCommandProc);
    }
    return result;
}

// Run the status exchange of a channel and wait for its answer.
BOOL GBAGetStatus(s32 chan, u8* status) {
    s32 result = GBAGetStatusAsync(chan, status, __GBASyncCallback);

    switch (result) {
    case GBA_RESULT_OK:
        return __GBASync(chan);
    default:
        return result;
    }
}

// Start the reset exchange of a channel, which brings the Game Boy Advance back
// to the state the link library hands its code to it in.
s32 GBAResetAsync(s32 chan, u8* status, GBACallback callback) {
    GBAControl* gba = &__GBA[chan];
    s32 result;

    if (gba->callback != NULL) {
        result = GBA_RESULT_BUSY;
    } else {
        gba->command = GBA_COMMAND_RESET;
        gba->status = status;
        gba->callback = callback;
        result = __GBATransfer(chan, GBA_COMMAND_BYTES, GBA_STATUS_BYTES, ShortCommandProc);
    }
    return result;
}

// Run the reset exchange of a channel and wait for its answer.
BOOL GBAReset(s32 chan, u8* status) {
    s32 result = GBAResetAsync(chan, status, __GBASyncCallback);

    switch (result) {
    case GBA_RESULT_OK:
        return __GBASync(chan);
    default:
        return result;
    }
}

static BOOL OnReset(BOOL final) {
    __GBAReset = TRUE;
    return TRUE;
}

// The records of the four SI channels.
GBAControl __GBA[GBA_MAX_CHAN];

// The blocks of parameters the security exchange of every channel is handed:
// the key exchange of the GBA writes its keys into them. The game never runs
// that exchange, so the blocks stay unused.
GBASecParam SecParams[GBA_MAX_CHAN];
