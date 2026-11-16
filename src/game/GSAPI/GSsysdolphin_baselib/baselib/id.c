#include <string.h>

#include <types.h>
#include <hsd/assert.h>
#include <hsd/id.h>
#include <hsd/objalloc.h>

#define HSD_ID_TABLE_SIZE 101
#define HSD_ID_NODE_ALIGNMENT 4

typedef struct _HSD_IDData {
    struct _HSD_IDData *next;
    u32 id;
    void *data;
} HSD_IDData;

struct _HSD_IDTable {
    HSD_IDData *table[HSD_ID_TABLE_SIZE];
};

static HSD_IDTable default_table;
static HSD_ObjAllocData alloc_data;

void _HSD_IDForgetMemory(void)
{
    memset(&default_table, 0, sizeof(default_table));
}

void *HSD_IDGetDataFromTable(HSD_IDTable *table, u32 id, int *found)
{
    HSD_IDData *entry;

    if (table == NULL) {
        table = &default_table;
    }

    entry = table->table[id % HSD_ID_TABLE_SIZE];
    while (entry != NULL) {
        if (entry->id == id) {
            if (found != NULL) {
                *found = 1;
            }
            return entry->data;
        }
        entry = entry->next;
    }

    if (found != NULL) {
        *found = 0;
    }
    return NULL;
}

void HSD_IDRemoveByIDFromTable(HSD_IDTable *table, u32 id)
{
    HSD_IDData *entry;
    HSD_IDData *prev;
    u32 bucket;

    if (table == NULL) {
        table = &default_table;
    }

    prev = NULL;
    bucket = id % HSD_ID_TABLE_SIZE;
    entry = table->table[bucket];
    while (entry != NULL) {
        if (entry->id == id) {
            if (prev != NULL) {
                prev->next = entry->next;
            } else {
                table->table[bucket] = entry->next;
            }
            HSD_ObjFree(&alloc_data, entry);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

void HSD_IDInsertToTable(HSD_IDTable *table, u32 id, void *data)
{
    HSD_IDData *entry;
    u32 bucket;

    if (table == NULL) {
        table = &default_table;
    }

    bucket = id % HSD_ID_TABLE_SIZE;
    entry = table->table[bucket];
    while (entry != NULL) {
        if (entry->id == id) {
            break;
        }
        entry = entry->next;
    }

    if (entry != NULL) {
        entry->id = id;
        entry->data = data;
        return;
    }

    entry = HSD_ObjAlloc(&alloc_data);
    HSD_ASSERTLINE(67, entry);
    memset(entry, 0, sizeof(HSD_IDData));
    entry->id = id;
    entry->data = data;
    entry->next = table->table[bucket];
    table->table[bucket] = entry;
}

void HSD_IDSetup(void)
{
    _HSD_IDForgetMemory();
}

void HSD_IDInitAllocData(void)
{
    HSD_ObjAllocInit(&alloc_data, sizeof(HSD_IDData), HSD_ID_NODE_ALIGNMENT);
}

HSD_ObjAllocData *HSD_IDGetAllocData(void)
{
    return &alloc_data;
}
