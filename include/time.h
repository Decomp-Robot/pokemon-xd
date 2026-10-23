#ifndef TIME_H
#define TIME_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Last frame render time in timer units; frozen at 0 while paused.
u32 timeGetLastFrameTime(void);
// Last frame render time in seconds; frozen at 0.0f while paused.
f32 timeGetLastFrameSec(void);

#ifdef __cplusplus
}
#endif

#endif
