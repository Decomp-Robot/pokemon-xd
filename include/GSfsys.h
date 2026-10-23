#ifndef GS_FSYS_H
#define GS_FSYS_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif
long GSfsysGetDriveStatus();
long GSfsysGetStatus(unsigned long fileNo);
void GSfsysStreamSetLoopFlag(unsigned long fileNo, unsigned long arg, unsigned long loopFlag);
void GSfsysStreamSetLoopPoint(unsigned long fileNo, unsigned long arg, unsigned long loopStart, unsigned long loopEnd);

void GSfsysFree(u32 fileID);
void GSfsysFreeCache(u16 groupId);
u8 GSfsysWait(u32 fileID);
void GSfsysReadID(u32 fileID, u32 resourceID);
void GSfsysPreRead(u32 fileID);
void GSfsysPreReadWait(u32 fileID);
void GSfsysReadWait(u32 fileID);

typedef void (*GSfsysCompletionFunc)(long requestType, void *context,
                                     long callbackArgument);
void *GSfsysOSLinkPostFunc(u32 group, u32 res, u32 size);
#ifdef __cplusplus
bool
#else
u8
#endif
GSfsysReadIDEXWait(u32 group, u32 res,
                        GSfsysCompletionFunc completionFunc, void *context,
                        long callbackArgument);
#ifdef __cplusplus
}
#endif
#endif
