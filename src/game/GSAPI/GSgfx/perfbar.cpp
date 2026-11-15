#include <GSgfx.h>
#include <GSgfxPerf.h>
#include <GSgfxVideo.h>
#include <GSprim.h>
#include <GSmem.h>
#include <string.h>
#include <GSmtx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>

#include <dolphin/gx/GXTransform.h>
#include <dolphin/gx/GXDispList.h>
#include <dolphin/gx/GXManage.h>
#include <dolphin/perf.h>

enum {
    GSGFX_PERFBAR_DISABLED = 0,
    GSGFX_PERFBAR_TIMING = 1,
    GSGFX_PERFBAR_SDK_PERF = 2
};

#define GSGFX_PERFBAR_CPU_ENTRY_MAX 1024
#define GSGFX_PERFBAR_GPU_ENTRY_MAX 64
#define GSGFX_PERFBAR_DLIST_SIZE 32768

#define GSGFX_PERFBAR_COLOR_WHITE  0xFFFFFF
#define GSGFX_PERFBAR_COLOR_CYAN   0x00FFFF
#define GSGFX_PERFBAR_COLOR_TEAL   0x008080
#define GSGFX_PERFBAR_COLOR_GREEN  0x00FF00
#define GSGFX_PERFBAR_COLOR_YELLOW 0xFFFF00
#define GSGFX_PERFBAR_COLOR_RED    0xFF0000

struct GSgfxPerfCPUFrame {
    OSTime start;
    f32 total;
    u32 count;
    f32 time[GSGFX_PERFBAR_CPU_ENTRY_MAX];
    u32 color[GSGFX_PERFBAR_CPU_ENTRY_MAX];
};

struct GSgfxPerfGPUFrame {
    OSTime start;
    f32 total;
    u32 count;
    f32 time[GSGFX_PERFBAR_GPU_ENTRY_MAX];
    u32 color[GSGFX_PERFBAR_GPU_ENTRY_MAX];
};

struct GSgfxPerfFrame {
    GSgfxPerfCPUFrame cpu;
    GSgfxPerfGPUFrame gpu;
    f32 renderTime;
};

extern "C" void PERFSetDrawSyncCallback(void (*callback)(u16));
extern "C" const f32 __float_epsilon[];

GSgfxPerfFrame _perfLastFrame;
GSgfxPerfFrame _perfCurFrame;
Mtx44 _perfPERFMtx;

s32 _perfRenderMode = GSGFX_PERFBAR_TIMING;
GSgfxColAu8 _perfBkgnd = { 0x20, 0x20, 0x20, 0xC0 };

u8 _perfFrameCount;
f32 _perfPixelSkip;
f32 _perfPeakCPU;
f32 _perfPeakGPU;
s32 _perfPeakCPUTimer;
s32 _perfPeakGPUTimer;
s32 _perfUpdateCount;
f32 _perfUpdateTime;
f32 _perfUpdateFPS;
void *_perfDisplayList;
u32 _perfDListSize;
u8 _perfPERFActive;
u16 _perfDListToken;

