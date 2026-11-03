#include "menuPdaMemoListXD.h"

#include <string.h>

// Sorts the memo-list records by their key, in place.
void pdaMemoList::Sort_Small(MemoSortBuff *entries, long count) {
    MemoSortBuff swap;
    long gap = count / 2;

    while (gap > 0) {
        for (long i = gap; i < count; i++) {
            for (long j = i - gap; j >= 0 && entries[j].key >= entries[j + gap].key; j -= gap) {
                memcpy(&swap, &entries[j], sizeof(MemoSortBuff));
                memcpy(&entries[j], &entries[j + gap], sizeof(MemoSortBuff));
                memcpy(&entries[j + gap], &swap, sizeof(MemoSortBuff));
            }
        }
        gap /= 2;
    }
}
