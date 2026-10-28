#ifndef MSL_UART_CONSOLE_IO_GCN_H
#define MSL_UART_CONSOLE_IO_GCN_H

#include <libc/stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

long __write_console(int handle, char *buffer, unsigned long *count, MWIdleProc idle);

/* Console hook provided by the MetroTRK nub. */
int __TRK_write_console(int handle, char *buffer, unsigned long *count, MWIdleProc idle);

#ifdef __cplusplus
}
#endif

#endif /* MSL_UART_CONSOLE_IO_GCN_H */
