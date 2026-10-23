#ifndef _DOLPHIN_OSALLOC_H_
#define _DOLPHIN_OSALLOC_H_

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef s32 OSHeapHandle;

extern volatile OSHeapHandle __OSCurrHeap;

void* OSAllocFromHeap(s32 heap, s32 size);
void* OSAllocFixed(void* rstart, void* rend);
void OSFreeToHeap(s32 heap, void* ptr);
s32 OSSetCurrentHeap(s32 heap);
void* OSInitAlloc(void* arenaStart, void* arenaEnd, s32 maxHeaps);
s32 OSCreateHeap(void* start, void* end);
void OSDestroyHeap(s32 heap);
void OSAddToHeap(s32 heap, void* start, void* end);
s32 OSCheckHeap(s32 heap);
u32 OSReferentSize(void* ptr);
void OSDumpHeap(s32 heap);
void OSVisitAllocated(void (*visitor)(void*, u32));

#define OSAlloc(size) OSAllocFromHeap(__OSCurrHeap, (size))
#define OSFree(ptr)   OSFreeToHeap(__OSCurrHeap, (ptr))

#ifdef __cplusplus
}
#endif

#endif
