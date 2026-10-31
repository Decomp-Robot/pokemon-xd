#ifndef GSGFXVIDEO_H
#define GSGFXVIDEO_H
#include <types.h>

typedef void (*GSgfxVideoVSyncCallback)(u32 retraceCount);

#ifdef __cplusplus
extern "C" {
#endif
u8 GSgfxVideoGetVsyncRate(void);
s32 GSgfxVideoGetLockFrameRate(void);
GSgfxVideoVSyncCallback GSgfxVideoSetVSyncCallback(GSgfxVideoVSyncCallback callback);
#ifdef __cplusplus
}
#endif
#endif
