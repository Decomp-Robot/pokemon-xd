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

GSmemPoolAllocator* _Sysdol8Allocator;
GSmemPoolAllocator* _Sysdol12Allocator;
GSmemPoolAllocator* _Sysdol20Allocator;
GSmemPoolAllocator* _Sysdol28Allocator;
GSmemPoolAllocator* _Sysdol48Allocator;

extern "C" void HSD_SetInitParameter(s32 type, ...);

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
    if (_Sysdol12Allocator->isRegistration(ptr)) {
        _Sysdol12Allocator->freeCell(ptr);
    } else if (_Sysdol20Allocator->isRegistration(ptr)) {
        _Sysdol20Allocator->freeCell(ptr);
    } else if (_Sysdol28Allocator->isRegistration(ptr)) {
        _Sysdol28Allocator->freeCell(ptr);
    } else if (_Sysdol48Allocator->isRegistration(ptr)) {
        _Sysdol48Allocator->freeCell(ptr);
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
    return GSmemAlloc(size);
}

extern "C" GSmemPoolAllocator* GSmemGetSysdolAllocator(u32 size) {
    GSmemPoolAllocator* pool;

    pool = NULL;
    switch (size) {
    case SYS_ALLOC_SIZE_8:
        pool = _Sysdol8Allocator;
        break;
    case SYS_ALLOC_SIZE_12:
        pool = _Sysdol12Allocator;
        break;
    case SYS_ALLOC_SIZE_20:
        pool = _Sysdol20Allocator;
        break;
    case SYS_ALLOC_SIZE_28:
        pool = _Sysdol28Allocator;
        break;
    case SYS_ALLOC_SIZE_48:
        pool = _Sysdol48Allocator;
        break;
    }
    return pool;
}

extern "C" void GSmemInitSysdolAllocator(void) {
    _Sysdol8Allocator = GSmemCreatePool(SYS_ALLOC_CELLS_8, SYS_ALLOC_SIZE_8);
    _Sysdol12Allocator = GSmemCreatePool(SYS_ALLOC_CELLS_12, SYS_ALLOC_SIZE_12);
    _Sysdol20Allocator = GSmemCreatePool(SYS_ALLOC_CELLS_20, SYS_ALLOC_SIZE_20);
    _Sysdol28Allocator = GSmemCreatePool(SYS_ALLOC_CELLS_28, SYS_ALLOC_SIZE_28);
    _Sysdol48Allocator = GSmemCreatePool(SYS_ALLOC_CELLS_48, SYS_ALLOC_SIZE_48);

    HSD_SetInitParameter(HSD_INIT_MEMORY_CALLBACKS, _memAllocCallback, _memFreeCallback,
                         _memGetRemainCallback, _memClearCallback, _memCheckOwnCallback, 0);
}
