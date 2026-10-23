/*
 * GSmem heap allocator (GSmemman.a).
 *
 * Heap descriptors live in the array handed to OSInitAlloc. Each heap keeps
 * its free and allocated blocks in address-ordered doubly linked lists of
 * GSmemCell. Heaps created by OSCreateExternalHeap hand out cells from a
 * caller-supplied array instead of splitting their own free blocks, and
 * additionally serve small "sysdol" allocations from GSmemPoolAllocator
 * instances.
 */

#ifndef GSEMMAN_H
#define GSEMMAN_H

#include <dolphin/types.h>

#define ALIGNMENT 32
#define MINOBJSIZE 64U
#define HEADERSIZE 32

#define HEAP_TYPE_INTERNAL 0
#define HEAP_TYPE_EXTERNAL 1

#define ALLOC_METHOD_FIRST 0
#define ALLOC_METHOD_BEST 1
#define ALLOC_METHOD_LARGEST 3
/* The last-cell policy uses the same selector value as the largest-cell one. */
#define ALLOC_METHOD_LAST 3

#define ROUNDUP(x, align) (((u32)(x) + ((align) - 1)) & ~((align) - 1))

#define InRange(cell, start, end) ((u32)(start) <= (u32)(cell) && (u32)(cell) < (u32)(end))

#define CHECK_CELLALIGN(hd, cell) \
    ((hd)->type == HEAP_TYPE_EXTERNAL || \
     ((hd)->type == HEAP_TYPE_INTERNAL && OFFSET(cell, ALIGNMENT) == 0))

/* Number of descriptors in the pool table used by GSmemCreatePool. */
#define POOL_MAX 32

/* Called by GSmemVisitHeapAllocated for every allocated cell of a heap. */
typedef void (*GSmemCellVisitor)(void* ptr, u32 size, u32 arg, u32 arg2);

class GSmemHeapDesc;

class GSmemCell {
public:
    GSmemCell* prev;   /* 0x00 */
    GSmemCell* next;   /* 0x04 */
    s32 size;          /* 0x08 */
    u32 area;          /* 0x0c: address of the block this cell describes */
    u16 id;            /* 0x10: id handed out when the block was allocated */

    void* get_area(GSmemHeapDesc* hd);
    void init(int size);
    void clear();

    s32 get_size() { return size; }
    GSmemCell* next_cell() { return next; }
    u32 area_after() { return area + size; }
};

class GSmemHeapDesc {
public:
    s32 size;               /* 0x00 */
    GSmemCell* free;        /* 0x04 */
    GSmemCell* allocated;   /* 0x08 */
    s32 type;               /* 0x0c: HEAP_TYPE_INTERNAL or HEAP_TYPE_EXTERNAL */
    s32 method;             /* 0x10: allocation method, see ALLOC_METHOD_* */
    s32 useCellPool;        /* 0x14: set for heaps created by OSCreateExternalHeap */
    GSmemCell* cellArray;   /* 0x18 */
    s32 cellCount;          /* 0x1c */
    GSmemCell* newCellList; /* 0x20: cells that createNewCell may hand out */
    u32 reserved0;          /* 0x24 */
    u32 reserved1;          /* 0x28 */
    u8 outOfMemory;         /* 0x2c: set when a request could not be satisfied */
    u8 padding[3];          /* 0x2d */

    GSmemCell* searchAllocCell(void* ptr);
    void addfreeCellList(GSmemCell* cell);
    GSmemCell* createNewCell();
    s32 getHeaderSize();
};

class GSmemPoolAllocator {
public:
    u32 usedCellCount;  /* 0x00 */
    u32 peakCellCount;  /* 0x04 */
    u32 cellCount;      /* 0x08 */
    u32 cellSize;       /* 0x0c */
    u8* begin;          /* 0x10 */
    u8* end;            /* 0x14 */
    void* freeCellList; /* 0x18 */

    u8 isRegistration(void* ptr);
    u32 getFreeCell() const;
    void freeCell(void* ptr);
    void* allocCell();
    void init(u32 cellCount, u32 cellSize);
};

/* Heap descriptor array, its length and the arena the internal heaps span. */
extern GSmemHeapDesc* HeapArray;
extern s32 NumHeaps;
extern u32 ArenaStart;
extern u32 ArenaEnd;

/* Heap state bytes that OSInitAlloc clears. */
extern u8 lbl_804EAA90;
extern u8 lbl_804EAA91;

/* Sysdol pools, one per allocation size class. */
extern GSmemPoolAllocator* gSysdolPool8;
extern GSmemPoolAllocator* gSysdolPool12;
extern GSmemPoolAllocator* gSysdolPool20;
extern GSmemPoolAllocator* gSysdolPool28;
extern GSmemPoolAllocator* gSysdolPool48;

/* Table of pool allocators; GSmemCreatePool fills the first free entry. */
extern GSmemPoolAllocator gPoolTable[POOL_MAX];

/* Entry points of the other members of this library that the heap code calls:
 * GSmemman.o holds the GSmem* wrappers, the rest of poolAlloc.o holds the
 * GSmemPoolAllocator methods. */
extern "C" {
u32 GSmemGetLargestFree(void);
u16 GSmemGetCellId(void);
void* GSmemAlloc(u32 size);
void* GSmemAllocNoClr(u32 size);
void GSmemFree(void* ptr);
GSmemPoolAllocator* GSmemCreatePool(u32 cellCount, u32 cellSize);
void GSmemDumpPoolInfo(GSmemPoolAllocator* pool);
}

BOOL _memCheckOwnCallback(void* ptr);
void _memClearCallback(void);
u32 _memGetRemainCallback(void);

#endif
