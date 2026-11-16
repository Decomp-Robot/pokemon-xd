#ifndef HSD_OBJALLOC_H
#define HSD_OBJALLOC_H

#include <types.h>

/* Bit position in flags that enables allocation_limit enforcement. */
#define HSD_OBJALLOC_FLAG_LIMIT_BIT 7

typedef struct HSD_ObjAllocData {
    u8 flags;
    u8 unknown_01[7];
    u32 allocated_count;
    u32 unknown_0C;
    u32 peak_count;
    u32 allocation_limit;
    u32 unknown_18;
    u32 unknown_1C;
    u32 object_size;
    u32 alignment;
    struct HSD_ObjAllocData *next;
} HSD_ObjAllocData;

#ifdef __cplusplus
extern "C" {
#endif

void HSD_ObjAllocInit(HSD_ObjAllocData *data, u32 size, u32 align);
void *HSD_ObjAlloc(HSD_ObjAllocData *data);
void HSD_ObjFree(HSD_ObjAllocData *data, void *object);
void HSD_ObjSetHeap(u32 heap_size, u32 heap_start);
void _HSD_ObjAllocForgetMemory(void);

#ifdef __cplusplus
}
#endif

#endif
