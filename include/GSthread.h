#ifndef GSTHREAD_H
#define GSTHREAD_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GSthreadEntry)(void);

u32 GSthreadCreate(s32 priority, u32 groupId, u32 stackSize, s32 saveFpr,
                   s32 terminateOnExit, GSthreadEntry entry);
void GSthreadSetArgs(u32 threadId, s32 count, ...);
void GSthreadSetName(u32 threadId, const char *name);
void GSthreadTerminate(u32 threadId);
void GSthreadSwitch(void);

#ifdef __cplusplus
}
#endif

#endif
