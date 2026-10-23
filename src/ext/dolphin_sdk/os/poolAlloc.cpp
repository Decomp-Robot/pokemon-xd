/*
 * Fixed-size cell pools (GSmemman.a poolAlloc member).
 */

#include <dolphin/os.h>

#include "GSmemman.h"

GSmemPoolAllocator gPoolTable[POOL_MAX];

extern "C" u32 GSmemGetFreePool(GSmemPoolAllocator* pool) {
    if (pool) {
        return pool->getFreeCell();
    }
    return 0;
}

extern "C" void GSmemDumpPool(GSmemPoolAllocator* pool) {
    s32 i;

    if (pool) {
        GSmemDumpPoolInfo(pool);
        return;
    }
    for (i = 0; i < POOL_MAX; i++) {
        if (gPoolTable[i].cellCount != 0) {
            GSmemDumpPoolInfo(&gPoolTable[i]);
        }
    }
}

extern "C" u8 GSmemPoolIsRegistration(GSmemPoolAllocator* pool, void* ptr) {
    return pool->isRegistration(ptr);
}

extern "C" void GSmemFreePool(GSmemPoolAllocator* pool, void* ptr) {
    if (ptr) {
        pool->freeCell(ptr);
    }
}

extern "C" void* GSmemAllocPool(GSmemPoolAllocator* pool) {
    void* cell;

    cell = pool->allocCell();
    return cell;
}

extern "C" GSmemPoolAllocator* GSmemCreatePool(u32 cellCount, u32 cellSize) {
    s32 i;

    for (i = 0; i < POOL_MAX; i++) {
        if (gPoolTable[i].cellCount == 0) {
            break;
        }
    }
    if (gPoolTable[i].cellCount != 0) {
        return 0;
    }
    gPoolTable[i].init(cellCount, cellSize);
    return &gPoolTable[i];
}
