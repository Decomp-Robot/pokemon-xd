#ifndef GSGFXVIDEO_H
#define GSGFXVIDEO_H
#include <types.h>

union GScolor;
typedef struct _GXRenderModeObj GXRenderModeObj;

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
void GSgfxVideoGetEFBSize(u16 *width, u16 *height);
u8 GSgfxVideoGetVsyncRate(void);
u32 GSgfxVideoGetVsyncCount(void);
u8 GSgfxVideoGetXFBCount(void);
f32 GSgfxVideoGetLastRealRenderTime(void);
f32 GSgfxVideoGetLastRenderTime(void);
s32 GSgfxVideoGetLockFrameRate(void);
GSgfxVideoVSyncCallback GSgfxVideoSetVSyncCallback(GSgfxVideoVSyncCallback callback);
bool GSgfxVideoSetMode(GSgfxVideoMode mode, GSgfxVideoFrame frame, bool interlace,
                       GSgfxVideoFilter filter, bool progressive);
void GSgfxVideoSetFilter(u8 *filter);
u8 GSgfxVideoGetLockFrameActive(void);
void GSgfxVideoSetLockFrameActive(bool active);
void GSgfxVideoGetClearColor(GScolor *color);
void GSgfxVideoSetClearColor(const GScolor *color);
f32 *GSgfxVideoGetViewport(void);
#ifdef __cplusplus
}
GXRenderModeObj *GSgfxVideoGetRenderMode(void);
void GSgfxVideoResetViewport(bool enable);
void GSgfxVideoSetupNextFrame(void);
void *GSgfxVideoCopyEFBToXFB(void);
u32 GSgfxVideoBuildEFB2XFBCopy(void *commands, void *xfb);
void *GSgfxVideoFindFreeXFB(bool wait);
bool GSgfxVideoSetXFBReady(void *xfb);
void GSgfxVideoSetOneShotPostCallback(void (*callback)(void *));
#endif
#endif
