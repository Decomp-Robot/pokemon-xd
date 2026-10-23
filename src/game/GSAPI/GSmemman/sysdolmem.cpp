/*
 * Heap callbacks the game hands to the renderer and the sysdol cell pools
 * they allocate from (GSmemman.a sysdolmem member).
 */

#include <stddef.h>
#include <dolphin/os.h>

#include <GSmemman.h>

/* Allocation size classes served by the sysdol pools. */
#define SYS_ALLOC_SIZE_8 8
#define SYS_ALLOC_SIZE_12 12
#define SYS_ALLOC_SIZE_20 20
#define SYS_ALLOC_SIZE_28 28
#define SYS_ALLOC_SIZE_48 48

#define SYS_ALLOC_CELLS_8 4000
#define SYS_ALLOC_CELLS_12 8000
#define SYS_ALLOC_CELLS_20 3000
#define SYS_ALLOC_CELLS_28 5000
#define SYS_ALLOC_CELLS_48 5000

#define SYS_ALLOC_MAX_SIZE 64

/* Parameter id of the memory callbacks in the renderer's init parameters. */
#define HSD_INIT_MEMORY_CALLBACKS 2

GSmemPoolAllocator* gSysdolPool8;
GSmemPoolAllocator* gSysdolPool12;
GSmemPoolAllocator* gSysdolPool20;
GSmemPoolAllocator* gSysdolPool28;
GSmemPoolAllocator* gSysdolPool48;

extern "C" void HSD_SetInitParameter(s32 type, ...);
extern "C" GSmemPoolAllocator* GSmemGetSysdolAllocator(u32 size);

BOOL _memCheckOwnCallback(void* ptr) {
    return TRUE;
}

void _memClearCallback(void) {
}

u32 _memGetRemainCallback(void) {
    return GSmemGetLargestFree();
}

void _memFreeCallback(void* ptr) {
    if (!ptr) {
        return;
    }
    if (gSysdolPool12->isRegistration(ptr)) {
        gSysdolPool12->freeCell(ptr);
    } else if (gSysdolPool20->isRegistration(ptr)) {
        gSysdolPool20->freeCell(ptr);
    } else if (gSysdolPool28->isRegistration(ptr)) {
        gSysdolPool28->freeCell(ptr);
    } else if (gSysdolPool48->isRegistration(ptr)) {
        gSysdolPool48->freeCell(ptr);
    } else {
        GSmemFree(ptr);
    }
}

void* _memAllocCallback(u32 size, u32 align, u32 flag) {
    void* cell;
    GSmemPoolAllocator* pool;

    if (!size) {
        return 0;
    }
    cell = NULL;
    if (size < SYS_ALLOC_MAX_SIZE) {
        pool = GSmemGetSysdolAllocator(size);
        if (pool) {
            cell = pool->allocCell();
            if (cell) {
                return cell;
            }
        }
    }
    return GSmemAllocNoClr(size);
}

extern "C" GSmemPoolAllocator* GSmemGetSysdolAllocator(u32 size) {
    GSmemPoolAllocator* pool;

    pool = NULL;
    switch (size) {
    case SYS_ALLOC_SIZE_8:
        pool = gSysdolPool8;
        break;
    case SYS_ALLOC_SIZE_12:
        pool = gSysdolPool12;
        break;
    case SYS_ALLOC_SIZE_20:
        pool = gSysdolPool20;
        break;
    case SYS_ALLOC_SIZE_28:
        pool = gSysdolPool28;
        break;
    case SYS_ALLOC_SIZE_48:
        pool = gSysdolPool48;
        break;
    }
    return pool;
}

extern "C" void GSmemInitSysdolAllocator(void) {
    gSysdolPool8 = GSmemCreatePool(SYS_ALLOC_CELLS_8, SYS_ALLOC_SIZE_8);
    gSysdolPool12 = GSmemCreatePool(SYS_ALLOC_CELLS_12, SYS_ALLOC_SIZE_12);
    gSysdolPool20 = GSmemCreatePool(SYS_ALLOC_CELLS_20, SYS_ALLOC_SIZE_20);
    gSysdolPool28 = GSmemCreatePool(SYS_ALLOC_CELLS_28, SYS_ALLOC_SIZE_28);
    gSysdolPool48 = GSmemCreatePool(SYS_ALLOC_CELLS_48, SYS_ALLOC_SIZE_48);

    HSD_SetInitParameter(HSD_INIT_MEMORY_CALLBACKS, _memAllocCallback, _memFreeCallback,
                         _memGetRemainCallback, _memClearCallback, _memCheckOwnCallback, 0);
}
