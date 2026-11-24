#ifndef GSTHREAD_H
#define GSTHREAD_H

#include <types.h>
#include <bool.h>

typedef struct GSthread GSthread;

#ifdef __cplusplus
extern "C" {
#endif
void GSthreadUnblockGroup(u32 groupId);
void GSthreadBlockGroup(u32 groupId);

typedef void (*GSthreadEntry)(void);

void GSthreadUnblock(GSthread *thread);
void GSthreadBlock(GSthread *thread);
void GSthreadSetGroupID(GSthread *thread, u32 groupId);
GSthread *GSthreadGetCurrentThread(void);
GSthread *GSthreadCreate(s32 priority, u32 groupId, u32 stackSize, u8 usesFPU,
                         s32 terminateOnExit, GSthreadEntry entry);
void GSthreadSetArgs(GSthread *thread, s32 count, ...);
void GSthreadSetName(GSthread *thread, const char *name);
void GSthreadTerminate(GSthread *thread);
void GSthreadTerminateGroup(u32 groupId);
void GSthreadSwitch(void);
long GSthreadClose(GSthread *thread);
bool GSthreadIsRunning(GSthread *thread);
u32 GSthreadInit(u32 maxThreads);
void GSthreadExecuteAll(void);
void GSthreadExecuteGroup(u32 groupId);

#ifdef __cplusplus
}
#endif

#endif
