#include "nub.h"
#include "CircleBuffer.h"
#include <dolphin/amc/AmcExi2Comm.h>

extern u8 ddhReceiveStorage[2048];
extern CircleBuffer ddhReceiveBuffer;
extern s32 ddhInitialized;

enum {
    kCCNotInitialized = -10001,
    kCCAlreadyInitialized = -10005,
    kCCLinkReadFailed = -10009
};

s32 ddh_cc_initinterrupts(void) {
    EXI2_EnableInterrupts();
    return 0;
}

s32 ddh_cc_peek(void) {
    u8 buf[2048];
    s32 polled;
    s32 err;

    polled = EXI2_Poll();
    if (polled <= 0) {
        return 0;
    }
    err = EXI2_ReadN(buf, polled);
    if (err == 0) {
        CircleBufferWriteBytes(&ddhReceiveBuffer, buf, polled);
    } else {
        return kCCLinkReadFailed;
    }
    return polled;
}

s32 ddh_cc_post_stop(void) {
    EXI2_Reserve();
    return 0;
}

s32 ddh_cc_pre_continue(void) {
    EXI2_Unreserve();
    return 0;
}

s32 ddh_cc_write(void* data, s32 length) {
    s32 remaining = length;
    u8* packet = (u8*)data;
    s32 sent;

    if (ddhInitialized == 0) {
        MWTRACE(TRK_TRACE_LEVEL_ERROR, "cc not initialized\n");
        return kCCNotInitialized;
    }
    MWTRACE(TRK_TRACE_LEVEL_ERROR, "cc_write : Output data 0x%08x %ld bytes\n", data, length);

    while (remaining > 0) {
        MWTRACE(TRK_TRACE_LEVEL_DETAIL, "cc_write sending %ld bytes\n", remaining);
        sent = EXI2_WriteN(packet, remaining);
        if (sent == 0) {
            break;
        }
        packet += sent;
        remaining -= sent;
    }
    return 0;
}

s32 ddh_cc_read(void* data, s32 length) {
    u8 buf[2048];
    u32 err = 0;
    s32 polled;

    if (ddhInitialized == 0) {
        return kCCNotInitialized;
    }
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "Expected packet size : 0x%08x (%ld)\n", length, length);

    while (CircleBufferGetBytesAvailable(&ddhReceiveBuffer) < length) {
        err = 0;
        polled = EXI2_Poll();
        if (polled == 0) {
            continue;
        }
        err = EXI2_ReadN(buf, polled);
        if (err != 0) {
            continue;
        }
        CircleBufferWriteBytes(&ddhReceiveBuffer, buf, polled);
    }

    if (err == 0) {
        CircleBufferReadBytes(&ddhReceiveBuffer, (u8*)data, length);
    } else {
        MWTRACE(TRK_TRACE_LEVEL_ERROR, "cc_read : error reading bytes from EXI2 %ld\n", err);
    }
    return err;
}

s32 ddh_cc_close(void) {
    return 0;
}

s32 ddh_cc_open(void) {
    if (ddhInitialized != 0) {
        return kCCAlreadyInitialized;
    }
    ddhInitialized = 1;
    return 0;
}

s32 ddh_cc_shutdown(void) {
    return 0;
}

s32 ddh_cc_initialize(volatile u8** inputPendingPtrRef,
                      TRKCommCallback callback) {
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "CALLING EXI2_Init\n");
    EXI2_Init(inputPendingPtrRef, callback);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "DONE CALLING EXI2_Init\n");
    CircleBufferInitialize(&ddhReceiveBuffer, ddhReceiveStorage, 2048);
    return 0;
}

