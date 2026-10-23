#ifndef MEMCARD_H
#define MEMCARD_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memcardMemAllocForSavedata(void);
void *memcardMemFreeForSavedata(void *save);

#ifdef __cplusplus
}
#endif

#endif
