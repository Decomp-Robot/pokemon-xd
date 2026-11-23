#include <GSgfx.h>
#include <GSgfxVideo.h>
#include <string.h>
#include <dolphin/os.h>
#include <dolphin/os/OSCache.h>

enum GSgfxVideoXFBState {
    GSGFX_VIDEO_XFB_STATE_UNALLOCATED = 0,
    GSGFX_VIDEO_XFB_STATE_DISPLAY = 1,
    GSGFX_VIDEO_XFB_STATE_READY = 2,
    GSGFX_VIDEO_XFB_STATE_FREE = 3,
    GSGFX_VIDEO_XFB_STATE_COPY = 4
};

struct GSgfxVideoXFB {
    GSgfxVideoXFBState state;
    void *xfb;
};

struct GSgfxVideoState {
    GXRenderModeObj renderMode;
    GSgfxVideoXFB xfb[3];
    GSgfxVideoXFB *current;
    volatile u8 xfbCount;
    u8 vsyncRate;
    u8 nextField;
    u16 efbHeight;
    u16 maxXfbHeight;
    u32 vsyncCount;
    GSgfxVideoMode mode;
    GSgfxVideoVSyncCallback vsyncCallback;
    u32 missedRetraceCount;
    u32 swappedRetraceCount;
    f32 lastRenderTime;
    f32 lastRealRenderTime;
    OSTime lastTime;
    s32 lockFrameRate;
    u8 lockFrameCount;
    u8 lockFrameActive;
    u32 gamma;
    s32 filter;
    f32 yScale;
    GScolor clearColor;
    u32 clearZ;
};

GSgfxVideoState _videoState;
f32 _viewport[6];

static inline GSgfxVideoXFB *_videoFindXFB(void *xfb)
{
    s32 i;

    for (i = 0; i < _videoState.xfbCount; i++) {
        if (_videoState.xfb[i].xfb == xfb) {
            return &_videoState.xfb[i];
        }
    }
    return NULL;
}

u32 GSgfxVideoBuildEFB2XFBCopy(void *commands, void *xfb)
{
    GSgfxCopyDispParams params;
    GSgfxVideoXFB *entry = _videoFindXFB(xfb);
    u8 *cmd = (u8 *)commands;
    u8 *end;
    u32 size;
    u32 aligned;
    u32 pad;

    entry->state = GSGFX_VIDEO_XFB_STATE_COPY;

    params.mLeft = 0;
    params.mTop = 0;
    params.mWidth = 640;
    params.mHeight = 480;
    params.mDestWidth = 640;
    params.mClamp = GX_CLAMP_TOP | GX_CLAMP_BOTTOM;
    params.mCopyMode = GX_COPY_PROGRESSIVE;
    params.mGamma = _videoState.gamma;
    params.mYScale = _videoState.yScale;
    params.mClear = true;
    *(u32 *)&params.mClearColor = _videoState.clearColor.rgba;
    params.mClearZ = _videoState.clearZ;

    end = GSgfx_GCWriteCopyFilterDirect(cmd, _videoState.renderMode.aa,
                                       _videoState.renderMode.sample_pattern[0], true,
                                       _videoState.renderMode.vfilter);
    end = GSgfx_GCWriteCopyDispDirect(end, &params, entry->xfb, true, true, true);

    size = end - cmd;
    aligned = (size + 31) & ~31;
    pad = aligned - size;
    if (pad != 0) {
        memset(end, 0, pad);
    }
    DCFlushRange(cmd, aligned);
    return aligned;
}

GXRenderModeObj *GSgfxVideoGetRenderMode(void)
{
    return &_videoState.renderMode;
}

f32 *GSgfxVideoGetViewport(void)
{
    return _viewport;
}
