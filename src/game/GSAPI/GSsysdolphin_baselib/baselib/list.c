#include <string.h>
#include <GSmemman.h>
#include <hsd/assert.h>
#include <hsd/list.h>

enum {
    HSD_DLIST_NODE_SIZE = 12,
    HSD_LIST_NODE_ALIGNMENT = 4
};

static HSD_ObjAllocData s_alloc_data;
static HSD_ObjAllocData d_alloc_data;

void HSD_ListInitAllocData(void)
{
    HSD_ObjAllocInit(&s_alloc_data, sizeof(HSD_SList), HSD_LIST_NODE_ALIGNMENT);
    HSD_ObjAllocInit(&d_alloc_data, HSD_DLIST_NODE_SIZE, HSD_LIST_NODE_ALIGNMENT);
}

HSD_ObjAllocData *HSD_SListGetAllocData(void)
{
    return &s_alloc_data;
}

HSD_ObjAllocData *HSD_DListGetAllocData(void)
{
    return &d_alloc_data;
}

HSD_SList *HSD_SListAlloc(void)
{
    HSD_SList *list;
    GSmemPoolAllocator *pool = GSmemGetSysdolAllocator(sizeof(HSD_SList));

    if (pool && GSmemGetFreePool(pool) != 0) {
        list = GSmemAllocPool(pool);
        HSD_ASSERTLINE(88, list);
        return list;
    } else {
        list = HSD_ObjAlloc(&s_alloc_data);
        HSD_ASSERTLINE(96, list);
        memset(list, 0, sizeof(HSD_SList));
        return list;
    }
}

HSD_SList *HSD_SListPrepend(HSD_SList *list, void *data)
{
    HSD_SList *prev = HSD_SListAlloc();

    prev->data = data;
    return HSD_SListPrependList(list, prev);
}

HSD_SList *HSD_SListPrependList(HSD_SList *list, HSD_SList *prev)
{
    HSD_ASSERTLINE(231, prev);
    prev->next = list;
    return prev;
}

static inline void freeSList(HSD_SList *list)
{
    GSmemPoolAllocator *pool = GSmemGetSysdolAllocator(sizeof(HSD_SList));

    if (pool && GSmemIsRegistrationPool(pool, list)) {
        GSmemFreePool(pool, list);
    } else {
        HSD_ObjFree(&s_alloc_data, list);
    }
}

HSD_SList *HSD_SListRemove(HSD_SList *list)
{
    HSD_SList *next;

    if (list) {
        next = list->next;
        freeSList(list);
        return next;
    }
    return NULL;
}
