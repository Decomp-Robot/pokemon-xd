#ifndef __MEM_H__
#define __MEM_H__

#include <libc/string.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memchr(const void *buffer, int character, size_t length);
void *__memrchr(const void *buffer, int character, unsigned long length);

#ifdef __cplusplus
}
#endif

#endif
