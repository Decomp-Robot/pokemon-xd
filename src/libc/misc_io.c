#include "file.h"
#include "abort_exit.h"

void __stdio_atexit(void) {
    __stdio_exit = __close_all;
}
