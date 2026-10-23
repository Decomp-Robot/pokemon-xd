#ifndef GSAPI_GSFSYS_H
#define GSAPI_GSFSYS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GSfsysReadCallback)(long fileMode, void* context, long argument);

void GSfsysFree(unsigned long groupID);
void GSfsysFreeCache(unsigned long groupID);
u8 GSfsysWait(unsigned long groupID);
u8 GSfsysReadWait(unsigned long groupID);
u8 GSfsysReadID(unsigned long groupID, unsigned long resourceID);
u8 GSfsysPreRead(unsigned long groupID);
u8 GSfsysPreReadWait(unsigned long groupID);
u8 GSfsysReadIDEX(unsigned long groupID, unsigned long resourceID,
                  GSfsysReadCallback callback, void* context, long argument);
u8 GSfsysReadIDEXWait(unsigned long groupID, unsigned long resourceID,
                      GSfsysReadCallback callback, void* context, long argument);
void* GSfsysOSLinkPostFunc(unsigned long groupID, unsigned long resourceID,
                          unsigned long size);
long GSfsysGetStatus(unsigned long groupID);
void GSfsysStreamSetLoopFlag(unsigned long groupID, unsigned long resourceID, u8 loopFlag);
void GSfsysStreamSetLoopPoint(unsigned long groupID, unsigned long resourceID,
                              unsigned long loopStart, unsigned long loopEnd);
unsigned long GSfsysCreateCacheHeap(void);

#ifdef __cplusplus
}
#endif

#endif
