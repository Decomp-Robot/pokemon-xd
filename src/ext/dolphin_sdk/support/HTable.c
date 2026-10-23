#include <charPipeline/structures/HTable.h>
#include <charPipeline/structures/List.h>

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
void* DSNextHTableObj(DSHashTable* hTable, Ptr obj) {
    s32 currentIndex;
    void* cursor;

    if (!hTable) {
        return NULL;
    }

    if (!obj) {
        currentIndex = 0;
        cursor = DSNextListObj(&hTable->table[currentIndex], NULL);
    } else {
        currentIndex = DSHTableIndex(hTable, obj);
        if (currentIndex == -1) {
            return NULL;
        }
        cursor = DSNextListObj(&hTable->table[currentIndex], obj);
    }

    while (cursor == NULL && currentIndex < hTable->tableSize - 1) {
        currentIndex++;
        cursor = DSNextListObj(&hTable->table[currentIndex], NULL);
    }
    return cursor;
}

// UNUSED: the game never calls this; dead-stripped at link.
s32 DSHTableIndex(DSHashTable* hTable, Ptr obj) {
    if (!hTable || !obj) {
        return -1;
    }
    return hTable->hash(obj);
}

// UNUSED: the game never calls this; dead-stripped at link.
void* DSHTableHead(DSHashTable* hTable, s32 index) {
    if (index < 0 || index >= hTable->tableSize) {
        return NULL;
    }
    return DSNextListObj(&hTable->table[index], NULL);
}
