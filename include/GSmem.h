#ifndef GSMEM_H
#define GSMEM_H

#include <types.h>

#ifdef __cplusplus
class GSmemCell;
#else
typedef struct GSmemCell GSmemCell;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* GSmemAlloc(u32 size);
void* GSmemAllocClr(u32 size);
void* GSmemAllocAlign(u32 size, u16 alignment);
void* GSmemAllocAlignFromEnd(u32 size, u16 alignment);
void GSmemFree(void* ptr);
void* GSmemAllocHeap(s32 heap, u32 size);
void GSmemFreeHeap(s32 heap, void* block);
s32 GSmemCreateExternalHeap(void* start, void* end, GSmemCell* cells, s32 cellCount);
long GSmemGetCurrentHeap(void);
long GSmemSetAllocMethodHeap(long heap, long method);

#ifdef __cplusplus
}
#endif

#endif
