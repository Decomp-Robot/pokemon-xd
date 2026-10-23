#ifndef GS_FSYS_H
#define GS_FSYS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GSfsysCompletionFunc)(long requestType, void *context,
                                   long callbackArgument);

void GSfsysFree(u32 archiveID);
void GSfsysPreRead(long archiveID);
u8 GSfsysWait(long archiveID);
void *GSfsysOSLinkPostFunc(u32 group, u32 res, u32 size);
// Waits for a read; false indicates failure or an unavailable resource.
bool GSfsysReadIDEXWait(u32 group, u32 res, GSfsysCompletionFunc completionFunc,
                      void *context, long callbackArgument);

#ifdef __cplusplus
}
#endif

#endif
