#ifndef HSD_MEMORY_H
#define HSD_MEMORY_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 _HSD_MemCheckOwn(void *memory);
void *HSD_Alloc(u32 size);
void HSD_Free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
