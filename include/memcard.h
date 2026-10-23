#ifndef MEMCARD_H
#define MEMCARD_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
s32 memcardTaskCall(s32 task, s32 mode, s32 argument);
void memcardSetupTask(long task, long mode, long argument);
u8 memcardCheckTaskWaitGBA();
void memcardUnlockWaitGBA(u8 abort);
long memcardGetTaskResult();
void memcardFinalizeTask();


void *memcardMemAllocForSavedata(void);
void *memcardMemFreeForSavedata(void *save);

#ifdef __cplusplus
}
#endif

#endif
