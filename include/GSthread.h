#ifndef GS_THREAD_H
#define GS_THREAD_H
#include "types.h"
typedef struct GSthread GSthread;
// Entries run from a saved register context; SetArgs supplies argument words.
// Cast other entry signatures at registration; no owning-thread argument is supplied.
typedef s32 (*GSthreadEntry)(GSthread *argument);
#ifdef __cplusplus
extern "C" {
#endif
GSthread *GSthreadCreate(u32 priority, u32 group, u32 stackSize, u32 useFPU, u32 autoTerminate, GSthreadEntry entry);
void GSthreadSetArgs(GSthread *thread, s32 argumentCount, ...);
void GSthreadTerminate(GSthread *thread);
void GSthreadSwitch(void);
#ifdef __cplusplus
}
#endif
#endif
