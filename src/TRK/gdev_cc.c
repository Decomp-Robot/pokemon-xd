#include "nub.h"
#include "CircleBuffer.h"
#include <dolphin/db/DBInterface.h>

extern u8 gdevReceiveStorage[1280];
extern CircleBuffer gdevReceiveBuffer;
extern s32 gdevInitialized;

enum {
    kCCNotInitialized = -10001,
    kCCAlreadyInitialized = -10005,
    kCCLinkReadFailed = -10009
};

s32 gdev_cc_initinterrupts(void) {
    DBInitInterrupts();
    return 0;
}

s32 gdev_cc_peek(void) {
    u8 buf[1280];
    s32 polled;
    s32 err;

    polled = DBQueryData();
    if (polled <= 0) {
        return 0;
    }
    err = DBRead(buf, polled);
    if (err == 0) {
        CircleBufferWriteBytes(&gdevReceiveBuffer, buf, polled);
    } else {
        return kCCLinkReadFailed;
    }
    return polled;
}

s32 gdev_cc_post_stop(void) {
    DBClose();
    return 0;
}

s32 gdev_cc_pre_continue(void) {
    DBOpen();
    return 0;
}

s32 gdev_cc_write(void* data, s32 length) {
    s32 remaining = length;
    u8* packet = (u8*)data;
    s32 sent;

    if (gdevInitialized == 0) {
        MWTRACE(TRK_TRACE_LEVEL_ERROR, "cc not initialized\n");
        return kCCNotInitialized;
    }
    MWTRACE(TRK_TRACE_LEVEL_ERROR, "cc_write : Output data 0x%08x %ld bytes\n", data, length);

    while (remaining > 0) {
        MWTRACE(TRK_TRACE_LEVEL_DETAIL, "cc_write sending %ld bytes\n", remaining);
        sent = DBWrite(packet, remaining);
        if (sent == 0) {
            break;
        }
        packet += sent;
        remaining -= sent;
    }
    return 0;
}

s32 gdev_cc_read(void* data, s32 length) {
    u8 buf[1280];
    u32 err = 0;
    s32 polled;

    if (gdevInitialized == 0) {
        return kCCNotInitialized;
    }
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "Expected packet size : 0x%08x (%ld)\n", length, length);

    while (CircleBufferGetBytesAvailable(&gdevReceiveBuffer) < length) {
        err = 0;
        polled = DBQueryData();
        if (polled == 0) {
            continue;
        }
        err = DBRead(buf, length);
        if (err != 0) {
            continue;
        }
        CircleBufferWriteBytes(&gdevReceiveBuffer, buf, polled);
    }

    if (err == 0) {
        CircleBufferReadBytes(&gdevReceiveBuffer, (u8*)data, length);
    } else {
        MWTRACE(TRK_TRACE_LEVEL_ERROR, "cc_read : error reading bytes from EXI2 %ld\n", err);
    }
    return err;
}

s32 gdev_cc_close(void) {
    return 0;
}

s32 gdev_cc_open(void) {
    if (gdevInitialized != 0) {
        return kCCAlreadyInitialized;
    }
    gdevInitialized = 1;
    return 0;
}

s32 gdev_cc_shutdown(void) {
    return 0;
}

s32 gdev_cc_initialize(volatile u8** inputPendingPtrRef,
                       TRKCommCallback callback) {
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "CALLING EXI2_Init\n");
    DBInitComm((u8**)inputPendingPtrRef, callback);
    MWTRACE(TRK_TRACE_LEVEL_DETAIL, "DONE CALLING EXI2_Init\n");
    CircleBufferInitialize(&gdevReceiveBuffer, gdevReceiveStorage, 1280);
    return 0;
}

