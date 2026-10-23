#ifndef HSD_DEBUG_H
#define HSD_DEBUG_H

extern "C" void __assert(const char *file, int line, const char *condition);

#define HSD_ASSERT(line, condition) \
    ((condition) ? (void)0 : __assert(__FILE__, line, #condition))

#endif
