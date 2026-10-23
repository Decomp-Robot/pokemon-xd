#ifdef __cplusplus
extern "C" {
#endif
extern int __aborting;
extern int __atexit_curr_func;
extern void (*__atexit_funcs[64])(void);
extern void (*__stdio_exit)(void);
extern void (*__console_exit)(void);
#ifdef __cplusplus
}
#endif
