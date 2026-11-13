/*
 * GSmem heap entry points: the current-heap/layer state the pool allocator
 * shares, the game-facing allocation wrappers and the heap debugging hooks.
 */

#include <string.h>
#include <dolphin/os.h>

#include <GSmemman.h>

/* Heap used by callers that do not name one; -1 until GSmemInit runs. */
s32 _currentHeap = -1;

/* Set once the arena and the pool allocator are ready. */
static u8 _isInitialized;

/* Layer stored in the cells of the blocks handed out. */
u16 _currentLayer;

bool GSmemCheckHeap(s32 heap) {
    return OSCheckHeap(heap) >= 0;
}

u32 GSmemGetLargestFree(void) {
    u32 largest;

    OSGetHeapFreeSize(GSmemGetCurrentHeap(), NULL, &largest);
    return largest;
}

u32 GSmemGetSize(void* ptr) {
    if (ptr == NULL) {
        return 0;
    }
    return OSReferentSize(ptr);
}

void GSmemFree(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    GSmemFreeHeap(GSmemGetCurrentHeap(), ptr);
}

s32 GSmemShrink(void* ptr, u32 size) {
    return GSmemShrinkHeap(GSmemGetCurrentHeap(), ptr, size);
}

void* GSmemAllocAlignClr(u32 size, u16 alignment) {
    void* ptr;

    ptr = GSmemAllocAlignHeap(GSmemGetCurrentHeap(), size, alignment);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void* GSmemAllocAlignFromEnd(u32 size, u16 alignment) {
    return GSmemAllocAlignHeap(GSmemGetCurrentHeap(), size, alignment);
}

void* GSmemAllocAlign(u32 size, u16 alignment) {
    return GSmemAllocAlignHeap(GSmemGetCurrentHeap(), size, alignment);
}

void* GSmemAllocClr(u32 size) {
    return GSmemAllocClrHeap(GSmemGetCurrentHeap(), size);
}

void* GSmemAlloc(u32 size) {
    return GSmemAllocHeap(GSmemGetCurrentHeap(), size);
}

void _dumpHeapVisitor(void* ptr, u32 size, void* context, void* context2) {
}

void GSmemDumpHeap(s32 heap) {
    u32 largestFree;
    u32 freeSize;
    u32 used;
    u32 allocated;
    u32 heapSize;

    allocated = 0;
    GSmemVisitHeapAllocated(heap, _dumpHeapVisitor, NULL, &allocated);
    OSGetHeapFreeSize(heap, &freeSize, &largestFree);
    heapSize = OSGetHeapSize(heap);
    used = heapSize - freeSize;
}

s32 GSmemGetHeapFreeSize(s32 heap, u32* totalSize, u32* largestSize) {
    return OSGetHeapFreeSize(heap, totalSize, largestSize);
}

void GSmemFreeHeap(s32 heap, void* block) {
    OSFreeToHeap(heap, block);
}

s32 GSmemShrinkHeap(s32 heap, void* ptr, u32 size) {
    return OSShrinkHeap(heap, ptr, size);
}

void* GSmemAllocAlignHeap(s32 heap, u32 size, u16 alignment) {
    if (alignment <= ALIGNMENT) {
        return GSmemAllocHeap(heap, size);
    }
    return NULL;
}

void* GSmemAllocClrHeap(s32 heap, u32 size) {
    return GSmemAllocFillHeap(heap, size, 0);
}

void* GSmemAllocFillHeap(s32 heap, u32 size, u8 fill) {
    void* ptr;

    ptr = GSmemAllocHeap(heap, size);
    if (ptr == NULL) {
        return NULL;
    }
    if (OSGetHeapType(heap) == HEAP_TYPE_INTERNAL) {
        memset(ptr, fill, size);
    }
    return ptr;
}

void* GSmemAllocHeap(s32 heap, u32 size) {
    void* ptr;

    ptr = OSAllocFromHeap(heap, size);
    return ptr;
}

s32 GSmemSetAllocMethodHeap(s32 heap, s32 method) {
    return OSSetAllocMethodHeap(heap, method);
}

u16 GSmemGetCurrentLayer(void) {
    return _currentLayer;
}

s32 GSmemSetCurrentHeap(s32 heap) {
    s32 prev;

    prev = GSmemGetCurrentHeap();
    _currentHeap = heap;
    return prev;
}

s32 GSmemGetCurrentHeap(void) {
    return _currentHeap;
}

s32 GSmemCreateExternalHeap(void* start, void* end, GSmemCell* cells, s32 cellCount) {
    if (!GSmemIsInitialized()) {
        return -1;
    }
    return OSCreateExternalHeap(start, end, cells, cellCount);
}

s32 GSmemCreateHeap(void* start, void* end) {
    if (!GSmemIsInitialized()) {
        return -1;
    }
    return OSCreateHeap(start, end);
}

void* GSmemInit(void* start, void* end, s32 heapCount) {
    void* arenaEnd;

    _currentHeap = -1;
    _currentLayer = 0;
    arenaEnd = OSInitAlloc(start, end, heapCount);
    _isInitialized = 1;
    GSmemInitPool();
    return arenaEnd;
}

u8 GSmemIsInitialized(void) {
    return _isInitialized;
}
