/*
 * Fixed-size cell pools (GSmemman.a poolAlloc member).
 */

#include <dolphin/os.h>
#include <string.h>

#include <GSmemman.h>

/* Free cells chain through a 4-byte link stored ahead of the user payload. */
class GSmemPoolCell {
public:
    GSmemPoolCell* next; /* 0x00 */
};

/* Pool state save/clear/restore around free-list updates. */
u32 _getDabr(void);
void _setDabr(unsigned long state);

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

/* Clears the whole pool table; called from GSmemInit. */
extern "C" void GSmemInitPool(void) {
    memset(gPoolTable, 0, sizeof(gPoolTable));
}

/* The pool dump is compiled out of retail. */
extern "C" void GSmemDumpPoolInfo(GSmemPoolAllocator* pool) {
}

u8 GSmemPoolAllocator::isRegistration(void* ptr) {
    if (ptr == 0) {
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
    GSmemPoolCell* cell;

    if (!ptr) {
        return;
    }
    ptr = (GSmemPoolCell*)ptr - 1;
    cell = (GSmemPoolCell*)ptr;
    cell->next = (GSmemPoolCell*)freeCellList;
    usedCellCount--;
    u32 state = _getDabr();
    _setDabr(0);
    freeCellList = cell;
    _setDabr(state);
}

void* GSmemPoolAllocator::allocCell() {
    GSmemPoolCell* cell;
    u32 state;

    cell = (GSmemPoolCell*)freeCellList;
    state = _getDabr();
    _setDabr(0);
    if (cell != 0) {
        freeCellList = cell->next;
    } else {
        _setDabr(state);
        return 0;
    }
    _setDabr(state);
    memset(cell, 0, cellSize);
    usedCellCount++;
    if (usedCellCount > peakCellCount) {
        peakCellCount = usedCellCount;
    }
    return (void*)(cell + 1);
}

/* cellSize grows by the link word and rounds up to the cell alignment. */
void GSmemPoolAllocator::init(u32 cellCount, u32 cellSize) {
    u8* cursor;
    u32 i;
    GSmemPoolCell* last;
    u8* next;
    this->cellCount = cellCount;
    this->cellSize = cellSize = ROUNDUP(cellSize + sizeof(GSmemPoolCell), 4);
    begin = (u8*)GSmemAlloc((cellCount + 1) * cellSize);
    end = begin + cellCount * cellSize;
    freeCellList = begin;
    last = 0;
    cursor = begin;
    for (i = 0; i < cellCount; i++) {
        next = cursor + cellSize;
        last = (GSmemPoolCell*)cursor;
        ((GSmemPoolCell*)cursor)->next = (GSmemPoolCell*)next;
        cursor = next;
    }
    last->next = 0;
}

void _setDabr(unsigned long state) {
}

u32 _getDabr(void) {
    return 0;
}
