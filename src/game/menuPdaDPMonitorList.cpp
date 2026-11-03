#include <types.h>

#include <string.h>

// One Dark Pokemon monitor entry while it is being sorted. The key is the
// value the currently selected sort orders by, and the two IDs identify the
// entry so the sorted order can be read back after sorting.
struct SnatchSortBuff {
    long key;
    u16 pokemonID;
    u16 darkPokemonID;
};

// The Dark Pokemon monitor's entry list, which orders its records by the
// currently selected sort key.
class menuPdaDPMList {
public:
    void SortSub2(SnatchSortBuff *entries, long count, bool ascending);
    void SortSub(SnatchSortBuff *entries, long count, bool ascending);
};

// Inserts every record into the already sorted prefix of the list.
void menuPdaDPMList::SortSub2(SnatchSortBuff *entries, long count, bool ascending) {
    SnatchSortBuff temp;
    long i;
    long j;
    if (ascending) {
        for (i = 1; i < count; i++) {
            memcpy(&temp, &entries[i], sizeof(SnatchSortBuff));
            for (j = i - 1; j >= 0 && entries[j].key > temp.key; j--) {
                memcpy(&entries[j + 1], &entries[j], sizeof(SnatchSortBuff));
            }
            memcpy(&entries[j + 1], &temp, sizeof(SnatchSortBuff));
        }
    } else {
        for (i = 1; i < count; i++) {
            memcpy(&temp, &entries[i], sizeof(SnatchSortBuff));
            for (j = i - 1; j >= 0 && entries[j].key < temp.key; j--) {
                memcpy(&entries[j + 1], &entries[j], sizeof(SnatchSortBuff));
            }
            memcpy(&entries[j + 1], &temp, sizeof(SnatchSortBuff));
        }
    }
}

// Orders the records with Shell sort, repeatedly halving the gap down to one.
void menuPdaDPMList::SortSub(SnatchSortBuff *entries, long count, bool ascending) {
    SnatchSortBuff temp;
    long gap;
    long i;
    long j;
    if (ascending) {
        gap = count / 2;
        while (gap > 0) {
            for (i = gap; i < count; i++) {
                for (j = i - gap; j >= 0 && entries[j].key > entries[j + gap].key; j -= gap) {
                    memcpy(&temp, &entries[j], sizeof(SnatchSortBuff));
                    memcpy(&entries[j], &entries[j + gap], sizeof(SnatchSortBuff));
                    memcpy(&entries[j + gap], &temp, sizeof(SnatchSortBuff));
                }
            }
            gap /= 2;
        }
    } else {
        gap = count / 2;
        while (gap > 0) {
            for (i = gap; i < count; i++) {
                for (j = i - gap; j >= 0 && entries[j].key < entries[j + gap].key; j -= gap) {
                    memcpy(&temp, &entries[j], sizeof(SnatchSortBuff));
                    memcpy(&entries[j], &entries[j + gap], sizeof(SnatchSortBuff));
                    memcpy(&entries[j + gap], &temp, sizeof(SnatchSortBuff));
                }
            }
            gap /= 2;
        }
    }
}
