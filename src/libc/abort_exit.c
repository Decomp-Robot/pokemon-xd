#include "abort_exit.h"
#include "critical_regions.h"

extern void _ExitProcess(void);
extern void __destroy_global_chain(void);
extern void (*_dtors[])(void);

void (*__atexit_funcs[64])(void);
void (*__console_exit)(void);
void (*__stdio_exit)(void);
int __atexit_curr_func;
int __aborting;

/**
 * Tear the program down: run the static destructors, flush the stdio
 * and console streams, run the atexit chain, then hand control to the
 * low level exit stub.
 */
void exit(int code) {
    void (**dtor)(void);

    if (__aborting == 0) {
        __begin_critical_region(atexit_funcs_access);
        __end_critical_region(atexit_funcs_access);
        __destroy_global_chain();
        for (dtor = _dtors; *dtor != 0; dtor++) {
            (*dtor)();
        }
        if (__stdio_exit != 0) {
            __stdio_exit();
            __stdio_exit = 0;
        }
    }
    __begin_critical_region(atexit_funcs_access);
    while (__atexit_curr_func > 0) {
        __atexit_curr_func--;
        __atexit_funcs[__atexit_curr_func]();
    }
    __end_critical_region(atexit_funcs_access);
    __kill_critical_regions();
    if (__console_exit != 0) {
        __console_exit();
        __console_exit = 0;
    }
    _ExitProcess();
}

