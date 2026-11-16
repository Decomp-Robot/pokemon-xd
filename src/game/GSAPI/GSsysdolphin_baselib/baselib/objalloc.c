#include <string.h>

#include <hsd/assert.h>
#include <hsd/memory.h>
#include <hsd/objalloc.h>
#include <hsd/util.h>

u32 obj_heap[4] = { 0, 0, -1, -1 };

static HSD_ObjAllocData *alloc_datas;

void _HSD_ObjAllocForgetMemory(void)
{
    alloc_datas = NULL;
}

void HSD_ObjAllocInit(HSD_ObjAllocData *data, u32 size, u32 align)
{
    HSD_ObjAllocData **link;
    HSD_ObjAllocData *cur;

    HSD_ASSERTLINE(381, data);

    if (data != NULL) {
        link = &alloc_datas;
        while ((cur = *link) != NULL) {
            if (cur == data) {
                *link = cur->next;
            } else {
                link = &cur->next;
            }
        }
    } else {
        alloc_datas = NULL;
    }

    memset(data, 0, sizeof(HSD_ObjAllocData));
    data->allocation_limit = -1;
    data->unknown_18 = 0;
    data->unknown_1C = -1;

    HSD_ASSERTLINE(392, align <= 32);
    HSD_ASSERTLINE(393, HSD_GetNbBits(align) == 1);

    data->alignment = align;
    data->object_size = (size + data->alignment - 1) & ~(data->alignment - 1);
    data->next = alloc_datas;
    alloc_datas = data;
}

void HSD_ObjFree(HSD_ObjAllocData *data, void *object)
{
    data->allocated_count--;
    HSD_Free(object);
}

void *HSD_ObjAlloc(HSD_ObjAllocData *data)
{
    if (((data->flags >> HSD_OBJALLOC_FLAG_LIMIT_BIT) & 1) && data->allocated_count >= data->allocation_limit) {
        return NULL;
    }

    data->allocated_count++;
    if (data->allocated_count > data->peak_count) {
        data->peak_count = data->allocated_count;
    }
    return HSD_Alloc(data->object_size);
}

void HSD_ObjSetHeap(u32 heap_size, u32 heap_start)
{
    obj_heap[0] = obj_heap[1] = heap_start;
    obj_heap[2] = obj_heap[3] = heap_size;
}