void GSgfxPerfbarRender(GSgfxLayer *layer)
{
    OSTime start;
    Mtx44 ortho;
    f32 x;
    f32 nextX;
    f32 maxTime;
    f32 prev;
    f32 cur;
    u32 i;
    int line;
    u32 color;
    GSgfxColAu8 c;
    long gpTick;
    long bpTick;

    if (_perfRenderMode == GSGFX_PERFBAR_DISABLED) {
        return;
    }

    start = OSGetTime();

    _perfLastFrame.cpu.total += _perfLastFrame.renderTime;
    if (_perfLastFrame.cpu.total > _perfPeakCPU || _perfPeakCPUTimer-- < 0) {
        _perfPeakCPU = _perfLastFrame.cpu.total;
        _perfPeakCPUTimer = 60;
    }
    if (_perfLastFrame.gpu.total > _perfPeakGPU || _perfPeakGPUTimer-- < 0) {
        _perfPeakGPU = _perfLastFrame.gpu.total;
        _perfPeakGPUTimer = 60;
    }

    if (_perfUpdateCount-- < 0) {
        _perfUpdateTime = GSgfxVideoGetLastRenderTime();
        if (_perfUpdateTime > __float_epsilon[0]) {
            _perfUpdateFPS = 0.1f + GSgfxVideoGetVsyncRate() / _perfUpdateTime;
            if (_perfUpdateFPS > 60.0f) {
                _perfUpdateFPS = 60.0f;
            }
        } else {
            _perfUpdateFPS = 0.0f;
        }
        _perfUpdateCount = 6;
    }

    GSprimSetRenderLayer(layer);
    GSprimSetQuadVertexOrder(GS_PRIM_VERTEX_ORDER_TRIANGLE_STRIP);
    GSprimSetLineWidth(1.0f);
    GSprimSetTextSize(10.0f, 10.0f);
    GSprimSetTextWidth(1.0f);

    layer->blendMode(GSGFX_BLEND_BLEND, GSGFX_BLEND_SRC_ALPHA, GSGFX_BLEND_INV_SRC_ALPHA, false);
    GSmtx44Ortho(ortho, 0.0f, 480.0f, 0.0f, 640.0f, 0.0f, -30000.0f);
    layer->orthographicProjection(ortho);
    layer->mMatrixStack->identity();
    layer->projectionDepthRange(0.0f, 30000.0f);
    layer->pixelMode(true, false, false);
    layer->zCompare(false, GSGFX_Z_COMPARE_LEQUAL);
    layer->alphaCompare(false, GSGFX_A_COMPARE_ALWAYS, 0, GSGFX_ALPHA_AND, GSGFX_A_COMPARE_ALWAYS, 0);
    layer->cullMode(GSGFX_CULL_NONE);
    layer->clipMode(GSGFX_CLIP_DISABLED);

    GSprimDraw2DQuadWH(_perfBkgnd, 10.0f, 420.0f, 620.0f, 40.0f);

    x = 20.0f;
    for (line = 0; line < _perfFrameCount; line++) {
        if (line < (u32)(GSgfxVideoGetLockFrameRate() + 1)) {
            color = GSGFX_PERFBAR_COLOR_GREEN;
        } else if (line >= 3) {
            color = GSGFX_PERFBAR_COLOR_RED;
        } else {
            color = GSGFX_PERFBAR_COLOR_YELLOW;
        }
        c = GSgfxPerfColorToGSgfxCol(color);
        nextX = x + _perfPixelSkip;
        GSprimDraw2DHLine(c, x, nextX, 455.0f);
        GSprimDraw2DVLine(c, x, 450.0f, 455.0f);
        GSprimDraw2DVLine(c, nextX, 450.0f, 455.0f);
        x = nextX;
    }

    maxTime = _perfLastFrame.cpu.total;
    if (_perfLastFrame.gpu.total > maxTime) {
        maxTime = _perfLastFrame.gpu.total;
    }
    GSprimDraw2DQuadWH(GSgfxPerfColorToGSgfxCol(GSGFX_PERFBAR_COLOR_TEAL), 20.0f, 446.0f,
                       _perfPixelSkip * (int)(0.9999f + maxTime), 5.0f);

    GSprimDraw2DQuadWH(GSgfxPerfColorToGSgfxCol(GSGFX_PERFBAR_COLOR_CYAN), 20.0f, 446.0f,
                       _perfPixelSkip * _perfLastFrame.gpu.total, 5.0f);
    prev = _perfPixelSkip * _perfLastFrame.gpu.time[0];
    for (i = 1; i < _perfLastFrame.gpu.count; i++) {
        cur = _perfPixelSkip * _perfLastFrame.gpu.time[i];
        if ((int)(cur - prev) >= 1) {
            GSprimDraw2DQuadWH(GSgfxPerfColorToGSgfxCol(_perfLastFrame.gpu.color[i - 1]),
                               20.0f + prev, 446.0f, cur - prev, 5.0f);
        }
        prev = cur;
        if (cur > 640.0f) {
            break;
        }
    }

    GSprimDraw2DQuadWH(GSgfxPerfColorToGSgfxCol(GSGFX_PERFBAR_COLOR_WHITE), 20.0f, 440.0f,
                       _perfPixelSkip * _perfLastFrame.cpu.total, 5.0f);
    prev = _perfPixelSkip * _perfLastFrame.cpu.time[0];
    for (i = 1; i < _perfLastFrame.cpu.count; i++) {
        cur = _perfPixelSkip * _perfLastFrame.cpu.time[i];
        if ((int)(cur - prev) >= 1) {
            GSprimDraw2DQuadWH(GSgfxPerfColorToGSgfxCol(_perfLastFrame.cpu.color[i - 1]),
                               20.0f + prev, 440.0f, cur - prev, 5.0f);
        }
        prev = cur;
        if (cur > 640.0f) {
            break;
        }
    }

    GSgfxGPUGetBPTick(&gpTick, &bpTick);
    if (gpTick == -1 && bpTick == -1) {
        GSprimWriteStr(30.0f, 425.0f,
                       "|00ff00CUR: |ffffff%2.3f |00ffff%2.3f  |00ff00PK: |ffffff%2.3f |00ffff%2.3f  |00ff00VUPD: |ffffff%1.3f  |ffff00%2.1f FPS",
                       _perfLastFrame.cpu.total, _perfLastFrame.gpu.total, _perfPeakCPU, _perfPeakGPU,
                       _perfUpdateTime, _perfUpdateFPS);
    } else {
        GSprimWriteStr(30.0f, 425.0f, "|ffffff%d    |ffff00%d",
                       OSTicksToMicroseconds(gpTick), OSTicksToMicroseconds(bpTick));
    }

    if (_perfRenderMode == GSGFX_PERFBAR_SDK_PERF && _perfDListSize != 0) {
        layer->FlushState();
        PERFPreDraw();
        GXSetProjection(_perfPERFMtx, GX_ORTHOGRAPHIC);
        GXCallDisplayList(_perfDisplayList, _perfDListSize);
        PERFPostDraw();
        layer->InvalidateState();
        _perfDListToken = layer->tokenMark();
    }

    _perfCurFrame.renderTime = (f32)(OSGetTime() - start) / (OS_TIMER_CLOCK / 60);
    _perfLastFrame.renderTime = _perfCurFrame.renderTime;
}

