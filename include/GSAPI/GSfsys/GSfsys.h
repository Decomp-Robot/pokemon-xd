#ifndef GSAPI_GSFSYS_H
#define GSAPI_GSFSYS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*GSfsysReadCallback)(long fileMode, void* context, long argument);

/*! What GSfsysGetStatus reports about the work filed on a group. */
#define GSFSYS_STATUS_ERROR (-2) /* file system uninitialized, or the work failed */
#define GSFSYS_STATUS_IDLE (-1) /* the group holds no work */
#define GSFSYS_STATUS_DONE 0 /* the work has run to completion */
#define GSFSYS_STATUS_BUSY 1 /* the work is in flight */

void GSfsysFree(unsigned long groupID);
void GSfsysFreeCache(unsigned long groupID);
u8 GSfsysWait(unsigned long groupID);
u8 GSfsysReadWait(unsigned long groupID);
u8 GSfsysRead(unsigned long groupID);
u8 GSfsysReadID(unsigned long groupID, unsigned long resourceID);
u8 GSfsysPreRead(unsigned long groupID);
u8 GSfsysPreReadWait(unsigned long groupID);
u8 GSfsysReadStreaming(unsigned long groupID);
u8 GSfsysReadStreamingEX(unsigned long groupID, unsigned long resourceID, long argument);
u8 GSfsysPreReadKeepCache(unsigned long groupID);
u8 GSfsysReadIDEX(unsigned long groupID, unsigned long resourceID,
                  GSfsysReadCallback callback, void* context, long argument);
u8 GSfsysReadIDEXWait(unsigned long groupID, unsigned long resourceID,
                      GSfsysReadCallback callback, void* context, long argument);
u8 GSfsysCancel(unsigned long groupID);
u8 GSfsysCancelWait(unsigned long groupID);
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
