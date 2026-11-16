#ifndef HSD_MEMORY_H
#define HSD_MEMORY_H

#include <types.h>

typedef struct _HSD_MemCallbacks {
    void *(*alloc)(u32 size, s32 alignment, s32 type);
    void (*free)(void *ptr);
    void (*clear)(void);
    s32 (*get_remain)(void);
    s32 (*check_own)(void *ptr);
} HSD_MemCallbacks;

#ifdef __cplusplus
extern "C" {
#endif

void _HSD_MemSetCallbacks(HSD_MemCallbacks *callbacks, u32 size);
s32 _HSD_MemCheckOwn(void *memory);
void *HSD_Alloc(u32 size);
void HSD_Free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
