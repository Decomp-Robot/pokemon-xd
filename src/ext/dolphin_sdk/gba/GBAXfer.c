// The transfers of the link library are driven from the SI interrupt, so the SDK
// headers that describe the transfers, the context the interrupt runs in and the
// threads that wait for them come first.
#include <dolphin/os.h>
#include <dolphin/si.h>

#include <dolphin/gba.h>

#include "__gba.h"

/*
 * The transfer layer of the link library.
 *
 * A call of the library that starts an exchange (GBAGetStatusAsync, GBAReadAsync,
 * GBAWriteAsync, GBAResetAsync) first hands the channel to __GBATransfer, which
 * looks up the type of the channel with SIGetTypeAsync. The exchange only starts
 * once the channel is identified as a Game Boy Advance.
 *
 * TypeAndStatusCallback runs the exchange the caller set up: it starts the SI
 * transfer of the command and hands the channel to __GBAHandler, which reports
 * the result of the transfer to the proc of the exchange and to the callback of
 * the caller. The blocking calls wait for that callback on the thread queue of
 * the channel record, which the callback of the caller wakes up.
 */

// Report the result of the transfer of the channel to the caller.
void __GBAHandler(s32 chan, u32 error, OSContext* context) {
    GBAControl* gba = &__GBA[chan];

    if (__GBAReset != 0) {
        return;
    }

    if ((error & GBA_ERR_MASK) != 0) {
        gba->result = GBA_RESULT_ERROR;
    } else {
        gba->result = GBA_RESULT_OK;
    }

    if (gba->proc != NULL) {
        void (*proc)(s32) = gba->proc;

        gba->proc = NULL;
        proc(chan);
    }

    if (gba->callback != NULL) {
        OSContext callbackContext;
        GBACallback callback;

        OSClearContext(&callbackContext);
        OSSetCurrentContext(&callbackContext);
        callback = gba->callback;
        gba->callback = NULL;
        callback(chan, gba->result);
        OSClearContext(&callbackContext);
        OSSetCurrentContext(context);
    }
}

// Wake the threads that wait for the transfer of the channel.
void __GBASyncCallback(s32 chan, s32 result) {
    OSWakeupThread(&__GBA[chan].threadQueue);
}

// Wait for the transfer of the channel and report its result.
s32 __GBASync(s32 chan) {
    GBAControl* gba = &__GBA[chan];
    BOOL enabled;
    s32 result;

    enabled = OSDisableInterrupts();

    while (gba->callback != NULL) {
        OSSleepThread(&gba->threadQueue);
    }

    result = gba->result;
    OSRestoreInterrupts(enabled);
    return result;
}

// Run the exchange of the channel once the type of the channel is in.
void TypeAndStatusCallback(s32 chan, u32 type) {
    GBAControl* gba = &__GBA[chan];

    if (__GBAReset != 0) {
        return;
    }

    if ((type & 0xFF) != GBA_TYPE_STATUS_BYTE ||
        (type & GBA_TYPE_MASK) != GBA_TYPE_ANSWER) {
        gba->result = GBA_RESULT_ERROR;
    } else {
        if (SITransfer(chan, gba, gba->outputBytes, gba->input, gba->inputBytes,
                       __GBAHandler, gba->delay) != 0) {
            return;
        }
        gba->result = GBA_RESULT_BUSY;
    }

    if (gba->proc != NULL) {
        void (*proc)(s32) = gba->proc;

        gba->proc = NULL;
        proc(chan);
    }

    if (gba->callback != NULL) {
        OSContext* context = OSGetCurrentContext();
        OSContext callbackContext;
        GBACallback callback;

        OSClearContext(&callbackContext);
        OSSetCurrentContext(&callbackContext);
        callback = gba->callback;
        gba->callback = NULL;
        callback(chan, gba->result);
        OSClearContext(&callbackContext);
        OSSetCurrentContext(context);
        __OSReschedule();
    }
}

// Look up the type of the channel and start the transfer of the exchange the
// caller set up.
s32 __GBATransfer(s32 chan, u32 outputBytes, u32 inputBytes, void (*proc)(s32)) {
    BOOL enabled;
    GBAControl* gba = &__GBA[chan];

    enabled = OSDisableInterrupts();
    gba->proc = proc;
    gba->outputBytes = outputBytes;
    gba->inputBytes = inputBytes;
    SIGetTypeAsync(chan, TypeAndStatusCallback);
    OSRestoreInterrupts(enabled);
    return GBA_RESULT_OK;
}
