/*
 * Fixed-size memory-pool descriptors and allocator operations.
 */

#include <dolphin/os.h>
#include <string.h>

#include "GSmemman.h"

namespace {
struct CellLink {
    CellLink* next;
};
}

GSmemPoolAllocator _poolArea[POOL_MAX];

void _setDabr(u32 dabr);
u32 _getDabr();

extern "C" u32 GSmemGetFreePool(GSmemPoolAllocator* pool) {
    if (pool) {
        return pool->getFreeCell();
    }
    return 0;
}

extern "C" void GSmemDumpPool(GSmemPoolAllocator* pool) {
    s32 i;

    if (pool) {
        pool->dump();
        return;
    }
    for (i = 0; i < POOL_MAX; i++) {
        if (_poolArea[i].cellCount != 0) {
            _poolArea[i].dump();
        }
    }
}

extern "C" u8 GSmemIsRegistrationPool(GSmemPoolAllocator* pool, void* ptr) {
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
        if (_poolArea[i].cellCount == 0) {
            break;
        }
    }
    if (_poolArea[i].cellCount != 0) {
        return 0;
    }
    _poolArea[i].init(cellCount, cellSize);
    return &_poolArea[i];
}

extern "C" void GSmemInitPool() {
    memset(_poolArea, 0, sizeof(_poolArea));
}

void GSmemPoolAllocator::dump() {}

u8 GSmemPoolAllocator::isRegistration(void* ptr) {
    if (!ptr) {
        return 0;
    }
    if (ptr >= begin && ptr < end) {
        return 1;
    }
    return 0;
}

u32 GSmemPoolAllocator::getFreeCell() const {
    return cellCount - usedCellCount;
}

void GSmemPoolAllocator::freeCell(void* ptr) {
    u32 dabr;

    if (ptr) {
        ptr = (u8*)ptr - sizeof(void*);
        void*& cell = *(void**)ptr;
        cell = freeCellList;
        usedCellCount--;
        dabr = _getDabr();
        _setDabr(0);
        freeCellList = &cell;
        _setDabr(dabr);
    }
}

void* GSmemPoolAllocator::allocCell() {
    void* cell;
    u32 dabr;

    cell = freeCellList;
    dabr = _getDabr();
    _setDabr(0);
    if (cell) {
        freeCellList = *(void**)cell;
    } else {
        _setDabr(dabr);
        return 0;
    }
    _setDabr(dabr);
    memset(cell, 0, cellSize);
    usedCellCount++;
    if (usedCellCount > peakCellCount) {
        peakCellCount = usedCellCount;
    }
    return (u8*)cell + sizeof(void*);
}

void GSmemPoolAllocator::init(u32 count, u32 size) {
    u32 i;
    CellLink* lastCell;
    CellLink* cell;

    cellCount = count;
    cellSize = size = (size + sizeof(void*) + 3) & ~3;
    begin = (u8*)GSmemAllocClr((count + 1) * size);
    end = begin + count * size;
    freeCellList = begin;
    lastCell = 0;
    i = 0;
    cell = (CellLink*)begin;
    for (; i < count; i++) {
        CellLink* nextCell = (CellLink*)((u8*)cell + size);
        lastCell = cell;
        cell->next = nextCell;
        cell = nextCell;
    }
    lastCell->next = 0;
}

void _setDabr(u32) {}

u32 _getDabr() {
    return 0;
}
