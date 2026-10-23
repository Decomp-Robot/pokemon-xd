#ifndef GS_MEM_H
#define GS_MEM_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *GSmemAlloc(u32 size);
void *GSmemAllocNoClr(unsigned long size);
void GSmemFree(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
