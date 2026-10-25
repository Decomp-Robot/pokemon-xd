#ifndef _DOLPHIN_AR_H_
#define _DOLPHIN_AR_H_

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ARQCallback)(u32 pointerToARQRequest);

struct ARQRequest {
    struct ARQRequest *next;
    u32 owner;
    u32 type;
    u32 priority;
    u32 source;
    u32 dest;
    u32 length;
    ARQCallback callback;
};

#define ARQ_DMA_ALIGNMENT 32

#define ARAM_DIR_MRAM_TO_ARAM 0
#define ARAM_DIR_ARAM_TO_MRAM 1

#define ARStartDMARead(mmem, aram, len) \
    ARStartDMA(ARAM_DIR_ARAM_TO_MRAM, mmem, aram, len)
#define ARStartDMAWrite(mmem, aram, len) \
    ARStartDMA(ARAM_DIR_MRAM_TO_ARAM, mmem, aram, len)

typedef struct ARQRequest ARQRequest;

#define ARQ_TYPE_MRAM_TO_ARAM ARAM_DIR_MRAM_TO_ARAM
#define ARQ_TYPE_ARAM_TO_MRAM ARAM_DIR_ARAM_TO_MRAM

#define ARQ_PRIORITY_LOW  0
#define ARQ_PRIORITY_HIGH 1

// AR
typedef void (*ARCallback)(void);

ARCallback ARRegisterDMACallback(ARCallback callback);
u32 ARGetDMAStatus(void);
void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length);
u32 ARAlloc(u32 length);
u32 ARFree(u32* length);
BOOL ARCheckInit(void);
u32 ARInit(u32* stack_index_addr, u32 num_entries);
void ARReset(void);
void ARSetSize(void);
u32 ARGetBaseAddress(void);
u32 ARGetSize(void);
u32 ARGetInternalSize(void);

enum {
    AR_CLEAR_INTERNAL_ALL = 0,
    AR_CLEAR_INTERNAL_USER = 1,
    AR_CLEAR_EXPANSION = 2
};

void ARClear(u32 flag);

// ARQ
void ARQInit(void);
void ARQReset(void);
void ARQPostRequest(ARQRequest* request, u32 owner, u32 type, u32 priority, u32 source, u32 dest, u32 length, ARQCallback callback);
void ARQRemoveRequest(ARQRequest* request);
void ARQRemoveOwnerRequest(u32 owner);
void ARQFlushQueue(void);
void ARQSetChunkSize(u32 size);
u32 ARQGetChunkSize(void);
BOOL ARQCheckInit(void);

#ifdef __cplusplus
}
#endif

#endif
