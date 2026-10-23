#ifndef GSTHREAD_H
#define GSTHREAD_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
void GSthreadSwitch(void);
u32 GSthreadCreate(s32 priority, u32 groupId, u32 stackSize, s32 useStack, s32 attributes, void (*entry)(void));
#ifdef __cplusplus
}
#endif
#endif
