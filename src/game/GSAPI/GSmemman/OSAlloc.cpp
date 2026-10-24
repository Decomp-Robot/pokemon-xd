/*
 * GSmem heap allocator: the heaps OSAlloc/OSFree use and the cell-list code
 * every heap operation is built on.
 */

#include <stddef.h>
#include <dolphin/os.h>
#include <string.h>

#include <GSmemman.h>

/* Keep each failed heap invariant and its source line in the diagnostic. */
#define ASSERTREPORT(line, cond) \
    if (!(cond)) { \
        OSReport("OSCheckHeap: Failed " #cond " in %d", line); \
        return -1; \
    }

GSmemHeapDesc* HeapArray;
s32 NumHeaps;
u32 ArenaStart;
u32 ArenaEnd;
u8 lbl_804EAA90;
u8 lbl_804EAA91;

GSmemCell* DLInsert(GSmemCell* list, GSmemCell* cell, GSmemHeapDesc* hd);
GSmemCell* DLExtract(GSmemCell* list, GSmemCell* cell);
GSmemCell* DLAddFront(GSmemCell* list, GSmemCell* cell);

extern "C" s32 OSSetAllocMethodHeap(s32 heap, s32 method) {
    GSmemHeapDesc* hd;
    s32 prev;

    hd = &HeapArray[heap];
    prev = hd->method;
    hd->method = method;
    return prev;
}

extern "C" s32 OSGetHeapType(s32 heap) {
    GSmemHeapDesc* hd;

    hd = &HeapArray[heap];
    return hd->type;
}

extern "C" s32 OSGetHeapFreeSize(s32 heap, u32* totalSize, u32* largestSize) {
    GSmemHeapDesc* hd;
    GSmemCell* cell;
    s32 total;
    s32 largest;

    total = 0;
    largest = 0;
    if (heap < 0 || heap >= NumHeaps) {
        if (totalSize) {
            *totalSize = 0;
        }
        if (largestSize) {
            *largestSize = 0;
        }
        return 0;
    }
    hd = &HeapArray[heap];
    for (cell = hd->free; cell != NULL; cell = cell->next) {
        total += cell->size;
        if (cell->size > largest) {
            largest = cell->size;
        }
    }
    if (totalSize) {
        *totalSize = total;
    }
    if (largestSize) {
        *largestSize = largest;
    }
    return 1;
}

extern "C" s32 OSGetHeapSize(s32 heap) {
    GSmemHeapDesc* hd;

    if (heap < 0 || heap >= NumHeaps) {
        return 0;
    }
    hd = &HeapArray[heap];
    return hd->size;
}

extern "C" s32 GSmemVisitHeapAllocated(s32 heap, GSmemCellVisitor visitor, void* context, void* context2) {
    GSmemCell* cell;
    s32 count;

    count = 0;
    if (HeapArray[heap].size <= 0 || visitor == NULL) {
        return 0;
    }
    if (HeapArray[heap].size >= 0) {
        for (cell = HeapArray[heap].allocated; cell != NULL; cell = cell->next) {
            if (visitor != NULL) {
                visitor(cell->get_area(&HeapArray[heap]), cell->size, context, context2);
            }
            count += 1;
        }
    }
    return count;
}

extern "C" u32 OSReferentSize(void* ptr) {
    GSmemCell* cell;

    cell = (GSmemCell*)((u32)ptr - HEADERSIZE);
    return (u32)(cell->size - HEADERSIZE);
}

extern "C" void OSDumpHeap(s32 heap) {
    GSmemHeapDesc* hd;
    GSmemCell* cell;

    OSReport("\nOSDumpHeap(%d):\n", heap);
    hd = &HeapArray[heap];
    if (hd->size < 0) {
        OSReport("--------Inactive\n");
        return;
    }
    OSReport("addr\tend\tsize\tprev\tnext\n");
    OSReport("--------Allocated\n");
    for (cell = hd->allocated; cell != NULL; cell = cell->next) {
        OSReport("%08xh\t%08xh\t%x\t%08xh\t%08xh\n", cell,
                 (char*)cell + cell->size, cell->size, cell->prev, cell->next);
    }
    OSReport("--------Free\n");
    for (cell = hd->free; cell != NULL; cell = cell->next) {
        OSReport("%08xh\t%08xh\t%x\t%08xh\t%08xh\n", cell,
                 (char*)cell + cell->size, cell->size, cell->prev, cell->next);
    }
}

extern "C" s32 OSCheckHeap(s32 heap) {
    GSmemHeapDesc* hd;
    GSmemCell* cell;
    s32 total = 0;
    s32 freeSize = 0;
    ASSERTREPORT(1605, HeapArray);
    ASSERTREPORT(1606, 0 <= heap && heap < NumHeaps);
    hd = &HeapArray[heap];

    ASSERTREPORT(1609, 0 <= hd->size);

    ASSERTREPORT(1611, hd->allocated == NULL || hd->allocated->prev == NULL);

    for (cell = hd->allocated; cell != NULL; cell = cell->next) {
        if (hd->type == HEAP_TYPE_INTERNAL) {
            ASSERTREPORT(1615, InRange(cell, ArenaStart, ArenaEnd));
            ASSERTREPORT(1616, CHECK_CELLALIGN( hd, cell));
        }
        ASSERTREPORT(1618, cell->next == NULL || cell->next->prev == cell);
        ASSERTREPORT(1619, MINOBJSIZE <= cell->size);
        ASSERTREPORT(1620, OFFSET(cell->size, ALIGNMENT) == 0);
        total += cell->size;

        ASSERTREPORT(1623, 0 < total && total <= hd->size);
    }

    /* The free list carries the same invariants. Its cells must also be
     * separated: a free cell ends before the free cell that follows it,
     * so two free blocks never overlap.
     */
    ASSERTREPORT(1631, hd->free == NULL || hd->free->prev == NULL);

    for (cell = hd->free; cell != NULL; cell = cell->next) {
        if (hd->type == HEAP_TYPE_INTERNAL) {
            ASSERTREPORT(1635, InRange(cell, ArenaStart, ArenaEnd));
            ASSERTREPORT(1636, CHECK_CELLALIGN(hd, cell ));
            ASSERTREPORT(1637, cell->next == NULL || (char*) cell + cell->size < (char*) cell->next);
        }
        ASSERTREPORT(1639, cell->next == NULL || cell->next->prev == cell);
        ASSERTREPORT(1640, MINOBJSIZE <= cell->size);
        ASSERTREPORT(1641, OFFSET(cell->size, ALIGNMENT) == 0);
        total += cell->size;
        freeSize += cell->size - hd->getHeaderSize();

        ASSERTREPORT(1645, 0 < total && total <= hd->size);
    }

    /* Both lists have been walked: every byte of the heap is now covered
     * by one of them.
     */
    ASSERTREPORT(1652, total == hd->size);
    return freeSize;
}

extern "C" void OSDestroyHeap(s32 heap) {
    GSmemHeapDesc& hd = HeapArray[heap];

    hd.size = -1;
}

extern "C" s32 OSCreateExternalHeap(void* start, void* end, GSmemCell* cells, s32 cellCount) {
    GSmemHeapDesc* hd;
    GSmemCell* cell;
    s32 i;
    s32 heap;

    start = (void*)ROUNDUP((u32)start, ALIGNMENT);
    end = (void*)((u32)end & ~(ALIGNMENT - 1));

    memset(cells, 0, cellCount * sizeof(GSmemCell));

    for (heap = 0; heap < NumHeaps; heap++) {
        hd = &HeapArray[heap];
        if (hd->size >= 0) {
            continue;
        }
        hd->size = (u32)end - (u32)start;
        hd->method = ALLOC_METHOD_FIRST;
        hd->cellArray = cells;
        hd->cellCount = cellCount;
        hd->type = HEAP_TYPE_EXTERNAL;
        hd->useCellPool = 1;
        cell = hd->cellArray;
        cell->init(hd->size);
        cell->area = (u32)start;
        hd->free = cell;
        hd->allocated = NULL;
        hd->newCellList = NULL;
        for (i = 0; i < cellCount; i++) {
            hd->addfreeCellList(&cells[i]);
        }
        return heap;
    }
    return -1;
}

extern "C" s32 OSCreateHeap(void* start, void* end) {
    GSmemHeapDesc* hd;
    s32 heap;
    GSmemCell* cell;

    OSReport("OSCreateHeap : start=%08xh, end=%08xh\n", start, end);

    start = (void*)ROUNDUP((u32)start, ALIGNMENT);
    end = (void*)((u32)end & ~(ALIGNMENT - 1));

    for (heap = 0; heap < NumHeaps; heap++) {
        hd = &HeapArray[heap];
        if (hd->size >= 0) {
            continue;
        }
        hd->size = (u32)end - (u32)start;
        cell = (GSmemCell*)start;
        cell->init(hd->size);
        cell->area = (u32)cell;
        hd->free = cell;
        hd->allocated = NULL;
        hd->outOfMemory = 0;
        return heap;
    }
    return -1;
}

extern "C" void* OSInitAlloc(void* arenaStart, void* arenaEnd, s32 maxHeaps) {
    GSmemHeapDesc* hd;
    s32 i;
    u32 size;

    OSReport("OSInitAlloc : arenaStart=%08xh, arenaEnd=%08xh, maxHeaps %d\n", arenaStart, arenaEnd, maxHeaps);

    HeapArray = (GSmemHeapDesc*)arenaStart;
    NumHeaps = maxHeaps;
    size = maxHeaps * sizeof(GSmemHeapDesc);

    for (i = 0; i < NumHeaps; i++) {
        hd = &HeapArray[i];
        hd->size = -1;
        hd->free = hd->allocated = NULL;
        hd->type = HEAP_TYPE_INTERNAL;
        hd->useCellPool = 0;
        hd->method = ALLOC_METHOD_FIRST;
        hd->cellArray = NULL;
        hd->cellCount = 0;
        hd->outOfMemory = 0;
    }

    __OSCurrHeap = -1;
    arenaStart = (void*)((u32)HeapArray + size);
    arenaStart = (void*)ROUNDUP((u32)arenaStart, ALIGNMENT);
    ArenaStart = (u32)arenaStart;
    ArenaEnd = (u32)arenaEnd & ~(ALIGNMENT - 1);
    lbl_804EAA90 = 0;
    lbl_804EAA91 = 0;
    return arenaStart;
}

extern "C" s32 OSSetCurrentHeap(s32 heap) {
    s32 prev;

    prev = __OSCurrHeap;
    __OSCurrHeap = heap;
    return prev;
}

extern "C" s32 OSShrinkHeap(s32 heap, void* ptr, u32 size) {
    GSmemHeapDesc* hd;
    GSmemCell* cell;
    GSmemCell* newCell;
    u32 newArea;
    s32 oldSize;
    s32 cellSize;
    s32 savedSize;
    u32 area;
    s32 rest;

    if (!size) {
        return 0;
    }
    hd = &HeapArray[heap];
    size += hd->getHeaderSize();
    size = ROUNDUP(size, ALIGNMENT);

    if (hd->type == HEAP_TYPE_INTERNAL) {
        if (!ptr) {
            return 0;
        }
        cell = (GSmemCell*)((u32)ptr - HEADERSIZE);
    } else {
        cell = hd->searchAllocCell(ptr);
    }
    cellSize = cell->size;
    oldSize = cell->size;
    savedSize = oldSize;
    rest = cellSize - size;
    if (rest < MINOBJSIZE) {
        return 1;
    }

    if (hd->type == HEAP_TYPE_EXTERNAL) {
        newCell = hd->createNewCell();
        if (!newCell) {
            return 0;
        }
        cell->size = size;
        newCell->area = (u32)newCell;
    } else {
        area = cell->area;
        cell->size = size;
        newCell = (GSmemCell*)(area + size);
        newArea = cell->area_after();
        newCell->area = newArea;
    }
    newCell->init(rest);
    hd->free = DLInsert(hd->free, newCell, hd);
    return 1;
}

extern "C" void OSFreeToHeap(s32 heap, void* ptr) {
    GSmemHeapDesc* hd;
    GSmemCell* cell;

    hd = &HeapArray[heap];

    if (hd->type == HEAP_TYPE_INTERNAL) {
        if (!ptr) {
            return;
        }
        cell = (GSmemCell*)((u32)ptr - HEADERSIZE);
    } else {
        cell = hd->searchAllocCell(ptr);
    }
    hd->allocated = DLExtract(hd->allocated, cell);
    hd->free = DLInsert(hd->free, cell, hd);
}

extern "C" void* OSAllocFromHeap(s32 heap, s32 size) {
    GSmemHeapDesc* hd;
    GSmemCell* cell;
    GSmemCell* tail;
    GSmemCell* newCell;
    s32 rest;
    s32 best;
    u32 area;

    hd = &HeapArray[heap];
    size += hd->getHeaderSize();
    size = ROUNDUP(size, ALIGNMENT);

    if (hd->method == ALLOC_METHOD_BEST) {
        cell = NULL;
        for (tail = hd->free; tail != NULL; tail = tail->next_cell()) {
            if (size > tail->get_size()) {
                continue;
            }
            if (size == tail->get_size()) {
                cell = tail;
                break;
            }
            if (cell) {
                if (tail->get_size() < cell->get_size()) {
                    cell = tail;
                }
            } else {
                cell = tail;
            }
        }
    }
    else if (hd->method == ALLOC_METHOD_LARGEST) {
        best = -1;
        cell = NULL;
        for (tail = hd->free; tail != NULL; tail = tail->next_cell()) {
            if (best <= tail->get_size()) {
                best = tail->get_size();
                cell = tail;
            }
        }
        if (size > best) {
            cell = NULL;
        }
    }
    else if (hd->method == ALLOC_METHOD_LAST) {
        cell = NULL;
        for (tail = hd->free; tail != NULL; tail = tail->next_cell()) {
            if (size <= tail->size && cell < tail) {
                cell = tail;
            }
        }
    }
    else {
        for (cell = hd->free; cell != NULL; cell = cell->next) {
            if (size <= cell->size) {
                break;
            }
        }
    }

    if (!cell) {
        hd->outOfMemory = 1;
        return 0;
    }

    rest = cell->size - size;
    if (rest < MINOBJSIZE) {
        hd->free = DLExtract(hd->free, cell);
    } else if (hd->method == ALLOC_METHOD_LARGEST && hd->type == HEAP_TYPE_INTERNAL) {
        newCell = cell;
        cell = (GSmemCell*)((u32)cell + rest);
        newCell->size = rest;
        cell->init(size);
        cell->area = (u32)cell;
    } else {
        if (hd->type == HEAP_TYPE_EXTERNAL) {
            newCell = hd->createNewCell();
            if (newCell == NULL) {
                OSReport("OSAllocFromHeap: Warning- not free cell\n");
                hd->outOfMemory = 1;
                return 0;
            }
            cell->size = size;
            newCell->init(rest);
            area = cell->area_after();
            newCell->area = area;
        } else {
            newCell = (GSmemCell*)((u32)cell + size);
            cell->size = size;
            newCell->init(rest);
            newCell->area = (u32)newCell;
        }
        newCell->prev = cell->prev;
        newCell->next = cell->next;
        if (newCell->next != NULL) {
            newCell->next->prev = newCell;
        }
        if (newCell->prev != NULL) {
            newCell->prev->next = newCell;
        } else {
            hd->free = newCell;
        }
    }
    hd->allocated = DLAddFront(hd->allocated, cell);
    cell->layer = GSmemGetCurrentLayer();
    hd->outOfMemory = 0;
    return cell->get_area(hd);
}

GSmemCell* DLInsert(GSmemCell* list, GSmemCell* cell, GSmemHeapDesc* hd) {
    GSmemCell* prev;
    GSmemCell* next;
    GSmemCell* absorbed;
    u32 nextArea;
    u32 cellArea;
    u32 cellEnd;
    u32 nextStart;
    u32 prevEnd;
    u32 cellStart;

    prev = NULL;
    for (next = list; next != NULL; next = next->next) {
        nextArea = next->area;
        cellArea = cell->area;
        if (cellArea <= nextArea) {
            break;
        }
        prev = next;
    }
    cell->next = next;
    cell->prev = prev;

    if (next) {
        next->prev = cell;
        cellEnd = cell->area + cell->size;
        nextStart = next->area;
        if (cellEnd == nextStart) {
            absorbed = next;
            cell->size += next->size;
            next = next->next;
            cell->next = next;
            if (next) {
                next->prev = cell;
            }
            absorbed->size = 0;
            if (hd != NULL && hd->type == HEAP_TYPE_EXTERNAL) {
                hd->addfreeCellList(absorbed);
            }
        }
    }

    if (prev) {
        prev->next = cell;
        prevEnd = prev->area + prev->size;
        cellStart = cell->area;
        if (prevEnd == cellStart) {
            absorbed = cell;
            prev->size += cell->size;
            prev->next = next;
            if (next) {
                next->prev = prev;
            }
            absorbed->size = 0;
            if (hd != NULL && hd->type == HEAP_TYPE_EXTERNAL) {
                hd->addfreeCellList(absorbed);
            }
        }
        return list;
    }
    return cell;
}

GSmemCell* DLExtract(GSmemCell* list, GSmemCell* cell) {
    if (cell->next != NULL) {
        cell->next->prev = cell->prev;
    }
    if (cell->prev == NULL) {
        return cell->next;
    }
    cell->prev->next = cell->next;
    return list;
}

GSmemCell* DLAddFront(GSmemCell* list, GSmemCell* cell) {
    cell->next = list;
    cell->prev = 0;
    if (list) {
        list->prev = cell;
    }
    return cell;
}

void* GSmemCell::get_area(GSmemHeapDesc* hd) {
    u32 area;

    area = this->area;
    return (void*)(area + hd->getHeaderSize());
}

void GSmemCell::init(int size) {
    clear();
    this->size = size;
}

void GSmemCell::clear() {
    prev = 0;
    next = NULL;
    size = 0;
}

GSmemCell* GSmemHeapDesc::searchAllocCell(void* ptr) {
    GSmemCell* cell;

    for (cell = allocated; cell != NULL; cell = cell->next_cell()) {
        if (cell->get_area(this) == ptr) {
            return cell;
        }
    }
    return 0;
}

void GSmemHeapDesc::addfreeCellList(GSmemCell* cell) {
    if (cell != NULL && cell->size == 0) {
        cell->prev = 0;
        cell->next = NULL;
        if (!newCellList) {
            newCellList = cell;
        } else {
            newCellList = DLAddFront(newCellList, cell);
        }
    }
}

GSmemCell* GSmemHeapDesc::createNewCell() {
    GSmemCell* cell;

    cell = newCellList;
    if (cell) {
        newCellList = newCellList->next_cell();
        if (newCellList) {
            newCellList->prev = NULL;
        }
    }
    return cell;
}

s32 GSmemHeapDesc::getHeaderSize() {
    if (type == HEAP_TYPE_INTERNAL) {
        return HEADERSIZE;
    }
    return 0;
}
