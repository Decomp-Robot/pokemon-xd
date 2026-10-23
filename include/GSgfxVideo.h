#ifndef GSGFXVIDEO_H
#define GSGFXVIDEO_H
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
u8 GSgfxVideoGetVsyncRate(void);
s32 GSgfxVideoGetLockFrameRate(void);
#ifdef __cplusplus
}
#endif
#endif
