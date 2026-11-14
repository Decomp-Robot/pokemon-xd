#ifndef GS_SCRATCH_H
#define GS_SCRATCH_H

#include <types.h>

typedef enum GSscratchNotify {
    GS_SCRATCH_INVALID = 0,
    GS_SCRATCH_VALID = 1
} GSscratchNotify;

typedef void (*GSscratchNotifyCallback)(GSscratchNotify notify, void* ptr, u8 blockCount);

typedef enum GSscratchStoreResult {
    GS_SCRATCH_STORE_OK = 0,
    GS_SCRATCH_STORE_QUEUE_FULL = 1,
    GS_SCRATCH_STORE_UNALIGNED = 2
} GSscratchStoreResult;

#ifdef __cplusplus
extern "C" {
#endif

void GSscratchSetValid(void);
void GSscratchSetInvalid(void);
u8 GSscratchIsPtr(void* ptr);
void GSscratchWaitForCompletion(void);
GSscratchStoreResult GSscratchStore(void* dest, void* src, u32 size);
void GSscratchFree(void* ptr);
void* GSscratchAlloc(u8 blockCount, GSscratchNotifyCallback notify);
u32 GSscratchInit(u8 stackBlocks);

#ifdef __cplusplus
}
#endif

#endif
