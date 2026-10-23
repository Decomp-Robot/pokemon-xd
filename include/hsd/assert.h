#ifndef HSD_ASSERT_H
#define HSD_ASSERT_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
void __assert(const char *file, s32 line, const char *expression);
#ifdef __cplusplus
}
#endif

#define HSD_ASSERT(condition) \
    ((condition) ? (void)0 : __assert(__FILE__, __LINE__, #condition))

#endif
