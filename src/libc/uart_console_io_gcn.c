#include <dolphin/exi.h>
#include <dolphin/os.h>
#include "uart_console_io_gcn.h"

/* Rate the serial port is opened at for console output. */
#define UART_CONSOLE_BAUD_RATE 57600

/*
 * Handle one console write.  Unless the machine has a TDEV console, the bytes
 * go out of the serial port, which is opened on the first write; a port that
 * will not open, or that refuses the bytes, is reported to the caller as a
 * failed write.  Whatever the console type, a write that was not reported as
 * a failure is then passed on to the TRK console.
 */
long __write_console(int handle, char *buffer, unsigned long *count, MWIdleProc idle) {
    /* Set once the serial port has been opened; later writes skip it. */
    static int initialized;
    int err;

    if ((OSGetConsoleType() & OS_CONSOLE_TDEV) == 0) {
        err = 0;
        if (initialized == 0) {
            err = InitializeUART(UART_CONSOLE_BAUD_RATE);
            if (err == 0) {
                initialized = 1;
            }
        }
        if (err != 0) {
            return 1;
        }
        if (WriteUARTN(buffer, *count) != 0) {
            *count = 0;
            return 1;
        }
    }
    __TRK_write_console(handle, buffer, count, idle);
    return 0;
}
