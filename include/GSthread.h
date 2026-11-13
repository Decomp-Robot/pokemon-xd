#ifndef GSTHREAD_H
#define GSTHREAD_H

#include <types.h>

typedef struct GSthread GSthread;

#ifdef __cplusplus
extern "C" {
#endif
void GSthreadUnblockGroup(unsigned long group);
void GSthreadBlockGroup(unsigned long group);

typedef void (*GSthreadEntry)(void);

void GSthreadUnblock(GSthread *thread);
void GSthreadBlock(GSthread *thread);
void GSthreadSetGroupID(GSthread *thread, u32 groupId);
GSthread *GSthreadGetCurrentThread(void);
GSthread *GSthreadCreate(s32 priority, u32 groupId, u32 stackSize, s32 saveFpr,
                         s32 terminateOnExit, GSthreadEntry entry);
void GSthreadSetArgs(GSthread *thread, s32 count, ...);
void GSthreadSetName(GSthread *thread, const char *name);
void GSthreadTerminate(GSthread *thread);
void GSthreadSwitch(void);
long GSthreadClose(GSthread *thread);
u8 GSthreadIsRunning(GSthread *thread);

#ifdef __cplusplus
}
#endif

#endif
