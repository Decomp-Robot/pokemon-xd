#ifndef MSL_CRITICAL_REGIONS_H
#define MSL_CRITICAL_REGIONS_H

enum {
    atexit_funcs_access = 0,
    malloc_pool_access = 1,
    stdio_access = 2
};

extern void __begin_critical_region(int id);
extern void __end_critical_region(int id);
extern void __kill_critical_regions(void);

#endif /* MSL_CRITICAL_REGIONS_H */
