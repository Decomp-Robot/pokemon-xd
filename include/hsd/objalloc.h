#ifndef HSD_OBJALLOC_H
#define HSD_OBJALLOC_H

#include <types.h>

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

void HSD_ObjAllocInit(HSD_ObjAllocData *data, u32 object_size, u32 alignment);
void *HSD_ObjAlloc(HSD_ObjAllocData *data);
void HSD_ObjFree(HSD_ObjAllocData *data, void *object);

#ifdef __cplusplus
}
#endif

#endif
