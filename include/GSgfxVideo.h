#ifndef GSGFXVIDEO_H
#define GSGFXVIDEO_H
#include <types.h>

typedef void (*GSgfxVideoVSyncCallback)(u32 retraceCount);

/* TV standard, indexed in the same order as the GX render mode table. */
enum GSgfxVideoMode {
    GSGFX_VIDEO_MODE_NTSC = 0,
    GSGFX_VIDEO_MODE_PAL = 1,
    GSGFX_VIDEO_MODE_EURGB60 = 2,
    GSGFX_VIDEO_MODE_MPAL = 3,
};

/* Vertical resolution class of the selected render mode. */
enum GSgfxVideoFrame {
    GSGFX_VIDEO_FRAME_240 = 0,
    GSGFX_VIDEO_FRAME_480 = 1,
};

/* Extra filtering stage requested for the selected render mode. */
enum GSgfxVideoFilter {
    GSGFX_VIDEO_FILTER_NONE = 0,
    GSGFX_VIDEO_FILTER_DEFLICKER = 1,
    GSGFX_VIDEO_FILTER_ANTIALIAS = 2,
};

#ifdef __cplusplus
extern "C" {
#endif
u8 GSgfxVideoGetVsyncRate(void);
s32 GSgfxVideoGetLockFrameRate(void);
GSgfxVideoVSyncCallback GSgfxVideoSetVSyncCallback(GSgfxVideoVSyncCallback callback);
bool GSgfxVideoSetMode(GSgfxVideoMode mode, GSgfxVideoFrame frame, bool interlace,
                       GSgfxVideoFilter filter, bool progressive);
void GSgfxVideoSetFilter(u8 *filter);
u8 GSgfxVideoGetLockFrameActive(void);
void GSgfxVideoSetLockFrameActive(bool active);
#ifdef __cplusplus
}
#endif
#endif