void _perfRestoreGPU(void);

u32 GSgfxPerfbarGPUEntry(u32 color)
{
    u32 prevColor;

    if (_perfCurFrame.gpu.count >= GSGFX_PERFBAR_GPU_ENTRY_MAX) {
        return 0;
    }

    if (_perfCurFrame.cpu.count == 0) {
        prevColor = GSGFX_PERFBAR_COLOR_CYAN;
    } else {
        prevColor = _perfCurFrame.gpu.color[_perfCurFrame.gpu.count - 1];
    }

    _perfCurFrame.gpu.time[_perfCurFrame.gpu.count] =
        (f32)(OSGetTime() - _perfCurFrame.gpu.start) / (OS_TIMER_CLOCK / 60);
    _perfCurFrame.gpu.color[_perfCurFrame.gpu.count] = color;
    _perfCurFrame.gpu.count++;
    return prevColor;
}

u32 GSgfxPerfbarCPUEntry(u32 color)
{
    u32 prevColor;

    if (_perfCurFrame.cpu.count >= GSGFX_PERFBAR_CPU_ENTRY_MAX) {
        return 0;
    }

    if (_perfCurFrame.cpu.count == 0) {
        prevColor = GSGFX_PERFBAR_COLOR_WHITE;
    } else {
        prevColor = _perfCurFrame.cpu.color[_perfCurFrame.cpu.count - 1];
    }

    _perfCurFrame.cpu.time[_perfCurFrame.cpu.count] =
        (f32)(OSGetTime() - _perfCurFrame.cpu.start) / (OS_TIMER_CLOCK / 60);
    _perfCurFrame.cpu.color[_perfCurFrame.cpu.count] = color;
    _perfCurFrame.cpu.count++;
    return prevColor;
}

