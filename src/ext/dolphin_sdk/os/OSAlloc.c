#include <dolphin.h>
#include <dolphin/os.h>

#define HEADERSIZE 32u

typedef struct Cell Cell;
struct Cell {
    Cell* prev;
    Cell* next;
    s32 size;
};

u32 OSReferentSize(void* ptr) {
    Cell* cell;

    cell = (void*)((u32)ptr - HEADERSIZE);
    return (s32)((u32)cell->size - HEADERSIZE);
}
