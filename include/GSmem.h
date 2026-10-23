#ifndef GSMEM_H
#define GSMEM_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void* GSmemAlloc(u32 size);
void* GSmemAllocClr(u32 size);
void* GSmemAllocAlign(u32 size, u16 alignment);
void* GSmemAllocAlignFromEnd(u32 size, u16 alignment);
void GSmemFree(void* ptr);
long GSmemGetCurrentHeap(void);
long GSmemSetAllocMethodHeap(long heap, long method);

#ifdef __cplusplus
}
#endif

#endif