void GSgfxPerfbarGPUEnd(void)
{
    GSgfxPerfbarGPUEntry(0);
    _perfCurFrame.gpu.total = (f32)(OSGetTime() - _perfCurFrame.gpu.start) / (OS_TIMER_CLOCK / 60);
    memcpy(&_perfLastFrame.gpu, &_perfCurFrame.gpu, sizeof(GSgfxPerfGPUFrame));
    memset(&_perfCurFrame.gpu, 0, sizeof(GSgfxPerfGPUFrame));
}

void GSgfxPerfbarGPUStart(void)
{
    _perfCurFrame.gpu.start = OSGetTime();
    _perfCurFrame.gpu.count = 0;
}

void GSgfxPerfbarCPUEnd(void)
{
    GSgfxPerfbarCPUEntry(0);
    _perfCurFrame.cpu.total = (f32)(OSGetTime() - _perfCurFrame.cpu.start) / (OS_TIMER_CLOCK / 60);
    memcpy(&_perfLastFrame.cpu, &_perfCurFrame.cpu, sizeof(GSgfxPerfCPUFrame));
    memset(&_perfCurFrame.cpu, 0, sizeof(GSgfxPerfCPUFrame));

    _perfDListSize = 0;
    if (_perfPERFActive == TRUE) {
        PERFEndFrame();
        PERFStopAutoSampling();
        _perfPERFActive = FALSE;
        if (_perfRenderMode == GSGFX_PERFBAR_SDK_PERF) {
            GSgfxTokenWaitForClearAndReset(&_perfDListToken);
            if (_perfDisplayList == NULL) {
                _perfDisplayList = GSmemAllocAlign(GSGFX_PERFBAR_DLIST_SIZE, 32);
                DCFlushRange(_perfDisplayList, GSGFX_PERFBAR_DLIST_SIZE);
            }
            GXBeginDisplayList(_perfDisplayList, GSGFX_PERFBAR_DLIST_SIZE);
            GXResetWriteGatherPipe();
            PERFDumpScreen();
            GXFlush();
            _perfDListSize = GXEndDisplayList();
        }
    }
}

void GSgfxPerfbarCPUStart(void)
{
    _perfCurFrame.cpu.start = OSGetTime();
    _perfCurFrame.cpu.count = 0;
    if (_perfRenderMode == GSGFX_PERFBAR_SDK_PERF) {
        PERFStartFrame();
        PERFStartAutoSampling(1.0f);
        _perfPERFActive = TRUE;
    }
}

s32 GSgfxPerfbarGetRenderMode(void)
{
    return _perfRenderMode;
}

void GSgfxPerfbarSetRenderMode(s32 renderMode)
{
    _perfRenderMode = renderMode;
}

void GSgfxPerfbarInit(u8 frameCount)
{
    int i;
    int j;

    _perfFrameCount = frameCount;
    if (frameCount == 0) {
        _perfFrameCount = 1;
    }
    _perfPixelSkip = 600.0f / _perfFrameCount;

    memset(&_perfLastFrame, 0, sizeof(GSgfxPerfFrame));
    memset(&_perfCurFrame, 0, sizeof(GSgfxPerfFrame));

    _perfPeakCPU = 0.0f;
    _perfPeakCPUTimer = 0;
    _perfPeakGPUTimer = 0;
    _perfUpdateTime = 0.0f;
    _perfUpdateFPS = 0.0f;
    _perfUpdateCount = 0;

    PERFInit(60, 1, 1, GSmemAlloc, GSmemFree, _perfRestoreGPU);
    PERFSetDrawBWBarKey(TRUE);
    GSgfxTokenSetDrawSyncCBFunc(PERFSetDrawSyncCallback);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            _perfPERFMtx[i][j] = 0.0f;
        }
    }
    _perfPERFMtx[0][0] = 0.003125f;
    _perfPERFMtx[1][1] = 0.00416667f;
    _perfPERFMtx[2][2] = 1.0f;
    _perfPERFMtx[3][3] = 1.0f;
    _perfPERFMtx[0][3] = -0.95f;
    _perfPERFMtx[1][3] = -0.6250007f;

    _perfDisplayList = NULL;
    _perfDListSize = 0;
    _perfPERFActive = FALSE;
    _perfDListToken = 0;

    GSgfxPerfbarCPUStart();
    GSgfxPerfbarGPUStart();
}

void _perfRestoreGPU(void)
{
}
