#ifndef GS_LIBC_MEMORY_H
#define GS_LIBC_MEMORY_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memset(void *dst, int value, unsigned int size);
void *memcpy(void *dst, const void *src, unsigned int size);
void *memmove(void *dst, const void *src, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif
