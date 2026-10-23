#ifndef __QSORT_H__
#define __QSORT_H__

#include <libc/stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void qsort(void *base, size_t count, size_t elementSize,
           int (*compare)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif
