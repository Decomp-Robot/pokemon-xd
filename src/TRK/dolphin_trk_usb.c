#include "nub.h"
#include "../libc/uart_console_io_gcn.h"

#include <libc/stdio.h>


enum {
    kTRKFileWrite = 208,
    kTRKFileRead = 209,
    kTRKFileClose = 211
};

enum {
    kConsoleInputHandle = 0,
    kConsoleOutputHandle = 1
};

int __close_console(int handle) {
    u8 result;

    if (GetTRKConnected() == FALSE) {
        return 1;
    }

    result = (u8)TRKCloseFile(kTRKFileClose, handle);
    switch (result) {
    case 0:
        return 0;
    case 2:
        return 2;
    default:
        return 1;
    }
}

int __TRK_write_console(int handle, char* buffer, unsigned long* count,
                        MWIdleProc idle) {
    u32 length;
    u32 result;

    if (GetUseSerialIO() == 0) {
        return 1;
    }
    if (GetTRKConnected() == FALSE) {
        return 1;
    }

    length = (u32)*count;
    result = TRKAccessFile(kTRKFileWrite, kConsoleOutputHandle, &length,
                           (u8*)buffer);
    *count = length;

    switch (result & 0xff) {
    case 0:
        return 0;
    case 2:
        return 2;
    default:
        return 1;
    }
}

int __read_console(int handle, char* buffer, unsigned long* count,
                   MWIdleProc idle) {
    u32 length;
    u32 result;

    if (GetUseSerialIO() == 0) {
        return 1;
    }
    if (GetTRKConnected() == FALSE) {
        return 1;
    }

    length = (u32)*count;
    result = TRKAccessFile(kTRKFileRead, kConsoleInputHandle, &length,
                           (u8*)buffer);
    *count = length;

    switch (result & 0xff) {
    case 0:
        return 0;
    case 2:
        return 2;
    default:
        return 1;
    }
}

__declspec(weak) s32 udp_cc_post_stop(void) {
    return -1;
}

__declspec(weak) s32 udp_cc_pre_continue(void) {
    return -1;
}

__declspec(weak) s32 udp_cc_peek(void) {
    return 0;
}

__declspec(weak) s32 udp_cc_write(void* data, s32 length) {
    return 0;
}

__declspec(weak) s32 udp_cc_read(void* data, s32 length) {
    return 0;
}

__declspec(weak) s32 udp_cc_close(void) {
    return -1;
}

__declspec(weak) s32 udp_cc_open(void) {
    return -1;
}

__declspec(weak) s32 udp_cc_shutdown(void) {
    return -1;
}

__declspec(weak) s32 udp_cc_initialize(volatile u8** inputPendingPtrRef,
                                       TRKCommCallback callback) {
    return -1;
}

